/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32
 * @brief       Implementation of the CAN controller driver for ESP32 (esp_can)
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#include <assert.h>
#include <errno.h>
#include <string.h>

#include "can_esp.h"
#include "can_params.h"
#include "esp_attr.h"
#include "gpio_arch.h"
#include "irq_arch.h"

#include "driver/periph_ctrl.h"
#include "esp_rom_gpio.h"
#include "hal/interrupt_controller_types.h"
#include "hal/interrupt_controller_ll.h"
#include "hal/twai_hal.h"
#include "log.h"
#include "rom/ets_sys.h"
#include "soc/gpio_sig_map.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define CAN     TWAI

/** Common ESP CAN definitions */
#define ESP_CAN_INTR_MASK   (0xffU)      /* interrupts handled by ESP CAN */
#define ESP_CAN_CLOCK       APB_CLK_FREQ /* controller main clock */

/** Error mode limits used for ESP CAN */
#define ESP_CAN_ERROR_WARNING_LIMIT   96    /* indicator for heavy loaded bus */
#define ESP_CAN_ERROR_PASSIVE_LIMIT   128   /* switch to error passive state */
#define ESP_CAN_ERROR_BUS_OFF_LIMIT   256   /* switch to bus off state */

/* ESP32 CAN events */
#define ESP_CAN_EVENT_BUS_OFF           (1 << 0)
#define ESP_CAN_EVENT_ERROR_WARNING     (1 << 1)
#define ESP_CAN_EVENT_ERROR_PASSIVE     (1 << 2)
#define ESP_CAN_EVENT_TX_ERROR          (1 << 3)
#define ESP_CAN_EVENT_RX_ERROR          (1 << 4)
#define ESP_CAN_EVENT_TX_CONFIRMATION   (1 << 5)
#define ESP_CAN_EVENT_RX_INDICATION     (1 << 6)
#define ESP_CAN_EVENT_ARBITRATION_LOST  (1 << 7)
#define ESP_CAN_EVENT_WAKE_UP           (1 << 8)

/* ESP32 CAN commands */
#define ESP_CMD_TX_REQ              0x01    /* Transmission Request */
#define ESP_CMD_ABORT_TX            0x02    /* Abort Transmission */
#define ESP_CMD_TX_SINGLE_SHOT      0x03    /* Single Shot Transmission */
#define ESP_CMD_RELEASE_RX_BUFF     0x04    /* Release Receive Buffer */
#define ESP_CMD_CLR_DATA_OVRN       0x08    /* Clear Data Overrun */
#define ESP_CMD_SELF_RX_REQ         0x10    /* Self Reception Request */
#define ESP_CMD_SELF_RX_SINGLE_SHOT 0x12    /* Single Shot Self Reception */

/**
 * Frame format definition as it is expected/received in the TX/RX buffer
 * of ESP32.
 */
#define ESP_CAN_SFF_ID_LEN    2     /* two byte (11 bit) in SFF ID format */
#define ESP_CAN_EFF_ID_LEN    4     /* two byte (29 bit) in EFF ID format */
#define ESP_CAN_MAX_DATA_LEN  8     /* 8 data bytes at maximum */
#define ESP_CAN_FRAME_LEN     13    /* 13 bytes at maximum */

#if !defined(CAN_CLK_OUT) && !defined(CAN_CLK_OUT_DIV)
/* if CAN_CLK_OUT is not used, CAN_CLKOUT_DIV is set to 0 */
#define CAN_CLK_OUT_DIV        0
#elif defined(CAN_CLK_OUT) && !defined(CAN_CLK_OUT_DIV)
/* if CAN_CLK_OUT is used, CAN_CLK_OUT_DIV has to be defined */
#error "CAN_CLK_OUT pin defined but not the CAN_CLK_OUT_DIV"
#endif

/* driver interface functions */
static int  _esp_can_init(candev_t *candev);
static int  _esp_can_send(candev_t *candev, const struct can_frame *frame);
static void _esp_can_isr(candev_t *candev);
static int  _esp_can_set(candev_t *candev, canopt_t opt, void *value, size_t value_len);
static int  _esp_can_get(candev_t *candev, canopt_t opt, void *value, size_t max_len);
static int  _esp_can_abort(candev_t *candev, const struct can_frame *frame);
static int  _esp_can_set_filter(candev_t *candev, const struct can_filter *filter);
static int  _esp_can_remove_filter(candev_t *candev, const struct can_filter *filter);

/* internal function declarations, we don't need the device since we have only one */
static void _esp_can_set_bittiming(can_t *dev);
static void _esp_can_start(can_t *dev);
static void _esp_can_stop(can_t *dev);
static void _esp_can_power_up(can_t *dev);
static void _esp_can_power_down(can_t *dev);
static int  _esp_can_set_mode(can_t *dev, canopt_state_t state);
static void _esp_can_init_pins(void);
static void _esp_can_deinit_pins(void);
static void _esp_can_intr_handler(void *arg);

/** ESP32 CAN low level device driver data */
static const candev_driver_t _esp_can_driver = {
    .send = _esp_can_send,
    .init = _esp_can_init,
    .isr = _esp_can_isr,
    .get = _esp_can_get,
    .set = _esp_can_set,
    .abort = _esp_can_abort,
    .set_filter = _esp_can_set_filter,
    .remove_filter = _esp_can_remove_filter,
};

/** hardware dependent constants used for bit timing calculations */
static const struct can_bittiming_const bittiming_const = {
    .tseg1_min = 1,
    .tseg1_max = 16,
    .tseg2_min = 1,
    .tseg2_max = 8,
    .sjw_max = 4,
    .brp_min = 2,
    .brp_max = 128,
    .brp_inc = 2,
};

static void _esp_can_isr(candev_t *candev)
{
    can_t *dev = container_of(candev, can_t, candev);

    DEBUG("%s candev=%p\n", __func__, candev);

    assert(dev);
    assert(dev->candev.event_callback);

    if (dev->events & ESP_CAN_EVENT_BUS_OFF) {
        dev->events &= ~ESP_CAN_EVENT_BUS_OFF;
        dev->candev.event_callback(&dev->candev, CANDEV_EVENT_BUS_OFF, NULL);
    }

    if (dev->events & ESP_CAN_EVENT_ERROR_PASSIVE) {
        dev->events &= ~ESP_CAN_EVENT_ERROR_PASSIVE;
        dev->candev.event_callback(&dev->candev,
                                   CANDEV_EVENT_ERROR_PASSIVE, NULL);
    }

    if (dev->events & ESP_CAN_EVENT_ERROR_WARNING) {
        dev->events &= ~ESP_CAN_EVENT_ERROR_WARNING;
        dev->candev.event_callback(&dev->candev,
                                   CANDEV_EVENT_ERROR_WARNING, NULL);
    }

    if (dev->events & ESP_CAN_EVENT_TX_ERROR) {
        /* a pending TX confirmation has also to be deleted on TX bus error */
        dev->events &= ~ESP_CAN_EVENT_TX_ERROR;
        dev->events &= ~ESP_CAN_EVENT_TX_CONFIRMATION;
        if (dev->tx_frame) {
            /* handle the event only if there is still a frame in TX buffer */
            dev->candev.event_callback(&dev->candev,
                                       CANDEV_EVENT_TX_ERROR, dev->tx_frame);
            dev->tx_frame = NULL;
        }
    }

    if (dev->events & ESP_CAN_EVENT_TX_CONFIRMATION) {
        dev->events &= ~ESP_CAN_EVENT_TX_CONFIRMATION;
        if (dev->tx_frame) {
            /* handle the event only if there is still a frame in TX buffer */
            dev->candev.event_callback(&dev->candev,
                                       CANDEV_EVENT_TX_CONFIRMATION,
                                       dev->tx_frame);
            dev->tx_frame = NULL;
        }
    }

    if (dev->events & ESP_CAN_EVENT_RX_ERROR) {
        dev->events &= ~ESP_CAN_EVENT_RX_ERROR;
        dev->candev.event_callback(&dev->candev, CANDEV_EVENT_RX_ERROR, NULL);
    }

    if (dev->events & ESP_CAN_EVENT_RX_INDICATION) {
        dev->events &= ~ESP_CAN_EVENT_RX_INDICATION;

        while (dev->rx_frames_num) {
            dev->candev.event_callback(&dev->candev,
                                       CANDEV_EVENT_RX_INDICATION,
                                       &dev->rx_frames[dev->rx_frames_rptr]);
            dev->rx_frames_num--;
            dev->rx_frames_rptr++;
            dev->rx_frames_rptr &= ESP_CAN_MAX_RX_FRAMES-1;
        }
    }
}

twai_hal_context_t hw;

static int _esp_can_init(candev_t *candev)
{
    can_t *dev = container_of(candev, can_t, candev);

    DEBUG("%s candev=%p\n", __func__, candev);

    assert(dev);

    /* power up and configure the CAN controller and initialize the pins */
    _esp_can_power_up(dev);

    /* start the CAN controller in configured operation mode */
    _esp_can_start(dev);

    return 0;
}

static int _esp_can_send(candev_t *candev, const struct can_frame *frame)
{
    can_t *dev = container_of(candev, can_t, candev);

    DEBUG("%s candev=%p frame=%p\n", __func__, candev, frame);

    assert(dev);
    assert(frame);

    critical_enter();

    /* check wthere the device is already transmitting a frame */
    if (dev->tx_frame != NULL) {
        critical_exit();
        return -EBUSY;
    }

    /* save reference to frame in transmission (marks transmitter as busy) */
    dev->tx_frame = (struct can_frame*)frame;

    /* prepare the frame as expected by ESP32 */
    twai_hal_frame_t esp_frame = { };

    esp_frame.dlc = frame->len;
    esp_frame.rtr = (frame->can_id & CAN_RTR_FLAG);
    esp_frame.frame_format = (frame->can_id & CAN_EFF_FLAG);

    /* esp_frame is a union that provides two views on the same memory: one
     * tailored for efficient access and the other for readable code. Likely
     * due to cppcheck not finding all headers it wrongly assumes that values
     * are assigned but never read again (unreadVariable). But the union members
     * are read via the aliases to the same memory. */
    if (esp_frame.frame_format) {
        uint32_t id = frame->can_id & CAN_EFF_MASK;
        /* cppcheck-suppress unreadVariable */
        esp_frame.extended.id[0] = (id >> 21) & 0xff;
        /* cppcheck-suppress unreadVariable */
        esp_frame.extended.id[1] = (id >> 13) & 0xff;
        /* cppcheck-suppress unreadVariable */
        esp_frame.extended.id[2] = (id >> 5) & 0xff;
        /* cppcheck-suppress unreadVariable */
        esp_frame.extended.id[3] = (id << 3) & 0xff;
    }
    else {
        uint32_t id = frame->can_id & CAN_SFF_MASK;
        /* cppcheck-suppress unreadVariable */
        esp_frame.standard.id[0] = (id >> 3) & 0xff;
        /* cppcheck-suppress unreadVariable */
        esp_frame.standard.id[1] = (id << 5) & 0xff;
    }

    /* copy data from can_frame to esp_frame */
    memcpy(esp_frame.frame_format ? &esp_frame.extended.data
                                  : &esp_frame.standard.data,
           frame->data, ESP_CAN_MAX_DATA_LEN);

    /* set the single shot transmit command without self-receiption */
    esp_frame.single_shot = 1;
    esp_frame.self_reception = 0;

    /* place the frame in TX buffer and trigger the command */
    twai_hal_set_tx_buffer_and_transmit(&hw, &esp_frame);

    critical_exit();

    return 0;
}

static int _esp_can_set(candev_t *candev, canopt_t opt, void *value, size_t value_len)
{
    can_t *dev = container_of(candev, can_t, candev);

    assert(dev);
    assert(value);

    int res = 0;

    switch (opt) {
        case CANOPT_BITTIMING:
            DEBUG("%s CANOPT_BITTIMING\n", __func__);
            if (value_len < sizeof(struct can_bittiming)) {
                DEBUG("%s size error\n", __func__);
                return -EOVERFLOW;
            }
            dev->candev.bittiming = *((struct can_bittiming*)value);
            _esp_can_stop(dev);
            _esp_can_set_bittiming(dev);
            _esp_can_start(dev);

            res = sizeof(struct can_bittiming);
            break;

        case CANOPT_STATE:
            DEBUG("%s CANOPT_STATE %d\n", __func__, *((canopt_state_t *)value));
            if (dev->powered_up) {
                _esp_can_stop(dev);
            }
            res = _esp_can_set_mode(dev, *((canopt_state_t *)value));
            if (dev->powered_up) {
                _esp_can_start(dev);
            }
            if (res == 0) {
                res = sizeof(uint16_t);
            }
            break;

        case CANOPT_TEC:
            DEBUG("%s %s\n", __func__, "CANOPT_TEC");
            if (value_len < sizeof(uint16_t)) {
                DEBUG("%s size error\n", __func__);
                return -EOVERFLOW;
            }
            _esp_can_stop(dev);
            twai_ll_set_tec(hw.dev, *((uint16_t*)value));
            _esp_can_start(dev);
            res = sizeof(uint16_t);
            break;

        case CANOPT_REC:
            DEBUG("%s %s\n", __func__, "CANOPT_REC");
            if (value_len < sizeof(uint16_t)) {
                DEBUG("%s size error\n", __func__);
                return -EOVERFLOW;
            }
            _esp_can_stop(dev);
            twai_ll_set_rec(hw.dev, *((uint16_t*)value));
            _esp_can_start(dev);
            res = sizeof(uint16_t);
            break;

        default:
            DEBUG("%s not supported opt\n", __func__);
            res = ENOTSUP;
    }
    return res;
}

static int _esp_can_get(candev_t *candev, canopt_t opt, void *value, size_t max_len)
{
    can_t *dev = container_of(candev, can_t, candev);

    DEBUG("%s\n", __func__);

    assert(dev);
    assert(value);

    int res = 0;
    switch (opt) {
        case CANOPT_BITTIMING:
            DEBUG("%s CANOPT_BITTIMING\n", __func__);
            if (max_len < sizeof(struct can_bittiming)) {
                res = -EOVERFLOW;
                break;
            }

            *((struct can_bittiming*)value) = dev->candev.bittiming;

            res = sizeof(struct can_bittiming);
            break;

        case CANOPT_BITTIMING_CONST:
            DEBUG("%s CANOPT_BITTIMING_CONST\n", __func__);
            if (max_len < sizeof(struct can_bittiming_const)) {
                res = -EOVERFLOW;
                break;
            }

            *((struct can_bittiming_const*)value) = bittiming_const;

            res = sizeof(struct can_bittiming_const);
            break;

        case CANOPT_CLOCK:
            DEBUG("%s CANOPT_CLOCK\n", __func__);
            if (max_len < sizeof(uint32_t)) {
                res = -EOVERFLOW;
                break;
            }

            *((uint32_t *)value) = APB_CLK_FREQ;

            res = sizeof(uint32_t);
            break;

        case CANOPT_TEC:
            DEBUG("%s %s\n", __func__, "CANOPT_TEC");
            if (max_len < sizeof(uint16_t)) {
                res = EOVERFLOW;
                break;
            }

            *((uint16_t *)value) = twai_ll_get_tec(hw.dev);

            res = sizeof(uint16_t);
            break;

        case CANOPT_REC:
            DEBUG("%s %s\n", __func__, "CANOPT_REC");
            if (max_len < sizeof(uint16_t)) {
                res = EOVERFLOW;
                break;
            }

            *((uint16_t *)value) = twai_ll_get_rec(hw.dev);

            res = sizeof(uint16_t);
            break;

        case CANOPT_RX_FILTERS:
            if (max_len % sizeof(struct can_filter) != 0) {
                res = -EOVERFLOW;
                break;
            }

            struct can_filter *list = value;
            unsigned filter_num_max = max_len / sizeof(struct can_filter);
            unsigned filter_num = 0;
            unsigned i;
            for (i = 0; i < ESP_CAN_MAX_RX_FILTERS && filter_num < filter_num_max; i++) {
                if (dev->rx_filters[i].can_id != 0) {
                    list[i] = dev->rx_filters[i];
                    filter_num++;
                }

            }

            res = filter_num * sizeof(struct can_filter);
            break;

        default:
            DEBUG("%s not supported opt\n", __func__);
            res = -ENOTSUP;
    }
    return res;
}

static int _esp_can_abort(candev_t *candev, const struct can_frame *frame)
{
    can_t *dev = container_of(candev, can_t, candev);

    DEBUG("%s candev=%p frame=%p\n", __func__, candev, frame);

    assert(dev);
    assert(frame);

    /* abort transmission command */
    twai_ll_set_cmd_abort_tx(hw.dev);

    /* mark the transmitter as free */
    dev->tx_frame = NULL;

    return -ENOTSUP;
}

static int _esp_can_set_filter(candev_t *candev, const struct can_filter *filter)
{
    can_t *dev = container_of(candev, can_t, candev);

    DEBUG("%s candev=%p filter=%p\n", __func__, candev, filter);

    assert(dev);
    assert(filter);

    int i;

    /* first, check whether filter is already set */
    for (i = 0; i < ESP_CAN_MAX_RX_FILTERS; i++) {
        if (dev->rx_filters[i].can_id == filter->can_id) {
            DEBUG("%s filter already set\n", __func__);
            return i;
        }
    }

    /* next, search for free filter entry */
    for (i = 0; i < ESP_CAN_MAX_RX_FILTERS; i++) {
        if (dev->rx_filters[i].can_id == 0) {
            break;
        }
    }

    if (i == ESP_CAN_MAX_RX_FILTERS) {
        DEBUG("%s no more filters available\n", __func__);
        return -EOVERFLOW;
    }

    /* set the filter and return the filter index */
    dev->rx_filters[i] = *filter;
    dev->rx_filter_num++;

    return i;
}

static int _esp_can_remove_filter(candev_t *candev, const struct can_filter *filter)
{
    can_t *dev = container_of(candev, can_t, candev);

    DEBUG("%s candev=%p filter=%p\n", __func__, candev, filter);

    assert(dev);
    assert(filter);

    /* search for the filter */
    for (unsigned i = 0; i < ESP_CAN_MAX_RX_FILTERS; i++) {
        if (dev->rx_filters[i].can_id == filter->can_id) {
            /* mark the filter entry as not in use */
            dev->rx_filters[i].can_id = 0;
            dev->rx_filter_num--;
            return 0;
        }
    }

    DEBUG("%s error filter not found\n", __func__);
    return -EOVERFLOW;
}

/**
 * Internal functions.
 */

static void _esp_can_start(can_t *dev)
{
    DEBUG("%s dev=%p\n", __func__, dev);
    assert(dev);

    /* start the CAN controller in configured mode */
    switch (dev->state) {
    case CANOPT_STATE_LISTEN_ONLY:
        twai_hal_start(&hw, TWAI_MODE_LISTEN_ONLY);
        break;
    case CANOPT_STATE_SLEEP:
        /* sleep mode is not supported, the normal mode is used instead */
    case CANOPT_STATE_ON:
        twai_hal_start(&hw, TWAI_MODE_NORMAL);
        break;
    default:
        break;
    }
}

static void _esp_can_stop(can_t *dev)
{
    DEBUG("%s dev=%p\n", __func__, dev);
    assert(dev);

    /* stop the CAN controller by entering the reset mode */
    twai_hal_stop(&hw);
}

static void _esp_can_power_up(can_t *dev)
{
    /**
     * Function esp_can_power_up
     * - powers up the CAN controller,
     * - initializes the HAL context,
     * - sets the timing and the acceptance filters according to configuration
     * - resets the error counters and sets the warning limit
     *
     * The CAN controller must be started in configured mode explicitly
     * afterwards using function _esp_can_start().
     */

    DEBUG("%s dev=%p\n", __func__, dev);
    assert(dev);

    /* just return when already powered up */
    if (dev->powered_up) {
        return;
    }

    critical_enter();

    /* power up the peripheral */
    periph_module_reset(PERIPH_TWAI_MODULE);
    periph_module_enable(PERIPH_TWAI_MODULE);

    /* initialize the HAL context, on return the CAN controller is in listen
     * only mode but not yet started, the error counters are reset and
     * pending interrupts cleared */

    if (!twai_hal_init(&hw)) {
        assert(false);
    }

    /* set bittiming from parameters as given in device data */
    _esp_can_set_bittiming(dev);

    /* set warning limit */
    twai_ll_set_err_warn_lim(hw.dev, ESP_CAN_ERROR_WARNING_LIMIT);

    /* route CAN interrupt source to CPU interrupt and enable it */
    intr_matrix_set(PRO_CPU_NUM, ETS_TWAI_INTR_SOURCE, CPU_INUM_CAN);
    intr_cntrl_ll_set_int_handler(CPU_INUM_CAN, _esp_can_intr_handler, (void*)(uintptr_t)dev);
    intr_cntrl_ll_enable_interrupts(BIT(CPU_INUM_CAN));

    /* initialize used GPIOs */
    _esp_can_init_pins();

    /* request board to power on CAN transceiver */
    board_candev_set_power(&dev->candev, true);

    dev->powered_up = true;

    critical_exit();
}

static void _esp_can_power_down(can_t *dev)
{
    DEBUG("%s dev=%p\n", __func__, dev);
    assert(dev);

    /* just return when already powered down */
    if (!dev->powered_up) {
        return;
    }

    /* request board to power off CAN transceiver */
    board_candev_set_power(&dev->candev, false);

    /* deinitialize used GPIOs */
    _esp_can_deinit_pins();

    /* stop the CAN controller and deinitialize the HAL context */
    twai_hal_stop(&hw);
    twai_hal_deinit(&hw);

    /* power down the CAN controller */
    periph_module_disable(PERIPH_TWAI_MODULE);

    dev->powered_up = false;
}

static void _esp_can_init_pins(void)
{
    DEBUG("%s\n", __func__);

    const can_conf_t *cfg = &candev_conf[0];

    /* Init TX pin */
    gpio_init(cfg->tx_pin, GPIO_OUT);
    esp_rom_gpio_connect_out_signal(cfg->tx_pin, TWAI_TX_IDX, false, false);
    esp_rom_gpio_pad_select_gpio(cfg->tx_pin);
    gpio_set_pin_usage(cfg->tx_pin, _CAN);

    /* Init RX pin */
    gpio_init(cfg->rx_pin, GPIO_IN);
    esp_rom_gpio_connect_in_signal(cfg->rx_pin, TWAI_RX_IDX, false);
    esp_rom_gpio_pad_select_gpio(cfg->rx_pin);
    gpio_set_pin_usage(cfg->rx_pin, _CAN);

#ifdef CAN_CLK_OUT
    /* Init CLK_OUT pin (optional) if defined */
    gpio_init(cfg->clk_out_pin, GPIO_OD);
    esp_rom_gpio_connect_out_signal(cfg->clk_out_pin, TWAI_CLKOUT_IDX, false, false);
    esp_rom_gpio_pad_select_gpio(cfg->clk_out_pin);
    gpio_set_pin_usage(cfg->clk_out_pin, _CAN);
#endif

#ifdef CAN_BUS_ON_OFF
    /* Init BUS_ON_OFF pin pin (optional) if defined */
    gpio_init(cfg->bus_on_of_pin, GPIO_OD);
    esp_rom_gpio_connect_out_signal(cfg->bus_on_of_pin, TWAI_BUS_OFF_ON_IDX, false, false);
    esp_rom_gpio_pad_select_gpio(cfg->bus_on_of_pin);
    gpio_set_pin_usage(cfg->bus_on_of_pin, _CAN);
#endif
}

static void _esp_can_deinit_pins(void)
{
    const can_conf_t *cfg = &candev_conf[0];

    /* Reset TX pin */
    gpio_set_pin_usage(cfg->tx_pin, _GPIO);
    gpio_init(cfg->tx_pin, GPIO_IN_PU);

    /* Reset RX pin */
    gpio_set_pin_usage(cfg->rx_pin, _GPIO);
    gpio_init(cfg->rx_pin, GPIO_IN_PU);

#ifdef CAN_CLK_OUT
    /* Reset CLK_OUT pin (optional) if defined */
    gpio_set_pin_usage(cfg->clk_out_pin, _GPIO);
    gpio_init(cfg->clk_out_pin, GPIO_IN);
#endif

#ifdef CAN_BUS_ON_OFF
    /* Reset BUS_ON_OFF pin pin (optional) if defined */
    gpio_set_pin_usage(cfg->bus_on_of_pin, _GPIO);
    gpio_init(cfg->bus_on_of_pin, GPIO_IN_PD);
#endif
}

static int _esp_can_set_mode(can_t *dev, canopt_state_t state)
{
    DEBUG("%s dev=%p state=%d\n", __func__, dev, state);

    assert(dev);

    critical_enter();

    /* set the new mode */
    dev->state = state;

    switch (state) {
        case CANOPT_STATE_OFF:
            /* stop and power down the CAN controller */
            _esp_can_power_down(dev);
            break;

        case CANOPT_STATE_SLEEP:
            /* sleep mode is not supported, so CAN can't be powered down */
        case CANOPT_STATE_LISTEN_ONLY:
        case CANOPT_STATE_ON:
            /* power up and (re)configure the CAN controller if necessary */
            _esp_can_power_up(dev);
            /* restart the CAN controller in new mode */
            _esp_can_stop(dev);
            _esp_can_start(dev);
            break;

        default:
            LOG_TAG_ERROR ("esp_can", "state value %d not supported\n", state);
            break;
    }
    critical_exit();

    return 0;
}

static void IRAM_ATTR _esp_can_intr_handler(void *arg)
{
    can_t* dev = (can_t *)(uintptr_t)arg;

    assert(arg);

    critical_enter();

    uint32_t events = twai_hal_get_events(&hw);

    DEBUG("%s events=%08"PRIx32"\n", __func__, events);

    /* Arbitration Lost Interrupt */
    if (events & TWAI_HAL_EVENT_ARB_LOST) {
        DEBUG("%s arbitration lost interrupt\n", __func__);
        /* can only happen during transmission, handle it as error in single shot transmission */
        dev->events |= ESP_CAN_EVENT_TX_ERROR;
    }

    /* BUS_OFF state condition */
    if (events & TWAI_HAL_EVENT_BUS_OFF) {
        DEBUG("%s bus-off state interrupt\n", __func__);
        /* save the event */
        dev->events |= ESP_CAN_EVENT_BUS_OFF;
    }

    /* ERROR_WARNING state condition, RX/TX error counter are > 96 */
    if (events & TWAI_HAL_EVENT_ABOVE_EWL) {
        DEBUG("%s error warning interrupt\n", __func__);
        /* save the event */
        dev->events |= ESP_CAN_EVENT_ERROR_WARNING;
    }

    /* enter to / return from ERROR_PASSIVE state */
    if (events & TWAI_HAL_EVENT_ERROR_PASSIVE) {
        DEBUG("%s error passive interrupt %"PRIu32" %"PRIu32"\n", __func__,
              twai_ll_get_tec(hw.dev), twai_ll_get_rec(hw.dev));
        /* save the event */
        dev->events |= ESP_CAN_EVENT_ERROR_PASSIVE;
    }

    /*
     * Bus Error Interrupt (bit, stuff, crc, form, ack), details are captured
     * in ECC register (see SJA1000 Data sheet, Table 20 and 21)
     */
    if (events & TWAI_HAL_EVENT_BUS_ERR) {
        DEBUG("%s bus error interrupt\n", __func__);
        /* save the event */
        dev->events |= ESP_CAN_EVENT_TX_ERROR;
    }

    /* TX buffer becomes free */
    if (events & TWAI_HAL_EVENT_TX_BUFF_FREE) {
        DEBUG("%s transmit interrupt\n", __func__);
        /* save the event */
        dev->events |= ESP_CAN_EVENT_TX_CONFIRMATION;
    }

    /* RX buffer has one or more frames */
    if (events & TWAI_HAL_EVENT_RX_BUFF_FRAME) {
        /* get the number of messages in receive buffer */
        uint32_t msg_cnt = twai_hal_get_rx_msg_count(&hw);

        DEBUG("%s receive interrupt, msg_cnt=%"PRIu32"\n", __func__, msg_cnt);

        for (unsigned i = 0; i < msg_cnt; i++) {
            twai_hal_frame_t esp_frame;
            if (twai_hal_read_rx_buffer_and_clear(&hw, &esp_frame) &&
                (dev->rx_frames_num < ESP_CAN_MAX_RX_FRAMES)) {
                /* prepare the CAN frame from ESP32 CAN frame */
                struct can_frame frame = {};

                if (esp_frame.frame_format) {
                    frame.can_id = esp_frame.extended.id[0];
                    frame.can_id = (frame.can_id << 8) | esp_frame.extended.id[1];
                    frame.can_id = (frame.can_id << 8) | esp_frame.extended.id[2];
                    frame.can_id = (frame.can_id << 5) | (esp_frame.extended.id[3] >> 3);
                    memcpy(frame.data, &esp_frame.extended.data, CAN_MAX_DLEN);
                }
                else {
                    frame.can_id = esp_frame.standard.id[0];
                    frame.can_id = (frame.can_id << 3) | (esp_frame.standard.id[1] >> 5);
                    memcpy(frame.data, &esp_frame.standard.data, CAN_MAX_DLEN);
                }
                frame.can_id |= esp_frame.rtr ? CAN_RTR_FLAG : 0;
                frame.can_id |= esp_frame.frame_format ? CAN_EFF_FLAG : 0;
                frame.len = esp_frame.dlc;

                /* apply acceptance filters only if they are set */
                unsigned f_id = 0;
                if (dev->rx_filter_num) {
                    for (f_id = 0; f_id < ESP_CAN_MAX_RX_FILTERS; f_id++) {
                        /* compared masked can_id with each filter */
                        struct can_filter* f = &dev->rx_filters[f_id];
                        if ((f->can_mask & f->can_id) ==
                            (f->can_mask & frame.can_id)) {
                            /* break the loop on first match */
                            break;
                        }
                    }
                }
                /*
                 * put the frame in the RX ring buffer if there are no
                 * acceptance filters (f_id == 0) or one filter matched
                 * (f_id < ESP_CAN_MAX_RX_FILTERS), otherwise drop it.
                 */
                if (f_id < ESP_CAN_MAX_RX_FILTERS) {
                    dev->rx_frames[dev->rx_frames_wptr] = frame;

                    dev->rx_frames_num++;
                    dev->rx_frames_wptr++;
                    dev->rx_frames_wptr &= ESP_CAN_MAX_RX_FRAMES-1;

                    /* at least one RX frame has been saved */
                    dev->events |= ESP_CAN_EVENT_RX_INDICATION;
               }
            }
            else {
                DEBUG("%s receive buffer overrun\n", __func__);
                /* we use rx error since there is no separate overrun error */
                dev->events |= ESP_CAN_EVENT_RX_ERROR;
            }
        }
    }

    DEBUG("%s events=%08"PRIx32"\n", __func__, dev->events);

    /* inform the upper layer that there are events to be handled */
    if (dev->events && dev->candev.event_callback) {
        dev->candev.event_callback(&dev->candev, CANDEV_EVENT_ISR, NULL);
    }

    critical_exit();
}

/* accept all CAN messages by default, filtering is done by software */
static const twai_filter_config_t twai_filter = {
    .single_filter = false,           /* single filter */
    .acceptance_mask = UINT32_MAX,    /* all bits masked */
};

static void _esp_can_set_bittiming(can_t *dev)
{
    /* sets the bus timing, the CAN controller has to stopped before using
     * function _esp_can_stop(dev) and has to be restartet using
     * function _esp_can_start() afterwards */

    assert(dev);

    struct can_bittiming* timing = &dev->candev.bittiming;

    DEBUG("%s %p bitrate=%"PRIu32", brp=%"PRIu32", sample_point=%"PRIu32", "
          "tq=%"PRIu32", prop_seg=%"PRIu32" phase_seg1=%"PRIu32", "
          "phase_seg2=%"PRIu32", sjw =%"PRIu32"\n", __func__, dev,
          timing->bitrate, timing->brp, timing->sample_point, timing->tq,
          timing->prop_seg, timing->phase_seg1, timing->phase_seg2,
          timing->sjw);

    twai_timing_config_t twai_timing = {
        .brp = timing->brp,
        .sjw = timing->sjw,
        .tseg_1 = timing->prop_seg + timing->phase_seg1,
        .tseg_2 = timing->phase_seg2,
    };

    twai_hal_configure(&hw, &twai_timing, &twai_filter,
                       ESP_CAN_INTR_MASK, CAN_CLK_OUT_DIV);
}

void can_init(can_t *dev, const can_conf_t *conf)
{
    DEBUG("%s dev=%p conf=%p\n", __func__, dev, conf);

    assert(dev);
    assert(conf);

    dev->candev.driver = &_esp_can_driver;
    dev->candev.bittiming.bitrate = conf->bitrate;

    /* determine the hardware bittiming constants */
    struct can_bittiming_const timing_const = bittiming_const;

    /* calculate the initial bittimings from the bittiming constants */
    can_device_calc_bittiming(ESP_CAN_CLOCK, &timing_const, &dev->candev.bittiming);

    /* initialize other members */
    dev->state = CANOPT_STATE_SLEEP;
    dev->tx_frame = NULL;
    dev->rx_frames_wptr = 0;
    dev->rx_frames_rptr = 0;
    dev->rx_frames_num = 0;
    dev->rx_filter_num = 0;
    dev->powered_up = false;
}

void can_print_config(void)
{
    printf("\tCAN_DEV(0)\ttxd=%d rxd=%d\n", CAN_TX, CAN_RX);
}

can_t *candev2periph(candev_t *candev)
{
    if (candev && (candev->driver == &_esp_can_driver)) {
        /* this is an ESP peripheral CAN, as opposed to e.g. an external SPI
         * attached one */
        return container_of(candev, can_t, candev);
    }

    return NULL;
}

/**@}*/

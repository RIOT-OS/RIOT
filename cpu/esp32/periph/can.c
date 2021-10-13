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
#include "esp_common.h"
#include "gpio_arch.h"
#include "irq_arch.h"
#include "tools.h"

#include "can/common.h"
#include "can/device.h"
#include "driver/periph_ctrl.h"
#include "freertos/FreeRTOS.h"
#include "log.h"
#include "rom/ets_sys.h"

#include "periph/can.h"
#include "periph/gpio.h"
#include "soc/can_struct.h"
#include "soc/gpio_struct.h"

#include "xtensa/xtensa_api.h"

#define ENABLE_DEBUG 0
#include "debug.h"

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

typedef union esp_can_frame {
    struct {
        struct {
            uint8_t dlc :4;  /* frame payload length */
            uint8_t zero:2;  /* don't care, should be zero */
            uint8_t rtr :1;  /* remote transmission request frame */
            uint8_t eff :1;  /* extended ID frame format */
        };
        union {
            struct {
                uint8_t id[ESP_CAN_SFF_ID_LEN];     /* 11 bit standard frame identifier */
                uint8_t data[ESP_CAN_MAX_DATA_LEN]; /* data bytes */
                uint8_t reserved8[2];
            } standard;
            struct {
                uint8_t id[ESP_CAN_EFF_ID_LEN];     /* 29 bit standard frame identifier */
                uint8_t data[ESP_CAN_MAX_DATA_LEN]; /* data bytes */
            } extended;
        };
    };
    uint8_t bytes[ESP_CAN_FRAME_LEN];
} _esp_can_frame_t;

/* driver interface functions */
static int  _esp_can_init(candev_t *candev);
static int  _esp_can_send(candev_t *candev, const struct can_frame *frame);
static void _esp_can_isr(candev_t *candev);
static int  _esp_can_set(candev_t *candev, canopt_t opt, void *value, size_t value_len);
static int  _esp_can_get(candev_t *candev, canopt_t opt, void *value, size_t max_len);
static int  _esp_can_abort(candev_t *candev, const struct can_frame *frame);
static int  _esp_can_set_filter(candev_t *candev, const struct can_filter *filter);
static int  _esp_can_remove_filter(candev_t *candev, const struct can_filter *filter);

/* internal function declarations, we don't need device since we habe only one */
static void _esp_can_set_bittiming (can_t *dev);
static void _esp_can_power_up(can_t *dev);
static void _esp_can_power_down(can_t *dev);
static void _esp_can_init_pins(can_t *dev);
static int  _esp_can_set_mode(can_t *dev, canopt_state_t state);
static void IRAM_ATTR _esp_can_intr_handler (void *arg);

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
    can_t *dev = (can_t *)candev;

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

static int _esp_can_init(candev_t *candev)
{
    can_t *dev = (can_t *)candev;

    DEBUG("%s candev=%p\n", __func__, candev);

    assert(dev);

    /* initialize used GPIOs */
    _esp_can_init_pins(dev);

    /* power up and configure the CAN controller */
    _esp_can_power_up(dev);

    return 0;
}

static int _esp_can_send(candev_t *candev, const struct can_frame *frame)
{
    can_t *dev = (can_t *)candev;

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
    _esp_can_frame_t esp_frame = {};

    esp_frame.dlc = frame->can_dlc;
    esp_frame.rtr = (frame->can_id & CAN_RTR_FLAG);
    esp_frame.eff = (frame->can_id & CAN_EFF_FLAG);

    if (esp_frame.eff) {
        uint32_t id = frame->can_id & CAN_EFF_MASK;
        esp_frame.extended.id[0] = (id >> 21) & 0xff;
        esp_frame.extended.id[1] = (id >> 13) & 0xff;
        esp_frame.extended.id[2] = (id >> 5) & 0xff;
        esp_frame.extended.id[3] = (id << 3) & 0xff;
    }
    else {
        uint32_t id = frame->can_id & CAN_SFF_MASK;
        esp_frame.standard.id[0] = (id >> 3) & 0xff;
        esp_frame.standard.id[1] = (id << 5) & 0xff;
    }

    memcpy(esp_frame.eff ? &esp_frame.extended.data
                         : &esp_frame.standard.data, frame->data, ESP_CAN_MAX_DATA_LEN);

    /* place the frame in TX buffer of ESP32 */
    for (unsigned i = 0; i < ESP_CAN_FRAME_LEN; i++) {
        CAN.tx_rx_buffer[i].val = esp_frame.bytes[i];
    }

    /* set the single shot transmit command without self-receiption */
    CAN.command_reg.val = ESP_CMD_TX_SINGLE_SHOT;

    critical_exit();

    return 0;
}

static int _esp_can_set(candev_t *candev, canopt_t opt, void *value, size_t value_len)
{
    can_t *dev = (can_t *)candev;

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
            _esp_can_set_bittiming(dev);

            res = sizeof(struct can_bittiming);
            break;

        case CANOPT_STATE:
            DEBUG("%s CANOPT_STATE\n", __func__);
            res = _esp_can_set_mode(dev, *((canopt_state_t *)value));
            if (res == 0) {
                res = sizeof(uint16_t);
            }
            break;

        case CANOPT_TEC:
        case CANOPT_REC:
            DEBUG("%s %s\n", __func__, (opt == CANOPT_TEC) ? "CANOPT_TEC" : "CANOPT_REC");
            if (value_len < sizeof(uint16_t)) {
                DEBUG("%s size error\n", __func__);
                return -EOVERFLOW;
            }
            if (opt == CANOPT_TEC) {
                CAN.tx_error_counter_reg.byte = *((uint16_t*)value);
            }
            else {
                CAN.rx_error_counter_reg.byte = *((uint16_t*)value);
            }
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
    can_t *dev = (can_t *)candev;

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
        case CANOPT_REC:
            DEBUG("%s %s\n", __func__, (opt == CANOPT_TEC) ? "CANOPT_TEC" : "CANOPT_REC");
            if (max_len < sizeof(uint16_t)) {
                res = EOVERFLOW;
                break;
            }

            if (opt == CANOPT_TEC) {
                *((uint16_t *)value) = CAN.tx_error_counter_reg.byte;
            }
            else {
                *((uint16_t *)value) = CAN.rx_error_counter_reg.byte;
            }

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
    can_t *dev = (can_t *)candev;

    DEBUG("%s candev=%p frame=%p\n", __func__, candev, frame);

    assert(dev);
    assert(frame);

    /* abort transmission command */
    CAN.command_reg.val = ESP_CMD_ABORT_TX;

    /* mark the transmitter as free */
    dev->tx_frame = NULL;

    return -ENOTSUP;
}

static int _esp_can_set_filter(candev_t *candev, const struct can_filter *filter)
{
    can_t *dev = (can_t *)candev;

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
    can_t *dev = (can_t *)candev;

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
 * Internal functions. All these functions have no dev parameter since
 * they directly access the only one existing CAN controller.
 */

static void _esp_can_set_reset_mode(void)
{
    DEBUG("%s\n", __func__);

    while (CAN.mode_reg.reset != 1) {
        CAN.mode_reg.reset = 1;
    }
}

static void _esp_can_set_operating_mode(void)
{
    DEBUG("%s\n", __func__);

    while (CAN.mode_reg.reset != 0) {
        CAN.mode_reg.reset = 0;
    }
}

static int _esp_can_config(can_t *dev)
{
    DEBUG("%s dev=%p\n", __func__, dev);

    assert(dev);

    critical_enter();

    /* reset mode to be able to write configuration registers */
    _esp_can_set_reset_mode();

    /* configuration is done in listen only mode */
    CAN.mode_reg.self_test = 0;
    CAN.mode_reg.listen_only = 1;

    /* Use SJA1000 PeliCAN mode and registers layout */
    CAN.clock_divider_reg.can_mode = 1;

#ifndef ESP_CAN_CLK_OUT
    CAN.clock_divider_reg.clock_off = 1;
    CAN.clock_divider_reg.clock_divider = 0;
#else
    uint32_t clk_out_div = ESP_CAN_CLK_OUT_DIV / 2 - 1;
    clk_out_div = (clk_out_div < 7) ? clk_out_div : 7;
    CAN.clock_divider_reg.clock_off = 0;
    CAN.clock_divider_reg.clock_divider = clk_out_div;
#endif

    /* set error counter values and warning limits */
    CAN.error_warning_limit_reg.byte = ESP_CAN_ERROR_WARNING_LIMIT;
    CAN.tx_error_counter_reg.byte = 0;
    CAN.rx_error_counter_reg.byte = 0;

    /* accept all CAN messages, filtering is done by software */
    CAN.mode_reg.acceptance_filter = 0;            /* single filter */
    CAN.acceptance_filter.mask_reg[0].byte = 0xff; /* all bits masked */
    CAN.acceptance_filter.mask_reg[1].byte = 0xff;
    CAN.acceptance_filter.mask_reg[2].byte = 0xff;
    CAN.acceptance_filter.mask_reg[3].byte = 0xff;

    /* clear interrupt status register by read and enable all interrupts */
    uint32_t tmp = CAN.interrupt_reg.val; (void)tmp;
    CAN.interrupt_enable_reg.val = ESP_CAN_INTR_MASK;

    /* route CAN interrupt source to CPU interrupt and enable it */
    intr_matrix_set(PRO_CPU_NUM, ETS_CAN_INTR_SOURCE, CPU_INUM_CAN);
    xt_set_interrupt_handler(CPU_INUM_CAN, _esp_can_intr_handler, (void*)dev);
    xt_ints_on(BIT(CPU_INUM_CAN));

    /* set bittiming from parameters as given in device data */
    _esp_can_set_bittiming(dev);

    /* switch to operating mode */
    _esp_can_set_operating_mode();

    critical_exit();

    return 0;
}

static void _esp_can_power_up(can_t *dev)
{
    assert(dev);

    /* just return when already powered up */
    if (dev->powered_up) {
        return;
    }

    DEBUG("%s dev=%p\n", __func__, dev);

    critical_enter();

    /* power up and (re)configure the CAN controller */
    periph_module_enable(PERIPH_CAN_MODULE);
    dev->powered_up = true;
    _esp_can_config (dev);

    critical_exit();
}

static void _esp_can_power_down(can_t *dev)
{
    assert(dev);

    /* just return when already powered down */
    if (!dev->powered_up) {
        return;
    }

    DEBUG("%s dev=%p\n", __func__, dev);

    /* power down the CAN controller */
    periph_module_disable(PERIPH_CAN_MODULE);
    dev->powered_up = false;
}

static void _esp_can_init_pins(can_t *dev)
{
    DEBUG("%s dev=%p\n", __func__, dev);

    assert(dev);

    /* Init TX pin */
    gpio_init(dev->tx_pin, GPIO_OUT);
    gpio_set_pin_usage(dev->tx_pin, _CAN);
    GPIO.func_out_sel_cfg[dev->tx_pin].func_sel = CAN_TX_IDX;

    /* Init RX pin */
    gpio_init(dev->rx_pin, GPIO_IN);
    gpio_set_pin_usage(dev->rx_pin, _CAN);
    GPIO.func_in_sel_cfg[CAN_RX_IDX].sig_in_sel = 1;
    GPIO.func_in_sel_cfg[CAN_RX_IDX].sig_in_inv = 0;
    GPIO.func_in_sel_cfg[CAN_RX_IDX].func_sel = dev->rx_pin;

#ifdef ESP_CAN_CLK_OUT
    /* Init CLK_OUT pin (optional) if defined */
    gpio_init(dev->clk_out_pin, GPIO_OD);
    gpio_set_pin_usage(dev->clk_out_pin, _CAN);
    GPIO.func_out_sel_cfg[dev->clk_out_pin].func_sel = CAN_CLKOUT_IDX;
#endif

    /* Init BUS_ON_OFF pin pin (optional) if defined */
#ifdef ESP_CAN_BUS_ON_OFF
    gpio_init(dev->bus_on_of_pin, GPIO_OD);
    gpio_set_pin_usage(dev->bus_on_of_pin, _CAN);
    GPIO.func_out_sel_cfg[dev->bus_on_of_pin].func_sel = CAN_BUS_OFF_ON_IDX;
#endif
}

static int _esp_can_set_mode(can_t *dev, canopt_state_t state)
{
    DEBUG("%s dev=%p state=%d\n", __func__, dev, state);

    assert(dev);

    critical_enter();

    switch (state) {
        case CANOPT_STATE_OFF:
            /* just power down the CAN controller */
            _esp_can_power_down(dev);
            break;

        case CANOPT_STATE_LISTEN_ONLY:
            /* power up and (re)configure the CAN controller if necessary */
            _esp_can_power_up(dev);
            /* set the new mode (has to be done in reset mode) */
            _esp_can_set_reset_mode();
            CAN.mode_reg.self_test = 0;
            CAN.mode_reg.listen_only = 1;
            _esp_can_set_operating_mode ();
            break;

        case CANOPT_STATE_SLEEP:
            /* Sleep mode is not supported by ESP32, State On is used instead. */
            #if 0
            /* power up and (re)configure the CAN controller if necessary */
            _esp_can_power_up(dev);
            /* set the sleep mode (not necessary to set reset mode before) */
            CAN.mode_reg.sleep_mode = 1;
            break;
            #endif

        case CANOPT_STATE_ON:
            /* power up and (re)configure the CAN controller if necessary */
            _esp_can_power_up(dev);
            /* set the new mode (has to be done in reset mode) */
            _esp_can_set_reset_mode();
            CAN.mode_reg.self_test = 0;
            CAN.mode_reg.listen_only = 0;
            _esp_can_set_operating_mode ();
            break;

        default:
            LOG_TAG_ERROR ("esp_can", "state value %d not supported\n", state);
            break;
    }
    dev->state = state;

    critical_exit();

    return 0;
}

static void IRAM_ATTR _esp_can_intr_handler(void *arg)
{
    can_t* dev = (can_t *)arg;

    assert(arg);

    critical_enter();

    /* read the registers to clear them */
    can_status_reg_t sta_reg = CAN.status_reg;
    can_intr_reg_t   int_reg = CAN.interrupt_reg;

    DEBUG("%s int=%08x sta=%08x\n", __func__, int_reg.val, sta_reg.val);

    /* Wake-Up Interrupt (not supported by ESP32) */
    if (int_reg.reserved1) {
        DEBUG("%s wake-up interrupt\n", __func__);
        dev->events |= ESP_CAN_EVENT_WAKE_UP;
    }

    /* Arbitration Lost Interrupt */
    if (int_reg.arb_lost) {
        DEBUG("%s arbitration lost interrupt\n", __func__);
        /* can only happen during transmission, handle it as error in single shot transmission */
        dev->events |= ESP_CAN_EVENT_TX_ERROR;
    }

    /* bus or error status has changed, handle this first */
    if (int_reg.err_warn) {

        /* BUS_OFF state condition */
        if (sta_reg.error && sta_reg.bus) {
            DEBUG("%s bus-off state interrupt\n", __func__);
            /* switch to listen only mode to freeze the RX error counter */
            _esp_can_set_mode (dev, CANOPT_STATE_LISTEN_ONLY);
            /* save the event */
            dev->events |= ESP_CAN_EVENT_BUS_OFF;
        }

        /* ERROR_WARNING state condition, RX/TX error counter are > 96 */
        else if (sta_reg.error && !sta_reg.bus) {
            DEBUG("%s error warning interrupt\n", __func__);
            /* save the event */
            dev->events |= ESP_CAN_EVENT_ERROR_WARNING;
        }
    }

    /* enter to / return from ERROR_PASSIVE state */
    if (int_reg.err_passive) {

        /* enter to the ERROR_PASSIVE state when one of the error counters is >= 128 */
        if (CAN.tx_error_counter_reg.byte >= ESP_CAN_ERROR_PASSIVE_LIMIT ||
            CAN.rx_error_counter_reg.byte >= ESP_CAN_ERROR_PASSIVE_LIMIT)
            DEBUG("%s error passive interrupt %d %d\n", __func__,
                  CAN.tx_error_counter_reg.byte,
                  CAN.rx_error_counter_reg.byte);
            /* save the event */
            dev->events |= ESP_CAN_EVENT_ERROR_PASSIVE;
    }

    /*
     * Bus Error Interrupt (bit, stuff, crc, form, ack), details are captured
     * in ECC register (see SJA1000 Data sheet, Table 20 and 21)
     */

    if (int_reg.bus_err) {
        can_err_code_cap_reg_t ecc = CAN.error_code_capture_reg;
        /* save the event */
        DEBUG("%s bus error interrupt, ecc=%08x\n", __func__, ecc.val);
        dev->events |= ecc.direction ? ESP_CAN_EVENT_RX_ERROR
                                             : ESP_CAN_EVENT_TX_ERROR;
    }

    /* TX buffer becomes free */
    if (int_reg.tx) {
        DEBUG("%s transmit interrupt\n", __func__);
        /* save the event */
        dev->events |= ESP_CAN_EVENT_TX_CONFIRMATION;
    }

    /* RX buffer has one or more frames */
    if (int_reg.rx) {
        /* get the number of messages in receive buffer */
        uint32_t msg_cnt = CAN.rx_message_counter_reg.val;

        DEBUG("%s receive interrupt, msg_cnt=%d\n", __func__, msg_cnt);

        for (unsigned i = 0; i < msg_cnt; i++) {
            _esp_can_frame_t esp_frame;
            /* fetch the frame from RX buffer of ESP32 */
            for (unsigned j = 0; j < ESP_CAN_FRAME_LEN; j++) {
                esp_frame.bytes[j] = CAN.tx_rx_buffer[j].val;
            }
            /* clear RX buffer at read position */
            CAN.command_reg.val = ESP_CMD_RELEASE_RX_BUFF;

            if (dev->rx_frames_num < ESP_CAN_MAX_RX_FRAMES) {
                /* prepare the CAN frame from ESP32 CAN frame */
                struct can_frame frame = {};

                if (esp_frame.eff) {
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
                frame.can_id |= esp_frame.eff ? CAN_EFF_FLAG : 0;
                frame.can_dlc = esp_frame.dlc;

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
                }
            }
            else {
                DEBUG("%s receive buffer overrun\n", __func__);
                /* we use rx error since there is no separate overrun error */
                dev->events |= ESP_CAN_EVENT_RX_ERROR;
            }

        }
        /* save the event */
        dev->events |= ESP_CAN_EVENT_RX_INDICATION;
    }

    /* data overrun interrupts are not handled */
    if (int_reg.data_overrun) {
        DEBUG("%s data overrun interrupt\n", __func__);
        /* we use rx error since there is no separate overrun error */
        dev->events |= ESP_CAN_EVENT_RX_INDICATION;
    }

    DEBUG("%s events=%08x\n", __func__, dev->events);

    /* inform the upper layer that there are events to be handled */
    if (dev->events && dev->candev.event_callback) {
        dev->candev.event_callback(&dev->candev, CANDEV_EVENT_ISR, NULL);
    }

    critical_exit();
}

static void _esp_can_set_bittiming(can_t *dev)
{
    assert(dev);

    struct can_bittiming* timing = &dev->candev.bittiming;

    DEBUG("%s %p bitrate=%d, brp=%d, sample_point=%d, tq=%d, "
          "prop_seg=%d phase_seg1=%d, phase_seg2=%d, sjw =%d\n", __func__, dev,
          timing->bitrate, timing->brp, timing->sample_point, timing->tq,
          timing->prop_seg, timing->phase_seg1, timing->phase_seg2,
          timing->sjw);

    can_bus_tim_0_reg_t reg_0;
    can_bus_tim_1_reg_t reg_1;

    reg_0.baud_rate_prescaler = (timing->brp / 2) - 1;
    reg_0.sync_jump_width = timing->sjw - 1;
    reg_1.time_seg_1 = (timing->prop_seg + timing->phase_seg1) - 1;
    reg_1.time_seg_2 = timing->phase_seg2 - 1;
    reg_1.sampling = 0;

    _esp_can_set_reset_mode();
    CAN.bus_timing_0_reg.val = reg_0.val;
    CAN.bus_timing_1_reg.val = reg_1.val;
    _esp_can_set_operating_mode();
}

void can_init(can_t *dev, const can_conf_t *conf)
{
    DEBUG("%s dev=%p conf=%p\n", __func__, dev, conf);

    assert(dev);
    assert(conf);

    dev->candev.driver = &_esp_can_driver;
    dev->candev.bittiming.bitrate = conf->bitrate;

    dev->tx_pin = conf->tx_pin;
    dev->rx_pin = conf->rx_pin;
#ifdef ESP_CAN_CLK_OUT
    dev->clk_out_pin = conf->clk_out_pin;
#endif
#ifdef ESP_CAN_BUS_ON_OFF
    dev->bus_on_off_pin = conf->bus_on_off_pin;
#endif

    /* determine the hardware bittiming constants */
    struct can_bittiming_const timing_const = bittiming_const;

    /* calculate the initial bittimings from the bittiming constants */
    can_device_calc_bittiming (ESP_CAN_CLOCK, &timing_const, &dev->candev.bittiming);

    /* initialize other members */
    dev->state = CAN_STATE_SLEEPING;
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

/**@}*/

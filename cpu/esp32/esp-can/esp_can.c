/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32_esp_can
 * @brief       CAN device driver implementation for ESP32
 *
 * This module realizes the low-level CAN driver interface for the ESP32 CAN
 * controller which is compatible with the NXP SJA1000 CAN controller.
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#define ENABLE_DEBUG (0)
#include "debug.h"
#include "log.h"

#include "esp_attr.h"
#include "esp_common.h"
#include "gpio_arch.h"
#include "irq_arch.h"
#include "tools.h"

#include "can/device.h"
#include "driver/periph_ctrl.h"
#include "freertos/FreeRTOS.h"
#include "rom/ets_sys.h"

#include "periph/gpio.h"
#include "soc/can_struct.h"
#include "soc/gpio_struct.h"

#include "xtensa/xtensa_api.h"

#ifdef MODULE_ESP_CAN

/** Default CAN signal configuration */
#ifndef CAN_TX
#define CAN_TX  GPIO5
#endif
#ifndef CAN_RX
#define CAN_RX  GPIO35
#endif

#ifndef CAN_CLK_OUT_DIV
#define CAN_CLK_OUT_DIV     1
#endif

/** Default CAN bitrate setup */
#ifndef CAN_DEFAULT_BITRATE
#define CAN_DEFAULT_BITRATE (500000)
#endif

/** Common ESP CAN definitions */
#define ESP_CAN_DEV_NUMOF           1       /* there is one CAN device at maximum */
#define ESP_CAN_DEV_STACKSIZE       (THREAD_STACKSIZE_DEFAULT + THREAD_EXTRA_STACKSIZE_PRINTF)

#ifndef ESP_CAN_DEV_BASE_PRIORITY
#define ESP_CAN_DEV_BASE_PRIORITY (THREAD_PRIORITY_MAIN - 1)
#endif

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

/* ESP CAN receiver definitions */
#define ESP_CAN_MAX_RX_FILTERS  16  /**< number of acceptance filters */
#define ESP_CAN_MAX_RX_FRAMES   8   /**< must be a power of two */

/* shorter typename definitions */
typedef struct can_frame  can_frame_t;
typedef struct can_filter can_filter_t;

/**
 * Low level device structure for ESP32 CAN (extension of candev_t)
 */
typedef struct _esp_can_dev {
    candev_t          candev;       /**< candev base structure   */
    canopt_state_t    state;        /**< current state of device */

    can_frame_t* tx_frame;                           /**< frame in transmission  */
    can_frame_t  rx_frames[ESP_CAN_MAX_RX_FRAMES];   /**< frames received        */
    can_filter_t rx_filters[ESP_CAN_MAX_RX_FILTERS]; /**< acceptance filter list */

    uint32_t rx_frames_wptr;    /**< pointer to ring buffer for write */
    uint32_t rx_frames_rptr;    /**< pointer to ring buffer for read  */
    uint32_t rx_frames_num;     /**< number of frames in ring buffer  */
    uint32_t rx_filter_num;     /**< number of acceptance filters     */

    bool   powered_up;      /**< device is powered up    */

    gpio_t tx_pin;          /**< TX pin */
    gpio_t rx_pin;          /**< RX pin */
    #ifdef CAN_CLK_OUT
    gpio_t clk_out_pin;     /**< optional CLK_OUT pin    */
    #endif
    #ifdef CAN_BUS_ON_OFF
    gpio_t bus_on_off_pin;  /**< optional BUS_ON_OFF pin */
    #endif

    uint32_t events;        /**< events triggered by the last interrupt */


} _esp_can_dev_t;

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
static void _esp_can_get_bittiming_const (struct can_bittiming_const* esp_const);
static void _esp_can_set_bittiming (void);
static void _esp_can_power_up  (void);
static void _esp_can_power_down(void);
static void _esp_can_init_pins (void);
static int  _esp_can_set_mode  (canopt_state_t state);
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


/** ESP32 CAN low level device initialisation, we have only one device */
static _esp_can_dev_t _esp_can_dev = {
    .candev.driver = &_esp_can_driver,
    .candev.state = CAN_STATE_SLEEPING,
    .candev.bittiming.bitrate = CAN_DEFAULT_BITRATE,
    .tx_frame = NULL,
    .rx_frames_wptr = 0,
    .rx_frames_rptr = 0,
    .rx_frames_num = 0,
    .rx_filter_num = 0,
    .state = CANOPT_STATE_OFF,
    .powered_up = false,
    .tx_pin = CAN_TX,
    .rx_pin = CAN_RX,
    #ifdef CAN_CLK_OUT
    .clk_out_pin = CAN_CLK_OUT,
    #endif
    #ifdef CAN_BUS_ON_OFF
    .bus_on_off_pin = CAN_BUS_ON_OFF,
    #endif
};

/** ESP CAN thread stack */
static char _esp_can_stack [ESP_CAN_DEV_STACKSIZE];

/** ESP CAN DLL device */
static candev_dev_t _esp_can_dll_dev;

static void _esp_can_isr(candev_t *candev)
{
    DEBUG("%s\n", __func__);

    CHECK_PARAM(candev == &_esp_can_dev.candev);
    CHECK_PARAM(candev->event_callback != NULL);

    if (_esp_can_dev.events & ESP_CAN_EVENT_BUS_OFF) {
        _esp_can_dev.events &= ~ESP_CAN_EVENT_BUS_OFF;
        _esp_can_dev.candev.event_callback(&_esp_can_dev.candev,
                                           CANDEV_EVENT_BUS_OFF, NULL);
    }

    if (_esp_can_dev.events & ESP_CAN_EVENT_ERROR_PASSIVE) {
        _esp_can_dev.events &= ~ESP_CAN_EVENT_ERROR_PASSIVE;
        _esp_can_dev.candev.event_callback(&_esp_can_dev.candev,
                                           CANDEV_EVENT_ERROR_PASSIVE, NULL);
    }

    if (_esp_can_dev.events & ESP_CAN_EVENT_ERROR_WARNING) {
        _esp_can_dev.events &= ~ESP_CAN_EVENT_ERROR_WARNING;
        _esp_can_dev.candev.event_callback(&_esp_can_dev.candev,
                                           CANDEV_EVENT_ERROR_WARNING, NULL);
    }

    if (_esp_can_dev.events & ESP_CAN_EVENT_TX_ERROR) {
        /* a pending TX confirmation has also to be deleted on TX bus error */
        _esp_can_dev.events &= ~ESP_CAN_EVENT_TX_ERROR;
        _esp_can_dev.events &= ~ESP_CAN_EVENT_TX_CONFIRMATION;
        if (_esp_can_dev.tx_frame) {
            /* handle the event only if there is still a frame in TX buffer */
            _esp_can_dev.candev.event_callback(&_esp_can_dev.candev,
                                               CANDEV_EVENT_TX_ERROR, _esp_can_dev.tx_frame);
            _esp_can_dev.tx_frame = NULL;
        }
    }

    if (_esp_can_dev.events & ESP_CAN_EVENT_TX_CONFIRMATION) {
        _esp_can_dev.events &= ~ESP_CAN_EVENT_TX_CONFIRMATION;
        if (_esp_can_dev.tx_frame) {
            /* handle the event only if there is still a frame in TX buffer */
            _esp_can_dev.candev.event_callback(&_esp_can_dev.candev,
                                               CANDEV_EVENT_TX_CONFIRMATION,
                                               _esp_can_dev.tx_frame);
            _esp_can_dev.tx_frame = NULL;
        }
    }

    if (_esp_can_dev.events & ESP_CAN_EVENT_RX_ERROR) {
        _esp_can_dev.events &= ~ESP_CAN_EVENT_RX_ERROR;
        _esp_can_dev.candev.event_callback(&_esp_can_dev.candev,
                                           CANDEV_EVENT_RX_ERROR, NULL);
    }

    if (_esp_can_dev.events & ESP_CAN_EVENT_RX_INDICATION) {
        _esp_can_dev.events &= ~ESP_CAN_EVENT_RX_INDICATION;

        while (_esp_can_dev.rx_frames_num) {
            _esp_can_dev.candev.event_callback(&_esp_can_dev.candev,
                                               CANDEV_EVENT_RX_INDICATION,
                                               &_esp_can_dev.rx_frames
                                                [_esp_can_dev.rx_frames_rptr]);
            _esp_can_dev.rx_frames_num--;
            _esp_can_dev.rx_frames_rptr++;
            _esp_can_dev.rx_frames_rptr &= ESP_CAN_MAX_RX_FRAMES-1;
        }
    }
}

static int _esp_can_init(candev_t *candev)
{
    DEBUG("%s\n", __func__);

    CHECK_PARAM_RET(candev != NULL, -ENODEV);
    CHECK_PARAM_RET(candev == &_esp_can_dev.candev, -ENODEV);

    /* initialize used GPIOs */
    _esp_can_init_pins();

    /* power up and configure the CAN controller */
    _esp_can_power_up();

    return 0;
}

static int _esp_can_send(candev_t *candev, const struct can_frame *frame)
{
    DEBUG("%s\n", __func__);

    CHECK_PARAM_RET(candev == &_esp_can_dev.candev, -ENODEV);
    CHECK_PARAM_RET(frame  != NULL, -EINVAL);

    critical_enter();

    /* check wthere the device is already transmitting a frame */
    if (_esp_can_dev.tx_frame != NULL) {
        critical_exit();
        return -EBUSY;
    }

    /* save reference to frame in transmission (marks transmitter as busy) */
    _esp_can_dev.tx_frame = (struct can_frame*)frame;

    /* prepare the frame as exected by ESP32 */
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
    CHECK_PARAM_RET(candev == &_esp_can_dev.candev, -ENODEV);
    CHECK_PARAM_RET(value != NULL, -EINVAL);

    int res = 0;

    switch (opt) {
        case CANOPT_BITTIMING:
            DEBUG("%s CANOPT_BITTIMING\n", __func__);
            if (value_len < sizeof(struct can_bittiming)) {
                DEBUG("%s size error\n", __func__);
                return -EOVERFLOW;
            }
            _esp_can_dev.candev.bittiming = *((struct can_bittiming*)value);
            _esp_can_set_bittiming();

            res = sizeof(struct can_bittiming);
            break;

        case CANOPT_STATE:
            DEBUG("%s CANOPT_STATE\n", __func__);
            res = _esp_can_set_mode(*((canopt_state_t *)value));
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
    DEBUG("%s\n", __func__);

    CHECK_PARAM_RET(candev == &_esp_can_dev.candev, -ENODEV);
    CHECK_PARAM_RET(value != NULL, -EINVAL);

    /* _esp_can_dev_t *dev = (_esp_can_dev_t *)candev; */

    int res = 0;
    switch (opt) {
        case CANOPT_BITTIMING:
            DEBUG("%s CANOPT_BITTIMING\n", __func__);
            if (max_len < sizeof(struct can_bittiming)) {
                res = -EOVERFLOW;
                break;
            }

            *((struct can_bittiming*)value) = _esp_can_dev.candev.bittiming;

            res = sizeof(struct can_bittiming);
            break;

        case CANOPT_BITTIMING_CONST:
            DEBUG("%s CANOPT_BITTIMING_CONST\n", __func__);
            if (max_len < sizeof(struct can_bittiming_const)) {
                res = -EOVERFLOW;
                break;
            }

            _esp_can_get_bittiming_const((struct can_bittiming_const*)value);

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
                if (_esp_can_dev.rx_filters[i].can_id != 0) {
                    list[i] = _esp_can_dev.rx_filters[i];
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
    DEBUG("%s\n", __func__);

    CHECK_PARAM_RET(candev == &_esp_can_dev.candev, -ENODEV);
    CHECK_PARAM_RET(frame != NULL, -EINVAL);

    /* abort transmission command */
    CAN.command_reg.val = ESP_CMD_ABORT_TX;

    /* mark the transmitter as free */
    _esp_can_dev.tx_frame = NULL;

    return -ENOTSUP;
}

static int _esp_can_set_filter(candev_t *candev, const struct can_filter *filter)
{
    DEBUG("%s\n", __func__);
    CHECK_PARAM_RET(candev == &_esp_can_dev.candev, -ENODEV);
    CHECK_PARAM_RET(filter != NULL, -EINVAL);

    int i;

    /* first, check whether filter is already set */
    for (i = 0; i < ESP_CAN_MAX_RX_FILTERS; i++) {
        if (_esp_can_dev.rx_filters[i].can_id == filter->can_id) {
            DEBUG("%s filter already set\n", __func__);
            return i;
        }
    }

    /* next, search for free filter entry */
    for (i = 0; i < ESP_CAN_MAX_RX_FILTERS; i++) {
        if (_esp_can_dev.rx_filters[i].can_id == 0) {
            break;
        }
    }

    if (i == ESP_CAN_MAX_RX_FILTERS) {
        DEBUG("%s no more filters available\n", __func__);
        return -EOVERFLOW;
    }

    /* set the filter and return the filter index */
    _esp_can_dev.rx_filters[i] = *filter;
    _esp_can_dev.rx_filter_num++;
    return i;
}

static int _esp_can_remove_filter(candev_t *candev, const struct can_filter *filter)
{
    DEBUG("%s\n", __func__);

    /* search for the filter */
    for (unsigned i = 0; i < ESP_CAN_MAX_RX_FILTERS; i++) {
        if (_esp_can_dev.rx_filters[i].can_id == filter->can_id) {
            /* mark the filter entry as not in use */
            _esp_can_dev.rx_filters[i].can_id = 0;
            _esp_can_dev.rx_filter_num--;
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

static void _esp_can_set_reset_mode (void)
{
    DEBUG("%s\n", __func__);

    while (CAN.mode_reg.reset != 1) {
        CAN.mode_reg.reset = 1;
    }
}

static void _esp_can_set_operating_mode (void)
{
    DEBUG("%s\n", __func__);

    while (CAN.mode_reg.reset != 0) {
        CAN.mode_reg.reset = 0;
    }
}

static int _esp_can_config(void)
{
    DEBUG("%s\n", __func__);

    critical_enter();

    /* reset mode to be able to write configuration registers */
    _esp_can_set_reset_mode();

    /* configuration is done in listen only mode */
    CAN.mode_reg.self_test = 0;
    CAN.mode_reg.listen_only = 1;

    /* Use SJA1000 PeliCAN mode and registers layout */
    CAN.clock_divider_reg.can_mode = 1;

    #ifndef CAN_CLK_OUT
    CAN.clock_divider_reg.clock_off = 1;
    CAN.clock_divider_reg.clock_divider = 0;
    #else
    uint32_t clk_out_div = CAN_CLK_OUT_DIV / 2 - 1;
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
    xt_set_interrupt_handler(CPU_INUM_CAN, _esp_can_intr_handler, NULL);
    xt_ints_on(BIT(CPU_INUM_CAN));

    /* set bittiming from parameters as given in device data */
    _esp_can_set_bittiming();

    /* switch to operating mode */
    _esp_can_set_operating_mode();

    critical_exit();

    return 0;
}

static void _esp_can_power_up(void)
{
    /* just return when already powered up */
    if (_esp_can_dev.powered_up) {
        return;
    }

    DEBUG("%s\n", __func__);

    critical_enter();

    /* power up and (re)configure the CAN controller */
    periph_module_enable(PERIPH_CAN_MODULE);
    _esp_can_dev.powered_up = true;
    _esp_can_config ();

    critical_exit();
}

static void _esp_can_power_down(void)
{
    /* just return when already powered down */
    if (!_esp_can_dev.powered_up) {
        return;
    }

    DEBUG("%s\n", __func__);

    /* power down the CAN controller */
    periph_module_disable(PERIPH_CAN_MODULE);
    _esp_can_dev.powered_up = false;
}

static void _esp_can_init_pins(void)
{
    DEBUG("%s\n", __func__);

    /* Init TX pin */
    gpio_init(_esp_can_dev.tx_pin, GPIO_OUT);
    gpio_set_pin_usage(_esp_can_dev.tx_pin, _CAN);
    GPIO.func_out_sel_cfg[_esp_can_dev.tx_pin].func_sel = CAN_TX_IDX;

    /* Init RX pin */
    gpio_init(_esp_can_dev.rx_pin, GPIO_IN);
    gpio_set_pin_usage(_esp_can_dev.rx_pin, _CAN);
    GPIO.func_in_sel_cfg[CAN_RX_IDX].sig_in_sel = 1;
    GPIO.func_in_sel_cfg[CAN_RX_IDX].sig_in_inv = 0;
    GPIO.func_in_sel_cfg[CAN_RX_IDX].func_sel = _esp_can_dev.rx_pin;

    #ifdef CAN_CLK_OUT
    /* Init CLK_OUT pin (optional) if defined */
    gpio_init(_esp_can_dev.clk_out_pin, GPIO_OD);
    gpio_set_pin_usage(_esp_can_dev.clk_out_pin, _CAN);
    GPIO.func_out_sel_cfg[_esp_can_dev.clk_out_pin].func_sel = CAN_CLKOUT_IDX;
    #endif

    /* Init BUS_ON_OFF pin pin (optional) if defined */
    #ifdef CAN_BUS_ON_OFF
    gpio_init(_esp_can_dev.bus_on_of_pin, GPIO_OD);
    gpio_set_pin_usage(_esp_can_dev.bus_on_of_pin, _CAN);
    GPIO.func_out_sel_cfg[_esp_can_dev.bus_on_of_pin].func_sel = CAN_BUS_OFF_ON_IDX;
    #endif
}

static int _esp_can_set_mode(canopt_state_t state)
{
    DEBUG("%s %d\n", __func__, state);

    critical_enter();

    switch (state) {
        case CANOPT_STATE_OFF:
            /* just power down the CAN controller */
            _esp_can_power_down();
            break;

        case CANOPT_STATE_LISTEN_ONLY:
            /* power up and (re)configure the CAN controller if necessary */
            _esp_can_power_up();
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
            _esp_can_power_up();
            /* set the sleep mode (not necessary to set reset mode before) */
            CAN.mode_reg.sleep_mode = 1;
            break;
            #endif

        case CANOPT_STATE_ON:
            /* power up and (re)configure the CAN controller if necessary */
            _esp_can_power_up();
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
    _esp_can_dev.state = state;

    critical_exit();

    return 0;
}

static void IRAM_ATTR _esp_can_intr_handler (void *arg)
{
    critical_enter();

    /* read the registers to clear them */
    can_status_reg_t sta_reg = CAN.status_reg;
    can_intr_reg_t   int_reg = CAN.interrupt_reg;

    DEBUG("%s int=%08x sta=%08x\n", __func__, int_reg.val, sta_reg.val);

    /* Wake-Up Interrupt (not supported by ESP32) */
    if (int_reg.reserved1) {
        DEBUG("%s wake-up interrupt\n", __func__);
        _esp_can_dev.events |= ESP_CAN_EVENT_WAKE_UP;
    }

    /* Arbitration Lost Interrupt */
    if (int_reg.arb_lost) {
        DEBUG("%s arbitration lost interrupt\n", __func__);
        /* can only happen during transmission, handle it as error in single shot transmission */
        _esp_can_dev.events |= ESP_CAN_EVENT_TX_ERROR;
    }

    /* bus or error status has changed, handle this first */
    if (int_reg.err_warn) {

        /* BUS_OFF state condition */
        if (sta_reg.error && sta_reg.bus) {
            DEBUG("%s bus-off state interrupt\n", __func__);
            /* switch to listen only mode to freeze the RX error counter */
            _esp_can_set_mode (CANOPT_STATE_LISTEN_ONLY);
            /* save the event */
            _esp_can_dev.events |= ESP_CAN_EVENT_BUS_OFF;
        }

        /* ERROR_WARNING state condition, RX/TX error counter are > 96 */
        else if (sta_reg.error && !sta_reg.bus) {
            DEBUG("%s error warning interrupt\n", __func__);
            /* save the event */
            _esp_can_dev.events |= ESP_CAN_EVENT_ERROR_WARNING;
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
            _esp_can_dev.events |= ESP_CAN_EVENT_ERROR_PASSIVE;
    }

    /*
     * Bus Error Interrupt (bit, stuff, crc, form, ack), details are captured
     * in ECC register (see SJA1000 Data sheet, Table 20 and 21)
     */

    if (int_reg.bus_err) {
        can_err_code_cap_reg_t ecc = CAN.error_code_capture_reg;
        /* save the event */
        DEBUG("%s bus error interrupt, ecc=%08x\n", __func__, ecc.val);
        _esp_can_dev.events |= ecc.direction ? ESP_CAN_EVENT_RX_ERROR
                                             : ESP_CAN_EVENT_TX_ERROR;
    }

    /* TX buffer becomes free */
    if (int_reg.tx) {
        DEBUG("%s transmit interrupt\n", __func__);
        /* save the event */
        _esp_can_dev.events |= ESP_CAN_EVENT_TX_CONFIRMATION;
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

            if (_esp_can_dev.rx_frames_num < ESP_CAN_MAX_RX_FRAMES) {
                /* prepare the CAN frame from ESP32 CAN frame */
                can_frame_t frame = {};

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
                if (_esp_can_dev.rx_filter_num) {
                    for (f_id = 0; f_id < ESP_CAN_MAX_RX_FILTERS; f_id++) {
                        /* compared masked can_id with each filter */
                        struct can_filter* f = &_esp_can_dev.rx_filters[f_id];
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
                    _esp_can_dev.rx_frames[_esp_can_dev.rx_frames_wptr] = frame;

                    _esp_can_dev.rx_frames_num++;
                    _esp_can_dev.rx_frames_wptr++;
                    _esp_can_dev.rx_frames_wptr &= ESP_CAN_MAX_RX_FRAMES-1;
                }
            }
            else {
                DEBUG("%s receive buffer overrun\n", __func__);
                /* we use rx error since there is no separate overrun error */
                _esp_can_dev.events |= ESP_CAN_EVENT_RX_ERROR;
            }

        }
        /* save the event */
        _esp_can_dev.events |= ESP_CAN_EVENT_RX_INDICATION;
    }

    /* data overrun interrupts are not handled */
    if (int_reg.data_overrun) {
        DEBUG("%s data overrun interrupt\n", __func__);
        /* we use rx error since there is no separate overrun error */
        _esp_can_dev.events |= ESP_CAN_EVENT_RX_INDICATION;
    }

    DEBUG("%s events=%08x\n", __func__, _esp_can_dev.events);

    /* inform the upper layer that there are events to be handled */
    if (_esp_can_dev.events && _esp_can_dev.candev.event_callback) {
        _esp_can_dev.candev.event_callback(&_esp_can_dev.candev, CANDEV_EVENT_ISR, NULL);
    }

    critical_exit();
}


static void _esp_can_set_bittiming (void)
{
    struct can_bittiming* timing = &_esp_can_dev.candev.bittiming;

    DEBUG("%s bitrate=%d, brp=%d, sample_point=%d, tq=%d, "
          "prop_seg=%d phase_seg1=%d, phase_seg2=%d, sjw =%d\n", __func__,
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


/** returns the hardware dependent constants used for bit timing calculations */
static void _esp_can_get_bittiming_const (struct can_bittiming_const* timing_const)
{
    CHECK_PARAM(timing_const != NULL);

    timing_const->tseg1_min = 1;
    timing_const->tseg1_max = 16;
    timing_const->tseg2_min = 1;
    timing_const->tseg2_max = 8;
    timing_const->sjw_max = 4;
    timing_const->brp_min = 2;
    timing_const->brp_max = 128;
    timing_const->brp_inc = 2;
}


void auto_init_esp_can(void)
{
    struct can_bittiming_const timing_const;

    DEBUG("%s\n", __func__);

    /* determine the hardware bittiming constants */
    _esp_can_get_bittiming_const(&timing_const);

    /* calculate the initial bittimings from the bittiming constants */
    can_device_calc_bittiming (ESP_CAN_CLOCK, &timing_const, &_esp_can_dev.candev.bittiming);

    _esp_can_dll_dev.dev = (candev_t*)&_esp_can_dev;
    _esp_can_dll_dev.name = "can";

    #ifdef MODULE_CAN_TRX
    _esp_can_dll_dev.trx = 0;
    #endif
    #ifdef MODULE_CAN_PM
    _esp_can_dll_dev.rx_inactivity_timeout = 0;
    _esp_can_dll_dev.tx_wakeup_timeout = 0;
    #endif

    can_device_init(_esp_can_stack, ESP_CAN_DEV_STACKSIZE, ESP_CAN_DEV_BASE_PRIORITY,
                    "esp_can", &_esp_can_dll_dev);
}

#endif /* MODULE_ESP_CAN */

void can_print_config(void)
{
    #ifdef MODULE_ESP_CAN
    ets_printf("\tCAN_DEV(0)\ttxd=%d rxd=%d\n", CAN_TX, CAN_RX);
    #endif
}

/**@}*/

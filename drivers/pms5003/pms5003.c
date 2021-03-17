/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_pms5003
 * @{
 *
 * @file
 * @brief       Device driver implementation for the sensors
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

#include <string.h>

#include "byteorder.h"
#include "pms5003.h"
#include "pms5003_params.h"
#include "pms5003_constants.h"

#include "ztimer.h"
#include "timex.h"
#include "mutex.h"

#define ENABLE_DEBUG    0
#include "debug.h"

#define PMS5003_START_BYTE_1            0x42
#define PMS5003_START_BYTE_2            0x4D

/* start, length, data and crc index in be_uint16_t parser buffer*/
#define PMS5003_PARSER_START_IDX         0U
#define PMS5003_PARSER_LENGTH_IDX        1U
#define PMS5003_PARSER_DATA_IDX          2U
#define PMS5003_PARSER_RESP_CRC_IDX      3U
#define PMS5003_PARSER_PM_CRC_IDX        15U

/* the response timeout is based on 2.3 seconds being the maximum time
   between measurements */
#define PMS5003_RESP_TIMEOUT        (3 * US_PER_SEC)
/* this is not documented in the datasheet but after a wakeup the device
   is not fully available before 3 seconds */
#define PMS5003_WAKEUP_TIME         (3 * US_PER_SEC)
/* this is not documented in the datasheet but commands can't be sent out
   within a short time of each other */
#define PMS5003_CMD_BUSY_TIME       (50 * US_PER_MS)

/**
 * @name      UART commands for pms5003 driver
 * @{
 */
typedef enum {
    PMS5003_CMD_PASSIVE_READ    = 0xe2, /**< Perform a passive read */
    PMS5003_CMD_SET_MODE        = 0xe1, /**< Set operating mode */
    PMS5003_CMD_SET_SLEEP       = 0xe4, /**< Set sleep state*/
} pms5003_cmd_t;

/**
 * @brief   Reponse to PMS5003_CMD_SET_MODE active
 */
#define PMS5003_CMD_ACTIVE_RESPONSE         0x01E1
/**
 * @brief   Reponse to PMS5003_CMD_SET_MODE passive
 */
#define PMS5003_CMD_PASSIVE_RESPONSE        0x00E1
/**
 * @brief   Reponse to PMS5003_CMD_SET_SLEEP sleep (wakeup does nto issue
 *          a response)
 */
#define PMS5003_CMD_SLEEP_RESPONSE          0x00E4

/**
 * @brief   Frame parser state
 *
 * The different states match the index of the data in the received frame.
 */
typedef enum {
    PMS5003_STATE_START_BYTE_1  = 0,                            /**< Waiting for first start byte */
    PMS5003_STATE_START_BYTE_2,                                 /**< Waiting for second start byte */
    PMS5003_STATE_FRAME_LENGTH_H,                               /**< Higher byte of frame length */
    PMS5003_STATE_FRAME_LENGTH_L,                               /**< Lower lower byte of frame length*/
    PMS5003_STATE_RESP_CRC_L    = PMS5003_RESP_FRAME_LEN - 1,   /**< Last byte of CMD response, lower crc byte */
    PMS5003_STATE_PM_CRC_L      = PMS5003_PM_FRAME_LEN - 1,     /**< Last byte of PM data, lower crc byte */
} pms5003_parser_state_t;

static int _wait_for_resp(pms5003_t *dev)
{
    /* acquire lock that will be freed after a uart cmd response is
       received */
    mutex_lock(&dev->_timeout);
    DEBUG_PUTS("[pms5003]: response wait");
    /* try to acquire lock with a timeout incase no response is received */
    int ret = ztimer_mutex_lock_timeout(ZTIMER_USEC, &dev->_timeout, PMS5003_RESP_TIMEOUT);
    if (ret) {
        DEBUG_PUTS("[pms5003]: response timeout");
    }
    /* unlock mutex, always needs unlocking, either failed to acquire above
       or succeeded and now needs unlocking */
    mutex_unlock(&dev->_timeout);
    return ret;
}

static void _cb_busy(void *arg)
{
    /* unlock busy mutex after elapsed time */
    DEBUG_PUTS("[pms5003]: not busy");
    mutex_unlock((mutex_t *)arg);
}

static void _set_busy(pms5003_t *dev, uint32_t time)
{
    /* lock the _busy mutex and set a timer to unlock it after 'time' */
    mutex_lock(&dev->_busy);
    DEBUG_PUTS("[pms5003]: busy");
    ztimer_set(ZTIMER_USEC, &dev->_busy_timer, time);
}

int _write_cmd(pms5003_t *dev, pms5003_cmd_t cmd, uint16_t data)
{
    uint8_t buf[PMS5003_CMD_FRAME_LEN];

    buf[0] = PMS5003_START_BYTE_1;
    buf[1] = PMS5003_START_BYTE_2;
    buf[2] = cmd;
    buf[3] = (uint8_t)(data >> 8);
    buf[4] = (uint8_t)data;
    /* calculate crc */
    uint16_t crc = 0;
    for (uint8_t i = 0; i < 5; i++) {
        crc += buf[i];
    }
    buf[5] = (uint8_t)(crc >> 8);
    buf[6] = (uint8_t)crc;
    /* set busy to avoid sendinf commands before PMS5003_CMD_BUSY_TIME of
       a previously issued command */
    _set_busy(dev, PMS5003_CMD_BUSY_TIME);
    /* write cmd */
    uart_write(dev->params.uart, buf, sizeof(buf));
    /* wait for response or timeout */
    return _wait_for_resp(dev);
}

void _load_frame(pms5003_t *dev)
{
    /* load valid data */
    for (uint8_t i = 0; i < sizeof(dev->data.pm.buffer) / 2; i++) {
        dev->data.pm.buffer[i] = byteorder_ntohs(dev->_parser.buf.be[PMS5003_PARSER_DATA_IDX + i]);
    }
    /* timestamp data */
    dev->data.timestamp = ztimer_now(ZTIMER_USEC);
    DEBUG("[pms5003]: new valid data at %" PRIu32 "\n", dev->data.timestamp);
    /* execute callback, can be used in active mode */
    if (dev->callback && dev->mode == PMS5003_ACTIVE_MODE) {
        dev->callback(dev->arg);
    }
    else {
        DEBUG_PUTS("[pms5003]: no callback");
    }
}

int _parse_frame_content(pms5003_t *dev)
{
    int ret = -1;

    switch (dev->state) {
    case PMS5003_STATE_MODE_ACTIVE_REQUEST:
        if (dev->_parser.buf.be[PMS5003_PARSER_DATA_IDX].u16 == PMS5003_CMD_ACTIVE_RESPONSE) {
            ret = 0;
            dev->state = PMS5003_STATE_READ_REQUEST;
            mutex_unlock(&dev->_timeout);
        }
        break;
    case PMS5003_STATE_MODE_PASSIVE_REQUEST:
        if (dev->_parser.buf.be[PMS5003_PARSER_DATA_IDX].u16 == PMS5003_CMD_PASSIVE_RESPONSE) {
            ret = 0;
            dev->state = PMS5003_STATE_IDLE;
            mutex_unlock(&dev->_timeout);
        }
        break;
    case PMS5003_STATE_SLEEP_REQUEST:
        if (dev->_parser.buf.be[PMS5003_PARSER_DATA_IDX].u16 == PMS5003_CMD_SLEEP_RESPONSE) {
            ret = 0;
            dev->state = PMS5003_STATE_SLEEP;
            mutex_unlock(&dev->_timeout);
        }
        break;
    case PMS5003_STATE_READ_REQUEST:
        if (dev->mode == PMS5003_PASSIVE_MODE) {
            dev->state = PMS5003_STATE_IDLE;
        }
        ret = 0;
        mutex_unlock(&dev->_timeout);
        _load_frame(dev);
    default:
        break;
    }
    return ret;
}

int _verify_frame(pms5003_t *dev)
{
    uint16_t crc = 0;
    uint16_t frame_crc = 0;
    int8_t len;

    if (dev->_parser.idx == PMS5003_RESP_FRAME_LEN - 1) {
        len = PMS5003_RESP_FRAME_LEN - PMS5003_CRC_LEN;
        frame_crc = byteorder_ntohs(dev->_parser.buf.be[PMS5003_PARSER_RESP_CRC_IDX]);
    }
    else {
        len = PMS5003_PM_FRAME_LEN - PMS5003_CRC_LEN;
        frame_crc = byteorder_ntohs(dev->_parser.buf.be[PMS5003_PARSER_PM_CRC_IDX]);
    }
    for (uint8_t i = 0; i < len; i++) {
        crc += dev->_parser.buf.bytes[i];
    }
    if (crc == frame_crc) {
        DEBUG_PUTS("[pms5003]: valid crc")
        return _parse_frame_content(dev);
    }
    DEBUG("[pms5003]: invalid crc %04x !+ %04x\n", frame_crc, crc);
    return 0;
}

void _rx_cb(void *arg, uint8_t data)
{
    /* the parsing state machine */
    pms5003_t *dev = (pms5003_t *)arg;

    switch (dev->_parser.idx) {
    case PMS5003_STATE_START_BYTE_1:
        if (data == PMS5003_START_BYTE_1) {
            dev->_parser.buf.bytes[dev->_parser.idx] = data;
            dev->_parser.idx++;
        }
        break;
    case PMS5003_STATE_START_BYTE_2:
        if (data == PMS5003_START_BYTE_2) {
            dev->_parser.buf.bytes[dev->_parser.idx] = data;
            dev->_parser.idx++;
        }
        break;
    case PMS5003_STATE_RESP_CRC_L:
        dev->_parser.buf.bytes[dev->_parser.idx] = data;
        /* if frame length is of a reponse then check crc */
        uint8_t len = byteorder_ntohs(dev->_parser.buf.be[PMS5003_PARSER_LENGTH_IDX]);
        if (len == PMS5003_RESP_FRAME_LEN - PMS5003_START_BYTES_LEN - PMS5003_FRAME_LENGTH_LEN) {
            _verify_frame(dev);
            /* reset state machine */
            dev->_parser.idx = 0;
            break;
        }
        dev->_parser.idx++;
        break;
    case PMS5003_STATE_PM_CRC_L:
        dev->_parser.buf.bytes[dev->_parser.idx] = data;
        _verify_frame(dev);
        /* reset state machine */
        dev->_parser.idx = 0;
        break;
    default:
        if (dev->_parser.idx >= PMS5003_PM_FRAME_LEN) {
            /* invalid idx, reset state machine */
            dev->_parser.idx = 0;
            break;
        }
        /* currently parsing data, store and move one */
        dev->_parser.buf.bytes[dev->_parser.idx] = data;
        dev->_parser.idx++;
        break;
    }
}

void pms5003_reset(pms5003_t *dev)
{
    gpio_clear(dev->params.reset_pin);
    ztimer_sleep(ZTIMER_USEC, 100 * US_PER_MS);
    gpio_set(dev->params.reset_pin);
    /* wait for wakeup */
    _set_busy(dev, PMS5003_WAKEUP_TIME);
    dev->state = PMS5003_STATE_IDLE;
}

int pms5003_init(pms5003_t *dev, const pms5003_params_t *params)
{
    assert(dev && params);
    memset(&dev->_parser, 0, sizeof(pms5003_parser_t));
    dev->callback = NULL;
    dev->params = *params;

    mutex_init(&dev->_busy);
    mutex_init(&dev->_timeout);
    dev->_busy_timer.callback = _cb_busy;
    dev->_busy_timer.arg = &dev->_busy;

    DEBUG_PUTS("[pms5003]: init uart");
    int ret = uart_init(dev->params.uart, params->baudrate, _rx_cb, dev);
    if (ret != UART_OK) {
        DEBUG_PUTS("[pms5003]: failed to init uart");
        return -1;
    }

    DEBUG_PUTS("[pms5003]: init gpio");
    if (gpio_is_valid(dev->params.reset_pin)) {
        gpio_init(dev->params.reset_pin, GPIO_OUT);
    }
    else {
        DEBUG_PUTS("[pms5003]: invalid reset gpio");
        return -1;
    }
    if (gpio_is_valid(dev->params.enable_pin)) {
        gpio_init(dev->params.enable_pin, GPIO_OUT);
        gpio_set(dev->params.enable_pin);
    }

    DEBUG_PUTS("[pms5003]: reset... ");
    pms5003_reset(dev);

    DEBUG_PUTS("[pms5003]: set passive mode... ");
    pms5003_set_mode(dev, PMS5003_PASSIVE_MODE);

    return 0;
}

void pms5003_set_callback(pms5003_t *dev, pms5003_callback_t callback, void *arg)
{
    dev->arg = arg;
    dev->callback = callback;
}

int pms5003_set_mode(pms5003_t *dev, pms5003_mode_t mode)
{
    dev->state = mode ? PMS5003_STATE_MODE_ACTIVE_REQUEST : PMS5003_STATE_MODE_PASSIVE_REQUEST;
    if (!_write_cmd(dev, PMS5003_CMD_SET_MODE, mode)) {
        dev->mode = mode;
        return 0;
    }
    else {
        return -1;
    }
}

pms5003_mode_t pms5003_get_mode(pms5003_t *dev)
{
    return dev->mode;
}

int pms5003_set_sleep(pms5003_t *dev, pms5003_sleep_t sleep)
{
    dev->state = sleep ? PMS5003_STATE_WAKEUP_REQUEST : PMS5003_STATE_SLEEP_REQUEST;
    if (_write_cmd(dev, PMS5003_CMD_SET_SLEEP, sleep)) {
        return -1;
    }
    if (sleep == PMS5003_SLEEP) {
        if (dev->params.enable_pin != GPIO_UNDEF) {
            gpio_clear(dev->params.enable_pin);
        }
    }
    else {
        if (dev->params.enable_pin != GPIO_UNDEF) {
            gpio_set(dev->params.enable_pin);
        }
        /* wait for wakeup */
        _set_busy(dev, PMS5003_WAKEUP_TIME);
    }
    return 0;
}

pms5003_sleep_t pms5003_get_sleep(pms5003_t *dev)
{
    return dev->state == PMS5003_STATE_SLEEP ? PMS5003_SLEEP : PMS5003_RUNNING;
}

int _req_read(pms5003_t *dev)
{
    /* data field is ignore for PMS5003_CMD_PASSIVE_READ */
    return _write_cmd(dev, PMS5003_CMD_PASSIVE_READ, 0x0000);
}

int pms5003_read_measurement(pms5003_t *dev, pms5003_data_t *data)
{
    dev->state = PMS5003_STATE_READ_REQUEST;
    if (dev->mode == PMS5003_PASSIVE_MODE) {
        DEBUG_PUTS("[pms5003]: passive read request");
        if (_req_read(dev)) {
            DEBUG_PUTS("[pms5003]: failed to read");
            return -1;
        }
    }
    memcpy(data, &dev->data, sizeof(pms5003_data_t));
    return 0;
}

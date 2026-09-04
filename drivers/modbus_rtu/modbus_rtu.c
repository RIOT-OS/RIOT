/*
 * SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_modbus_rtu
 * @brief       Modbus RTU
 *
 * @{
 * @file
 * @brief       Modbus RTU transport implementation
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 * @}
 */

#include <assert.h>
#include <errno.h>

#include "modbus.h"
#include "modbus_rtu.h"

#include "periph/gpio.h"
#include "periph/uart.h"

#include "isrpipe.h"
#include "isrpipe/read_timeout.h"

#include "checksum/ucrc16.h"
#include "time_units.h"
#include "ztimer.h"

#define ENABLE_DEBUG 0
#define ENABLE_TRACE 0
#include "debug.h"

#if ENABLE_TRACE
#  define TRACE(...) DEBUG(__VA_ARGS__)
#  define TRACE_PUTS(...) DEBUG_PUTS(__VA_ARGS__)
#else
#  define TRACE(...)
#  define TRACE_PUTS(...)
#endif

static inline void _enable_rts(const modbus_rtu_params_t *params)
{
    TRACE_PUTS("[modbus_rtu] _enable_rts");

    if (gpio_is_valid(params->pin_rts)) {
        gpio_write(params->pin_rts, params->pin_rts_enable);
    }
}

static inline void _disable_rts(const modbus_rtu_params_t *params)
{
    TRACE_PUTS("[modbus_rtu] _disable_rts");

    if (gpio_is_valid(params->pin_rts)) {
        gpio_write(params->pin_rts, !params->pin_rts_enable);
    }
}

static void _reset_idle(modbus_rtu_t *dev)
{
    mutex_trylock(&dev->idle_lock);
    ztimer_set(ZTIMER_USEC, &(dev->idle_timer), dev->frame_timeout);
}

static int _wait_for_idle(modbus_rtu_t *dev, uint32_t timeout)
{
    if (timeout > 0) {
        if (ztimer_mutex_lock_timeout(ZTIMER_USEC, &(dev->idle_lock), timeout) == -ECANCELED) {
            TRACE_PUTS("[modbus_rtu] _wait_for_idle: bus not idle");
            return -EBUSY;
        }
    }
    else {
        mutex_lock(&(dev->idle_lock));
    }

    /* immediately unlock, the mutex is only used for synchronisation */
    mutex_unlock(&(dev->idle_lock));

    return 0;
}

static void _timer_cb(void *arg)
{
    modbus_rtu_t *dev = (modbus_rtu_t *)arg;

    /* the bus has been idle for the inter-frame timeout: discard any data
     * that has not been consumed by a receive call, so that the next receive
     * call starts aligned at a frame boundary (a receive call normally
     * consumes bytes while the frame is still arriving, so this only drops
     * frames nobody was listening for) */
    isrpipe_clear(&dev->isrpipe);
    mutex_unlock(&dev->idle_lock);
}

static void _uart_cb(void *arg, uint8_t data)
{
    modbus_rtu_t *dev = (modbus_rtu_t *)arg;

    TRACE("[modbus_rtu] _uart_cb: 0x%02x\n", data);

    /* mark bus as busy */
    _reset_idle(dev);

    if (isrpipe_write_one(&dev->isrpipe, data) != 0) {
        DEBUG_PUTS("[modbus_rtu] _uart_cb: isrpipe buffer full");
    }
}

static int modbus_rtu_recv(modbus_t *modbus, modbus_message_t *message, bool request)
{
    modbus_rtu_t *dev = (modbus_rtu_t *)modbus->dev;
    const modbus_rtu_params_t *params = (const modbus_rtu_params_t *)modbus->params;

    DEBUG("[modbus_rtu] modbus_rtu_recv: request=%d\n", request);

    /* wait for the first byte of the frame (the address byte) */
    if (request) {
        /* for requests, wait for the bus to become idle first */
        _wait_for_idle(dev, 0);

        /* isrpipe_read blocks until at least one byte is available */
        isrpipe_read(&dev->isrpipe, &dev->buf[0], 1);
    }
    else {
        /* for responses, do NOT wait for idle or clear the buffer, the
         * response may have already started arriving */
        if (isrpipe_read_timeout(&dev->isrpipe, &dev->buf[0], 1,
                                 params->response_timeout) == -ETIMEDOUT) {
            return -ETIMEDOUT;
        }
    }

    size_t i = 1;

    /* read bytes until the inter-frame idle gap is observed (per spec, an idle
     * gap of 3.5 character times marks end-of-frame) */
    while (i < sizeof(dev->buf)) {
        uint8_t byte;

        if (isrpipe_read_timeout(&dev->isrpipe, &byte, 1, dev->byte_timeout) == -ETIMEDOUT) {
            /* timeout: end of frame */
            break;
        }

        dev->buf[i++] = byte;
    }

    /* a complete ADU consists of address(1) + pdu(>=1) + crc(2) */
    if (i < (MODBUS_RTU_ADU_ID_SIZE + MODBUS_PDU_MIN_REQUEST_SIZE + MODBUS_RTU_ADU_CRC_SIZE)) {
        DEBUG("[modbus_rtu] modbus_rtu_recv: ADU too short (%" PRIuSIZE ")\n", i);
        return -EBADMSG;
    }

    /* verify the CRC (transmitted low byte first) */
    size_t size = i - MODBUS_RTU_ADU_CRC_SIZE;
    uint16_t expected = dev->buf[size] | (dev->buf[size + 1] << 8);

    uint16_t actual = ucrc16_calc_le(dev->buf, size, 0xa001, 0xffff);

    if (actual != expected) {
        DEBUG("[modbus_rtu] modbus_rtu_recv: CRC error (expected=0x%04x, actual=0x%04x)\n",
              expected, actual);
        return -EBADMSG;
    }

    /* hand over the PDU and address to the caller */
    message->id = dev->buf[0];
    modbus->buffer_size = size - MODBUS_RTU_ADU_ID_SIZE;

    return 0;
}

static int modbus_rtu_send(modbus_t *modbus, const modbus_message_t *message)
{
    modbus_rtu_t *dev = (modbus_rtu_t *)modbus->dev;
    const modbus_rtu_params_t *params = (const modbus_rtu_params_t *)modbus->params;

    DEBUG("[modbus_rtu] modbus_rtu_send: size=%" PRIuSIZE "\n", modbus->buffer_size);

    /* compute frame checksum over [id][PDU] */
    uint16_t crc = 0xffff;

    crc = ucrc16_calc_le(&(message->id), sizeof(message->id), 0xa001, crc);
    crc = ucrc16_calc_le(modbus->buffer, modbus->buffer_size, 0xa001, crc);

    /* wait for the bus to become idle: the wait is bounded by the inter-frame
     * timeout, so sending fails fast with -EBUSY when another frame is
     * (still) in flight, rather than waiting for it to complete */
    int res = _wait_for_idle(dev, dev->frame_timeout);

    if (res != 0) {
        DEBUG("[modbus_rtu] modbus_rtu_send: bus not idle\n");
        return res;
    }

    /* Modbus RTU transmits the CRC low byte first, independent of host
     * endianness */
    uint8_t crc_bytes[2] = { crc & 0xff, (crc >> 8) & 0xff };

    _enable_rts(params);
    uart_write(params->uart, &(message->id), sizeof(message->id));
    uart_write(params->uart, modbus->buffer, modbus->buffer_size);
    uart_write(params->uart, crc_bytes, sizeof(crc_bytes));
    _disable_rts(params);

    /* mark bus as busy until next inter-frame silence */
    _reset_idle(dev);

    return 0;
}

static int modbus_rtu_init(modbus_t *modbus)
{
    int res;

    modbus_rtu_t *dev = (modbus_rtu_t *)modbus->dev;
    const modbus_rtu_params_t *params = (const modbus_rtu_params_t *)modbus->params;

    /* pre-compute timeouts */
    assert(params != NULL);
    assert(params->baudrate > 0);

    uint32_t char_time = (US_PER_SEC * 10U) / params->baudrate; /* ~1 char time (10 bits for 8N1) */

    if (params->baudrate <= 19200) {
        dev->byte_timeout = ((char_time * 3U) + 1U) / 2U;   /* ~1.5 char times, rounded up */
        dev->frame_timeout = ((char_time * 7U) + 1U) / 2U;  /* ~3.5 char times, rounded up */
    }
    else {
        /* section 2.5.1.1 of Modbus over Serial Line specifies fixed timeouts
         * for higher baud rates */
        dev->byte_timeout = 750;
        dev->frame_timeout = 1750;
    }

    /* section 2.5.1.1 of the Modbus specification specifies the timeout as the
     * gap between char end and next char start, but this transport applies the
     * timeouts from char end to next char end, so compensate for the char
     * transport time */
    dev->byte_timeout += char_time;
    dev->frame_timeout += char_time;

    DEBUG(
        "[modbus_rtu] modbus_rtu_init: byte_timeout=%" PRIu32 " frame_timeout=%" PRIu32
        " response_timeout=%" PRIu32 "\n",
        dev->byte_timeout, dev->frame_timeout, params->response_timeout);

    /* wire the PDU slice into the modbus context */
    modbus->buffer = &dev->buf[MODBUS_RTU_ADU_ID_SIZE];
    modbus->buffer_capacity = MODBUS_PDU_MAX_SIZE;
    modbus->buffer_size = 0;

    /* initialize isrpipe */
    isrpipe_init(&dev->isrpipe, dev->isr_buf, sizeof(dev->isr_buf));

    /* initialize mutex */
    mutex_init(&dev->idle_lock);

    /* initialize idle timer */
    dev->idle_timer.callback = _timer_cb;
    dev->idle_timer.arg = (void *)dev;

    /* initialize GPIO */
    if (gpio_is_valid(params->pin_rts)) {
        res = gpio_init(params->pin_rts, GPIO_OUT);

        if (res != 0) {
            DEBUG("[modbus_rtu] modbus_rtu_init: GPIO init failed (%d)\n", res);
            return -ENODEV;
        }

        gpio_write(params->pin_rts, !params->pin_rts_enable);
    }

    /* initialize UART */
    res = uart_init(params->uart, params->baudrate, _uart_cb, dev);

    if (res != UART_OK) {
        DEBUG("[modbus_rtu] modbus_rtu_init: UART init failed (%d)\n", res);
        return -ENODEV;
    }

    /* assume bus busy initially */
    _reset_idle(dev);

    return 0;
}

const modbus_transport_desc_t modbus_rtu_transport = {
    .init = modbus_rtu_init,
    .send = modbus_rtu_send,
    .recv = modbus_rtu_recv,
};

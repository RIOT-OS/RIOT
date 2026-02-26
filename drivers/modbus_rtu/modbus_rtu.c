/*
 * SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_modbus
 * @brief       Modbus
 *
 * @{
 * @file
 * @brief       Modbus RTU interface implementation
 *
 * @author      Bas Stottelaar <bas.stottelaar@gmail.com>
 * @}
 */

#include <assert.h>
#include <stdlib.h>

#include "modbus.h"
#include "modbus_rtu.h"

#include "periph/gpio.h"
#include "periph/uart.h"

#include "isrpipe.h"
#include "isrpipe/read_timeout.h"

#include "checksum/ucrc16.h"
#include "timex.h"
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

static inline void enable_rts(modbus_rtu_params_t *params)
{
    TRACE_PUTS("[modbus_rtu] enable_rts");

    if (gpio_is_valid(params->pin_rts)) {
        gpio_write(params->pin_rts, params->pin_rts_enable);
    }
}

static inline void disable_rts(modbus_rtu_params_t *params)
{
    TRACE_PUTS("[modbus_rtu] disable_rts");

    if (gpio_is_valid(params->pin_rts)) {
        gpio_write(params->pin_rts, !params->pin_rts_enable);
    }
}

static void reset_idle(modbus_rtu_t *dev)
{
    mutex_trylock(&dev->idle_lock);
    ztimer_set(ZTIMER_USEC, &(dev->idle_timer), dev->frame_timeout);
}

static int wait_for_idle(modbus_rtu_t *dev, uint32_t timeout)
{
    if (timeout > 0) {
        if (ztimer_mutex_lock_timeout(ZTIMER_USEC, &(dev->idle_lock), timeout) == -ECANCELED) {
            TRACE_PUTS("[modbus_rtu] wait_for_idle: bus not idle");
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

static void timer_cb(void *arg)
{
    modbus_rtu_t *dev = (modbus_rtu_t *)arg;

    tsrb_clear(&dev->isrpipe.tsrb);
    mutex_unlock(&dev->idle_lock);
}

static void uart_cb(void *arg, uint8_t data)
{
    modbus_rtu_t *dev = (modbus_rtu_t *)arg;

    TRACE("[modbus_rtu] uart_cb: 0x%02x\n", data);

    /* mark bus as busy */
    reset_idle(dev);

    if (isrpipe_write_one(&dev->isrpipe, data) != 0) {
        DEBUG_PUTS("[modbus_rtu] uart_cb: isrpipe buffer full");
        return;
    }
}

int modbus_rtu_send(modbus_t *modbus, modbus_message_t *message, uint8_t flags)
{
    (void)flags;

    modbus_rtu_t *dev = (modbus_rtu_t *)modbus->dev;
    modbus_rtu_params_t *params = (modbus_rtu_params_t *)modbus->params;

    DEBUG("[modbus_rtu] modbus_rtu_send: size=%" PRIuSIZE " flags=%d\n", modbus->buffer_size,
          flags);

    /* compute frame checksum */
    uint16_t crc = 0xffff;

    crc = ucrc16_calc_le(&(message->id), sizeof(message->id), 0xa001, crc);
    crc = ucrc16_calc_le(modbus->buffer, modbus->buffer_size, 0xa001, crc);

    /* wait for bus to become idle */
    wait_for_idle(dev, dev->frame_timeout);

    enable_rts(params);
    uart_write(params->uart, &(message->id), sizeof(message->id));
    uart_write(params->uart, modbus->buffer, modbus->buffer_size);
    uart_write(params->uart, (uint8_t *)&crc, sizeof(crc));
    disable_rts(params);

    /* mark bus as busy */
    reset_idle(dev);

    return 0;
}

int modbus_rtu_recv(modbus_t *modbus, modbus_message_t *message, size_t size, uint8_t flags)
{
    modbus_rtu_t *dev = (modbus_rtu_t *)modbus->dev;
    modbus_rtu_params_t *params = (modbus_rtu_params_t *)modbus->params;

    DEBUG("[modbus_rtu] modbus_rtu_recv: size=%" PRIuSIZE " flags=%d\n",  size, flags);

    uint8_t data;

    /* wait for the first byte */
    if ((flags & MODBUS_RECV_FLAG_START) == MODBUS_RECV_FLAG_START) {
        if ((flags & MODBUS_RECV_FLAG_REQUEST) == MODBUS_RECV_FLAG_REQUEST) {
            /* for requests, wait for the bus to become idle first */
            wait_for_idle(dev, 0);

            /* block for the first byte of a request frame, which is the ID byte */
            if (isrpipe_read(&dev->isrpipe, &data,
                             MODBUS_RTU_ADU_ID_SIZE) != MODBUS_RTU_ADU_ID_SIZE) {
                /* should not happen */
                return -ETIMEDOUT;
            }
        }
        else if ((flags & MODBUS_RECV_FLAG_RESPONSE) == MODBUS_RECV_FLAG_RESPONSE) {
            /* for responses, do NOT wait for idle or clear the buffer, the
             * response may have already started arriving */

            /* wait for the first byte of the response frame, which is the ID byte */
            if (isrpipe_read_timeout(&dev->isrpipe, &data, MODBUS_RTU_ADU_ID_SIZE,
                                     params->response_timeout) == -ETIMEDOUT) {
                return -ETIMEDOUT;
            }
        }
        else {
            /* should not happen */
            return -EBADMSG;
        }

        message->id = data;
        modbus->buffer_size = 0;
    }

    /* wait for the actual bytes requested */
    if ((modbus->buffer_size + size) > sizeof(modbus->buffer)) {
        return -ENOBUFS;
    }

    if (isrpipe_read_all_timeout(&dev->isrpipe, &(modbus->buffer[modbus->buffer_size]), size,
                                 dev->byte_timeout) == -ETIMEDOUT) {
        return -ETIMEDOUT;
    }

    modbus->buffer_size += size;

    /* wait for two more bytes (CRC), if no more data is expected */
    if ((flags & MODBUS_RECV_FLAG_STOP) == MODBUS_RECV_FLAG_STOP) {
        uint16_t expected;

        if (isrpipe_read_all_timeout(&dev->isrpipe, (uint8_t *)&expected, sizeof(expected),
                                     dev->byte_timeout) == -ETIMEDOUT) {
            return -ETIMEDOUT;
        }

        uint16_t actual = 0xffff;

        actual = ucrc16_calc_le(&(message->id), sizeof(message->id), 0xa001, actual);
        actual = ucrc16_calc_le(modbus->buffer, modbus->buffer_size, 0xa001, actual);

        if (actual != expected) {
            DEBUG("[modbus_rtu] modbus_rtu_recv: CRC error (expected=0x%04x, actual=0x%04x)\n",
                  expected, actual);
            return -EBADMSG;
        }
    }

    return 0;
}

int modbus_rtu_init(modbus_t *modbus)
{
    int err;

    modbus_rtu_t *dev = (modbus_rtu_t *)modbus->dev;
    modbus_rtu_params_t *params = (modbus_rtu_params_t *)modbus->params;

    /* pre-compute timeouts */
    assert(params != NULL);
    assert(params->baudrate > 0);

    uint32_t char_time = (US_PER_SEC * 10U) / params->baudrate; /* ~1 char time (10 bits for 8N1) */

    if (params->baudrate <= 19200) {
        dev->byte_timeout = ((char_time * 3U) + 1U) / 2U; /* ~1.5 char times, rounded up */
        dev->frame_timeout = ((char_time * 7U) + 1U) / 2U; /* ~3.5 char times, rounded up */
    }
    else {
        /* section 2.5.1.1 of Modbus over Serial Line specifies fixed timeouts
         * for higher baud rates */
        dev->byte_timeout = 750;
        dev->frame_timeout = 1750;
    }

    /* the Modbus specification specifies the timeout as the gap between char
     * end and next char start, but this driver applies the timeouts from char
     * end to next char end, so compensate for the char transport time */
    dev->byte_timeout += char_time;
    dev->frame_timeout += char_time;

    DEBUG(
        "[modbus_rtu] modbus_rtu_init: byte_timeout=%" PRIu32 " frame_timeout=%" PRIu32
        " response_timeout=%" PRIu32 "\n",
        dev->byte_timeout, dev->frame_timeout, params->response_timeout);

    /* initialize isrpipe */
    isrpipe_init(&dev->isrpipe, dev->buf, sizeof(dev->buf));

    /* initialize mutex */
    mutex_init(&dev->idle_lock);

    /* initialize idle timer */
    dev->idle_timer.callback = timer_cb;
    dev->idle_timer.arg = (void *)dev;

    /* initialize GPIO */
    if (gpio_is_valid(params->pin_rts)) {
        err = gpio_init(params->pin_rts, GPIO_OUT);

        if (err != 0) {
            DEBUG("[modbus_rtu] modbus_rtu_init: GPIO init failed (%d)\n", err);
            return -ENODEV;
        }

        gpio_write(params->pin_rts, !params->pin_rts_enable);
    }

    /* initialize UART */
    err = uart_init(params->uart, params->baudrate, uart_cb, dev);

    if (err != UART_OK) {
        DEBUG("[modbus_rtu] modbus_rtu_init: UART init failed (%d)\n", err);
        return -ENODEV;
    }

    /* assume bus busy */
    reset_idle(dev);

    return 0;
}

const modbus_desc_t modbus_rtu_driver = {
    .init = modbus_rtu_init,
    .send = modbus_rtu_send,
    .recv = modbus_rtu_recv
};

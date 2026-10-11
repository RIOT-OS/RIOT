/*
 * SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_modbus_ascii
 * @brief       Modbus ASCII
 *
 * @{
 * @file
 * @brief       Modbus ASCII transport implementation
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 * @}
 */

#include <assert.h>
#include <errno.h>

#include "modbus.h"
#include "modbus_ascii.h"

#include "periph/gpio.h"
#include "periph/uart.h"

#include "isrpipe.h"
#include "isrpipe/read_timeout.h"

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

#define MODBUS_ASCII_CHAR_START     ':'
#define MODBUS_ASCII_CHAR_CR        '\r'
#define MODBUS_ASCII_CHAR_LF        '\n'

static int _hex_to_nibble(uint8_t c)
{
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }
    if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }
    return -1;
}

static uint8_t _nibble_to_hex(uint8_t n)
{
    return (n < 10) ? (uint8_t)('0' + n) : (uint8_t)('A' + (n - 10));
}

static inline void _enable_rts(const modbus_ascii_params_t *params)
{
    TRACE_PUTS("[modbus_ascii] _enable_rts");

    if (gpio_is_valid(params->pin_rts)) {
        gpio_write(params->pin_rts, params->pin_rts_enable);
    }
}

static inline void _disable_rts(const modbus_ascii_params_t *params)
{
    TRACE_PUTS("[modbus_ascii] _disable_rts");

    if (gpio_is_valid(params->pin_rts)) {
        gpio_write(params->pin_rts, !params->pin_rts_enable);
    }
}

static void _uart_cb(void *arg, uint8_t data)
{
    modbus_ascii_t *dev = (modbus_ascii_t *)arg;

    TRACE("[modbus_ascii] _uart_cb: 0x%02x\n", data);

    if (isrpipe_write_one(&dev->isrpipe, data) != 0) {
        DEBUG_PUTS("[modbus_ascii] _uart_cb: isrpipe buffer full");
    }
}

static void _write_hex_byte(uart_t uart, uint8_t b)
{
    uint8_t hex[2] = {
        _nibble_to_hex((b >> 4) & 0x0F),
        _nibble_to_hex(b & 0x0F),
    };

    uart_write(uart, hex, sizeof(hex));
}

static int _read_byte(modbus_ascii_t *dev, uint8_t *out, uint32_t timeout)
{
    if (timeout > 0) {
        if (isrpipe_read_timeout(&dev->isrpipe, out, 1, timeout) == -ETIMEDOUT) {
            return -ETIMEDOUT;
        }
    }
    else {
        /* isrpipe_read blocks until at least one byte is available */
        isrpipe_read(&dev->isrpipe, out, 1);
    }

    return 0;
}

static int modbus_ascii_recv(modbus_t *modbus, modbus_message_t *message, bool request)
{
    modbus_ascii_t *dev = (modbus_ascii_t *)modbus->dev;
    const modbus_ascii_params_t *params = (const modbus_ascii_params_t *)modbus->params;

    DEBUG("[modbus_ascii] modbus_ascii_recv: request=%d\n", request);

    /* pick the timeout for the search of the start-of-frame: requests block
     * indefinitely, responses observe the response timeout */
    uint32_t sof_timeout;

    if (request) {
        sof_timeout = 0;
    }
    else {
        sof_timeout = params->response_timeout;
    }

    /* discard everything until the start-of-frame ':' character is observed */
    while (1) {
        uint8_t c;
        int res = _read_byte(dev, &c, sof_timeout);

        if (res != 0) {
            return res;
        }

        if (c == MODBUS_ASCII_CHAR_START) {
            break;
        }
    }

    /* read hex byte pairs into the binary ADU buffer until CR LF */
    size_t pos = 0;
    uint8_t lrc = 0;

    while (1) {
        uint8_t hi;
        int res = _read_byte(dev, &hi, params->char_timeout);

        if (res != 0) {
            return res;
        }

        /* end-of-frame: CR followed by LF */
        if (hi == MODBUS_ASCII_CHAR_CR) {
            uint8_t lf;

            res = _read_byte(dev, &lf, params->char_timeout);
            if (res != 0) {
                return res;
            }

            if (lf != MODBUS_ASCII_CHAR_LF) {
                DEBUG("[modbus_ascii] modbus_ascii_recv: expected LF after CR (got 0x%02x)\n", lf);
                return -EBADMSG;
            }

            break;
        }

        if (pos >= sizeof(dev->buf)) {
            return -ENOBUFS;
        }

        uint8_t lo;
        res = _read_byte(dev, &lo, params->char_timeout);

        if (res != 0) {
            return res;
        }

        int hi_n = _hex_to_nibble(hi);
        int lo_n = _hex_to_nibble(lo);

        if (hi_n < 0 || lo_n < 0) {
            DEBUG("[modbus_ascii] modbus_ascii_recv: invalid hex chars 0x%02x 0x%02x\n", hi, lo);
            return -EBADMSG;
        }

        uint8_t b = (uint8_t)((hi_n << 4) | lo_n);

        dev->buf[pos++] = b;
        lrc += b;
    }

    /* a complete ADU consists of address(1) + PDU(>=1) + LRC(1) */
    if (pos < (MODBUS_ASCII_ADU_ID_SIZE + MODBUS_PDU_MIN_REQUEST_SIZE +
               MODBUS_ASCII_ADU_LRC_SIZE)) {
        DEBUG("[modbus_ascii] modbus_ascii_recv: ADU too short (%" PRIuSIZE ")\n", pos);
        return -EBADMSG;
    }

    /* the LRC is the 2's complement of the sum of all bytes excluding it; the
     * 8-bit sum of all bytes including it must therefore be zero */
    if (lrc != 0) {
        DEBUG_PUTS("[modbus_ascii] modbus_ascii_recv: LRC error");
        return -EBADMSG;
    }

    /* hand over the PDU and address to the caller */
    message->id = dev->buf[0];
    modbus->buffer_size = pos - MODBUS_ASCII_ADU_ID_SIZE - MODBUS_ASCII_ADU_LRC_SIZE;

    return 0;
}

static int modbus_ascii_send(modbus_t *modbus, const modbus_message_t *message)
{
    const modbus_ascii_params_t *params = (const modbus_ascii_params_t *)modbus->params;

    DEBUG("[modbus_ascii] modbus_ascii_send: size=%" PRIuSIZE "\n", modbus->buffer_size);

    /* compute LRC over [id][PDU]; LRC is the 2's complement of the 8-bit sum */
    uint8_t lrc = message->id;

    for (size_t i = 0; i < modbus->buffer_size; i++) {
        lrc += modbus->buffer[i];
    }
    lrc = (uint8_t)(-(int)lrc);

    _enable_rts(params);

    /* start of frame */
    uint8_t start = MODBUS_ASCII_CHAR_START;
    uart_write(params->uart, &start, 1);

    /* hex(id) */
    _write_hex_byte(params->uart, message->id);

    /* hex(PDU) */
    for (size_t i = 0; i < modbus->buffer_size; i++) {
        _write_hex_byte(params->uart, modbus->buffer[i]);
    }

    /* hex(LRC) */
    _write_hex_byte(params->uart, lrc);

    /* end of frame: CR LF */
    uint8_t end[2] = { MODBUS_ASCII_CHAR_CR, MODBUS_ASCII_CHAR_LF };
    uart_write(params->uart, end, sizeof(end));

    _disable_rts(params);

    return 0;
}

static int modbus_ascii_init(modbus_t *modbus)
{
    int res;

    modbus_ascii_t *dev = (modbus_ascii_t *)modbus->dev;
    const modbus_ascii_params_t *params = (const modbus_ascii_params_t *)modbus->params;

    assert(params != NULL);
    assert(params->baudrate > 0);

    DEBUG("[modbus_ascii] modbus_ascii_init: char_timeout=%" PRIu32
          " response_timeout=%" PRIu32 "\n",
          params->char_timeout, params->response_timeout);

    /* wire the PDU slice into the modbus context */
    modbus->buffer = &dev->buf[MODBUS_ASCII_ADU_ID_SIZE];
    modbus->buffer_capacity = MODBUS_PDU_MAX_SIZE;
    modbus->buffer_size = 0;

    /* initialize isrpipe */
    isrpipe_init(&dev->isrpipe, dev->isr_buf, sizeof(dev->isr_buf));

    /* initialize GPIO */
    if (gpio_is_valid(params->pin_rts)) {
        res = gpio_init(params->pin_rts, GPIO_OUT);

        if (res != 0) {
            DEBUG("[modbus_ascii] modbus_ascii_init: GPIO init failed (%d)\n", res);
            return -ENODEV;
        }

        gpio_write(params->pin_rts, !params->pin_rts_enable);
    }

    /* initialize UART */
    res = uart_init(params->uart, params->baudrate, _uart_cb, dev);

    if (res != UART_OK) {
        DEBUG("[modbus_ascii] modbus_ascii_init: UART init failed (%d)\n", res);
        return -ENODEV;
    }

    return 0;
}

const modbus_transport_desc_t modbus_ascii_transport = {
    .init = modbus_ascii_init,
    .send = modbus_ascii_send,
    .recv = modbus_ascii_recv,
};

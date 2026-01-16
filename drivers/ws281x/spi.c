/*
 * SPDX-FileCopyrightText: 2026 ML!PA Consulting GmbH.
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_ws281x
 * @{
 *
 * @file
 * @brief       Implementation of WS281x interface through SPI
 *
 * @author      Hugues Larrive <hlarrive@pm.me>
 * @author      Fabian Hüßler <fabian.huessler@ml-pa.com>
 *
 * @}
 */

#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>

#include "compiler_hints.h"
#include "macros/math.h"
#include "od.h"
#include "periph/gpio.h"
#include "periph/spi.h"
#include "string_utils.h"
#include "time_units.h"
#include "ztimer.h"

#include "ws281x.h"
#include "ws281x_constants.h"
#include "ws281x_params.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* Trying to reduce the number of parameters, but you can still overwrite them */
/* Duration of one SPI bit in nanoseconds */
#define _SPI_NS_PER_BIT         (NS_PER_SEC / WS281X_SPI_CLK)
/* Total SPI bits per WS281x bit (rounded) */
#define _SPI_BITS_PER_WS        DIV_ROUND(WS281X_T_DATA_NS, _SPI_NS_PER_BIT)
/* Number of HIGH SPI bits for WS 0 */
#define _SPI_BITS_ZERO          DIV_ROUND(WS281X_T_DATA_ZERO_NS, _SPI_NS_PER_BIT)
/* Number of HIGH SPI bits for WS 1 */
#define _SPI_BITS_ONE           DIV_ROUND_UP(WS281X_T_DATA_ONE_NS, _SPI_NS_PER_BIT)
/* MSB aligned  bit pattern for WS 0 */
#define _SPI_NIBBLE_0           (((1U << _SPI_BITS_ZERO) - 1) << (_SPI_BITS_PER_WS - _SPI_BITS_ZERO))
/* MSB aligned  bit pattern for WS 1 */
#define _SPI_NIBBLE_1           (((1U << _SPI_BITS_ONE) - 1) << (_SPI_BITS_PER_WS - _SPI_BITS_ONE))
/* Number of SPI 0-bytes to trigger a reset */
#define _SPI_RESET_BYTES        ((WS281X_T_END_US * NS_PER_US) / _SPI_NS_PER_BIT / 8U)

MAYBE_UNUSED
static const uint8_t _WS281X_SPI_NIBBLE_0 = (uint8_t)_SPI_NIBBLE_0;
MAYBE_UNUSED
static const uint8_t _WS281X_SPI_NIBBLE_1 = (uint8_t)_SPI_NIBBLE_1;

#ifndef WS281X_SPI_NIBBLE_0
#define WS281X_SPI_NIBBLE_0             _WS281X_SPI_NIBBLE_0   /* e.g., 0x08: 1000: short high */
#endif

#ifndef WS281X_SPI_NIBBLE_1
#define WS281X_SPI_NIBBLE_1             _WS281X_SPI_NIBBLE_1   /* e.g., 0x0E: 1110: long high  */
#endif

#ifndef WS281X_SPI_BITS_PER_WS_BIT
#define WS281X_SPI_BITS_PER_WS_BIT      _SPI_BITS_PER_WS       /* e.g., 4: 4 SPI bits per WS bit */
#endif

typedef struct ws281x_spi_data {
    uint8_t data[WS281X_BYTES_PER_DEVICE * WS281X_SPI_BITS_PER_WS_BIT];
} ws281x_spi_data_t;

/* Add space for one RESET pulse to eat phantom bit */
static uint8_t _spi_buf[_SPI_RESET_BYTES + WS281X_PARAM_NUMOF * sizeof(ws281x_spi_data_t)];
static size_t _spi_size;

int ws281x_init(ws281x_t *dev, const ws281x_params_t *params)
{
    if (!dev || !params || !params->buf) {
        return -EINVAL;
    }
    memset(dev, 0, sizeof(*dev));
    dev->params = *params;
    gpio_clear(dev->params.pin);
    gpio_init(dev->params.pin, GPIO_OUT);
    DEBUG("WS281x SPI encoding: %u bits per WS bit (0: 0x%02X, 1: 0x%02X)\n",
          (unsigned)WS281X_SPI_BITS_PER_WS_BIT,
          WS281X_SPI_NIBBLE_0,
          WS281X_SPI_NIBBLE_1);
    return 0;
}

void ws281x_prepare_transmission(ws281x_t *dev)
{
    (void)dev;
    memset(_spi_buf, 0, sizeof(_spi_buf));
    _spi_size = _SPI_RESET_BYTES;
}

void ws281x_end_transmission(ws281x_t *dev)
{
    (void)dev;
    /* spi_periph_reconfigure is required to keep MOSI line stable low before and after transfer */
    spi_init_pins(WS281X_SPI_DEV);
    spi_acquire(WS281X_SPI_DEV, SPI_CS_UNDEF, SPI_MODE_0, WS281X_SPI_CLK);
    spi_transfer_bytes(WS281X_SPI_DEV, SPI_CS_UNDEF, false,
                       _spi_buf, _spi_buf, _spi_size);
    ztimer_sleep(ZTIMER_USEC, WS281X_T_END_US);
    spi_release(WS281X_SPI_DEV);
    gpio_clear(dev->params.pin);
    spi_deinit_pins(WS281X_SPI_DEV);
    gpio_init(dev->params.pin, GPIO_OUT);
    /* Can get the size when LED chain tail is connected to MISO */
    const uint8_t *non_zero_spi_byte = memchk(_spi_buf, 0x00, sizeof(_spi_buf));
    if (non_zero_spi_byte) {
        size_t led_numof = (non_zero_spi_byte - _spi_buf) / sizeof(ws281x_spi_data_t);
        led_numof -= (_SPI_RESET_BYTES / sizeof(ws281x_spi_data_t));
        dev->params.numof = led_numof;
        DEBUG("Detected %u LEDs in chain\n", (unsigned)dev->params.numof);
    }
#if ENABLE_DEBUG
#if MODULE_OD
    DEBUG("Received SPI data (%u bytes):\n", (unsigned)_spi_size);
    od_hex_dump(_spi_buf, _spi_size, sizeof(ws281x_spi_data_t));
#endif
#endif
}

static void _ws281x_write_buffer_unaligned(const void *_buf, size_t size)
{
    const uint8_t *buf = _buf;
    uint8_t *out = _spi_buf + _spi_size;
    size_t bit_pos = 0;
    for (size_t i = 0; i < size; i++) {
        uint8_t byte = buf[i];

        for (int b = 7; b >= 0; b--) {
            uint8_t nibble = (byte & (1u << b)) ? WS281X_SPI_NIBBLE_1
                                                : WS281X_SPI_NIBBLE_0;
            /* Pack _SPI_BITS_PER_WS bits into _spi_buf */
            for (int k = WS281X_SPI_BITS_PER_WS_BIT - 1; k >= 0; k--) {
                size_t byte_index = bit_pos / 8;
                size_t bit_index  = 7 - (bit_pos % 8);

                if (nibble & (1u << k)) {
                    out[byte_index] |=  (1u << bit_index);
                }
                else {
                    out[byte_index] &= ~(1u << bit_index);
                }
                bit_pos++;
            }
        }
    }
    _spi_size += (bit_pos + 7) / 8;
}

static void _ws281x_write_buffer_aligned(const void *_buf, size_t size)
{
    const uint8_t *buf = _buf;
    uint8_t *out = _spi_buf + _spi_size;

    const unsigned ws_bits_per_spi_byte = 8 / WS281X_SPI_BITS_PER_WS_BIT;

    for (size_t i = 0; i < size; i++) {
        uint8_t byte = buf[i];

        /* Encode multiple WS bits per SPI byte */
        for (unsigned bit = 0; bit < 8; bit += ws_bits_per_spi_byte) {
            uint8_t spi_byte = 0;

            for (unsigned j = 0; j < ws_bits_per_spi_byte; j++) {
                spi_byte <<= WS281X_SPI_BITS_PER_WS_BIT;
                spi_byte |= (byte & 0x80) ? WS281X_SPI_NIBBLE_1 : WS281X_SPI_NIBBLE_0;
                byte <<= 1;
            }
            *out++ = spi_byte;
        }
    }
    _spi_size = out - _spi_buf;
}

void ws281x_write_buffer(ws281x_t *dev, const void *_buf, size_t size)
{
    assert(dev);
    assert(size % WS281X_BYTES_PER_DEVICE == 0);
    assert(size * WS281X_SPI_BITS_PER_WS_BIT <= sizeof(_spi_buf));

    if (8 % WS281X_SPI_BITS_PER_WS_BIT) {
        _ws281x_write_buffer_unaligned(_buf, size);
    }
    else {
        _ws281x_write_buffer_aligned(_buf, size);
    }
}

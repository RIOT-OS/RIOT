/*
 * Copyright 2023 Hugues Larrive <hlarrive@pm.me>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_ws281x
 *
 * @{
 *
 * @file
 * @brief       Implementation of `ws281x_write_buffer()` through SPI
 *
 * @author      Hugues Larrive <hlarrive@pm.me>
 *
 * @}
 */
#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>

#include "ws281x.h"
#include "ws281x_params.h"
#include "ws281x_constants.h"
#include "periph/spi.h"

static struct {
    spi_t dev;
    spi_mode_t mode;
    spi_clk_t clk;
    spi_cs_t cs;
} spiconf;

static uint8_t spi_buf[WS281X_PARAM_NUMOF * WS281X_BYTES_PER_DEVICE * 4];

int ws281x_init(ws281x_t *dev, const ws281x_params_t *params)
{
    if (!dev || !params || !params->buf) {
        return -EINVAL;
    }

    memset(dev, 0, sizeof(ws281x_t));
    dev->params = *params;

    spiconf.dev = SPI_DEV(0);
    spiconf.mode = SPI_MODE_0;
    spiconf.clk = 4000000;
    spiconf.cs = SPI_HWCS(0);

    return 0;
}

void ws281x_prepare_transmission(ws281x_t *dev)
{
    (void)dev;
    spi_acquire(spiconf.dev, spiconf.cs, spiconf.mode, spiconf.clk);
}

void ws281x_end_transmission(ws281x_t *dev)
{
    (void)dev;
    spi_release(spiconf.dev);
    xtimer_usleep(WS281X_T_END_US);
}

void ws281x_write_buffer(ws281x_t *dev, const void *buf, size_t size)
{
    assert(dev);

    const uint8_t *pos = buf;
    const uint8_t *end = pos + size;

    int i = 0;
    while (pos < end) {
        spi_buf[i] = *pos & 0x80 ? 0xE0 : 0x80;
        spi_buf[i] |= *pos & 0x40 ? 0x0E : 0x08;
        spi_buf[i + 1] = *pos & 0x20 ? 0xE0 : 0x80;
        spi_buf[i + 1] |= *pos & 0x10 ? 0x0E : 0x08;
        spi_buf[i + 2] = *pos & 0x08 ? 0xE0 : 0x80;
        spi_buf[i + 2] |= *pos & 0x04 ? 0x0E : 0x08;
        spi_buf[i + 3] = *pos & 0x02 ? 0xE0 : 0x80;
        spi_buf[i + 3] |= *pos & 0x01 ? 0x0E : 0x08;
        i += 4;
        pos++;
    }

    spi_transfer_bytes(spiconf.dev, spiconf.cs, false, spi_buf, NULL, sizeof(spi_buf));
}

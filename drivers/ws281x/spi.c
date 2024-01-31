/*
 * Copyright 2024 Hugues Larrive <hlarrive@pm.me>
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

#include "macros/units.h"
#include "periph/spi.h"
#include "ws281x.h"
#include "ws281x_constants.h"
#include "ws281x_params.h"

#ifndef WS281X_SPI_DEV
#define WS281X_SPI_DEV  (0)
#endif
#ifndef WS281X_SPI_PATTERN_LENGTH
#define WS281X_SPI_PATTERN_LENGTH   (3)
#endif
#ifndef WS281X_SPI_CLK
#define WS281X_SPI_CLK  KHZ(800 * WS281X_SPI_PATTERN_LENGTH)
#endif

#if (WS281X_SPI_PATTERN_LENGTH == 3)
/* The LSB of the 2nd byte (always 1) is the MSB of the pattern for bit 2,
 * which is encoded in the 3rd byte, so we will use a 4-bit pattern. */
#define WS281X_SPI_DATA_0_MASK   (0b00001001) 
#define WS281X_SPI_DATA_1_MASK   (0b00001101)
/* 1u01u01u 01u01u01 u01u01u0 */
#define WS281X_SPI_SHIFTS   {4,1,-2,3,0,5,2,-1}
#elif (WS281X_SPI_PATTERN_LENGTH == 4)
#define WS281X_SPI_DATA_0_MASK   (0b00001000) 
#define WS281X_SPI_DATA_1_MASK   (0b00001110)
#define WS281X_SPI_SHIFTS   {4,0,4,0,4,0,4,0}
#elif (WS281X_SPI_PATTERN_LENGTH == 8)
#define WS281X_SPI_DATA_0_MASK   (0b11000000) 
#define WS281X_SPI_DATA_1_MASK   (0b11111100)
#define WS281X_SPI_SHIFTS   {0,0,0,0,0,0,0,0}
#endif

static uint8_t spi_buf[WS281X_PARAM_NUMOF * WS281X_BYTES_PER_DEVICE * WS281X_SPI_PATTERN_LENGTH];

int ws281x_init(ws281x_t *dev, const ws281x_params_t *params)
{
    if (!dev || !params || !params->buf) {
        return -EINVAL;
    }

    memset(dev, 0, sizeof(ws281x_t));
    dev->params = *params;

    return 0;
}

void ws281x_prepare_transmission(ws281x_t *dev)
{
    (void)dev;
    spi_acquire(SPI_DEV(WS281X_SPI_DEV), SPI_CS_UNDEF, SPI_MODE_0, WS281X_SPI_CLK);
}

void ws281x_end_transmission(ws281x_t *dev)
{
    (void)dev;
    spi_release(SPI_DEV(WS281X_SPI_DEV));
    xtimer_usleep(WS281X_T_END_US);
}

void ws281x_write_buffer(ws281x_t *dev, const void *_buf, size_t size)
{
    assert(dev);

    const uint8_t *buf = _buf;
    const int8_t shift[8] = WS281X_SPI_SHIFTS;

    for (int i = 0; i < (int)size; ++i) {
        uint8_t byte = buf[i];
        uint8_t offset = 0;
        spi_buf[i * WS281X_SPI_PATTERN_LENGTH] = 0;
        for (uint8_t cnt = 0; cnt < 8; ++cnt) {
            spi_buf[i * WS281X_SPI_PATTERN_LENGTH + offset]
                |= (byte & 0b10000000)
                    ? (shift[cnt] >= 0)
                        ? WS281X_SPI_DATA_1_MASK << shift[cnt]
                        : WS281X_SPI_DATA_1_MASK >> -shift[cnt]
                    : (shift[cnt] >= 0)
                        ? WS281X_SPI_DATA_0_MASK << shift[cnt]
                        : WS281X_SPI_DATA_0_MASK >> -shift[cnt];
            byte <<= 1;
            if (shift[cnt] <= 0) {
                ++offset;
                spi_buf[i * WS281X_SPI_PATTERN_LENGTH + offset] = 0;
            }
       }
    }

    spi_transfer_bytes(SPI_DEV(WS281X_SPI_DEV), SPI_CS_UNDEF, false, spi_buf, NULL, sizeof(spi_buf));
}

/*
 * Copyright (C) 2019 Tristan Bruns
 *               2019 Inria
 *               2023 Hugues Larrive
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_fe310
 * @ingroup     drivers_periph_spi
 *
 * @{
 *
 * @file        spi.c
 * @brief       Low-level SPI driver implementation
 *
 * @author      Tristan Bruns
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Hugues Larrive <hugues.larrive@pm.me>
 *
 * @}
 */

#include <assert.h>

#include "clk.h"
#include "cpu.h"
#include "macros/math.h"
#include "mutex.h"
#include "periph/spi.h"

#include "vendor/spi.h"

#define ENABLE_DEBUG        0
#include "debug.h"

/**
 * @brief   Allocation device locks
 */
static mutex_t lock;

void spi_init(spi_t dev)
{
    /* make sure given bus device is valid */
    assert(dev < SPI_NUMOF);

    /* initialize the buses lock */
    mutex_init(&lock);

    /* trigger pin initialization */
    spi_init_pins(dev);

    /* disable hardware chip select
       (hardware chip select only supports one-byte transfers...) */
    _REG32(spi_config[dev].addr, SPI_REG_CSMODE) = SPI_CSMODE_OFF;
}

void spi_init_pins(spi_t dev)
{
    assert(dev < SPI_NUMOF);

    const gpio_t spi1_pins =
        (1 << spi_config[dev].mosi) |
        (1 << spi_config[dev].miso) |
        (1 << spi_config[dev].sclk);

    /* Enable I/O Function 0 */
    GPIO_REG(GPIO_IOF_EN) |=  spi1_pins;
    GPIO_REG(GPIO_IOF_SEL) &= ~spi1_pins;
}

int spi_init_cs(spi_t dev, spi_cs_t cs)
{
    (void)dev;
    assert(dev < SPI_NUMOF);

    /* setting the CS high before configuring it as an
       output should be fine on FE310. */
    gpio_set(cs);

    if (gpio_init(cs, GPIO_OUT)) {
        return SPI_NOCS;
    }

    return SPI_OK;
}

spi_clk_t spi_get_clk(spi_t bus, uint32_t freq)
{
    (void)bus;
    /* fsck = fin / (2 * (div + 1))
     * div 0..4095 (12 bit)
     *
     * div = fin / 2 / fsck - 1
     */

    uint32_t source_clock = coreclk() / 2;

    /* bound divider to 4096 */
    if (freq < DIV_ROUND_UP(source_clock, 4096)) {
        return (spi_clk_t){ .err = -EDOM };
    }
    return (spi_clk_t){ .clk = DIV_ROUND_UP(source_clock, freq) - 1 };
}

int32_t spi_get_freq(spi_t bus, spi_clk_t clk)
{
    (void)bus;
    if (clk.err) {
        return -EINVAL;
    }
    return coreclk() / (2 * (clk.clk + 1));
}

void spi_acquire(spi_t dev, spi_cs_t cs, spi_mode_t mode, spi_clk_t clk)
{
    (void)cs;
    assert(dev < SPI_NUMOF);
    if (clk.err) { return; }

    mutex_lock(&lock);

    _REG32(spi_config[dev].addr, SPI_REG_SCKDIV) = clk.clk;
    _REG32(spi_config[dev].addr, SPI_REG_SCKMODE) = mode;
}

void spi_release(spi_t dev)
{
    (void)dev;

    mutex_unlock(&lock);
}

void spi_transfer_bytes(spi_t dev, spi_cs_t cs, bool cont,
                        const void *out_, void *in_, size_t len)
{
    assert(dev < SPI_NUMOF);
    assert((out_ || in_) && len > 0);
    assert(_REG32(spi_config[dev].addr, SPI_REG_RXFIFO) & SPI_RXFIFO_EMPTY);
    assert(!(_REG32(spi_config[dev].addr, SPI_REG_TXFIFO) & SPI_TXFIFO_FULL));

    const uint8_t *out = out_;
    uint8_t *in = in_;

    if (cs != SPI_CS_UNDEF) {
        gpio_clear(cs);
    }

    for (size_t i = 0; i < len; i++) {
        _REG32(spi_config[dev].addr, SPI_REG_TXFIFO) = out ? out[i] : 0;

        uint32_t rxdata;
        do {
            rxdata = _REG32(spi_config[dev].addr, SPI_REG_RXFIFO);
        } while (rxdata & SPI_RXFIFO_EMPTY);

        if (in) {
            in[i] = (uint8_t)rxdata;
        }
    }

    if (cs != SPI_CS_UNDEF && !cont) {
        gpio_set(cs);
    }
}

/*
 * Copyright (C) 2019 Tristan Bruns
 *               2019 Inria
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
 *
 * @}
 */

#include "cpu.h"
#include "mutex.h"
#include "assert.h"
#include "periph/spi.h"

#include "vendor/spi.h"

#define ENABLE_DEBUG        (0)
#include "debug.h"

static const uint32_t _spi_clks[] = {
    100000,
    400000,
    1000000,
    5000000,
    10000000,
};

#define SPI_CLK_NUMOF       ARRAY_SIZE(_spi_clks)

static uint32_t _spi_clks_config[SPI_CLK_NUMOF] = { 0 };

/* DIV_UP is division which rounds up instead of down */
#define SPI_DIV_UP(a,b) (((a) + ((b) - 1)) / (b))

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

    for (uint8_t i = 0; i < SPI_CLK_NUMOF; ++i) {
        _spi_clks_config[i] = SPI_DIV_UP(cpu_freq(), 2 * _spi_clks[i]) - 1;
    }

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
    GPIO_REG(GPIO_IOF_EN)  |=  spi1_pins;
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

int spi_acquire(spi_t dev, spi_cs_t cs, spi_mode_t mode, spi_clk_t clk)
{
    (void)cs;
    assert(dev < SPI_NUMOF);

    mutex_lock(&lock);

    _REG32(spi_config[dev].addr, SPI_REG_SCKDIV)  = _spi_clks_config[clk];
    _REG32(spi_config[dev].addr, SPI_REG_SCKMODE) = mode;

    return SPI_OK;
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
        do  {
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

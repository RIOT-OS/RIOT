/*
 * Copyright (C) 2019 Tristan Bruns
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
 *
 * @}
 */

#include "cpu.h"
#include "mutex.h"
#include "assert.h"
#include "periph/spi.h"
#include "vendor/platform.h"

#define ENABLE_DEBUG        (0)
#include "debug.h"

/**
 * @brief   Allocation device locks
 */
static mutex_t lock;

void spi_init(spi_t bus)
{
    /* make sure given bus device is valid */
    assert(bus == 0);

    /* initialize the buses lock */
    mutex_init(&lock);

    /* trigger pin initialization */
    spi_init_pins(bus);

    /* disable hardware chip select
       (hardware chip select only supports one-byte transfers...) */
    SPI1_REG(SPI_REG_CSMODE) = SPI_CSMODE_OFF;
}

void spi_init_pins(spi_t bus)
{
    assert(bus == 0);

    const gpio_t spi1_pins =
        (1 << IOF_SPI1_MOSI) |
        (1 << IOF_SPI1_MISO) |
        (1 << IOF_SPI1_SCK);

    /* Enable I/O Function 0 */
    GPIO_REG(GPIO_IOF_EN)  |=  spi1_pins;
    GPIO_REG(GPIO_IOF_SEL) &= ~spi1_pins;
}

int spi_init_cs(spi_t bus, spi_cs_t cs)
{
    if (bus != 0) {
        return SPI_NODEV;
    }

    /* setting the CS high before configuring it as an
       output should be fine on FE310. */
    gpio_set(cs);

    if (gpio_init(cs, GPIO_OUT)) {
        return SPI_NOCS;
    }

    return SPI_OK;
}

int spi_acquire(spi_t bus, spi_cs_t cs, spi_mode_t mode, spi_clk_t clk)
{
    (void) bus;
    (void) cs;

    mutex_lock(&lock);

    SPI1_REG(SPI_REG_SCKDIV)  = spi_clk_config[clk];
    SPI1_REG(SPI_REG_SCKMODE) = mode;

    return SPI_OK;
}

void spi_release(spi_t bus)
{
    (void) bus;

    mutex_unlock(&lock);
}

void spi_transfer_bytes(spi_t bus, spi_cs_t cs, bool cont,
                        const void *out_, void *in_, size_t len)
{
    (void) bus;

    assert((out_ || in_) && len > 0);
    assert(SPI1_REG(SPI_REG_RXFIFO) & SPI_RXFIFO_EMPTY);
    assert(!(SPI1_REG(SPI_REG_TXFIFO) & SPI_TXFIFO_FULL));

    const uint8_t *out = out_;
    uint8_t *in = in_;

    if (cs != SPI_CS_UNDEF) {
        gpio_clear(cs);
    }

    for (size_t i = 0; i < len; i++) {
        SPI1_REG(SPI_REG_TXFIFO) = out ? out[i] : 0;

        uint32_t rxdata = SPI_RXFIFO_EMPTY;
        while (rxdata & SPI_RXFIFO_EMPTY) {
            rxdata = SPI1_REG(SPI_REG_RXFIFO);
        }

        if (in) {
            in[i] = (uint8_t)rxdata;
        }
    }

    if (cs != SPI_CS_UNDEF && !cont) {
        gpio_set(cs);
    }
}

/*
 * Copyright (C) 2015 Daniel Amkaer Sorensen
 *               2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_periph
 * @{
 *
 * @file
 * @brief       Low-level SPI driver implementation for ATmega family
 *
 * @author      Daniel Amkaer Sorensen <daniel.amkaer@gmail.com>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "mutex.h"
#include "assert.h"
#include "periph/spi.h"

/**
 * @brief   Extract BR0, BR1 and SPI2X bits from speed value
 * @{
 */
#define CLK_MASK            (0x3)
#define S2X_SHIFT           (2)
/** @} */

static mutex_t lock = MUTEX_INIT;

void spi_init(spi_t bus)
{
    assert(bus == 0);
    /* power off the SPI peripheral */
    MEGA_PRR |= (1 << PRSPI);
    /* trigger the pin configuration */
    spi_init_pins(bus);
}

void spi_init_pins(spi_t bus)
{

    /* the pin configuration for this CPU is fixed:
     * - PB3: MISO (configure as input - done automatically)
     * - PB2: MOSI (configure as output)
     * - PB1: SCK  (configure as output)
     * - PB0: SS   (configure as output, but unused)
     *
     * The SS pin must be configured as output for the SPI device to work as
     * master correctly, though we do not use it for now (as we handle the chip
     * select externally for now)
     */
    DDRB |= ((1 << DDB2) | (1 << DDB1) | (1 << DDB0));
}

int spi_acquire(spi_t bus, spi_cs_t cs, spi_mode_t mode, spi_clk_t clk)
{
    (void)cs;

    /* lock the bus and power on the SPI peripheral */
    mutex_lock(&lock);
    MEGA_PRR &= ~(1 << PRSPI);

    /* configure as master, with given mode and clock */
    SPSR = (clk >> S2X_SHIFT);
    SPCR = ((1 << SPE) | (1 << MSTR) | mode | (clk & CLK_MASK));
    SPCR |= (1 << SPE);

    /* clear interrupt flag by reading SPSR and data register by reading SPDR */
    (void)SPSR;
    (void)SPDR;

    return SPI_OK;
}

void spi_release(spi_t bus)
{
    /* power off and release the bus */
    SPCR &= ~(1 << SPE);
    MEGA_PRR |= (1 << PRSPI);
    mutex_unlock(&lock);
}

void spi_transfer_bytes(spi_t bus, spi_cs_t cs, bool cont,
                        const void *out, void *in, size_t len)
{
    uint8_t *out_buf = (uint8_t *)out;
    uint8_t *in_buf = (uint8_t *)in;

    assert(out_buf || in_buf);

    if (cs != SPI_CS_UNDEF) {
        gpio_clear((gpio_t)cs);
    }

    for (size_t i = 0; i < len; i++) {
        uint8_t tmp = (out_buf) ? out_buf[i] : 0;
        SPDR = tmp;
        while (!(SPSR & (1 << SPIF))) {}
        tmp = SPDR;
        if (in_buf) {
            in_buf[i] = tmp;
        }
    }

    if ((!cont) && (cs != SPI_CS_UNDEF)) {
        gpio_set((gpio_t)cs);
    }
}

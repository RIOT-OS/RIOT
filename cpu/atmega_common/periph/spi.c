/*
 * Copyright (C) 2015 Daniel Amkaer Sorensen
 *               2016 Freie Universität Berlin
 *               2017 Hamburg University of Applied Sciences
 *               2017 Thomas Perrot <thomas.perrot@tupi.fr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atmega_common
 * @ingroup     drivers_periph_spi
 * @{
 *
 * @file
 * @brief       Low-level SPI driver implementation for ATmega family
 *
 * @author      Daniel Amkaer Sorensen <daniel.amkaer@gmail.com>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Dimitri Nahm <dimitri.nahm@haw-hamburg.de>
 * @author      Thomas Perrot <thomas.perrot@tupi.fr>
 *
 * @}
 */
#include <assert.h>

#include "cpu.h"
#include "mutex.h"
#include "periph/spi.h"
#include "macros/units.h"

#define ENABLE_DEBUG        0
#include "debug.h"

static mutex_t lock = MUTEX_INIT;

/**
 * @brief   Extract BR0, BR1 and SPI2X bits from speed value
 * @{
 */
/* Table of frequencies corresponding to the spi_clk_t enum */
static uint32_t spi_clk[5] = {
    /* CLOCK_CORECLOCK: 8M      10M     16M    20M      32M  */
    KHZ(100),       /*  62.5K   78.125K 125K   156.25K  250K */
    KHZ(400),       /*  250K    312.5K  250K   312.5K   250K */
    MHZ(1),         /*  1MHz    625K    1M     625K     1M   */
    MHZ(5),         /*  4MHz    5M      4M     5M       4M   */
    MHZ(10)         /*  4MHz    10M     8M     10M      8M   */
};

/* Tables for Double SPI speed (SPI2X) and SPI Clock Rate Select (SPR1,
 * SPR0) register bits */
static uint8_t spi2x[5], spr[5];

/* Helper function to compute a right shift value corresponding to
 * dividers */
static uint8_t _clk_shift(uint32_t clk)
{
    /* Atmega datasheets give the following table:
     * SPI2X    SPR1    SPR0    SCK Frequency
     * 0        0       0       Fosc/4
     * 0        0       1       Fosc/16
     * 0        1       0       Fosc/64
     * 0        1       1       Fosc/128
     * 1        0       0       Fosc/2
     * 1        0       1       Fosc/8
     * 1        1       0       Fosc/32
     * 1        1       1       Fosc/64
     *
     * We can easily sort it by dividers by inverting the SPI2X bit and
     * taking it as LSB:
     * Divider  SPR1    SPR2    ~SPI2X
     * 2        0       0       0
     * 4        0       0       1
     * 8        0       1       0
     * 16       0       1       1
     * 32       1       0       0
     * 64       1       0       1
     * 64       1       1       0
     * 128      1       1       1
     * Dividers are all power of two so we can apply them by
     * right shifting these register value up to 5:
     * clk = (CLOCK_CORECLOCK / 2) >> (SPR1 << 2 | SPR0 << 1 | ~SPI2X)
     */
    uint8_t shift = 0;
    while (clk << shift < CLOCK_CORECLOCK / 2) {
        shift++;
    }
    /* Beyond 5 we must shift one more time */
    return shift > 5 ? ++shift : shift;
}

static void _init_clk(void)
{
    for (uint8_t i = 0; i < 5; i++) {

        DEBUG("[spi] spi_clk[%"PRIu8"]: %8"PRIu32" -> ", i, spi_clk[i]);

        /* bound divider from 2 to 128 */
        if (spi_clk[i] > CLOCK_CORECLOCK / 2) {
            spi_clk[i] = CLOCK_CORECLOCK / 2;
        }
        if (spi_clk[i] < CLOCK_CORECLOCK / 128) {
            spi_clk[i] = CLOCK_CORECLOCK / 128;
        }

        /* Compute shift values */
        uint8_t shift = _clk_shift(spi_clk[i]);

        /* Save registers bits */
        spi2x[i] = ~shift & 1;
        spr[i] = shift >> 1;

        /* Save SPI clock frequencies */
        spi_clk[i] = shift > 5 ?
            CLOCK_CORECLOCK >> shift : (CLOCK_CORECLOCK / 2) >> shift;

        DEBUG("%8"PRIu32" (%"PRIu32" / %3"PRIu8") "
            " SPI2X: %"PRIu8"  SPR1: %"PRIu8"  SPR0: %"PRIu8"\n",
            spi_clk[i], CLOCK_CORECLOCK, ((shift > 5 ? 1 : 2) << shift),
            spi2x[i], (spr[i] >> 1), (spr[i] & 1));
    }
}
/** @} */

void spi_init(spi_t bus)
{
    assert(bus == 0);
    /* power off the SPI peripheral */
    power_spi_disable();
    /* trigger the pin configuration */
    spi_init_pins(bus);
    _init_clk();
}

void spi_init_pins(spi_t bus)
{
    (void)bus;
    /* set SPI pins as output */
#if defined (CPU_ATMEGA2560) || defined (CPU_ATMEGA1281)
    DDRB |= ((1 << DDB2) | (1 << DDB1) | (1 << DDB0));
#endif
#if defined (CPU_ATMEGA328P)
    DDRB |= ((1 << DDB2) | (1 << DDB3) | (1 << DDB5));
#endif
#if defined (CPU_ATMEGA1284P)
    DDRB |= ((1 << DDB4) | (1 << DDB5) | (1 << DDB7));
#endif
#if defined(CPU_ATMEGA128RFA1) || defined(CPU_ATMEGA256RFR2)
    /* Master: PB3 MISO set to out
     *         PB2 MOSI set to input by hardware
     *         PB1 SCK  set to out
     *         PB0 /CS  kept as is, has to be configured by user. Flexibility to
     *                  use different /CS pin.
     * Only Master supported. Slave: Only MOSI has to be set as Input.
     * ATmega256RFR2 data sheet p. 365
     * */
    DDRB |= ((1 << DDB2) | (1 << DDB1));
#endif
#ifdef CPU_ATMEGA32U4
    DDRB |= ((1 << DDB0) | (1 << DDB1) | (1 << DDB2));
#endif
}

void spi_acquire(spi_t bus, spi_cs_t cs, spi_mode_t mode, spi_clk_t clk)
{
    (void)bus;
    (void)cs;
    assert(bus == SPI_DEV(0));

    /* lock the bus and power on the SPI peripheral */
    mutex_lock(&lock);
    power_spi_enable();

    /* configure as master, with given mode and clock */
    SPSR = spi2x[clk];
    SPCR = ((1 << SPE) | (1 << MSTR) | mode | spr[clk]);

    /* clear interrupt flag by reading SPSR and data register by reading SPDR */
    (void)SPSR;
    (void)SPDR;
}

void spi_release(spi_t bus)
{
    (void)bus;
    /* power off and release the bus */
    SPCR &= ~(1 << SPE);
    power_spi_disable();
    mutex_unlock(&lock);
}

void spi_transfer_bytes(spi_t bus, spi_cs_t cs, bool cont,
                        const void *out, void *in, size_t len)
{
    (void)bus;

    const uint8_t *out_buf = out;
    uint8_t *in_buf = in;

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

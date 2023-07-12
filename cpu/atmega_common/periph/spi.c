/*
 * Copyright (C) 2015 Daniel Amkaer Sorensen
 *               2016 Freie Universität Berlin
 *               2017 Hamburg University of Applied Sciences
 *               2017 Thomas Perrot <thomas.perrot@tupi.fr>
 *               2023 Hugues Larrive
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
 * @author      Hugues Larrive <hugues.larrive@pm.me>
 *
 * @}
 */
#include <assert.h>

#include "cpu.h"
#include "macros/math.h"
#include "macros/units.h"
#include "mutex.h"
#include "periph/spi.h"

#define ENABLE_DEBUG        0
#include "debug.h"

static mutex_t lock = MUTEX_INIT;

/**
 * @brief   Helper function to compute a right shift value corresponding
 *          to dividers
 * @{
 */
static uint8_t _clk_shift(uint32_t freq)
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
     * We can easily sort it by dividers by inverting the SPI2X bit and
     * taking it as LSB:
     * Divider  SPR1    SPR0    ~SPI2X  shift
     * 2        0       0       0       0
     * 4        0       0       1       1
     * 8        0       1       0       2
     * 16       0       1       1       3
     * 32       1       0       0       4
     * 64       1       0       1       5
     * 64       1       1       0       6
     * 128      1       1       1       7 */
    uint8_t shift;
    for (shift = 0; freq << shift < CLOCK_CORECLOCK / 2;
         shift = ++shift > 5 ? shift + 1 : shift) {}
    return shift;
}
/** @} */

void spi_init(spi_t bus)
{
    assert(bus == 0);
#ifdef PRSPI
    /* power off the SPI peripheral */
    power_spi_disable();
#endif
    /* trigger the pin configuration */
    spi_init_pins(bus);
}

void spi_init_pins(spi_t bus)
{
    (void)bus;
    /* set SPI pins as output */
#if defined(CPU_ATMEGA2560) || defined(CPU_ATMEGA1281)
    DDRB |= ((1 << DDB2) | (1 << DDB1) | (1 << DDB0));
#endif
#if defined(CPU_ATMEGA328P) || defined(CPU_ATMEGA8)
    DDRB |= ((1 << DDB2) | (1 << DDB3) | (1 << DDB5));
#endif
#if defined(CPU_ATMEGA1284P)
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

spi_clk_t spi_get_clk(spi_t bus, uint32_t freq)
{
    (void)bus;
    /* bound divider to 128 */
    if(freq < DIV_ROUND_UP(CLOCK_CORECLOCK, 128)) {
        return (spi_clk_t){ .err = -EDOM };
    }

    uint8_t shift = _clk_shift(freq);
    return (spi_clk_t){
        .spsr_spi2x = ~shift & 1,
        .spcr_spr = shift >> 1
    };
}

int32_t spi_get_freq(spi_t bus, spi_clk_t clk)
{
    (void)bus;
    if (clk.err) {
        return -EINVAL;
    }
    uint8_t shift = (~clk.spsr_spi2x & 1) | (clk.spcr_spr << 1);
    return shift > 5 ?
        CLOCK_CORECLOCK >> shift : (CLOCK_CORECLOCK / 2) >> shift;
}

void spi_acquire(spi_t bus, spi_cs_t cs, spi_mode_t mode, spi_clk_t clk)
{
    (void)bus;
    (void)cs;

    assert(bus == SPI_DEV(0));
    if (clk.err) {
        return;
    }

    /* lock the bus and power on the SPI peripheral */
    mutex_lock(&lock);
#ifdef PRSPI
    power_spi_enable();
#endif

    /* configure as master, with given mode and clock */
    SPSR = clk.spsr_spi2x;
    SPCR = (1 << SPE | 1 << MSTR | mode | clk.spcr_spr);

    /* clear interrupt flag by reading SPSR and data register by reading SPDR */
    (void)SPSR;
    (void)SPDR;
}

void spi_release(spi_t bus)
{
    (void)bus;
    /* power off and release the bus */
    SPCR &= ~(1 << SPE);
#ifdef PRSPI
    power_spi_disable();
#endif
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

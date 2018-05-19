/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_lpc2387
 * @ingroup     drivers_periph_spi
 * @{
 *
 * @file
 * @brief       Low-level SPI driver implementation
 *
 * This implementation is very basic and only supports a single SPI device with
 * limited configuration options.
 *
 * @todo        This implementation needs a major rework
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "mutex.h"
#include "assert.h"
#include "periph/spi.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define SPI_TX_EMPTY                (SSP0SR & SSPSR_TFE)
#define SPI_BUSY                    (SSP0SR & SSPSR_BSY)
#define SPI_RX_AVAIL                (SSP0SR & SSPSR_RNE)

/**
 * @brief Array holding one pre-initialized mutex for each SPI device
 */
static mutex_t lock = MUTEX_INIT;

void spi_init(spi_t bus)
{
    assert(bus == SPI_DEV(0));

    /* configure pins */
    spi_init_pins(bus);
    /*  power off the bus (default is on) */
    PCONP &= ~(PCSSP0);
}

void spi_init_pins(spi_t bus)
{
    (void) bus;

    PINSEL3 |= (BIT8 | BIT9);     /* SCLK */
    PINSEL3 |= (BIT14 | BIT15);   /* MISO */
    PINSEL3 |= (BIT16 | BIT17);   /* MOSI */
}

int spi_acquire(spi_t bus, spi_cs_t cs, spi_mode_t mode, spi_clk_t clk)
{
    (void) bus;
    (void) cs;

    uint32_t pclksel;
    uint32_t cpsr;

    /* only support for mode 0 at the moment */
    if (mode != SPI_MODE_0) {
        return SPI_NOMODE;
    }

    /* lock bus */
    mutex_lock(&lock);
    /*  power on */
    PCONP |= (PCSSP0);
    /* interface setup */
    SSP0CR0 = 7;

    /* configure bus clock */
    lpc2387_pclk_scale(CLOCK_CORECLOCK / 1000, (uint32_t)clk, &pclksel, &cpsr);
    PCLKSEL1 &= ~(BIT10 | BIT11);   /* CCLK to PCLK divider*/
    PCLKSEL1 |= pclksel << 10;
    SSP0CPSR = cpsr;

    /* enable the bus */
    SSP0CR1 |= BIT1;

    /* clear RxFIFO */
    int dummy;
    while (SPI_RX_AVAIL) {         /* while RNE (Receive FIFO Not Empty)...*/
        dummy = SSP0DR;            /* read data*/
    }
    (void) dummy;                  /* to suppress unused-but-set-variable */

    return SPI_OK;
}

void spi_release(spi_t bus)
{
    (void) bus;
    /* disable, power off, and release the bus */
    SSP0CR1 &= ~(BIT1);
    PCONP &= ~(PCSSP0);
    mutex_unlock(&lock);
}

void spi_transfer_bytes(spi_t bus, spi_cs_t cs, bool cont,
                        const void *out, void *in, size_t len)
{
    (void) bus;

    const uint8_t *out_buf = out;
    uint8_t *in_buf = in;

    assert(out_buf || in_buf);

    if (cs != SPI_CS_UNDEF) {
        gpio_clear((gpio_t)cs);
    }

    for (size_t i = 0; i < len; i++) {
        uint8_t tmp = (out_buf) ? out_buf[i] : 0;
        while (!SPI_TX_EMPTY) {}
        SSP0DR = tmp;
        while (SPI_BUSY) {}
        while (!SPI_RX_AVAIL) {}
        tmp = (uint8_t)SSP0DR;
        if (in_buf) {
            in_buf[i] = tmp;
        }
    }

    if ((!cont) && cs != SPI_CS_UNDEF) {
        gpio_set((gpio_t)cs);
    }
}

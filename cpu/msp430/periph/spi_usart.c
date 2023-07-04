/*
 * Copyright (C) 2015-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_msp430_x1xx
 * @ingroup     drivers_periph_spi
 * @{
 *
 * @file
 * @brief       Low-level SPI driver implementation
 *
 * This SPI driver implementation does only support one single SPI device for
 * now. This is sufficient, as most MSP430 x1xx CPU's only support two serial
 * devices - one used as UART and one as SPI.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <assert.h>

#include "cpu.h"
#include "mutex.h"
#include "periph/spi.h"

/**
 * @brief   Mutex for locking the SPI device
 */
static mutex_t spi_lock = MUTEX_INIT;

void spi_init(spi_t bus)
{
    assert((unsigned)bus < SPI_NUMOF);

    /* put SPI device in reset state */
    SPI_BASE->CTL = SWRST;
    SPI_BASE->CTL |= (CHAR | SYNC | MM);
    SPI_BASE->RCTL = 0;
    SPI_BASE->MCTL = 0;
    /* enable SPI mode */
    SPI_SFR->ME |= SPI_ME_BIT;

    /* trigger the pin configuration */
    spi_init_pins(bus);
}

void spi_init_pins(spi_t bus)
{
    (void)bus;

    gpio_periph_mode(SPI_PIN_MISO, true);
    gpio_periph_mode(SPI_PIN_MOSI, true);
    gpio_periph_mode(SPI_PIN_CLK, true);
}

void spi_acquire(spi_t bus, spi_cs_t cs, spi_mode_t mode, spi_clk_t clk)
{
    (void)bus;
    (void)cs;
    assert((unsigned)bus < SPI_NUMOF);
    assert(clk != SPI_CLK_10MHZ);

    /* lock the bus */
    mutex_lock(&spi_lock);

    /* calculate baudrate */
    uint32_t br = msp430_submain_clock_freq() / clk;
    /* make sure the is not smaller then 2 */
    if (br < 2) {
        br = 2;
    }
    SPI_BASE->BR0 = (uint8_t)br;
    SPI_BASE->BR1 = (uint8_t)(br >> 8);

    /* configure bus mode */
    /* configure mode */
    SPI_BASE->TCTL = (UXTCTL_SSEL_SMCLK | STC | mode);
    /* release from software reset */
    SPI_BASE->CTL &= ~(SWRST);
}

void spi_release(spi_t bus)
{
    (void)bus;
    /* put SPI device back in reset state */
    SPI_BASE->CTL |= SWRST;

    /* release the bus */
    mutex_unlock(&spi_lock);
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

    /* if we only send out data, we do this the fast way... */
    if (!in_buf) {
        for (size_t i = 0; i < len; i++) {
            while (!(SPI_SFR->IFG & SPI_IE_TX_BIT)) {}
            SPI_BASE->TXBUF = out_buf[i];
        }
        /* finally we need to wait, until all transfers are complete */
        while (!(SPI_SFR->IFG & SPI_IE_TX_BIT) || !(SPI_SFR->IFG & SPI_IE_RX_BIT)) {}
        SPI_BASE->RXBUF;
    }
    else if (!out_buf) {
        for (size_t i = 0; i < len; i++) {
            SPI_BASE->TXBUF = 0;
            while (!(SPI_SFR->IFG & SPI_IE_RX_BIT)) {}
            in_buf[i] = (char)SPI_BASE->RXBUF;
        }
    }
    else {
        for (size_t i = 0; i < len; i++) {
            while (!(SPI_SFR->IFG & SPI_IE_TX_BIT)) {}
            SPI_BASE->TXBUF = out_buf[i];
            while (!(SPI_SFR->IFG & SPI_IE_RX_BIT)) {}
            in_buf[i] = (char)SPI_BASE->RXBUF;
        }
    }

    if ((!cont) && (cs != SPI_CS_UNDEF)) {
        gpio_set((gpio_t)cs);
    }
}

/*
 * Copyright (C) 2015-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_msp430fxyz
 * @{
 *
 * @file
 * @brief       Low-level SPI driver implementation
 *
 * This SPI driver implementation does only support one single SPI device for
 * now. This is sufficient, as most MSP430 CPU's only support two serial
 * devices - one used as UART and one as SPI.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "mutex.h"
#include "assert.h"
#include "periph/spi.h"

/**
 * @brief   Mutex for locking the SPI device
 */
static mutex_t spi_lock = MUTEX_INIT;


void spi_init(spi_t bus)
{
    assert(bus <= SPI_NUMOF);

/* we need to differentiate between the legacy SPI device and USCI */
#ifndef SPI_USE_USCI
    /* put SPI device in reset state */
    SPI_BASE->CTL = USART_CTL_SWRST;
    SPI_BASE->CTL |= (USART_CTL_CHAR | USART_CTL_SYNC | USART_CTL_MM);
    SPI_BASE->RCTL = 0;
    SPI_BASE->MCTL = 0;
    /* enable SPI mode */
    SPI_ME |= SPI_ME_BIT;
#else
    /* reset SPI device */
    SPI_BASE->CTL1 = USCI_SPI_CTL1_SWRST;
    SPI_BASE->CTL1 |= (USCI_SPI_CTL1_SSEL_SMCLK);
#endif

    /* trigger the pin configuration */
    spi_init_pins(bus);
}

void spi_init_pins(spi_t bus)
{
    gpio_periph_mode(SPI_PIN_MISO, true);
    gpio_periph_mode(SPI_PIN_MOSI, true);
    gpio_periph_mode(SPI_PIN_CLK, true);
}

int spi_acquire(spi_t bus, spi_cs_t cs, spi_mode_t mode, spi_clk_t clk)
{
    if (clk == SPI_CLK_10MHZ) {
        return SPI_NOCLK;
    }

    /* lock the bus */
    mutex_lock(&spi_lock);

    /* calculate baudrate */
    uint32_t br = CLOCK_CMCLK / clk;
    /* make sure the is not smaller then 2 */
    if (br < 2) {
        br = 2;
    }
    SPI_BASE->BR0 = (uint8_t)br;
    SPI_BASE->BR1 = (uint8_t)(br >> 8);

    /* configure bus mode */
#ifndef SPI_USE_USCI
    /* configure mode */
    SPI_BASE->TCTL = (USART_TCTL_SSEL_SMCLK | USART_TCTL_STC | mode);
    /* release from software reset */
    SPI_BASE->CTL &= ~(USART_CTL_SWRST);
#else
    /* configure mode */
    SPI_BASE->CTL0 = (USCI_SPI_CTL0_UCSYNC | USCI_SPI_CTL0_MST|
                     USCI_SPI_CTL0_MODE_0 | USCI_SPI_CTL0_MSB | mode);
    /* release from software reset */
    SPI_BASE->CTL1 &= ~(USCI_SPI_CTL1_SWRST);
#endif

    return SPI_OK;
}

void spi_release(spi_t dev)
{
    /* put SPI device back in reset state */
#ifndef SPI_USE_USCI
    SPI_BASE->CTL |= (USART_CTL_SWRST);
#else
    SPI_BASE->CTL1 |= (USCI_SPI_CTL1_SWRST);
#endif

    /* release the bus */
    mutex_unlock(&spi_lock);
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

    /* if we only send out data, we do this the fast way... */
    if (!in_buf) {
        for (size_t i = 0; i < len; i++) {
            while (!(SPI_IF & SPI_IE_TX_BIT)) {}
            SPI_BASE->TXBUF = (uint8_t)out_buf[i];
        }
        /* finally we need to wait, until all transfers are complete */
#ifndef SPI_USE_USCI
        while (!(SPI_IF & SPI_IE_TX_BIT) || !(SPI_IF & SPI_IE_RX_BIT)) {}
#else
        while (SPI_BASE->STAT & USCI_SPI_STAT_UCBUSY) {}
#endif
        SPI_BASE->RXBUF;
    }
    else if (!out_buf) {
        for (size_t i = 0; i < len; i++) {
            SPI_BASE->TXBUF = 0;
            while (!(SPI_IF & SPI_IE_RX_BIT)) {}
            in_buf[i] = (char)SPI_BASE->RXBUF;
        }
    }
    else {
        for (size_t i = 0; i < len; i++) {
            while (!(SPI_IF & SPI_IE_TX_BIT)) {}
            SPI_BASE->TXBUF = out_buf[i];
            while (!(SPI_IF & SPI_IE_RX_BIT)) {}
            in_buf[i] = (char)SPI_BASE->RXBUF;
        }
    }

    if ((!cont) && (cs != SPI_CS_UNDEF)) {
        gpio_set((gpio_t)cs);
    }
}

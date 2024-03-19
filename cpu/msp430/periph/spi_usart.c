/*
 * Copyright (C) 2015-2016 Freie Universität Berlin
 *               2023 Otto-von-Guericke-Universität Magdeburg
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
 * @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>
 *
 * @}
 */

#include <assert.h>

#include "compiler_hints.h"
#include "periph/spi.h"
#include "periph_cpu.h"

void spi_init(spi_t bus)
{
    assume((unsigned)bus < SPI_NUMOF);

    /* trigger the pin configuration */
    spi_init_pins(bus);
}

void spi_init_pins(spi_t bus)
{
    assume((unsigned)bus < SPI_NUMOF);

    const msp430_usart_spi_params_t *params = spi_config[bus].spi;

    /* set output GPIOs to idle levels of the peripheral */
    gpio_set(params->mosi);
    gpio_clear(params->sck);

    /* configure the pins as GPIOs, not attaching to the peripheral as of now */
    gpio_init(params->miso, GPIO_IN);
    gpio_init(params->mosi, GPIO_OUT);
    gpio_init(params->sck, GPIO_OUT);
}

void spi_acquire(spi_t bus, spi_cs_t cs, spi_mode_t mode, spi_clk_t clk)
{
    assume((unsigned)bus < SPI_NUMOF);
    (void)cs;

    const msp430_usart_spi_params_t *params = spi_config[bus].spi;
    msp430_usart_t *dev = params->usart_params.dev;

    msp430_usart_conf_t conf = {
        .prescaler = msp430_usart_prescale(clk, USART_MIN_BR_SPI),
        .ctl = CHAR | SYNC | MM,
    };
    /* get exclusive access to the USART (this will also indirectly ensure
     * exclusive SPI bus access */
    msp430_usart_acquire(&params->usart_params, &conf, params->enable_mask);

    /* clock and phase are encoded in mode so that they can be directly be
     * written into TCTL. TCTL has been initialized by
     * msp430_usart_acquire(), so we don't need to wipe any previous clock
     * phase or polarity state.
     *
     * STC disables "multi-master" mode, in which the STE pin would be connected
     * to the CS output of any other SPI controller */
    dev->TCTL |= STC | mode;

    /* release from software reset */
    dev->CTL &= ~(SWRST);

    /* attach the pins only now after the peripheral is up and running, as
     * otherwise noise is send out (could be observed on SCK with a logic
     * analyzer). */
    gpio_periph_mode(params->miso, true);
    gpio_periph_mode(params->mosi, true);
    gpio_periph_mode(params->sck, true);
}

void spi_release(spi_t bus)
{
    assume((unsigned)bus < SPI_NUMOF);
    const msp430_usart_spi_params_t *params = spi_config[bus].spi;

    /* release the pins to avoid sending noise while the peripheral is
     * reconfigured or used to provide other interfaces */
    gpio_periph_mode(params->miso, false);
    gpio_periph_mode(params->mosi, false);
    gpio_periph_mode(params->sck, false);

    /* release the peripheral */
    msp430_usart_release(&params->usart_params);
}

void spi_transfer_bytes(spi_t bus, spi_cs_t cs, bool cont,
                        const void *out, void *in, size_t len)
{
    assume((unsigned)bus < SPI_NUMOF);
    const msp430_usart_spi_params_t *params = spi_config[bus].spi;
    const msp430_usart_params_t *usart = &params->usart_params;
    msp430_usart_t *dev = params->usart_params.dev;

    const uint8_t *out_buf = out;
    uint8_t *in_buf = in;

    assert(out_buf || in_buf);

    if (cs != SPI_CS_UNDEF) {
        gpio_clear((gpio_t)cs);
    }

    /* if we only send out data, we do this the fast way... */
    if (!in_buf) {
        for (size_t i = 0; i < len; i++) {
            while (!msp430_usart_get_tx_irq_flag(usart)) {
                /* still busy waiting for TX to complete */
            }
            dev->TXBUF = out_buf[i];
        }
        /* finally we need to wait, until all transfers are complete */
        while (!msp430_usart_are_both_irq_flags_set(usart)) {
            /* still either TX, or RX, or both not completed */
        }
        (void)dev->RXBUF;
    }
    else if (!out_buf) {
        for (size_t i = 0; i < len; i++) {
            dev->TXBUF = 0;
            while (!msp430_usart_get_rx_irq_flag(usart)) {
                /* still busy waiting for RX to complete */
            }
            in_buf[i] = dev->RXBUF;
        }
    }
    else {
        for (size_t i = 0; i < len; i++) {
            while (!msp430_usart_get_tx_irq_flag(usart)) {
                /* still busy waiting for TX to complete */
            }
            dev->TXBUF = out_buf[i];
            while (!msp430_usart_get_rx_irq_flag(usart)) {
                /* still busy waiting for RX to complete */
            }
            in_buf[i] = dev->RXBUF;
        }
    }

    if ((!cont) && (cs != SPI_CS_UNDEF)) {
        gpio_set((gpio_t)cs);
    }
}

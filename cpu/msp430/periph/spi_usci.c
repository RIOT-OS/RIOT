/*
 * Copyright (C) 2015-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_msp430_f2xx_g2xx
 * @ingroup     drivers_periph_spi
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

#include <assert.h>

#include "compiler_hints.h"
#include "cpu.h"
#include "mutex.h"
#include "periph/spi.h"

/* caching most recently used SPI configuration */
static msp430_usci_conf_t _confs[SPI_NUMOF];
/* clock frequency for which the SPI configuration was cached */
static uint32_t _conf_clks[SPI_NUMOF];
/* the USCI is using a mutex already, but we need to guard the access to the
 * configuration cache with a mutex as well */
static mutex_t _locks[SPI_NUMOF];

void spi_init(spi_t bus)
{
    assume((unsigned)bus < SPI_NUMOF);
    /* `spi_init()` should be done only once during boot-up */
    assert(_conf_clks[bus] == 0);

    /* spi_init_pins() unlocks a mutex, so initializing it locked here */
    _locks[bus] = (mutex_t)MUTEX_INIT_LOCKED;

    /* trigger the pin configuration */
    spi_init_pins(bus);
}

void spi_init_pins(spi_t bus)
{
    assume((unsigned)bus < SPI_NUMOF);
    const msp430_usci_spi_params_t *params = spi_config[bus].spi;

    gpio_init(params->miso, GPIO_IN);
    gpio_periph_mode(params->miso, true);
    gpio_init(params->mosi, GPIO_OUT);
    gpio_periph_mode(params->mosi, true);
    gpio_init(params->sck, GPIO_OUT);
    gpio_periph_mode(params->sck, true);

    mutex_unlock(&_locks[bus]);
}

void spi_deinit_pins(spi_t bus)
{
    assume((unsigned)bus < SPI_NUMOF);
    const msp430_usci_spi_params_t *params = spi_config[bus].spi;
    mutex_lock(&_locks[bus]);

    gpio_periph_mode(params->miso, true);
    gpio_periph_mode(params->mosi, true);
    gpio_periph_mode(params->sck, true);
}

void spi_acquire(spi_t bus, spi_cs_t cs, spi_mode_t mode, spi_clk_t clk)
{
    assume((unsigned)bus < SPI_NUMOF);
    (void)cs;
    const msp430_usci_params_t *usci = &spi_config[bus].spi->usci_params;

    mutex_lock(&_locks[bus]);

    if (_conf_clks[bus] != clk) {
        /* prescaler not cached, recomputing */
        _confs[bus].prescaler = msp430_usci_prescale(clk);
        /* no modulation setting in SPI mode */
        _confs[bus].prescaler.mctl = 0;
    }

    /* Repopulate CTL0 register settings every time. This is rather cheap
     * compared to the prescaler configuration and allows reusing the prescaler
     * across SPI modes */
    _confs[bus].ctl0 = UCSYNC | UCMST | UCMODE_0 | UCMSB | mode;

    msp430_usci_acquire(usci, &_confs[bus]);

    /* finally, pull USCI out of reset */
    usci->dev->CTL1 &= ~UCSWRST;
}

void spi_release(spi_t bus)
{
    assume((unsigned)bus < SPI_NUMOF);
    /* unlock in reverse order */
    msp430_usci_release(&spi_config[bus].spi->usci_params);
    mutex_unlock(&_locks[bus]);
}

void spi_transfer_bytes(spi_t bus, spi_cs_t cs, bool cont,
                        const void *out, void *in, size_t len)
{
    assume((unsigned)bus < SPI_NUMOF);

    const uint8_t *out_buf = out;
    uint8_t *in_buf = in;
    const msp430_usci_spi_params_t *params = spi_config[bus].spi;
    const msp430_usci_params_t *usci = &params->usci_params;

    assert(out_buf || in_buf);

    if (cs != SPI_CS_UNDEF) {
        gpio_clear((gpio_t)cs);
    }

    /* if we only send out data, we do this the fast way... */
    if (!in_buf) {
        for (size_t i = 0; i < len; i++) {
            while (!(*usci->interrupt_flag & usci->tx_irq_mask)) { }
            usci->dev->TXBUF = out_buf[i];
        }

        /* finally we need to wait, until all transfers are complete */
        while (usci->dev->STAT & UCBUSY) {}
        (void)usci->dev->RXBUF;
    }
    else if (!out_buf) {
        for (size_t i = 0; i < len; i++) {
            usci->dev->TXBUF = 0;
            while (!(*usci->interrupt_flag & usci->rx_irq_mask)) { }
            in_buf[i] = usci->dev->RXBUF;
        }
    }
    else {
        for (size_t i = 0; i < len; i++) {
            while (!(*usci->interrupt_flag & usci->tx_irq_mask)) { }
            usci->dev->TXBUF = out_buf[i];
            while (!(*usci->interrupt_flag & usci->rx_irq_mask)) { }
            in_buf[i] = usci->dev->RXBUF;
        }
    }

    if ((!cont) && (cs != SPI_CS_UNDEF)) {
        gpio_set((gpio_t)cs);
    }
}

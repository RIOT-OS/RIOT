/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_lpc23xx
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

#include <assert.h>

#include "cpu.h"
#include "mutex.h"
#include "periph/spi.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* DIV_UP is division which rounds up instead of down */
#define SPI_DIV_UP(a, b)    (((a) + ((b) - 1)) / (b))

/**
 * @brief   Get the pointer to the base register of the given SPI device
 *
 * @param[in] dev       SPI device identifier
 *
 * @return              base register address
 */
static inline lpc23xx_spi_t *get_dev(spi_t dev)
{
    return spi_config[dev].dev;
}

/**
 * @brief Array holding one pre-initialized mutex for each SPI device
 */
static mutex_t lock[SPI_NUMOF];

static void _power_off(spi_t bus)
{
    switch ((uint32_t) get_dev(bus)) {
    case SSP0_BASE_ADDR:
        PCONP &= ~PCSSP0;
        break;
    case SSP1_BASE_ADDR:
        PCONP &= ~PCSSP1;
        break;
    }
}

static void _power_on(spi_t bus)
{
    switch ((uint32_t) get_dev(bus)) {
    case SSP0_BASE_ADDR:
        PCONP |= PCSSP0;
        break;
    case SSP1_BASE_ADDR:
        PCONP |= PCSSP1;
        break;
    }
}

void spi_init(spi_t bus)
{
    assert(bus < SPI_NUMOF);

    /* configure pins */
    spi_init_pins(bus);

    /*  power off the bus (default is on) */
    _power_off(bus);
}

void spi_init_pins(spi_t bus)
{
    const spi_conf_t *cfg = &spi_config[bus];

    *(&PINSEL0 + cfg->pinsel_mosi) |= cfg->pinsel_msk_mosi;
    *(&PINSEL0 + cfg->pinsel_miso) |= cfg->pinsel_msk_miso;
    *(&PINSEL0 + cfg->pinsel_clk) |= cfg->pinsel_msk_clk;
}

spi_clk_t spi_get_clk(spi_t bus, uint32_t freq)
{
    (void)bus;

    /* CLOCK_CORECLOCK / (1 << 0..3) / 2..254 and even */

    uint32_t pclksel, cpsr, source_clock = CLOCK_CORECLOCK / 2;

    /* bound divider from 2 to 2032 */
    if (freq > source_clock) {
        freq = source_clock;
    }
    assert(freq >= SPI_DIV_UP(source_clock, 2032));

    /* result must be at most the requested frequency */
    freq = CLOCK_CORECLOCK / SPI_DIV_UP(CLOCK_CORECLOCK, freq);

    lpc23xx_pclk_scale(CLOCK_CORECLOCK, freq, &pclksel, &cpsr);

    return (pclksel<<10) | cpsr;
}

uint32_t spi_get_freq(spi_t bus, spi_clk_t clk)
{
    (void)bus;

    uint32_t pclksel, pclkdiv, cpsr;

    pclksel = clk >> 8;
    cpsr = clk & 0xff;

    switch (pclksel) {
        case 0: pclkdiv = 4; break;
        case 1: pclkdiv = 1; break;
        case 2: pclkdiv = 2; break;
        case 3: pclkdiv = 8; break;
        default: pclkdiv = 4; break;
    }
    return CLOCK_CORECLOCK / pclkdiv / cpsr;
}

void spi_acquire(spi_t bus, spi_cs_t cs, spi_mode_t mode, spi_clk_t clk)
{
    (void)cs; (void)mode;
    assert((unsigned)bus < SPI_NUMOF);
    assert(mode == SPI_MODE_0);

    uint32_t pclksel;
    uint32_t cpsr;

    lpc23xx_spi_t *dev = get_dev(bus);

    /* lock bus */
    mutex_lock(&lock[bus]);

    /*  power on */
    _power_on(bus);

    /* interface setup */
    dev->CR0 = 7;

    /* configure bus clock */
    pclksel = clk >> 8;
    cpsr = clk & 0xff;

    switch ((uint32_t)dev) {
    case SSP0_BASE_ADDR:
        PCLKSEL1 &= ~(BIT10 | BIT11);   /* CCLK to PCLK divider*/
        PCLKSEL1 |= pclksel << 10;
        break;
    case SSP1_BASE_ADDR:
        PCLKSEL0 &= ~(BIT20 | BIT21);   /* CCLK to PCLK divider*/
        PCLKSEL0 |= pclksel << 20;
        break;
    }

    dev->CPSR = cpsr;

    /* enable the bus */
    dev->CR1 |= BIT1;

    /* clear RxFIFO */
    while (dev->SR & SSPSR_RNE) {       /* while RNE (Receive FIFO Not Empty)...*/
        dev->DR;                        /* read data*/
    }
}

void spi_release(spi_t bus)
{
    lpc23xx_spi_t *dev = get_dev(bus);

    /* disable, power off, and release the bus */
    dev->CR1 &= ~BIT1;

    _power_off(bus);

    mutex_unlock(&lock[bus]);
}

void spi_transfer_bytes(spi_t bus, spi_cs_t cs, bool cont,
                        const void *out, void *in, size_t len)
{
    const uint8_t *out_buf = out;
    uint8_t *in_buf = in;

    lpc23xx_spi_t *dev = get_dev(bus);

    assert(out_buf || in_buf);

    if (cs != SPI_CS_UNDEF) {
        gpio_clear((gpio_t)cs);
    }

    for (size_t i = 0; i < len; i++) {
        uint8_t tmp = (out_buf) ? out_buf[i] : 0;
        /* wait for TX buffer empty */
        while (!(dev->SR & SSPSR_TFE)) {}
        dev->DR = tmp;
        while (dev->SR & SSPSR_BSY) {}
        /* wait for RX not empty */
        while (!(dev->SR & SSPSR_RNE)) {}
        tmp = (uint8_t)dev->DR;
        if (in_buf) {
            in_buf[i] = tmp;
        }
    }

    if ((!cont) && cs != SPI_CS_UNDEF) {
        gpio_set((gpio_t)cs);
    }
}

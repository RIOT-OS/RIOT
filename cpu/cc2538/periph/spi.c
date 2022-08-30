/*
 * Copyright (C) 2015 Loci Controls Inc.
 *               2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cc2538
 * @ingroup     drivers_periph_spi
 * @{
 *
 * @file
 * @brief       Low-level SPI driver implementation
 *
 * @author      Ian Martin <ian@locicontrols.com>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <assert.h>

#include "vendor/hw_memmap.h"
#include "vendor/hw_ssi.h"

#include "cpu.h"
#include "mutex.h"
#include "periph/spi.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* DIV_UP is division which rounds up instead of down */
#define SPI_DIV_UP(a, b)    (((a) + ((b) - 1)) / (b))

/**
 * @brief   Array holding one pre-initialized mutex for each SPI device
 */
static mutex_t locks[SPI_NUMOF];

static inline cc2538_ssi_t *dev(spi_t bus)
{
    /* .num is either 0 or 1, return respective base address */
    return (spi_config[bus].num) ? (cc2538_ssi_t *)SSI1_BASE : (cc2538_ssi_t *)SSI0_BASE;
}

static inline void poweron(spi_t bus)
{
    SYS_CTRL_RCGCSSI |= (1 << spi_config[bus].num);
    SYS_CTRL_SCGCSSI |= (1 << spi_config[bus].num);
    SYS_CTRL_DCGCSSI |= (1 << spi_config[bus].num);
}

static inline void poweroff(spi_t bus)
{
    SYS_CTRL_RCGCSSI &= ~(1 << spi_config[bus].num);
    SYS_CTRL_SCGCSSI &= ~(1 << spi_config[bus].num);
    SYS_CTRL_DCGCSSI &= ~(1 << spi_config[bus].num);
}

void spi_init(spi_t bus)
{
    DEBUG("%s: bus=%u\n", __FUNCTION__, bus);

    assert(bus < SPI_NUMOF);

    /* init mutex for given bus */
    mutex_init(&locks[bus]);
    /* temporarily power on the device */
    poweron(bus);
    /* configure device to be a master and disable SSI operation mode */
    dev(bus)->CR1 = 0;
    /* configure system clock as SSI clock source */
    dev(bus)->CC = SSI_CC_CS_IODIV;
    /* and power off the bus again */
    poweroff(bus);

    /* trigger SPI pin configuration */
    spi_init_pins(bus);
}

void spi_init_pins(spi_t bus)
{
    DEBUG("%s: bus=%u\n", __FUNCTION__, bus);
    /* select values according to SPI device */
    cc2538_ioc_sel_t txd = spi_config[bus].num ? SSI1_TXD : SSI0_TXD;
    cc2538_ioc_sel_t clk = spi_config[bus].num ? SSI1_CLK_OUT : SSI0_CLK_OUT;
    cc2538_ioc_sel_t fss = spi_config[bus].num ? SSI1_FSS_OUT : SSI0_FSS_OUT;
    cc2538_ioc_pin_t rxd = spi_config[bus].num ? SSI1_RXD : SSI0_RXD;
    /* init pin functions and multiplexing */
    gpio_init_mux(spi_config[bus].mosi_pin, OVERRIDE_ENABLE,  txd, GPIO_MUX_NONE);
    gpio_init_mux(spi_config[bus].sck_pin,  OVERRIDE_ENABLE,  clk, GPIO_MUX_NONE);
    gpio_init_mux(spi_config[bus].cs_pin,   OVERRIDE_ENABLE,  fss, GPIO_MUX_NONE);
    gpio_init_mux(spi_config[bus].miso_pin, OVERRIDE_DISABLE, GPIO_MUX_NONE, rxd);
}

spi_clk_t spi_get_clk(spi_t bus, uint32_t freq)
{
    (void)bus;

    /* SPI bus frequency =  CLOCK_CORECLOCK / (CPSR * (SCR + 1)), with
     * CPSR = 2..254 and even,
     *  SCR = 0..255 */

    /* bound divider from 2 to 65024 (254 * (255 + 1)) */
    if (freq > CLOCK_CORECLOCK / 2) {
        freq = CLOCK_CORECLOCK / 2;
    }
    assert(freq >= CLOCK_CORECLOCK / 65024);

    uint8_t cpsr = 2, scr = 0;
    uint32_t divider = SPI_DIV_UP(CLOCK_CORECLOCK, freq);
    if (divider % 2) {
        divider++;
    }
    while (divider / cpsr > 256) {
        cpsr += 2;
    }
    scr = divider / cpsr - 1;

    return (cpsr << 8) | scr;
}

uint32_t spi_get_freq(spi_t bus, spi_clk_t clk)
{
    (void)bus;
    return CLOCK_CORECLOCK / ((clk >> 8) * ((clk & 0xff) + 1));
}

void spi_acquire(spi_t bus, spi_cs_t cs, spi_mode_t mode, spi_clk_t clk)
{
    assert((unsigned)bus < SPI_NUMOF);
    DEBUG("%s: bus=%u\n", __FUNCTION__, bus);
    (void)cs;
    assert((unsigned)bus < SPI_NUMOF);
    /* lock the bus */
    mutex_lock(&locks[bus]);
    /* power on device */
    poweron(bus);
    /* configure SCR clock field, data-width and mode */
    dev(bus)->CR0 = 0;
    dev(bus)->CPSR = clk >> 8;
    dev(bus)->CR0 = ((clk & 0xff) << 8) | mode | SSI_CR0_DSS(8);
    /* enable SSI device */
    dev(bus)->CR1 = SSI_CR1_SSE;
}

void spi_release(spi_t bus)
{
    DEBUG("%s: bus=%u\n", __FUNCTION__, bus);
    /* disable and power off device */
    dev(bus)->CR1 = 0;
    poweroff(bus);
    /* and release lock... */
    mutex_unlock(&locks[bus]);
}

static uint8_t _trx(cc2538_ssi_t *dev, uint8_t in)
{
    while (!(dev->SR & SSI_SR_TNF)) {}
    dev->DR = in;
    while (!(dev->SR & SSI_SR_RNE)) {}
    return dev->DR;
}

void spi_transfer_bytes(spi_t bus, spi_cs_t cs, bool cont,
                        const void *out, void *in, size_t len)
{
    DEBUG("%s: bus=%u, len=%u\n", __FUNCTION__, bus, (unsigned)len);

    const uint8_t *out_buf = out;
    uint8_t *in_buf = in;

    assert(out_buf || in_buf);

    if (cs != SPI_CS_UNDEF) {
        gpio_clear((gpio_t)cs);
    }

    if (!in_buf) {
        for (const void *end = out_buf + len; out_buf != end; ++out_buf) {
            _trx(dev(bus), *out_buf);
        }
    }
    else if (!out_buf) {
        for (void *end = in_buf + len; in_buf != end; ++in_buf) {
            *in_buf = _trx(dev(bus), 0);
        }
    }
    else {
        for (void *end = in_buf + len; in_buf != end; ++in_buf, ++out_buf) {
            *in_buf = _trx(dev(bus), *out_buf);
        }
    }

    if ((!cont) && (cs != SPI_CS_UNDEF)) {
        gpio_set((gpio_t)cs);
    }
}

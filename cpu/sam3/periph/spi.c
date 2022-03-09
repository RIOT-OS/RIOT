/*
* Copyright (C) 2014 Hamburg University of Applied Sciences
*               2016-2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
*/

/**
 * @ingroup     cpu_sam3
 * @ingroup     drivers_periph_spi
 * @{
 *
 * @file
 * @brief       Low-level SPI driver implementation
 *
 * @author      Maxime Blanloeil <maxime.blanloeil@phelma.grenoble-inp.fr>
 * @author      Peter Kietzmann  <peter.kietzmann@haw-hamburg.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include <assert.h>

#include "cpu.h"
#include "mutex.h"
#include "periph/gpio.h"
#include "periph/spi.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief   Array holding one pre-initialized mutex for each SPI device
 */
static mutex_t locks[SPI_NUMOF];

static inline Spi *dev(spi_t bus)
{
    return spi_config[bus].dev;
}

void spi_init(spi_t bus)
{
    assert(bus < SPI_NUMOF);

    /* initialize device lock */
    mutex_init(&locks[bus]);
    /* initialize pins */
    spi_init_pins(bus);
}

void spi_init_pins(spi_t bus)
{
    gpio_init(spi_config[bus].clk, GPIO_OUT);
    gpio_init(spi_config[bus].mosi, GPIO_OUT);
    gpio_init(spi_config[bus].miso, GPIO_IN);
    gpio_init_mux(spi_config[bus].clk, spi_config[bus].mux);
    gpio_init_mux(spi_config[bus].mosi, spi_config[bus].mux);
    gpio_init_mux(spi_config[bus].miso, spi_config[bus].mux);
}

void spi_acquire(spi_t bus, spi_cs_t cs, spi_mode_t mode, spi_clk_t clk)
{
    (void)cs;
    assert((unsigned)bus < SPI_NUMOF);

    /* lock bus */
    mutex_lock(&locks[bus]);
    /* enable SPI device clock */
    PMC->PMC_PCER0 |= (1 << spi_config[bus].id);
    /* set mode and speed */
    dev(bus)->SPI_CSR[0] = (SPI_CSR_SCBR(CLOCK_CORECLOCK / clk) | mode);
    dev(bus)->SPI_MR = (SPI_MR_MSTR | SPI_MR_MODFDIS);
    dev(bus)->SPI_CR = SPI_CR_SPIEN;
}

void spi_release(spi_t bus)
{
    /* disable device and turn off clock signal */
    dev(bus)->SPI_CR = 0;
    PMC->PMC_PCER0 &= ~(1 << spi_config[bus].id);
    /* release device lock */
    mutex_unlock(&locks[bus]);
}

void spi_transfer_bytes(spi_t bus, spi_cs_t cs, bool cont,
                        const void *out, void *in, size_t len)
{
    const uint8_t *out_buf = out;
    uint8_t *in_buf = in;

    assert(in_buf || out_buf);

    if (cs != SPI_CS_UNDEF) {
        gpio_clear((gpio_t)cs);
    }

    if (!in_buf) {
        for (size_t i = 0; i < len; i++) {
            while(!(dev(bus)->SPI_SR & SPI_SR_TDRE)) {}
            dev(bus)->SPI_TDR = out_buf[i];
        }
        while (!(dev(bus)->SPI_SR & SPI_SR_RDRF)) {}
        dev(bus)->SPI_RDR;
    }
    else if (!out_buf) {
        for (size_t i = 0; i < len; i++) {
            dev(bus)->SPI_TDR = 0;
            while (!(dev(bus)->SPI_SR & SPI_SR_RDRF)) {}
            in_buf[i] = dev(bus)->SPI_RDR;
        }
    }
    else {
        for (size_t i = 0; i < len; i++) {
            while (!(dev(bus)->SPI_SR & SPI_SR_TDRE));
            dev(bus)->SPI_TDR = out_buf[i];
            while (!(dev(bus)->SPI_SR & SPI_SR_RDRF));
            in_buf[i] = dev(bus)->SPI_RDR;
        }
    }

    if ((!cont) && (cs != SPI_CS_UNDEF)) {
        gpio_set((gpio_t)cs);
    }
}

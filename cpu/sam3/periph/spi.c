/*
* Copyright (C) 2014 Hamburg University of Applied Sciences
*               2016 Freie Universität Berlin
*
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
*/

/**
 * @ingroup     cpu_sam3x8e
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

#include "cpu.h"
#include "mutex.h"
#include "periph/spi.h"
#include "periph/gpio.h"

/**
 * @brief   Array holding one pre-initialized mutex for each SPI device
 */
static mutex_t locks[] =  {
#if SPI_0_EN
    [SPI_0] = MUTEX_INIT,
#endif
#if SPI_1_EN
    [SPI_1] = MUTEX_INIT,
#endif
#if SPI_2_EN
    [SPI_2] = MUTEX_INIT
#endif
};

static inline Spi *dev(spi_t bus)
{
    return spi_config[bus].dev;
}

static inline clk_en(spi_t bus)
{
    PMC->PMC_PCER0 |= (1 << spi_config[bus].id);
}

static inline clk_dis(spi_t bus)
{
    PMC->PMC_PCER0 &= ~(1 << spi_config[bus].id);
}


int spi_init_pins(spi_t bus, spi_cs_t cs)
{
    if ((bus >= SPI_NUMOF) || (cs == SPI_CS_UNDEF)) {
        return -1;
    }

    gpio_init(spi_config[bus].clk, GPIO_OUT);
    gpio_init(spi_config[bus].mosi, GPIO_OUT);
    gpio_init(spi_config[bus].miso, GPIO_IN);
    gpio_init_mux(spi_config[bus].clk, spi_config[bus].mux);
    gpio_init_mux(spi_config[bus].mosi, spi_config[bus].mux);
    gpio_init_mux(spi_config[bus].miso, spi_config[bus].mux);

    gpio_init((gpio_t)cs, GPIO_OUT);
    gpio_set((gpio_t)cs);
}

int spi_acquire(spi_t bus, spi_mode_t mode, spi_clk_t clk, spi_cs_t cs)
{
    if ((bus >= SPI_NUMOF) || (cs == SPI_CS_UNDEF)) {
        return -1;
    }

    /* lock bus */
    mutex_lock(&locks[bus]);
    /* enable SPI device clock */
    clk_en(bus);
    /* set mode and speed */
    dev(bus)->SPI_CSR[0] = (clk | mode);
    dev(bus)->SPI_MR = (SPI_MR_MSTR | SPI_MR_MODFDIS);
    dev(bus)->SPI_CR |= SPI_CR_SPIEN;

    return 0;
}

void spi_release(spi_t bus)
{
    dev(bus)->SPI_CR = 0;
    clk_dis(bus);
    mutex_unlock(&locks[bus]);
}

void spi_transfer_byte(spi_t bus, spi_cs_t cs, bool cont,
                       uint8_t out, uint8_t *in)
{
    spi_transfer_bytes(bus, cs, cont, &out, in, 1);
}

void spi_transfer_bytes(spi_t bus, spi_cs_t cs, bool cont,
                        uint8_t *out, uint8_t *in, size_t len)
{
    gpio_clear((gpio_t)cs);

    for (size_t i = 0; i < len; i++) {
        uint8_t tmp = (out) ? out[i] : 0;

        while (!(dev(bus)->SPI_SR & SPI_SR_TDRE));
        dev(bus)->SPI_TDR = tmp;
        while (!(dev(bus)->SPI_SR & SPI_SR_RDRF));
        tmp = (uint8_t)dev(bus)->SPI_RDR;

        if (in) {
            in[i] = tmp;
        }
    }

    if (!cont) {
        gpio_set((gpio_t)cs);
    }
}

void spi_transfer_reg(spi_t bus, spi_cs_t cs,
                      uint8_t reg, uint8_t out, uint8_t *in)
{
    spi_transfer_bytes(bus, cs, true, &reg, NULL, 1);
    spi_transfer_bytes(bus, cs, false, &out, in, 1);
}

void spi_transfer_regs(spi_t bus, spi_cs_t cs,
                       uint8_t reg, uint8_t *out, uint8_t *in, size_t len)
{
    spi_transfer_bytes(bus, cs, true, &reg, NULL, 1);
    spi_transfer_bytes(bus, cs, false, out, in, len);
}

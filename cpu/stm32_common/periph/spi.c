/*
 * Copyright (C) 2014 Hamburg University of Applied Sciences
 *               2014-2017 Freie Universität Berlin
 *               2016 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32_common
 * @{
 *
 * @file
 * @brief       Low-level SPI driver implementation
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "mutex.h"
#include "assert.h"
#include "periph/spi.h"

/* Remove this ugly guard once we selectively build the periph drivers */
#ifdef SPI_NUMOF

/**
 * @brief   Number of bits to shift the BR value in the CR1 register
 */
#define BR_SHIFT            (3U)

/**
 * @brief   Allocate one lock per SPI device
 */
static mutex_t locks[SPI_NUMOF];

static inline SPI_TypeDef *dev(spi_t bus)
{
    return spi_config[bus].dev;
}

void spi_init(spi_t bus)
{
    assert(bus < SPI_NUMOF);

    /* initialize device lock */
    mutex_init(&locks[bus]);
    /* trigger pin initialization */
    spi_init_pins(bus);

    periph_clk_en(spi_config[bus].apbbus, spi_config[bus].rccmask);
    /* reset configuration */
    dev(bus)->CR1 = 0;
#ifdef SPI_I2SCFGR_I2SE
    dev(bus)->I2SCFGR = 0;
#endif
    /* configure SPI for 8-bit data width */
#ifdef SPI_CR2_FRXTH
    dev(bus)->CR2 = (SPI_CR2_FRXTH | SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2);
#else
    dev(bus)->CR2 = 0;
#endif
    periph_clk_dis(spi_config[bus].apbbus, spi_config[bus].rccmask);
}

void spi_init_pins(spi_t bus)
{
#ifdef CPU_FAM_STM32F1
    gpio_init_af(spi_config[bus].sclk_pin, GPIO_AF_OUT_PP);
    gpio_init_af(spi_config[bus].mosi_pin, GPIO_AF_OUT_PP);
    gpio_init(spi_config[bus].miso_pin, GPIO_IN);
#else
    gpio_init(spi_config[bus].mosi_pin, GPIO_OUT);
    gpio_init(spi_config[bus].miso_pin, GPIO_IN);
    gpio_init(spi_config[bus].sclk_pin, GPIO_OUT);
    gpio_init_af(spi_config[bus].mosi_pin, spi_config[bus].af);
    gpio_init_af(spi_config[bus].miso_pin, spi_config[bus].af);
    gpio_init_af(spi_config[bus].sclk_pin, spi_config[bus].af);
#endif
}

int spi_init_cs(spi_t bus, spi_cs_t cs)
{
    if (bus >= SPI_NUMOF) {
        return SPI_NODEV;
    }
    if (cs == SPI_CS_UNDEF ||
        (((cs & SPI_HWCS_MASK) == SPI_HWCS_MASK) && (cs & ~(SPI_HWCS_MASK)))) {
        return SPI_NOCS;
    }

    if (cs == SPI_HWCS_MASK) {
        if (spi_config[bus].cs_pin == GPIO_UNDEF) {
            return SPI_NOCS;
        }
#ifdef CPU_FAM_STM32F1
        gpio_init_af(spi_config[bus].cs_pin, GPIO_AF_OUT_PP);
#else
        gpio_init(spi_config[bus].cs_pin, GPIO_OUT);
        gpio_init_af(spi_config[bus].cs_pin, spi_config[bus].af);
#endif
    }
    else {
        gpio_init((gpio_t)cs, GPIO_OUT);
        gpio_set((gpio_t)cs);
    }

    return SPI_OK;
}

int spi_acquire(spi_t bus, spi_cs_t cs, spi_mode_t mode, spi_clk_t clk)
{
    assert((clk >= SPI_CLK_100KHZ) && (clk <= SPI_CLK_10MHZ));

    /* lock bus */
    mutex_lock(&locks[bus]);
    /* enable SPI device clock */
    periph_clk_en(spi_config[bus].apbbus, spi_config[bus].rccmask);
    /* enable device */
    uint8_t br = spi_divtable[spi_config[bus].apbbus][clk];
    dev(bus)->CR1 = ((br << BR_SHIFT) | mode | SPI_CR1_MSTR);
    if (cs != SPI_HWCS_MASK) {
        dev(bus)->CR1 |= (SPI_CR1_SSM | SPI_CR1_SSI);
    }
    else {
        dev(bus)->CR2 |= (SPI_CR2_SSOE);
    }

    return SPI_OK;
}

void spi_release(spi_t bus)
{
    /* disable device and release lock */
    dev(bus)->CR1 = 0;
    dev(bus)->CR2 &= ~(SPI_CR2_SSOE);
    periph_clk_dis(spi_config[bus].apbbus, spi_config[bus].rccmask);
    mutex_unlock(&locks[bus]);
}

void spi_transfer_bytes(spi_t bus, spi_cs_t cs, bool cont,
                        const void *out, void *in, size_t len)
{
    uint8_t *inbuf = (uint8_t *)in;
    uint8_t *outbuf = (uint8_t *)out;

    /* make sure at least one input or one output buffer is given */
    assert(outbuf || inbuf);

    /* we need to recast the data register to uint_8 to force 8-bit access */
    volatile uint8_t *DR = (volatile uint8_t*)&(dev(bus)->DR);

    /* active the given chip select line */
    dev(bus)->CR1 |= (SPI_CR1_SPE);     /* this pulls the HW CS line low */
    if ((cs != SPI_HWCS_MASK) && (cs != SPI_CS_UNDEF)) {
        gpio_clear((gpio_t)cs);
    }

    /* transfer data, use shortpath if only sending data */
    if (!inbuf) {
        for (size_t i = 0; i < len; i++) {
            while (!(dev(bus)->SR & SPI_SR_TXE));
            *DR = outbuf[i];
        }
        /* wait until everything is finished and empty the receive buffer */
        while (dev(bus)->SR & SPI_SR_BSY) {}
        while (dev(bus)->SR & SPI_SR_RXNE) {
            dev(bus)->DR;       /* we might just read 2 bytes at once here */
        }
    }
    else if (!outbuf) {
        for (size_t i = 0; i < len; i++) {
            while (!(dev(bus)->SR & SPI_SR_TXE));
            *DR = 0;
            while (!(dev(bus)->SR & SPI_SR_RXNE));
            inbuf[i] = *DR;
        }
    }
    else {
        for (size_t i = 0; i < len; i++) {
            while (!(dev(bus)->SR & SPI_SR_TXE));
            *DR = outbuf[i];
            while (!(dev(bus)->SR & SPI_SR_RXNE));
            inbuf[i] = *DR;
        }
    }

    /* make sure the transfer is completed before continuing, see reference
     * manual(s) -> section 'Disabling the SPI' */
    while (!(dev(bus)->SR & SPI_SR_TXE)) {}
    while (dev(bus)->SR & SPI_SR_BSY) {}

    /* release the chip select if not specified differently */
    if ((!cont) && (cs != SPI_CS_UNDEF)) {
        dev(bus)->CR1 &= ~(SPI_CR1_SPE);    /* pull HW CS line high */
        if (cs != SPI_HWCS_MASK) {
            gpio_set((gpio_t)cs);
        }
    }
}

#endif /* SPI_NUMOF */

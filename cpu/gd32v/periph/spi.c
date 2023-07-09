/*
 * Copyright (C) 2014 Hamburg University of Applied Sciences
 *               2014-2017 Freie Universität Berlin
 *               2016-2017 OTA keys S.A.
 *               2023 Gunar Schorcht <gunar@schorcht.net>
 *               2023 Hugues Larrive
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_gd32v
 * @ingroup     drivers_periph_spi
 * @{
 *
 * @file
 * @brief       Low-level SPI driver implementation
 *
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @author      Hugues Larrive <hugues.larrive@pm.me>
 *
 * @}
 */

#include <assert.h>

#include "bitarithm.h"
#include "cpu.h"
#include "mutex.h"
#include "periph/gpio.h"
#include "periph/spi.h"
#include "pm_layered.h"

#define ENABLE_DEBUG        0
#include "debug.h"

/**
 * @brief   Number of bits to shift the BR value in the CR1 register
 */
#define BR_SHIFT            (3U)
#define BR_MAX              (7U)

#define SPI_CTL1_SETTINGS    0

/**
 * @brief   Allocate one lock per SPI device
 */
static mutex_t locks[SPI_NUMOF];


static inline SPI_Type *dev(spi_t bus)
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

    periph_clk_en(spi_config[bus].apbbus, spi_config[bus].rcumask);

    /* reset configuration */
    dev(bus)->CTL0 = 0;
#ifdef SPI0_I2SCTL_I2SSEL_Msk
    dev(bus)->I2SCTL = 0;
#endif
    dev(bus)->CTL1 = SPI_CTL1_SETTINGS;
    periph_clk_dis(spi_config[bus].apbbus, spi_config[bus].rcumask);
}

void spi_init_pins(spi_t bus)
{
    if (spi_config[bus].sclk_pin == GPIO_PIN(PORT_B, 3) &&
        spi_config[bus].miso_pin == GPIO_PIN(PORT_B, 4) &&
        spi_config[bus].mosi_pin == GPIO_PIN(PORT_B, 5)) {
        /* The remapping periph clock must first be enabled */
        RCU->APB2EN |= RCU_APB2EN_AFEN_Msk;
        /* Then the remap can occur */
        AFIO->PCF0 |= AFIO_PCF0_SPI0_REMAP_Msk;
    }

    if (gpio_is_valid(spi_config[bus].sclk_pin)) {
        gpio_init_af(spi_config[bus].sclk_pin, GPIO_AF_OUT_PP);
    }

    if (gpio_is_valid(spi_config[bus].mosi_pin)) {
        gpio_init_af(spi_config[bus].mosi_pin, GPIO_AF_OUT_PP);
    }

    if (gpio_is_valid(spi_config[bus].miso_pin)) {
        gpio_init(spi_config[bus].miso_pin, GPIO_IN);
    }
}

int spi_init_cs(spi_t bus, spi_cs_t cs)
{
    if (bus >= SPI_NUMOF) {
        return SPI_NODEV;
    }

    if (!gpio_is_valid(cs) ||
        (((cs & SPI_HWCS_MASK) == SPI_HWCS_MASK) && (cs & ~(SPI_HWCS_MASK)))) {
        return SPI_NOCS;
    }

    if (cs == SPI_HWCS_MASK) {
        if (!gpio_is_valid(spi_config[bus].cs_pin)) {
            return SPI_NOCS;
        }
        gpio_init_af(spi_config[bus].cs_pin, GPIO_AF_OUT_PP);
    }
    else {
        gpio_init((gpio_t)cs, GPIO_OUT);
        gpio_set((gpio_t)cs);
    }

    return SPI_OK;
}

#ifdef MODULE_PERIPH_SPI_GPIO_MODE
int spi_init_with_gpio_mode(spi_t bus, const spi_gpio_mode_t* mode)
{
    assert(bus < SPI_NUMOF);

    int ret = 0;

    /* This has no effect on GD32VF103 */
    return ret;
}
#endif

spi_clk_t spi_get_clk(spi_t bus, uint32_t freq)
{
    /* freq = f_pclk / (1 << (BR + 1))
     * BR = 0..7 */
    uint8_t br = 0;
    while (periph_apb_clk(spi_config[bus].apbbus) / (1 << (br + 1)) > freq) {
        br++;
    }
    /* bound divider to 256 */
    if (br > BR_MAX) {
        return (spi_clk_t){ .err = -EDOM };
    }
    return (spi_clk_t){ .clk = (br << BR_SHIFT) };
}

int32_t spi_get_freq(spi_t bus, spi_clk_t clk)
{
    if (clk.err) {
        return -EINVAL;
    }
    return periph_apb_clk(spi_config[bus].apbbus)
            / (1 << ((clk.clk >> BR_SHIFT) + 1));
}

void spi_acquire(spi_t bus, spi_cs_t cs, spi_mode_t mode, spi_clk_t clk)
{
    assert((unsigned)bus < SPI_NUMOF);
    if (clk.err) { return; }

    /* lock bus */
    mutex_lock(&locks[bus]);

    /* block DEEP_SLEEP mode */
    pm_block(GD32V_PM_DEEPSLEEP);

    /* enable SPI device clock */
    periph_clk_en(spi_config[bus].apbbus, spi_config[bus].rcumask);
    /* enable device */
    uint16_t ctl0_settings = clk.clk | mode | SPI0_CTL0_MSTMOD_Msk;
    /* Settings to add to CTL1 in addition to SPI_CTL1_SETTINGS */
    uint16_t ctl1_extra_settings = 0;
    if (cs != SPI_HWCS_MASK) {
        ctl0_settings |= (SPI0_CTL0_SWNSSEN_Msk | SPI0_CTL0_SWNSS_Msk);
    }
    else {
        ctl1_extra_settings = (SPI0_CTL1_NSSDRV_Msk);
    }

    dev(bus)->CTL0 = ctl0_settings;
    /* Only modify CR2 if needed */
    if (ctl1_extra_settings) {
        dev(bus)->CTL1 = (SPI_CTL1_SETTINGS | ctl1_extra_settings);
    }
}

void spi_release(spi_t bus)
{
    /* disable device and release lock */
    dev(bus)->CTL0 = 0;
    dev(bus)->CTL1 = SPI_CTL1_SETTINGS; /* Clear the DMA and SSOE flags */
    periph_clk_dis(spi_config[bus].apbbus, spi_config[bus].rcumask);

    /* unblock DEEP_SLEEP mode */
    pm_unblock(GD32V_PM_DEEPSLEEP);

    mutex_unlock(&locks[bus]);
}

static inline void _wait_for_end(spi_t bus)
{
    /* make sure the transfer is completed before continuing, see reference
     * manual(s) -> section 'Disabling the SPI' */
    while (!(dev(bus)->STAT & SPI0_STAT_TBE_Msk)) {}
    while (dev(bus)->STAT & SPI0_STAT_TRANS_Msk) {}
}

static void _transfer_no_dma(spi_t bus, const void *out, void *in, size_t len)
{
    const uint8_t *outbuf = out;
    uint8_t *inbuf = in;

    /* we need to recast the data register to uint_8 to force 8-bit access */
    volatile uint8_t *DR = (volatile uint8_t*)&(dev(bus)->DATA);

    /* transfer data, use shortpath if only sending data */
    if (!inbuf) {
        for (size_t i = 0; i < len; i++) {
            while (!(dev(bus)->STAT & SPI0_STAT_TBE_Msk)) {}
            *DR = outbuf[i];
        }
        /* wait until everything is finished and empty the receive buffer */
        while (!(dev(bus)->STAT & SPI0_STAT_TBE_Msk)) {}
        while (dev(bus)->STAT & SPI0_STAT_TRANS_Msk) {}
        while (dev(bus)->STAT & SPI0_STAT_RBNE_Msk) {
            dev(bus)->DATA;       /* we might just read 2 bytes at once here */
        }
    }
    else if (!outbuf) {
        for (size_t i = 0; i < len; i++) {
            while (!(dev(bus)->STAT & SPI0_STAT_TBE_Msk)) {}
            *DR = 0;
            while (!(dev(bus)->STAT & SPI0_STAT_RBNE_Msk)) {}
            inbuf[i] = *DR;
        }
    }
    else {
        for (size_t i = 0; i < len; i++) {
            while (!(dev(bus)->STAT & SPI0_STAT_TBE_Msk)) {}
            *DR = outbuf[i];
            while (!(dev(bus)->STAT & SPI0_STAT_RBNE_Msk)) {}
            inbuf[i] = *DR;
        }
    }

    _wait_for_end(bus);
}

void spi_transfer_bytes(spi_t bus, spi_cs_t cs, bool cont,
                        const void *out, void *in, size_t len)
{
    /* make sure at least one input or one output buffer is given */
    assert(out || in);

    /* active the given chip select line */
    dev(bus)->CTL0 |= (SPI0_CTL0_SPIEN_Msk);     /* this pulls the HW CS line low */
    if ((cs != SPI_HWCS_MASK) && gpio_is_valid(cs)) {
        gpio_clear((gpio_t)cs);
    }

    _transfer_no_dma(bus, out, in, len);

    /* release the chip select if not specified differently */
    if ((!cont) && gpio_is_valid(cs)) {
        dev(bus)->CTL0 &= ~(SPI0_CTL0_SPIEN_Msk);    /* pull HW CS line high */
        if (cs != SPI_HWCS_MASK) {
            gpio_set((gpio_t)cs);
        }
    }
}

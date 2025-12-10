/*
 * SPDX-FileCopyrightText: 2014 Hamburg University of Applied Sciences
 * SPDX-FileCopyrightText: 2014-2017 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2016-2017 OTA keys S.A.
 * SPDX-FileCopyrightText: 2023 Gunar Schorcht <gunar@schorcht.net>
 * SPDX-License-Identifier: LGPL-2.1-only
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

/**
 * @brief   Clock configuration cache
 */
static uint32_t clocks[SPI_NUMOF];

/**
 * @brief   Clock divider cache
 */
static uint8_t dividers[SPI_NUMOF];

static inline SPI_Type *dev(spi_t bus)
{
    return spi_config[bus].dev;
}

/**
 * @brief Multiplier for clock divider calculations
 *
 * Makes the divider calculation fixed point
 */
#define SPI_APB_CLOCK_SHIFT          (4U)
#define SPI_APB_CLOCK_MULT           (1U << SPI_APB_CLOCK_SHIFT)

static uint8_t _get_clkdiv(const spi_conf_t *conf, uint32_t clock)
{
    uint32_t bus_clock = periph_apb_clk(conf->apbbus);
    /* Shift bus_clock with SPI_APB_CLOCK_SHIFT to create a fixed point int */
    uint32_t div = (bus_clock << SPI_APB_CLOCK_SHIFT) / (2 * clock);
    DEBUG("[spi] clock: divider: %"PRIu32"\n", div);
    /* Test if the divider is 2 or smaller, keeping the fixed point in mind */
    if (div <= SPI_APB_CLOCK_MULT) {
        return 0;
    }
    /* determine MSB and compensate back for the fixed point int shift */
    uint8_t rounded_div = bitarithm_msb(div) - SPI_APB_CLOCK_SHIFT;
    /* Determine if rounded_div is not a power of 2 */
    if ((div & (div - 1)) != 0) {
        /* increment by 1 to ensure that the clock speed at most the
         * requested clock speed */
        rounded_div++;
    }
    return rounded_div > BR_MAX ? BR_MAX : rounded_div;
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

void spi_acquire(spi_t bus, spi_cs_t cs, spi_mode_t mode, spi_clk_t clk)
{
    assert((unsigned)bus < SPI_NUMOF);

    /* lock bus */
    mutex_lock(&locks[bus]);

    /* block DEEP_SLEEP mode */
    pm_block(GD32V_PM_DEEPSLEEP);

    /* enable SPI device clock */
    periph_clk_en(spi_config[bus].apbbus, spi_config[bus].rcumask);
    /* enable device */
    if (clk != clocks[bus]) {
        dividers[bus] = _get_clkdiv(&spi_config[bus], clk);
        clocks[bus] = clk;
    }
    uint8_t br = dividers[bus];

    DEBUG("[spi] acquire: requested clock: %"PRIu32", resulting clock: %"PRIu32
          " BR divider: %u\n",
          clk,
          periph_apb_clk(spi_config[bus].apbbus)/(1 << (br + 1)),
          br);

    uint16_t ctl0_settings = ((br << BR_SHIFT) | mode | SPI0_CTL0_MSTMOD_Msk);
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

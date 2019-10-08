/*
 * Copyright (C) 2014-2016 Freie Universität Berlin
 *               2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2015 FreshTemp, LLC.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_sam0_common
 * @ingroup     drivers_periph_spi
 * @{
 *
 * @file
 * @brief       Low-level SPI driver implementation
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Troels Hoffmeyer <troels.d.hoffmeyer@gmail.com>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "cpu.h"
#include "mutex.h"
#include "assert.h"
#include "periph/spi.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief Array holding one pre-initialized mutex for each SPI device
 */
static mutex_t locks[SPI_NUMOF];

/**
 * @brief   Shortcut for accessing the used SPI SERCOM device
 */
static inline SercomSpi *dev(spi_t bus)
{
    return spi_config[bus].dev;
}

static inline void poweron(spi_t bus)
{
    sercom_clk_en(dev(bus));
}

static inline void poweroff(spi_t bus)
{
    sercom_clk_dis(dev(bus));
}

void spi_init(spi_t bus)
{
    /* make sure given bus is good */
    assert(bus < SPI_NUMOF);

    /* initialize the device lock */
    mutex_init(&locks[bus]);

    /* configure pins and their muxes */
    spi_init_pins(bus);

    /* wake up device */
    poweron(bus);

    /* reset all device configuration */
    dev(bus)->CTRLA.reg |= SERCOM_SPI_CTRLA_SWRST;
    while ((dev(bus)->CTRLA.reg & SERCOM_SPI_CTRLA_SWRST) ||
           (dev(bus)->SYNCBUSY.reg & SERCOM_SPI_SYNCBUSY_SWRST));

    /* configure base clock: using GLK GEN 0 */
#ifdef GCLK_CLKCTRL_GEN_GCLK0
    sercom_set_gen(dev(bus), GCLK_CLKCTRL_GEN_GCLK0);
#else
    sercom_set_gen(dev(bus), GCLK_PCHCTRL_GEN_GCLK0);
#endif

    /* enable receiver and configure character size to 8-bit
     * no synchronization needed, as SERCOM device is not enabled */
    dev(bus)->CTRLB.reg = (SERCOM_SPI_CTRLB_CHSIZE(0) | SERCOM_SPI_CTRLB_RXEN);

    /* put device back to sleep */
    poweroff(bus);
}

void spi_init_pins(spi_t bus)
{
    /* MISO must always have PD/PU, see #5968. This is a ~65uA difference */
    gpio_init(spi_config[bus].miso_pin, GPIO_IN_PD);
    gpio_init(spi_config[bus].mosi_pin, GPIO_OUT);
    gpio_init(spi_config[bus].clk_pin, GPIO_OUT);
    gpio_init_mux(spi_config[bus].miso_pin, spi_config[bus].miso_mux);
    gpio_init_mux(spi_config[bus].mosi_pin, spi_config[bus].mosi_mux);
    gpio_init_mux(spi_config[bus].clk_pin, spi_config[bus].clk_mux);
}

int spi_acquire(spi_t bus, spi_cs_t cs, spi_mode_t mode, spi_clk_t clk)
{
    (void) cs;

    /* configure bus clock, in synchronous mode its calculated from
     * BAUD.reg = (f_ref / (2 * f_bus) - 1)
     * with f_ref := CLOCK_CORECLOCK as defined by the board */
    const uint8_t baud = (((uint32_t)CLOCK_CORECLOCK) / (2 * clk) - 1);

    /* configure device to be master and set mode and pads,
     *
     * NOTE: we could configure the pads already during spi_init, but for
     * efficiency reason we do that here, so we can do all in one single write
     * to the CTRLA register */
    const uint32_t ctrla = SERCOM_SPI_CTRLA_MODE(0x3)       /* 0x3 -> master */
                         | SERCOM_SPI_CTRLA_DOPO(spi_config[bus].mosi_pad)
                         | SERCOM_SPI_CTRLA_DIPO(spi_config[bus].miso_pad)
                         | (mode << SERCOM_SPI_CTRLA_CPHA_Pos);

    /* get exclusive access to the device */
    mutex_lock(&locks[bus]);

    /* power on the device */
    poweron(bus);

    /* configuration did not change */
    if (dev(bus)->BAUD.reg == baud && dev(bus)->CTRLA.reg == ctrla) {
        return SPI_OK;
    }

    /* disable the device */
    dev(bus)->CTRLA.reg &= ~(SERCOM_SPI_CTRLA_ENABLE);
    while (dev(bus)->SYNCBUSY.reg & SERCOM_SPI_SYNCBUSY_ENABLE) {}

    dev(bus)->BAUD.reg = baud;
    dev(bus)->CTRLA.reg = ctrla;

    /* also no synchronization needed here, as CTRLA is write-synchronized */

    /* finally enable the device */
    dev(bus)->CTRLA.reg |= SERCOM_SPI_CTRLA_ENABLE;
    while (dev(bus)->SYNCBUSY.reg & SERCOM_SPI_SYNCBUSY_ENABLE) {}

    return SPI_OK;
}

void spi_release(spi_t bus)
{
    /* release access to the device */
    mutex_unlock(&locks[bus]);
}

void spi_transfer_bytes(spi_t bus, spi_cs_t cs, bool cont,
                        const void *out, void *in, size_t len)
{
    const uint8_t *out_buf = out;
    uint8_t *in_buf = in;

    assert(out || in);

    if (cs != SPI_CS_UNDEF) {
        gpio_clear((gpio_t)cs);
    }

    for (int i = 0; i < (int)len; i++) {
        uint8_t tmp = (out_buf) ? out_buf[i] : 0;
        while (!(dev(bus)->INTFLAG.reg & SERCOM_SPI_INTFLAG_DRE)) {}
        dev(bus)->DATA.reg = tmp;
        while (!(dev(bus)->INTFLAG.reg & SERCOM_SPI_INTFLAG_RXC)) {}
        tmp = (uint8_t)dev(bus)->DATA.reg;
        if (in_buf) {
            in_buf[i] = tmp;
        }
    }

    if ((!cont) && (cs != SPI_CS_UNDEF)) {
        gpio_set((gpio_t)cs);
    }
}

/*
 * Copyright (C) 2014-2016 Freie Universität Berlin
 *               2023 Hugues Larrive
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_nrf51
 * @ingroup     drivers_periph_spi
 * @{
 *
 * @file
 * @brief       Low-level SPI driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Frank Holtz <frank-riot2015@holtznet.de>
 * @author      Jan Wagner <mail@jwagner.eu>
 * @author      Hugues Larrive <hugues.larrive@pm.me>
 *
 * @}
 */

#include <assert.h>

#include "cpu.h"
#include "mutex.h"
#include "periph/spi.h"
#include "periph/gpio.h"

/**
 * @brief   array holding one pre-initialized mutex for each SPI device
 */
static mutex_t locks[SPI_NUMOF];

static inline NRF_SPI_Type *dev(spi_t bus)
{
    return spi_config[bus].dev;
}

void spi_init(spi_t bus)
{
    assert(bus < SPI_NUMOF);

    /* initialize mutex */
    mutex_init(&locks[bus]);
    /* initialize pins */
    spi_init_pins(bus);
}

void spi_init_pins(spi_t bus)
{
    /* set pin direction */
    gpio_init(spi_config[bus].sclk, GPIO_OUT);
    gpio_init(spi_config[bus].mosi, GPIO_OUT);
    gpio_init(spi_config[bus].miso, GPIO_IN);
    /* select pins for the SPI device */
    SPI_SCKSEL  = spi_config[bus].sclk;
    SPI_MOSISEL = spi_config[bus].mosi;
    SPI_MISOSEL = spi_config[bus].miso;
}

spi_clk_t spi_get_clk(spi_t bus, uint32_t freq)
{
    (void)bus;

    if (freq >= MHZ(8)) {
        return (spi_clk_t){ .clk = SPI_FREQUENCY_FREQUENCY_M8 };
    }
    if (freq >= MHZ(4)) {
        return (spi_clk_t){ .clk = SPI_FREQUENCY_FREQUENCY_M4 };
    }
    if (freq >= MHZ(2)) {
        return (spi_clk_t){ .clk = SPI_FREQUENCY_FREQUENCY_M2 };
    }
    if (freq >= MHZ(1)) {
        return (spi_clk_t){ .clk = SPI_FREQUENCY_FREQUENCY_M1 };
    }
    if (freq >= 500000) {
        return (spi_clk_t){ .clk = SPI_FREQUENCY_FREQUENCY_K500 };
    }
    if (freq >= 250000) {
        return (spi_clk_t){ .clk = SPI_FREQUENCY_FREQUENCY_K250 };
    }
    if (freq >= 125000) {
        return (spi_clk_t){ .clk = SPI_FREQUENCY_FREQUENCY_K125 };
    }
    return (spi_clk_t){ .err = -EDOM };
}

int32_t spi_get_freq(spi_t bus, spi_clk_t clk)
{
    (void)bus;
    if (clk.err) {
        return -EINVAL;
    }

    switch (clk.clk) {
        case SPI_FREQUENCY_FREQUENCY_K125: return 125000;
        case SPI_FREQUENCY_FREQUENCY_K250: return 250000;
        case SPI_FREQUENCY_FREQUENCY_K500: return 500000;
        case SPI_FREQUENCY_FREQUENCY_M1: return MHZ(1);
        case SPI_FREQUENCY_FREQUENCY_M2: return MHZ(2);
        case SPI_FREQUENCY_FREQUENCY_M4: return MHZ(4);
        case SPI_FREQUENCY_FREQUENCY_M8: return MHZ(8);
        default: return -EINVAL;
    }
}

void spi_acquire(spi_t bus, spi_cs_t cs, spi_mode_t mode, spi_clk_t clk)
{
    (void)cs;
    assert((unsigned)bus < SPI_NUMOF);
    if (clk.err) { return; }

    mutex_lock(&locks[bus]);
    /* power on the bus (NRF51 only) */
    dev(bus)->POWER = 1;
    /* configure bus */
    dev(bus)->CONFIG = mode;
    dev(bus)->FREQUENCY = clk.clk;
    /* enable the bus */
    dev(bus)->ENABLE = 1;
}

void spi_release(spi_t bus)
{
    /* power off everything */
    dev(bus)->ENABLE = 0;
    dev(bus)->POWER = 0;
    mutex_unlock(&locks[bus]);
}

void spi_transfer_bytes(spi_t bus, spi_cs_t cs, bool cont,
                        const void *out, void *in, size_t len)
{
    const uint8_t *out_buf = out;
    uint8_t *in_buf = in;

    assert(out_buf || in_buf);

    if (cs != SPI_CS_UNDEF) {
        gpio_clear((gpio_t)cs);
    }

    for (int i = 0; i < (int)len; i++) {
        uint8_t tmp = (out_buf) ? out_buf[i] : 0;

        dev(bus)->EVENTS_READY = 0;
        dev(bus)->TXD = (uint8_t)tmp;
        while (dev(bus)->EVENTS_READY != 1);
        tmp = (uint8_t)dev(bus)->RXD;

        if (in_buf) {
            in_buf[i] = tmp;
        }
    }

    if ((cs != SPI_CS_UNDEF) && (!cont)) {
        gpio_set((gpio_t)cs);
    }
}

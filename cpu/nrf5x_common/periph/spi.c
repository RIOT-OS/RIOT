/*
 * Copyright (C) 2014-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_nrf5x_common
 * @{
 *
 * @file
 * @brief       Low-level SPI driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Frank Holtz <frank-riot2015@holtznet.de>
 * @author      Jan Wagner <mail@jwagner.eu>
 *
 * @}
 */

#include "cpu.h"
#include "mutex.h"
#include "assert.h"
#include "periph/spi.h"

#ifdef SPI_NUMOF

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
    GPIO_BASE->DIRSET = ((1 << spi_config[bus].sclk) |
                        (1 << spi_config[bus].mosi));
    GPIO_BASE->DIRCLR =  (1 << spi_config[bus].miso);
    /* select pins for the SPI device */
    SPI_SCKSEL  = spi_config[bus].sclk;
    SPI_MOSISEL = spi_config[bus].mosi;
    SPI_MISOSEL = spi_config[bus].miso;
}

int spi_acquire(spi_t bus, spi_cs_t cs, spi_mode_t mode, spi_clk_t clk)
{
    mutex_lock(&locks[bus]);
#ifdef CPU_FAM_NRF51
    /* power on the bus (NRF51 only) */
    dev(bus)->POWER = 1;
#endif
    /* configure bus */
    dev(bus)->CONFIG = mode;
    dev(bus)->FREQUENCY = clk;
    /* enable the bus */
    dev(bus)->ENABLE = 1;

    return SPI_OK;
}

void spi_release(spi_t bus)
{
    /* power off everything */
    dev(bus)->ENABLE = 0;
#ifdef CPU_FAM_NRF51
    dev(bus)->POWER = 0;
#endif
    mutex_unlock(&locks[bus]);
}

void spi_transfer_bytes(spi_t bus, spi_cs_t cs, bool cont,
                        const void *out, void *in, size_t len)
{
    uint8_t *in_buf = (uint8_t *)in;
    uint8_t *out_buf = (uint8_t *)out;

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

#endif /* SPI_NUMOF */

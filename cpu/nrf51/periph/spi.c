/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_nrf51822
 * @{
 *
 * @file
 * @brief       Low-level SPI driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Frank Holtz <frank-riot2015@holtznet.de>
 *
 * @}
 */

#include "cpu.h"
#include "mutex.h"
#include "periph/spi.h"
#include "periph_conf.h"

/* guard this file in case no SPI device is defined */
#if SPI_NUMOF

/**
 * @brief   array holding one pre-initialized mutex for each SPI device
 */
static mutex_t locks[] =  {
#if SPI_0_EN
    [SPI_0] = MUTEX_INIT,
#endif
#if SPI_1_EN
    [SPI_1] = MUTEX_INIT,
#endif
};

static inline NRF_SPI_Type *dev(spi_t bus)
{
    return spi_config[bus].dev;
}

int spi_init_pins(spi_t bus, spi_cs_t cs)
{
    if ((bus >= SPI_NUMOF) || (cs == SPI_CS_UNDEF)) {
        return -1;
    }

    mutex_lock(&locks[bus]);

    /* set pin direction */
    NRF_GPIO->DIRSET = ((1 << spi_config[bus].sck) |
                        (1 << spi_config[bus].mosi));
    NRF_GPIO->DIRCLR =  (1 << spi_config[bus].miso);
    /* select pins for the SPI device */
    dev(bus)->PSELSCK  = spi_config[bus].sck;
    dev(bus)->PSELMOSI = spi_config[bus].mosi;
    dev(bus)->PSELMISO = spi_config[bus].miso;

    mutex_unlock(&locks[bus]);

    return 0;
}

int spi_acquire(spi_t bus, spi_mode_t mode, spi_clk_t clk, spi_cs_t cs)
{
    if ((bus >= SPI_NUMOF) || (cs == SPI_CS_UNDEF)) {
        return -1;
    }

    mutex_lock(&locks[bus]);
    /* power on the bus */
    dev(bus)->POWER = 1;
    /* configure bus */
    dev(bus)->CONFIG = mode;
    dev(bus)->FREQUENCY = clk;
    /* enable the bus */
    dev(bus)->ENABLE = 1;

    return 0;
}

void spi_release(spi_t bus)
{
    /* power off everything */
    dev(bus)->ENABLE = 0;
    dev(bus)->POWER = 0;
    mutex_unlock(&locks[dev]);
}

void spi_transfer_bytes(spi_t bus, spi_cs_t cs, bool cont,
                        uint8_t *out, uint8_t *in, size_t len)
{
    gpio_clear((gpio_t)cs);

    for (unsigned i = 0; i < length; i++) {
        uint8_t tmp = (out) ? out[i] : 0;

        spi[dev]->EVENTS_READY = 0;
        spi[dev]->TXD = (uint8_t)tmp;
        while (spi[dev]->EVENTS_READY != 1);
        tmp = (uint8_t)spi[dev]->RXD;

        if (in) {
            in[i] = tmp;
        }
    }

    if (!cont) {
        gpio_set((gpio_t)cs);
    }
}

void spi_transfer_byte(spi_t bus, spi_cs_t cs, bool cont,
                       uint8_t out, uint8_t *in)
{
    spi_transfer_bytes(bus, cs, cont, &out, in, 1);
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

#endif /* SPI_NUMOF */

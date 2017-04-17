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
 * @ingroup     cpu_s5p4418
 * @{
 *
 * @file
 * @brief       Low-level SPI driver implementation
 *
 * @author      Dang Minh Phuong <kamejoko80@yahoo.com>
 *
 * @}
 */

#include "cpu.h"
#include "mutex.h"
#include "assert.h"
#include "periph/spi.h"
#include "s5p4418_ssp.h"

/* Remove this ugly guard once we selectively build the periph drivers */
#ifdef SPI_NUMOF

/**
 * @brief   Allocate one lock per SPI device
 */
static mutex_t locks[SPI_NUMOF];

void spi_init(spi_t bus)
{
    assert(bus < SPI_NUMOF);

    /* initialize device lock */
    mutex_init(&locks[bus]);

    s5p4418_ssp_gpio_config((ssp_channel_t)bus);
}

void spi_init_pins(spi_t bus)
{
    s5p4418_ssp_gpio_config((ssp_channel_t)bus);
}

int spi_init_cs(spi_t bus, spi_cs_t cs)
{
    s5p4418_ssp_cs_config((uint8_t)cs);
    return SPI_OK;
}

int spi_acquire(spi_t bus, spi_cs_t cs, spi_mode_t mode, spi_clk_t clk)
{
    ssp_info_t ssp_info;

    assert((clk >= SPI_CLK_100KHZ) && (clk <= SPI_CLK_10MHZ));

    /* lock bus */
    mutex_lock(&locks[bus]);

    /* Initialize SPI pin */
    spi_init_pins(bus);

    /* Setting up SSP information */
    ssp_info.channel      = (ssp_channel_t)bus;  // SSP channel
    ssp_info.device_mode  = MASTER_MODE;
    ssp_info.mode         = (ssp_mode_t)mode;
    ssp_info.protocol     = PROTOCOL_MOTOROLA;
    ssp_info.data_size    = DATA_SIZE_08_BIT;

    switch(clk)
    {
        case SPI_CLK_100KHZ:
            ssp_info.clock_freq = 100000;
        break;

        case SPI_CLK_400KHZ:
            ssp_info.clock_freq = 400000;
        break;

        case SPI_CLK_1MHZ:
            ssp_info.clock_freq = 1000000;
        break;

        case SPI_CLK_5MHZ:
            ssp_info.clock_freq = 5000000;
        break;

        case SPI_CLK_10MHZ:
            ssp_info.clock_freq = 10000000;
        break;

        default:
            ssp_info.clock_freq = 1000000;
        break;
    }

    if ( true == s5p4418_ssp_init(ssp_info))
    {
        return SPI_OK;
    }
    else
    {
        return SPI_NODEV;
    }
}

void spi_release(spi_t bus)
{
    /* unlock bus */
    mutex_unlock(&locks[bus]);
}

uint8_t spi_transfer_byte(spi_t bus, spi_cs_t cs, bool cont, uint8_t out)
{
    uint8_t rxdata;

    if (cs != SPI_CS_UNDEF) {
        gpio_clear((gpio_t)cs);
    }

    s5p4418_ssp_wr_byte((ssp_channel_t)bus, &out, &rxdata);

    if ((!cont) && (cs != SPI_CS_UNDEF)) {
        gpio_set((gpio_t)cs);
    }

    return rxdata;
}

void spi_transfer_bytes(spi_t bus, spi_cs_t cs, bool cont,
                        const void *out, void *in, size_t len)
{
    size_t i;
    uint8_t *txdata = (uint8_t *)out;
    uint8_t *rxdata = (uint8_t *)in;

    if (cs != SPI_CS_UNDEF) {
        gpio_clear((gpio_t)cs);
    }

    for (i = 0; i < len; i++)
    {
        s5p4418_ssp_wr_byte((ssp_channel_t)bus, &txdata[i], &rxdata[i]);
    }

    if ((!cont) && (cs != SPI_CS_UNDEF)) {
        gpio_set((gpio_t)cs);
    }
}

uint8_t spi_transfer_reg(spi_t bus, spi_cs_t cs, uint8_t reg, uint8_t out)
{
    uint8_t rxdata;

    if (cs != SPI_CS_UNDEF) {
        gpio_clear((gpio_t)cs);
    }

    s5p4418_ssp_wr_byte((ssp_channel_t)bus, &reg, &rxdata);
    s5p4418_ssp_wr_byte((ssp_channel_t)bus, &out, &rxdata);

    if (cs != SPI_CS_UNDEF) {
        gpio_set((gpio_t)cs);
    }

    return rxdata;
}

#endif /* SPI_NUMOF */

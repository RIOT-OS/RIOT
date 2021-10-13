/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_uwb_dw1000
 * @{
 *
 * @file
 * @brief       SPI abstraction layer implementation
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#include "hal/hal_spi.h"
#include "periph/spi.h"

static uint32_t spi_clk[SPI_NUMOF] = { SPI_CLK_1MHZ };
static uint32_t spi_mode[SPI_NUMOF] = { SPI_MODE_0 };

int hal_spi_set_txrx_cb(int spi_num, hal_spi_txrx_cb txrx_cb, void *arg)
{
    (void) txrx_cb;
    (void) spi_num;
    (void) arg;
    return 0;
}

int hal_spi_init(int spi_num, void *cfg, uint8_t spi_type)
{
    (void) cfg;
    (void) spi_type;
    spi_init(spi_num);
    return 0;
}

int hal_spi_config(int spi_num, struct hal_spi_settings *settings)
{
    spi_clk[spi_num] = settings->baudrate;
    spi_mode[spi_num] = settings->data_mode;
    return 0;
}

int hal_spi_enable(int spi_num)
{
    (void) spi_num;
    return 0;
}

int hal_spi_disable(int spi_num)
{
    (void) spi_num;
    return 0;
}

int hal_spi_txrx(int spi_num, void *txbuf, void *rxbuf, int cnt)
{
    spi_acquire(spi_num,
                SPI_CS_UNDEF,
                spi_mode[spi_num],
                spi_clk[spi_num]);

    spi_transfer_bytes(spi_num,
                       SPI_CS_UNDEF,
                       false,
                       txbuf,
                       rxbuf,
                       cnt);

    spi_release(spi_num);
    return 0;
}

int hal_spi_txrx_noblock(int spi_num, void *txbuf, void *rxbuf, int cnt)
{
    (void) spi_num;
    (void) txbuf;
    (void) rxbuf;
    (void) cnt;
    return 0;
}

int hal_spi_deinit(int spi_num)
{
    (void) spi_num;
    return 0;
}

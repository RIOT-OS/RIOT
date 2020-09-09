/*
 * Copyright (C) 2020 Koen Zandberg <koen@bergzand.net>
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nrf52840dk
 * @{
 *
 * @file
 * @brief       MTD configuration for the nRF52840 DK
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#ifdef MODULE_MTD

#include "board.h"
#include "mtd.h"
#include "mtd_spi_nor.h"
#include "periph_conf.h"
#include "timex.h"

static const mtd_spi_nor_params_t _nrf52840dk_nor_params = {
    .opcode = &mtd_spi_nor_opcode_default,
    .wait_chip_erase = 50LU * US_PER_SEC,
    .wait_32k_erase = 240LU *US_PER_MS,
    .wait_sector_erase = 40LU * US_PER_MS,
    .wait_4k_erase = 40LU * US_PER_MS,
    .wait_chip_wake_up = 35LU * US_PER_MS,
    .clk = NRF52840DK_NOR_SPI_CLK,
    .flag = NRF52840DK_NOR_FLAGS,
    .spi = NRF52840DK_NOR_SPI_DEV,
    .mode = NRF52840DK_NOR_SPI_MODE,
    .cs = NRF52840DK_NOR_SPI_CS,
    .addr_width = 3,
};

static mtd_spi_nor_t nrf52840dk_nor_dev = {
    .base = {
        .driver = &mtd_spi_nor_driver,
        .page_size = NRF52840DK_NOR_PAGE_SIZE,
        .pages_per_sector = NRF52840DK_NOR_PAGES_PER_SECTOR,
        .sector_count = NRF52840DK_NOR_SECTOR_COUNT,
    },
    .params = &_nrf52840dk_nor_params,
};

mtd_dev_t *mtd0 = (mtd_dev_t *)&nrf52840dk_nor_dev;

#endif

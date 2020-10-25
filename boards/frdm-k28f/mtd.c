/*
 * Copyright (C) 2020 Koen Zandberg <koen@bergzand.net>
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_frdm-k28f
 * @{
 *
 * @file
 * @brief       MTD configuration for the FRDM-K28F
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

static const mtd_spi_nor_params_t _frdm_k28f_nor_params = {
    .opcode = &mtd_spi_nor_opcode_default,
    .wait_chip_erase = 40LU * US_PER_SEC,
    .wait_32k_erase = 100LU *US_PER_MS,
    .wait_sector_erase = 150LU * US_PER_MS,
    .wait_4k_erase = 50LU * US_PER_MS,
    .wait_chip_wake_up = 30LU * US_PER_MS,
    .clk = FRDM_K28F_NOR_SPI_CLK,
    .flag = FRDM_K28F_NOR_FLAGS,
    .spi = FRDM_K28F_NOR_SPI_DEV,
    .mode = FRDM_K28F_NOR_SPI_MODE,
    .cs = FRDM_K28F_NOR_SPI_CS,
    .addr_width = 3,
};

static mtd_spi_nor_t frdm_k28f_nor_dev = {
    .base = {
        .driver = &mtd_spi_nor_driver,
        .page_size = FRDM_K28F_NOR_PAGE_SIZE,
        .pages_per_sector = FRDM_K28F_NOR_PAGES_PER_SECTOR,
        .sector_count = FRDM_K28F_NOR_SECTOR_COUNT,
    },
    .params = &_frdm_k28f_nor_params,
};

mtd_dev_t *mtd0 = (mtd_dev_t *)&frdm_k28f_nor_dev;

#endif

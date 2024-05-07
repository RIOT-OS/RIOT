/*
 * Copyright (C) 2020 iosabi
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_qn9080dk
 * @{
 *
 * @file
 * @brief       Board specific implementations for the QN9080DK base board
 *
 * @author      iosabi <iosabi@protonmail.com>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"

#include "mtd.h"
#include "mtd_spi_nor.h"
#include "timex.h"

#include "periph/gpio.h"

#ifdef MODULE_MTD
/* MX25R2035F */
static const mtd_spi_nor_params_t _mtd_nor_params = {
    .opcode = &mtd_spi_nor_opcode_default,
    .wait_chip_erase   = 15000LU * US_PER_MS,
    .wait_64k_erase    = 3500LU * US_PER_MS,
    .wait_32k_erase    = 1750LU * US_PER_MS,
    .wait_sector_erase = 240LU * US_PER_MS,
    .wait_chip_wake_up = 1LU * US_PER_MS,
    .clk  = CLOCK_CORECLOCK,  /* Max fR and fC is 33 MHz, max core is 32 MHz. */
    .flag = SPI_NOR_F_SECT_4K | SPI_NOR_F_SECT_32K | SPI_NOR_F_SECT_64K,
    .spi  = SPI_DEV(0),
    .mode = SPI_MODE_0,
    .cs   = SPI_HWCS(0),  /* GPIO(PORT_A, 3) is used for HWCS(0) on FC2 */
    .wp   = GPIO_UNDEF,
    .hold = GPIO_UNDEF,
};

static mtd_spi_nor_t mtd_nor_dev = {
    .base = {
        .driver = &mtd_spi_nor_driver,
        .page_size = 256,
        .pages_per_sector = 16,  /* 4 KiB sectors */
        .sector_count = 64,
    },
    .params = &_mtd_nor_params,
};

MTD_XFA_ADD(mtd_nor_dev, 0);
#endif /* MODULE_MTD */

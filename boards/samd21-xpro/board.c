/*
 * Copyright (C) 2017 Travis Griggs <travisgriggs@gmail.com>
 * Copyright (C) 2017 Dan Evans <photonthunder@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_samd21-xpro
 * @{
 *
 * @file
 * @brief       Board specific implementations for the Atmel SAM D21 Xplained
 *              Pro board
 *
 * @author      Travis Griggs <travisgriggs@gmail.com>
 * @author      Dan Evans <photonthunder@gmail.com>
 * @author      Sebastian Meiling <s@mlng.net>
 * @}
 */

#include "board.h"
#include "periph/gpio.h"

#ifdef MODULE_MTD
#include "mtd_spi_nor.h"
#include "timex.h"
#include "macros/units.h"

/* AT25DF081A */
static const mtd_spi_nor_params_t _mtd_nor_params = {
    .opcode = &mtd_spi_nor_opcode_default,
    .wait_chip_erase = 16 * US_PER_SEC,
    .wait_64k_erase = 400 * US_PER_MS,
    .wait_32k_erase = 250 * US_PER_MS,
    .wait_sector_erase = 50 * US_PER_MS,
    .wait_chip_wake_up = 1 * US_PER_MS,
    .clk  = MHZ(16),
    .flag = SPI_NOR_F_SECT_4K | SPI_NOR_F_SECT_32K | SPI_NOR_F_SECT_64K,
    .spi  = SPI_DEV(2),
    .mode = SPI_MODE_0,
    .cs   = GPIO_PIN(PA, 13),
    .wp   = GPIO_UNDEF,
    .hold = GPIO_UNDEF,
};

static mtd_spi_nor_t mtd_nor_dev = {
    .base = {
        .driver = &mtd_spi_nor_driver,
        .page_size = 256,
        .pages_per_sector = 16,
    },
    .params = &_mtd_nor_params,
};
mtd_dev_t *mtd0 = (mtd_dev_t *)&mtd_nor_dev;
#endif

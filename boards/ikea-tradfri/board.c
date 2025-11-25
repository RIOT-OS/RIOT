/*
 * Copyright (C) 2017-2020 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_ikea-tradfri
 * @{
 *
 * @file
 * @brief       Board specific implementations IKEA TRÅDFRI modules
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include "board.h"
#include "cpu.h"
#include "mtd.h"
#include "mtd_spi_nor.h"
#include "timex.h"

#ifdef MODULE_MTD
static const mtd_spi_nor_params_t _ikea_tradfri_nor_params = {
    .opcode = &mtd_spi_nor_opcode_default,
    .wait_chip_erase = 2LU * US_PER_SEC,
    .wait_32k_erase = 500LU *US_PER_MS,
    .wait_sector_erase = 300LU * US_PER_MS,
    .wait_chip_wake_up = 1LU * US_PER_MS,
    .clk = IKEA_TRADFRI_NOR_SPI_CLK,
    .flag = IKEA_TRADFRI_NOR_FLAGS,
    .spi = IKEA_TRADFRI_NOR_SPI_DEV,
    .mode = IKEA_TRADFRI_NOR_SPI_MODE,
    .cs = IKEA_TRADFRI_NOR_SPI_CS,
    .wp = GPIO_UNDEF,
    .hold = GPIO_UNDEF,
};

static mtd_spi_nor_t ikea_tradfri_nor_dev = {
    .base = {
        .driver = &mtd_spi_nor_driver,
        .page_size = IKEA_TRADFRI_NOR_PAGE_SIZE,
        .pages_per_sector = IKEA_TRADFRI_NOR_PAGES_PER_SECTOR,
        .sector_count = IKEA_TRADFRI_NOR_SECTOR_COUNT,
    },
    .params = &_ikea_tradfri_nor_params,
};

MTD_XFA_ADD(ikea_tradfri_nor_dev, 0);
#endif /* MODULE_MTD */

void board_init(void)
{
#ifdef MODULE_MTD
    /* enable NOR flash (only on the ICC-1-A) */
    if (gpio_is_valid(IKEA_TRADFRI_NOR_EN)) {
        gpio_init(IKEA_TRADFRI_NOR_EN, GPIO_OUT);
        gpio_set(IKEA_TRADFRI_NOR_EN);
    }
#endif
}

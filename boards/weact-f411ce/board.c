/*
 * Copyright (C) 2019 Benjamin Valentin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_weact-f411ce
 * @{
 *
 * @file
 * @brief       Board initialization code for the WeAct-F411CE board.
 *
 * @author      Benjamin Valentin <benpicco@googlemail.com>
 *
 * @}
 */

#include "board.h"
#include "cpu.h"
#include "mtd.h"
#include "timex.h"
#include "mtd_spi_nor.h"
#include "periph/gpio.h"

#ifdef MODULE_MTD
/* AT25SF041 */
static const mtd_spi_nor_params_t _weact_nor_params = {
    .opcode = &mtd_spi_nor_opcode_default,
    .wait_chip_erase = 4800LU * US_PER_MS,
    .wait_32k_erase = 300LU * US_PER_MS,
    .wait_sector_erase = 70LU * US_PER_MS,
    .wait_chip_wake_up = 1LU * US_PER_MS,
    .clk = WEACT_411CE_NOR_SPI_CLK,
    .flag = WEACT_411CE_NOR_FLAGS,
    .spi = WEACT_411CE_NOR_SPI_DEV,
    .mode = WEACT_411CE_NOR_SPI_MODE,
    .cs = WEACT_411CE_NOR_SPI_CS,
    .wp = GPIO_UNDEF,
    .hold = GPIO_UNDEF,
    .addr_width = 3,
};

static mtd_spi_nor_t weact_nor_dev = {
    .base = {
        .driver = &mtd_spi_nor_driver,
        .page_size = WEACT_411CE_NOR_PAGE_SIZE,
        .pages_per_sector = WEACT_411CE_NOR_PAGES_PER_SECTOR,
    },
    .params = &_weact_nor_params,
};

mtd_dev_t *mtd0 = (mtd_dev_t *)&weact_nor_dev;
#endif /* MODULE_MTD */

void board_init(void)
{
    cpu_init();

    gpio_init(LED0_PIN, GPIO_OUT);
    LED0_OFF;
}

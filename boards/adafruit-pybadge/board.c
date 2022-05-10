/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_adafruit-pybadge
 * @{
 *
 * @file
 * @brief       Board initialization for the Adafruit PyBadge
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"
#include "mtd_spi_nor.h"
#include "timex.h"

#include "periph/gpio.h"

#if IS_USED(MODULE_MTD)
/* GD25Q16C */
static const mtd_spi_nor_params_t _samd51_nor_params = {
    .opcode = &mtd_spi_nor_opcode_default,
    .wait_chip_erase = 15 * US_PER_SEC,
    .wait_32k_erase = 150 * US_PER_MS,
    .wait_64k_erase = 250 * US_PER_MS,
    .wait_sector_erase = 50 * US_PER_MS,
    .wait_chip_wake_up = 1 * US_PER_MS,
    .clk  = MHZ(54),
    .flag = SPI_NOR_F_SECT_4K
          | SPI_NOR_F_SECT_32K
          | SPI_NOR_F_SECT_64K,
    .spi  = SPI_DEV(3),
    .mode = SPI_MODE_0,
    .cs   = SAM0_QSPI_PIN_CS,
    .wp   = SAM0_QSPI_PIN_DATA_2,
    .hold = SAM0_QSPI_PIN_DATA_3,
};

static mtd_spi_nor_t samd51_nor_dev = {
    .base = {
        .driver = &mtd_spi_nor_driver,
        .page_size = 256,
        .pages_per_sector = 16,
    },
    .params = &_samd51_nor_params,
};

mtd_dev_t *mtd0 = (mtd_dev_t *)&samd51_nor_dev;
#endif /* MODULE_MTD */

void board_init(void)
{
    if (IS_USED(MODULE_DAC_DDS)) {
        gpio_init(SPEAKER_ENABLE_PIN, GPIO_OUT);
        gpio_set(SPEAKER_ENABLE_PIN);
    }

    if (IS_USED(MODULE_ST7735)) {
        gpio_init(BACKLIGHT_PIN, GPIO_OUT);
    }
}

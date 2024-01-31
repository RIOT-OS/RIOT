/*
 * Copyright (C) 2017 Freie Universität Berlin
 *               2020 Inria
 *               2020 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_pinetime
 * @{
 *
 * @file
 * @brief       Board initialization for the PineTime
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"
#include "mtd.h"
#include "mtd_spi_nor.h"
#include "periph/gpio.h"
#include "periph/spi.h"
#include "timex.h"

#ifdef MODULE_MTD
static const mtd_spi_nor_params_t _pinetime_nor_params = {
    .opcode = &mtd_spi_nor_opcode_default,
    .wait_chip_erase = 9LU * US_PER_SEC,
    .wait_32k_erase = 160LU *US_PER_MS,
    .wait_sector_erase = 70LU * US_PER_MS,
    .wait_chip_wake_up = 1LU * US_PER_MS,
    .clk = PINETIME_NOR_SPI_CLK,
    .flag = PINETIME_NOR_FLAGS,
    .spi = PINETIME_NOR_SPI_DEV,
    .mode = PINETIME_NOR_SPI_MODE,
    .cs = PINETIME_NOR_SPI_CS,
    .wp = GPIO_UNDEF,
    .hold = GPIO_UNDEF,
};

static mtd_spi_nor_t pinetime_nor_dev = {
    .base = {
        .driver = &mtd_spi_nor_driver,
        .page_size = PINETIME_NOR_PAGE_SIZE,
        .pages_per_sector = PINETIME_NOR_PAGES_PER_SECTOR,
        .sector_count = PINETIME_NOR_SECTOR_COUNT,
    },
    .params = &_pinetime_nor_params,
};

MTD_XFA_ADD(pinetime_nor_dev, 0);
#endif /* MODULE_MTD */

void board_init(void)
{
    /* initialize pins */
    gpio_init(VCC33, GPIO_OUT);
    gpio_init(BUTTON0_ENABLE, GPIO_OUT);
    gpio_init(BUTTON0, GPIO_IN);
    gpio_init(VIBRATOR, GPIO_OUT);
    gpio_init(LCD_BACKLIGHT_LOW, GPIO_OUT);
    gpio_init(LCD_BACKLIGHT_MID, GPIO_OUT);
    gpio_init(LCD_BACKLIGHT_HIGH, GPIO_OUT);

    gpio_set(VCC33);
    gpio_set(BUTTON0_ENABLE);
    gpio_set(VIBRATOR);
    gpio_set(LCD_BACKLIGHT_LOW);
    gpio_set(LCD_BACKLIGHT_MID);
    gpio_set(LCD_BACKLIGHT_HIGH);
}

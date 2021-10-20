/**
 * Native Board board_init implementation
 *
 * Copyright (C) 2014 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup boards_native
 * @{
 * @file
 * @author  Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 * @}
 */
#include <stdio.h>
#include "board.h"

#include "board_internal.h"

#ifdef MODULE_MTD
#include "mtd_native.h"
#endif

#ifdef MODULE_CFG_PAGE
#include "cfg_page.h"
cfg_page_desc_t cfgpage;
#endif

/**
 * Nothing to initialize at the moment.
 * Turns the red LED on and the green LED off.
 */
void board_init(void)
{
    LED0_OFF;
    LED1_ON;

    puts("RIOT native board initialized.");
#ifdef MODULE_CFG_PAGE
    cfg_page_init(&cfgpage);
#endif
}

#ifdef MODULE_MTD
static mtd_native_dev_t mtd0_dev = {
    .dev = {
        .driver = &native_flash_driver,
        .sector_count = MTD_SECTOR_NUM,
        .pages_per_sector = MTD_SECTOR_SIZE / MTD_PAGE_SIZE,
        .page_size = MTD_PAGE_SIZE,
    },
    .fname = MTD_NATIVE_FILENAME,
};

mtd_dev_t *mtd0 = (mtd_dev_t *)&mtd0_dev;
#endif

#ifdef MODULE_CFG_PAGE
#ifndef MTD1_SECTOR_NUM
#define MTD1_SECTOR_NUM 2
#endif
static mtd_native_dev_t mtd1_dev = {
    .dev = {
        .driver = &native_flash_driver,
        .sector_count = MTD1_SECTOR_NUM,
        .pages_per_sector = MTD_SECTOR_SIZE / MTD_PAGE_SIZE,
        .page_size = MTD_PAGE_SIZE,
    },
    .fname = MTD_NATIVE_CFG_FILENAME,
};

mtd_dev_t *mtd1 = (mtd_dev_t *)&mtd1_dev;
#endif

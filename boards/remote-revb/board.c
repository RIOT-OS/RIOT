/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2015 Zolertia SL
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_remote-revb
 * @{
 *
 * @file
 * @brief       Board specific implementations for the RE-Mote revision B board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *              Antonio Lignan <alinan@zolertia.com>
 *
 * @}
 */

#include "board.h"
#include "cpu.h"
#include "fancy_leds.h"

#if defined(MODULE_MTD_SDCARD)
#include "mtd_sdcard.h"
#include "sdcard_spi.h"
#include "sdcard_spi_params.h"

#if defined(MODULE_FATFS_VFS)
#include "fs/fatfs.h"
#include "vfs_default.h"
#endif

/* this is provided by the sdcard_spi driver see drivers/sdcard_spi/sdcard_spi.c */
extern sdcard_spi_t sdcard_spi_devs[ARRAY_SIZE(sdcard_spi_params)];

mtd_sdcard_t mtd_sdcard_dev = {
    .base = {
        .driver = &mtd_sdcard_driver,
    },
    .sd_card = &sdcard_spi_devs[0],
    .params = &sdcard_spi_params[0]
};

mtd_dev_t *mtd0 = (mtd_dev_t *)&mtd_sdcard_dev;

#if IS_USED(MODULE_FATFS_VFS)
VFS_AUTO_MOUNT(fatfs, VFS_MTD(mtd_sdcard_dev), VFS_DEFAULT_SD(0), 0);
#endif /* MODULE_FATFS_VFS */

#endif /* MODULE_MTD_SDCARD */

/**
 * @brief Initialize the boards on-board LEDs
 *
 * The LED initialization is hard-coded in this function. As the LED (RGB) are
 * soldered onto the board they are fixed to their CPU pins.
 */
void led_init(void)
{

    /* Shoot rainbows */
    LED_RAINBOW();
}

/**
 * @brief Initialize the Radio interface SW-controlled Switch
 *
 * The RE-Mote features an on-board RF switch to programmatically select to
 * enable either the 2.4GHz or Sub-1GHz RF interface to the RP-SMA connector
 * to use an external antenna.  As default we prefer to use the 2.4GHz RF.
 *
 */
static void rf_switch_init(void)
{
    /* Set RF 2.4GHz as default */
    gpio_init(RF_SWITCH_GPIO, GPIO_OUT);
    RF_SWITCH_2_4_GHZ;
}

void board_init(void)
{
    /* initialize the 2.4GHz RF switch */
    rf_switch_init();
}

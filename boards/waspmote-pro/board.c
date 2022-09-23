/*
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *               2015-18 Kaspar Schleiser <kaspar@schleiser.de>
 *               2016 INRIA, Francisco Acosta
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_waspmote-pro
 * @{
 *
 * @file
 * @brief       Board specific initializations
 *
 * @author      Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 *
 * @}
 */

#include "board.h"
#include "cpu.h"

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

void board_init(void)
{
    /* initialize UART_1 on AUX1 */
    SET_MUX_AUX1_MODULE;

#ifdef XBEE_UART
#if XBEE_UART == 0
    /* initialize UART0 on SOCKET0 (XBee) */
    SET_MUX_SOCKET0;
#else
    /* Initialize UART0 on USB */
    SET_MUX_USB_MODULE;
#endif
#endif
}

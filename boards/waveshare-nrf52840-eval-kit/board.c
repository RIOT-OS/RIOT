/*
 * Copyright (C) 2022 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_waveshare_nrf52840_eval_kit
 * @{
 *
 * @file
 * @brief       Board specific definitions for Waveshare nRF52840 Eval Kit
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#if defined(MODULE_MTD_SDCARD)

#include "board.h"
#include "kernel_defines.h"

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

#ifdef __cplusplus
 extern "C" {
#endif

#ifdef __cplusplus
} /* end extern "C" */
#endif

/** @} */

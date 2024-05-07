/*
 * Copyright (C) 2017 HAW-Hamburg
 *               2023 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_mtd_sdmmc mtd wrapper for sdmmc
 * @ingroup     drivers_storage
 * @brief       Driver for SD Memory Cards and MMCs/eMMCs using mtd interface
 *
 * @{
 *
 * @file
 * @brief       Interface definition for mtd_sdmmc driver
 *
 * @author      Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef MTD_SDMMC_H
#define MTD_SDMMC_H

#include <stdint.h>

#include "mtd.h"
#include "sdmmc/sdmmc.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief   Device descriptor for a mtd_sdmmc device
 *
 * This is an extension of the @c mtd_dev_t struct
 */
typedef struct {
    mtd_dev_t base;                    /**< inherit mtd_dev_t object */
    sdmmc_dev_t *sdmmc;                /**< SD/MMC device descriptor */
    uint8_t sdmmc_idx;                 /**< SD/MMC peripheral index  */
} mtd_sdmmc_t;

/**
 * @defgroup drivers_mtd_sdmmc_config   SD Memory Card driver compile configuration
 * @ingroup config_drivers_storage
 * @{
 */
/**
 * @brief   Enable SD Memory Card Erase
 *
 * SD Memory Cards and MMCs/eMMCs handle sector erase internally
 * so it's possible to directly write to the card without erasing
 * the sector first.
 *
 * @note An erase call will NOT touch the content if `CONFIG_MTD_SDMMC_ERASE`
 *       is not set, so enable this feature to ensure overriding the data.
 *
 * @pre This feature requires the `mtd_write_page` module.
 */
#ifdef DOXYGEN
#define CONFIG_MTD_SDMMC_ERASE
#endif
/** @} */

/**
 * @brief   sdcard device operations table for mtd
 */
extern const mtd_desc_t mtd_sdmmc_driver;

#ifdef __cplusplus
}
#endif

#endif /* MTD_SDMMC_H */
/** @} */

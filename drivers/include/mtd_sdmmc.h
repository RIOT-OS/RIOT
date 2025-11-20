/*
 * Copyright (C) 2017 HAW-Hamburg
 *               2023 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    drivers_mtd_sdmmc MTD wrapper for SD/MMC devices
 * @ingroup     drivers_storage
 * @brief       Driver for SD Memory Cards and MMCs/eMMCs using the MTD interface
 *
 * @{
 *
 * @file
 * @brief       Interface definition for the mtd_sdmmc driver
 *
 * @author      Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

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
 * @brief   SD/MMC device operations table for mtd
 */
extern const mtd_desc_t mtd_sdmmc_driver;

#ifdef __cplusplus
}
#endif

/** @} */

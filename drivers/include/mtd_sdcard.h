/*
 * Copyright (C) 2017 HAW-Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_mtd_sdcard MTD wrapper for SPI SD Cards
 * @ingroup     drivers_storage
 * @brief       Driver for SPI SD Cards using the MTD interface
 *
 * @{
 *
 * @file
 * @brief       Interface definition for the mtd_sdcard driver
 *
 * @author      Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 */

#ifndef MTD_SDCARD_H
#define MTD_SDCARD_H

#include <stdint.h>

#include "sdcard_spi.h"
#include "mtd.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief   Device descriptor for mtd_sdcard device
 *
 * This is an extension of the @c mtd_dev_t struct
 */
typedef struct {
    mtd_dev_t base;                    /**< inherit from mtd_dev_t object */
    sdcard_spi_t *sd_card;             /**< sdcard_spi dev descriptor */
    const sdcard_spi_params_t *params; /**< params for sdcard_spi init */
} mtd_sdcard_t;

/**
 * @defgroup drivers_mtd_sdcard_config     SDCard driver compile configuration
 * @ingroup config_drivers_storage
 * @{
 */
/**
 * @brief   Enable SDCard Erase
 * @note    SDCards handle sector erase internally so it's
 *          possible to directly write to the card without erasing
 *          the sector first.
 *          Attention: an erase call will therefore NOT touch the content,
 *          so enable this feature to ensure overriding the data.
 *
 *          This feature requires the `mtd_write_page` module.
 */
#ifdef DOXYGEN
#define CONFIG_MTD_SDCARD_ERASE
#endif
/** @} */

/**
 * @brief   sdcard device operations table for mtd
 */
extern const mtd_desc_t mtd_sdcard_driver;

#ifdef __cplusplus
}
#endif

#endif /* MTD_SDCARD_H */
/** @} */

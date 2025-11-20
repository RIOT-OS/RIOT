/*
 * Copyright (C) 2018 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    drivers_mtd_flashpage MTD wrapper for Flashpage devices
 * @ingroup     drivers_storage
 * @brief       Driver for internal flash devices implementing flashpage interface
 *
 * The MTD device created by @ref MTD_FLASHPAGE_INIT_VAL spans the complete
 * accessible flash page memory. To expose merely an area of that as a single
 * MTD partition, the @ref drivers_mtd_mapper can be used.
 *
 * @{
 *
 * @file
 * @brief       Interface definition for the flashpage memory driver
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 */

#include "mtd.h"
#include "periph/flashpage.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief   Macro helper to initialize a mtd_t with flashpage driver
 */
#define MTD_FLASHPAGE_INIT_VAL(_pages_per_sector) { \
    .base = {                                       \
        .driver = &mtd_flashpage_driver,            \
        .sector_count = FLASHPAGE_NUMOF,            \
        .pages_per_sector = _pages_per_sector,      \
        .page_size = FLASHPAGE_SIZE / _pages_per_sector, \
        .write_size = 1                             \
    },                                              \
}

/**
 * @brief   Macro helper to initialize a mtd_t with a portion of the flash
 *
 * @param[in] start     Start address of the flash section
 * @param[in] len       Size of the flash section in bytes
 */
#define MTD_FLASHPAGE_AUX_INIT_VAL(start, len) {    \
    .base = {                                       \
        .driver = &mtd_flashpage_driver,            \
        .sector_count = len / FLASHPAGE_SIZE,       \
        .pages_per_sector = 1,                      \
        .page_size = FLASHPAGE_SIZE,                \
        .write_size = 1,                            \
    },                                              \
    .offset = start / FLASHPAGE_SIZE,               \
}

/**
 * @brief   Flashpage MTD device operations table
 */
extern const mtd_desc_t mtd_flashpage_driver;

/**
 * @brief    MTD flashpage descriptor
 */
typedef struct {
    mtd_dev_t base;     /**< MTD generic device */
    uint32_t offset;    /**< Offset in terms of MTD pages, which must comprise
                             a whole number of sectors from the start of the
                             flash */
} mtd_flashpage_t;

#if CONFIG_SLOT_AUX_LEN || DOXYGEN
/**
 * @brief   MTD device representing the auxiliary flash slot
 */
extern mtd_flashpage_t mtd_flash_aux_slot;

/**
 * @brief   Generic MTD device backed by the auxiliary flash slot
 */
extern mtd_dev_t *mtd_aux;
#endif

/**
 * @brief   Size of the auxiliary slot on the internal flash
 *          Must align with the flash page size.
 *
 * @note    Don't set this config value directly!
 *          Instead set `SLOT_AUX_LEN` in the board's `Makefile.include`
 *
 * This value is fixed and can not be changed in the field / via firmware
 * updates. Changing this value requires re-flashing of riotboot.
 *
 */
#ifndef CONFIG_SLOT_AUX_LEN
#define CONFIG_SLOT_AUX_LEN 0
#endif

/**
 * @brief   Default MTD offset for the AUX slot
 */
#ifndef CONFIG_SLOT_AUX_MTD_OFFSET
#define CONFIG_SLOT_AUX_MTD_OFFSET  1
#endif

#ifdef __cplusplus
}
#endif

/** @} */

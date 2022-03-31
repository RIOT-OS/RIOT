/*
 * Copyright (C) 2018 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_mtd_flashpage   Flashpage MTD
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

#ifndef MTD_FLASHPAGE_H
#define MTD_FLASHPAGE_H

#include "mtd.h"
#include "periph/flashpage.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief   Macro helper to initialize a mtd_t with flash-age driver
 */
#define MTD_FLASHPAGE_INIT_VAL(_pages_per_sector) { \
    .base = {                                       \
        .driver = &mtd_flashpage_driver,            \
        .sector_count = FLASHPAGE_NUMOF,            \
        .pages_per_sector = _pages_per_sector,      \
        .page_size = FLASHPAGE_SIZE / _pages_per_sector, \
    },                                              \
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
} mtd_flashpage_t;

#ifdef __cplusplus
}
#endif

#endif /* MTD_FLASHPAGE_H */
/** @} */

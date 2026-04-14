/*
 * SPDX-FileCopyrightText: ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_mtd_eeprom MTD wrapper for internal EEPROM devices
 * @ingroup     drivers_mtd
 * @brief       Driver for internal EEPROM devices implementing the MTD interface
 *
 * This header exposes the MTD implementation of RIOT for internal EEPROMs.
 * Supported CPUs must provide the feature @c periph_eeprom and define the
 * internal EEPROM size as @c EEPROM_SIZE.
 * Supported boards must allocate the EEPROM MTD device by using the @ref MTD_EEPROM_INIT_VAL.
 *
 * Compared to external EEPROM like e.g. @ref drivers_mtd_at24cxxx, internal EEPROM is typically
 * not organized in pages. Thus, the default MTD adaptation assumes 1 sector with 1 page having
 * the size of the whole EEPROM. If a new CPU comes with EEPROM, which actually is divided in pages,
 * the configurables @ref MTD_EEPROM_SECTORS_NUMOF, @ref MTD_EEPROM_PAGES_PER_SECTOR,
 * and @ref MTD_EEPROM_PAGE_SIZE should be adjusted accordingly. If the layer above MTD
 * e.g. filesystem requires multiple pages or a certain page size, then @ref MTD_EEPROM_PAGES_PER_SECTOR to
 *
 * @verbatim
 * EEPROM_SIZE / <required page size or number of pages>
 * @endverbatim
 *
 * @{
 *
 * @file
 * @brief       MTD Driver for internal EEPROM
 *
 * @author      Fabian Hüßler <fabian.huessler@ml-pa.com>
 */

#include "mtd.h"
#include "periph/eeprom.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Divide internal EEPROM in sectors if required */

#if !defined(MTD_EEPROM_SECTORS_NUMOF) || defined(DOXYGEN)
#  define MTD_EEPROM_SECTORS_NUMOF        (1U)  /**< number of sectors */
#endif

#if !defined(MTD_EEPROM_PAGES_PER_SECTOR) || defined(DOXYGEN)
#  define MTD_EEPROM_PAGES_PER_SECTOR     (1U)  /**< number of pages per sector */
#endif

#if !defined(MTD_EEPROM_PAGE_SIZE) || defined(DOXYGEN)
#  define MTD_EEPROM_PAGE_SIZE            ((EEPROM_SIZE / MTD_EEPROM_SECTORS_NUMOF) / \
                                          MTD_EEPROM_PAGES_PER_SECTOR)  /**< page size in bytes */
#endif

/**
 * @brief   Macro helper to initialize a mtd_t with EEPROM driver
 */
#define MTD_EEPROM_INIT_VAL {                                   \
    .base = {                                                   \
        .driver = &mtd_eeprom_driver,                           \
        .sector_count = MTD_EEPROM_SECTORS_NUMOF,               \
        .pages_per_sector = MTD_EEPROM_PAGES_PER_SECTOR,        \
        .page_size = MTD_EEPROM_PAGE_SIZE,                      \
        .write_size = 1                                         \
    },                                                          \
}

/**
 * @brief   EEPROM MTD device operations table
 */
extern const mtd_desc_t mtd_eeprom_driver;

/**
 * @brief    MTD EEPROM descriptor
 */
typedef struct {
    mtd_dev_t base;     /**< MTD generic device */
} mtd_eeprom_t;

#ifdef __cplusplus
}
#endif
/** @}  */

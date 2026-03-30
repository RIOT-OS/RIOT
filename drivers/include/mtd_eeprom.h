/*
 * SPDX-FileCopyrightText: ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_mtd_eeprom MTD wrapper for internal EEPROM devices
 * @ingroup     drivers_mtd
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

/*
 * Copyright (C) 2022 Juergen Fitschen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @ingroup  tests
 * @brief    Fake MTD device
 *
 * @author   Juergen Fitschen <me@jue.yt>
 */
#ifndef FAKEMTD_H
#define FAKEMTD_H

#include "mtd.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Page size
 */
#define FAKE_MTD_PAGE_SIZE        128

/**
 * @brief Pages per sector
 */
#define FAKE_MTD_PAGES_PER_SECTOR 4

/**
 * @brief Sector count
 */
#define FAKE_MTD_SECTOR_COUNT     4

/**
 * @brief Storage size in byte
 */
#define MAKE_MTD_MEM_SIZE         (FAKE_MTD_PAGE_SIZE * \
                                   FAKE_MTD_PAGES_PER_SECTOR * \
                                   FAKE_MTD_SECTOR_COUNT)

/**
 * @brief Driver for fake MTD
 */
extern const mtd_desc_t fake_mtd_driver;

/**
 * @brief Context for the fake MTD
 */
typedef struct {
    mtd_dev_t mtd;                     /**< MTD instance */
    uint8_t buffer[MAKE_MTD_MEM_SIZE]; /**< Storage of the MTD */
} fake_mtd_t;

/**
 * @brief Initializes fake_mtd_t
 */
#define FAKE_MTD_INIT { .mtd = { .driver = &fake_mtd_driver, \
                                 .sector_count = FAKE_MTD_SECTOR_COUNT, \
                                 .pages_per_sector = FAKE_MTD_PAGES_PER_SECTOR, \
                                 .page_size = FAKE_MTD_PAGE_SIZE, \
                                 .write_size = 1 } }

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* FAKEMTD_H */

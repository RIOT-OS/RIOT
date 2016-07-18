/*
 * Copyright (C) 2016  OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef NATIVE_MTDI_H
#define NATIVE_MTDI_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NATIVE_MTD_PAGE_SIZE
#define NATIVE_MTD_PAGE_SIZE     256
#endif
#ifndef NATIVE_MTD_PAGE_NUM
#define NATIVE_MTD_PAGE_NUM      32768
#endif
#ifndef NATIVE_MTD_SECTOR_SIZE
#define NATIVE_MTD_SECTOR_SIZE   4096
#endif
#ifndef NATIVE_MTD_SECTOR_NUM
#define NATIVE_MTD_SECTOR_NUM    2048
#endif
#ifndef NATIVE_MTD_FLASH_SIZE
#define NATIVE_MTD_FLASH_SIZE    (NATIVE_MTD_SECTOR_SIZE * NATIVE_MTD_SECTOR_NUM)
#endif

#define MTD_NATIVE_DESC(_dev) \
    static mtd_dev_t _dev = { \
        .driver = &native_flash_driver, \
        .sector_count = NATIVE_MTD_SECTOR_NUM, \
        .pages_per_sector = NATIVE_MTD_SECTOR_SIZE / NATIVE_MTD_PAGE_SIZE, \
        .page_size = NATIVE_MTD_PAGE_SIZE, \
    }

extern const mtd_desc_t native_flash_driver;

#ifdef __cplusplus
}
#endif

#endif /* NATIVE_MTDI_H */

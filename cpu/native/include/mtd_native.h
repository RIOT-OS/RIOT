/*
 * Copyright (C) 2016  OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef MTD_NATIVE_H
#define MTD_NATIVE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MTD_NATIVE_PAGE_SIZE
#define MTD_NATIVE_PAGE_SIZE     256
#endif
#ifndef MTD_NATIVE_PAGE_NUM
#define MTD_NATIVE_PAGE_NUM      32768
#endif
#ifndef MTD_NATIVE_SECTOR_SIZE
#define MTD_NATIVE_SECTOR_SIZE   4096
#endif
#ifndef MTD_NATIVE_SECTOR_NUM
#define MTD_NATIVE_SECTOR_NUM    2048
#endif
#ifndef MTD_NATIVE_FLASH_SIZE
#define MTD_NATIVE_FLASH_SIZE    (MTD_NATIVE_SECTOR_SIZE * MTD_NATIVE_SECTOR_NUM)
#endif

#define MTD_NATIVE_DESC(_dev, _name) \
    static mtd_native_dev_t _dev = { \
        .dev = { \
            .driver = &native_flash_driver, \
            .sector_count = MTD_NATIVE_SECTOR_NUM, \
            .pages_per_sector = MTD_NATIVE_SECTOR_SIZE / MTD_NATIVE_PAGE_SIZE, \
            .page_size = MTD_NATIVE_PAGE_SIZE, \
        }, \
        .fname = _name, \
    }

typedef struct mtd_native_dev {
    mtd_dev_t dev;
    char *fname;
} mtd_native_dev_t;

extern const mtd_desc_t native_flash_driver;

#ifdef __cplusplus
}
#endif

#endif /* MTD_NATIVE_H */

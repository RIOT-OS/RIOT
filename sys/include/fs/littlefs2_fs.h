/*
 * Copyright (C) 2017 OTA keys S.A.
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_littlefs2 littlefs v2 integration
 * @ingroup     pkg_littlefs2
 * @brief       RIOT integration of littlefs version 2.x.y
 *
 * @{
 *
 * @file
 * @brief       littlefs v2 integration with vfs
 *
 * ## Migrating
 *
 * Migrating from Littlefs 1.x.y to 2.x.y can be hard, the disk format changed
 * enough to be binary incompatible. See also
 * @url https://github.com/ARMmbed/littlefs/releases/tag/v2.0.0 for a change set
 * and a possible migration path.
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef FS_LITTLEFS2_FS_H
#define FS_LITTLEFS2_FS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "vfs.h"
#include "lfs.h"
#include "mtd.h"
#include "mutex.h"

/**
 * @name    littlefs configuration
 * @{
 */
#ifndef LITTLEFS2_LOOKAHEAD_SIZE
/** Default lookahead size */
#define LITTLEFS2_LOOKAHEAD_SIZE     (16)
#endif

#ifndef LITTLEFS2_FILE_BUFFER_SIZE
/** File buffer size, if 0, dynamic allocation is used.
 * If set, only one file can be used at a time, must be program size (mtd page size
 * is used internally as program size) */
#define LITTLEFS2_FILE_BUFFER_SIZE   (0)
#endif

#ifndef LITTLEFS2_READ_BUFFER_SIZE
/** Read buffer size, if 0, dynamic allocation is used.
 * If set, it must be read size (mtd page size is used internally as read size) */
#define LITTLEFS2_READ_BUFFER_SIZE   (0)
#endif

#ifndef LITTLEFS2_PROG_BUFFER_SIZE
/** Prog buffer size, if 0, dynamic allocation is used.
 * If set, it must be program size */
#define LITTLEFS2_PROG_BUFFER_SIZE   (0)
#endif

#ifndef LITTLEFS2_CACHE_PAGES
/** Sets the number of pages used as cache. Has to be at least 1.
 */
#define LITTLEFS2_CACHE_PAGES        (1)
#endif

#ifndef LITTLEFS2_BLOCK_CYCLES
/** Sets the maximum number of erase cycles before blocks are evicted as a part
 * of wear leveling. -1 disables wear-leveling. */
#define LITTLEFS2_BLOCK_CYCLES       (512)
#endif
/** @} */

/**
 * @brief   littlefs descriptor for vfs integration
 */
typedef struct {
    lfs_t fs;                   /**< littlefs descriptor */
    struct lfs_config config;   /**< littlefs config */
    mtd_dev_t *dev;             /**< mtd device to use */
    mutex_t lock;               /**< mutex */
    /** first block number to use,
     * total number of block is defined in @p config.
     * if set to 0, the total number of sectors from the mtd is used */
    uint32_t base_addr;
#if LITTLEFS2_FILE_BUFFER_SIZE || DOXYGEN
    /** file buffer to use internally if LITTLEFS2_FILE_BUFFER_SIZE is set */
    uint8_t file_buf[LITTLEFS2_FILE_BUFFER_SIZE];
#endif
#if LITTLEFS2_READ_BUFFER_SIZE || DOXYGEN
    /** read buffer to use internally if LITTLEFS2_READ_BUFFER_SIZE is set */
    uint8_t read_buf[LITTLEFS2_READ_BUFFER_SIZE];
#endif
#if LITTLEFS2_PROG_BUFFER_SIZE || DOXYGEN
    /** prog buffer to use internally if LITTLEFS2_PROG_BUFFER_SIZE is set */
    uint8_t prog_buf[LITTLEFS2_PROG_BUFFER_SIZE];
#endif
    /** lookahead buffer to use internally */
    uint8_t lookahead_buf[LITTLEFS2_LOOKAHEAD_SIZE];
} littlefs_desc_t;

/** The littlefs vfs driver */
extern const vfs_file_system_t littlefs2_file_system;

#ifdef __cplusplus
}
#endif

#endif /* FS_LITTLEFS2_FS_H */
/** @} */

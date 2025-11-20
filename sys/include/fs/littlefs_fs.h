/*
 * Copyright (C) 2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    sys_littlefs  littlefs integration
 * @ingroup     pkg_littlefs
 * @brief       RIOT integration of littlefs
 *
 * @{
 *
 * @file
 * @brief       littlefs integration with vfs
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 */

#include <stdalign.h>

#include "vfs.h"
#include "lfs.h"
#include "mtd.h"
#include "mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    littlefs configuration
 * @{
 */
#ifndef LITTLEFS_LOOKAHEAD_SIZE
/** Default lookahead size */
#define LITTLEFS_LOOKAHEAD_SIZE     (128)
#endif

#ifndef LITTLEFS_FILE_BUFFER_SIZE
/** File buffer size, if 0, dynamic allocation is used.
 * If set, only one file can be used at a time, must be program size (mtd page size
 * is used internally as program size) */
#define LITTLEFS_FILE_BUFFER_SIZE   (0)
#endif

#ifndef LITTLEFS_READ_BUFFER_SIZE
/** Read buffer size, if 0, dynamic allocation is used.
 * If set, it must be read size (mtd page size is used internally as read size) */
#define LITTLEFS_READ_BUFFER_SIZE   (0)
#endif

#ifndef LITTLEFS_PROG_BUFFER_SIZE
/** Prog buffer size, if 0, dynamic allocation is used.
 * If set, it must be program size */
#define LITTLEFS_PROG_BUFFER_SIZE   (0)
#endif

#ifndef LITTLEFS_MIN_BLOCK_SIZE_EXP
/**
 * The exponent of the minimum acceptable block size in bytes (2^n).
 * The desired block size is not guaranteed to be applicable but will be respected. */
#define LITTLEFS_MIN_BLOCK_SIZE_EXP (-1)
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
#if LITTLEFS_FILE_BUFFER_SIZE || DOXYGEN
    /** file buffer to use internally if LITTLEFS_FILE_BUFFER_SIZE is set */
    alignas(uint32_t) uint8_t file_buf[LITTLEFS_FILE_BUFFER_SIZE];
#endif
#if LITTLEFS_READ_BUFFER_SIZE || DOXYGEN
    /** read buffer to use internally if LITTLEFS_READ_BUFFER_SIZE is set */
    alignas(uint32_t) uint8_t read_buf[LITTLEFS_READ_BUFFER_SIZE];
#endif
#if LITTLEFS_PROG_BUFFER_SIZE || DOXYGEN
    /** prog buffer to use internally if LITTLEFS_PROG_BUFFER_SIZE is set */
    alignas(uint32_t) uint8_t prog_buf[LITTLEFS_PROG_BUFFER_SIZE];
#endif
    /** lookahead buffer to use internally */
    alignas(uint32_t) uint8_t lookahead_buf[LITTLEFS_LOOKAHEAD_SIZE / 8];
    uint16_t sectors_per_block; /**< number of sectors per block */
} littlefs_desc_t;

/** The littlefs vfs driver */
extern const vfs_file_system_t littlefs_file_system;

#ifdef __cplusplus
}
#endif

/** @} */

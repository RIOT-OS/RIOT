/*
 * Copyright (C) 2022 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    sys_lwext4 lwEXT4 integration
 * @ingroup     pkg_lwext4
 * @brief       RIOT integration of lwEXT4
 *
 * @{
 *
 * @file
 * @brief       lwext4 integration with vfs
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include <stdalign.h>

#include "vfs.h"
#include "mtd.h"
#include "mutex.h"
#include "ext4.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   lwext4 descriptor for vfs integration
 */
typedef struct {
    struct ext4_mountpoint mp;  /**< lwext4 mountpoint struct   */
    struct ext4_blockdev bdev;  /**< lwext4 block device struct */
    struct ext4_blockdev_iface iface; /**< lwext4 block device interface */

    mtd_dev_t *dev;             /**< mtd device to use */
    mutex_t lock;               /**< mutex */
} lwext4_desc_t;

/**
 * @brief   The littlefs vfs driver
 */
extern const vfs_file_system_t lwext4_file_system;

#ifdef __cplusplus
}
#endif

/** @} */

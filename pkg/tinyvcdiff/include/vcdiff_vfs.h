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
 * @ingroup  pkg_tinyvcdiff
 *
 * @author  Juergen Fitschen <me@jue.yt>
 */

#ifndef VCDIFF_VFS_H
#define VCDIFF_VFS_H

#include "vcdiff.h"
#include "vfs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Driver for accessing VFS-based file access
 */
extern const vcdiff_driver_t vcdiff_vfs_driver;

/**
 * @brief Context for the underlying file
 */
typedef struct {
    int fd;          /**< File descriptor of the VFS file */
    size_t max_size; /**< Maximum file size for vcdiff target files */
} vcdiff_vfs_t;

/**
 * @brief Initializes vcdiff_vfs_t with maximum allowed file size
 */
#define VCDIFF_VFS_INIT_MAX_SIZE(FD, SIZE) { .fd = FD, .max_size = SIZE }

/**
 * @brief Initializes vcdiff_vfs_t
 */
#define VCDIFF_VFS_INIT(FD) VCDIFF_VFS_INIT_MAX_SIZE(FD, SIZE_MAX)

#ifdef __cplusplus
}
#endif

#endif /* VCDIFF_VFS_H */
/** @} */

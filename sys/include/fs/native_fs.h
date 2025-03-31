/*
 * Copyright (C) 2023 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef FS_NATIVE_FS_H
#define FS_NATIVE_FS_H

/**
 * @defgroup sys_fs_native Native FS Integration
 * @ingroup cpu_native
 * @brief Access to the host filesystem from RIOT native
 *
 * @{
 *
 * @file
 * @brief       Native integration with virtual filesystem (VFS)
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include <stdalign.h>

#include "vfs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   native filesystem access
 */
typedef struct {
    const char *hostpath;   /**< base directory, with no trailing slash */
} native_desc_t;

/**
 * @brief   The native vfs driver
 */
extern const vfs_file_system_t native_file_system;

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* FS_NATIVE_FS_H */

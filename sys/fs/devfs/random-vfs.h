/*
 * Copyright (C) 2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

#pragma once

/**
 * @ingroup     sys_fs_devfs
 * @{
 *
 * @file
 * @brief       Random backends for devfs
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 */


#include "vfs.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(MODULE_PERIPH_HWRNG) || defined(DOXYGEN)
/**
 * @brief hwrng driver for vfs
 */
extern const vfs_file_ops_t hwrng_vfs_ops;
#endif

#if defined(MODULE_RANDOM) || defined(DOXYGEN)
/**
 * @brief urandom driver for vfs
 */
extern const vfs_file_ops_t random_vfs_ops;
#endif

#ifdef __cplusplus
}
#endif

/** @} */

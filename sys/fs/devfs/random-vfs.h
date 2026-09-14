/*
 * SPDX-FileCopyrightText: 2017 OTA keys S.A.
 * SPDX-License-Identifier: LGPL-2.1-only
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

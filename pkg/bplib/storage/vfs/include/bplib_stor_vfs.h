/*
 * SPDX-FileCopyrightText: 2026 Hamburg University of Technology (TUHH)
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    pkg_bplib_storage_vfs VFS based storage
 * @ingroup     pkg_bplib_storage
 * @brief       VFS based storage
 *
 * @see pkg_bplib_storage for more information
 *
 * @{
 *
 * @file
 * @brief       Generic functions for vfs based storage.
 *
 * @author      Simon Grund <mail@simongrund.de>
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Number of bundle references to be cached.
 *
 * They do not store full bundles, but a way to find them on vfs, so the memory
 * impact is small.
 */
#ifndef CONFIG_BPLIB_EGRESS_CACHE_LEN
#  define CONFIG_BPLIB_EGRESS_CACHE_LEN    (8)
#endif

/** @brief Base folder to store bundles into */
#ifndef CONFIG_BPLIB_STOR_BASE
#  define CONFIG_BPLIB_STOR_BASE "/nvm0/bp"
#endif

/** @brief Length of CONFIG_BPLIB_STOR_BASE without terminator */
#define BPLIB_STOR_BASELEN (sizeof(CONFIG_BPLIB_STOR_BASE) - 1)

/**
 * @brief Notify the storage implementation to clear the cache for the given contact
 */
void bplib_stor_vfs_contact_changed(uint32_t contact_index);

/**
 * @brief Notify the storage implementation to clear the cache for the given channel
 */
void bplib_stor_vfs_channel_changed(uint32_t channel_index);

#ifdef __cplusplus
}
#endif

/** @} */

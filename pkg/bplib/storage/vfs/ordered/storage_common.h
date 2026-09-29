/*
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     pkg_bplib_storage_vfs
 * @brief       Common functions for vfs storage
 *
 * @see pkg_bplib_storage for more information
 *
 * @{
 *
 * @file
 * @brief       Common defines and functions
 *
 * @author      Simon Grund <mail@simongrund.de>
 */

#include <stdint.h>

#include "bplib.h"

#include "bplib_stor_vfs.h"
#include "cache.h"

#include "mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief If the bundle is a custodial bundle */
#define STORAGE_HEADER_FLAG_CUSTODIAL       0x01
/**
 * @brief If retransmissions are turned off
 *
 * This can happen if custody at a next hop was rejected. In that case no
 * retransmissions to the same node are done. This only changes when the CLA
 * changes.
 */
#define STORAGE_HEADER_FLAG_RETRANS_OFF     0x02

/**
 * @brief Size of a buffer required to hold a path of a bundle data file
 *
 * 16 chars each for 64 bit hex numbers, 1 for all / and the last 1 + 8 + 1 is used
 * for the _[bundle-id] suffix and the null terminator
 */
#define BPLIB_STOR_PATHLEN_DAT  (BPLIB_STOR_DATA_LEN + 1 + 16 + 1 + 16 + 1 + 16 + 1 + 8 + 1)

/**
 * @brief Size of a buffer required to hold a path of a bundle index file
 *
 * 1 for the /, 8 for the 32bit ID and the terminator
 */
#define BPLIB_STOR_PATHLEN_IDX  (BPLIB_STOR_INDEX_LEN + 1 + 8 + 1)

/** @brief Longer of the two BPLIB_STOR_PATHLEN (which should be the /dat path) */
#define BPLIB_STOR_PATHLEN_MAX  MAX(BPLIB_STOR_PATHLEN_DAT, BPLIB_STOR_PATHLEN_IDX)

/**
 * @brief Size of the index file
 *
 * It contains 3 * uint64_t => 24
 */
#define BUNDLE_STORAGE_INDEX_SIZE       (3 * sizeof(uint64_t))

/**
 * @brief Size of the data file in storage
 *
 * Depends on the payload size. Does not include the directories created on the way.
 */
#define BUNDLE_STORAGE_USAGE(payload)   (payload + (sizeof(bundle_file_structure_t) - 1) + \
                                         BUNDLE_STORAGE_INDEX_SIZE)

/**
 * @brief Wrapper for BUNDLE_STORAGE_USAGE to pass bundle.Meta.TotalBytes directly
 *
 * The bundle.Meta.TotalBytes already includes the BPLib_BBlocks_t, so get rid of that again
 */
#define BUNDLE_STORAGE_USAGE_FROM_TOTAL_BYTES(total_bytes)  BUNDLE_STORAGE_USAGE(total_bytes \
                                                                - sizeof(BPLib_BBlocks_t))

/**
 * @brief Header including fields from the sqlite based implementation
 */
typedef struct {
    /**
     * @brief Timestamp in monotonic time after which a retransmit can happen
     *
     * This currently does not work with restarts.
     */
    uint64_t retransmit_timestamp;
    /** @brief Storage flags. See STORAGE_HEADER_FLAG_* */
    uint8_t  flags;
} bundle_file_header_t;

/**
 * @brief Helper to mirror the layout of bundles in a file (hence packed)
 *
 * This packed struct should only be used for offset calculation and not for
 * storing data
 */
typedef struct __attribute__((packed)) {
    /** @brief First: Storage specific header */
    bundle_file_header_t header;
    /** @brief Second: bplib given metadata of the bundle */
    BPLib_BundleMetaData_t meta;
    /** @brief Third: The unencoded bundle blocks. These are large (~800 B) */
    BPLib_BBlocks_t bblocks;
    /** @brief Final: The raw payload */
    uint8_t payload;
} bundle_file_structure_t;

/**
 * @brief Data needed across the different parts of the vfs storage
 *
 * Currently this includes only the different caches.
 */
typedef struct {
    /** @brief Lock over all caches */
    mutex_t caches_lock;
    /** @brief Bundle discovery caches per contact */
    cache_list_t contact_caches[BPLIB_MAX_NUM_CONTACTS];
    /** @brief Bundle discovery caches per channel */
    cache_list_t channel_caches[BPLIB_MAX_NUM_CHANNELS];
} bplib_stor_common_data_t;

/** @brief Shared data between the load, store, custody files */
extern bplib_stor_common_data_t bplib_stor_common_data;

/**
 * @brief Find the path of the bundle data by the ID
 *
 * @param bundle_id Bundle ID. An index file with this ID should exist.
 * @param[out] path_dat Pointer to buffer of size >= BPLIB_STOR_PATHLEN_DAT
 * @return success indicated by bool. Only on true the path is valid.
 */
bool bplib_stor_vfs_bundle_path_from_id(uint32_t bundle_id, char* path_dat);

/**
 * @brief Delete a bundle from storage
 *
 * Will (try to) delete the index file and the bundle data.
 *
 * If the index file is gone, but the blob is still there, this cannot delete the
 * bundle data.
 *
 * @param bundle_id ID of the bundle
 */
void bplib_stor_vfs_delete_bundle(uint32_t bundle_id);

/**
 * @brief Update the retransmission timestamp of the custodial bundle at path
 *
 * The next retransmission will happen NOT BEFORE the current time +
 * retrans_interval [ms]. One exception to this is the case where retrans_interval
 * == 0. Then retransmissions will be turned off, until this function is called
 * again with non zero value.
 *
 * @param path Path of the bundle
 * @param retrans_interval Time [ms] in which to do the next retransmission
 *
 * @return bool success of operation
 */
bool bplib_stor_vfs_update_retransmission_time(const char* path,
                                                      uint32_t retrans_interval);

#ifdef __cplusplus
}
#endif

/** @} */

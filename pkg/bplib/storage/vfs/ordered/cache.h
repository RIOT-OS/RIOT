/*
 * SPDX-FileCopyrightText: 2026 Hamburg University of Technology (TUHH)
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     pkg_bplib_storage_vfs
 * @brief       Cache for ordered egress
 *
 * @see pkg_bplib_storage for more information
 *
 * @{
 *
 * @file
 * @brief       clist based cache ordered by expiry time
 *
 * @author      Simon Grund <mail@simongrund.de>
 */
#include "clist.h"

#include <stdint.h>
#include <stdbool.h>

#include "bplib.h"
#include "bplib_stor_vfs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Bundle Cache element, containing information to uniquely identify a bundle on vfs.
 *
 * Contains the filepath (the node/allocator and service),
 * its expiry time and the (storage internal) duplicate index. With this information a
 * bundle in storage is uniquely identified. The list should be maintained sorted by
 * the expiry DTN time, so that egress can be sped up. A node of 0 is reserved for
 * unused cache elements.
 */
typedef struct {
    /** @brief clist base */
    clist_node_t list;
    /** @brief destination node ID of the bundle */
    uint64_t node;
    /** @brief destination service ID of the bundle */
    uint64_t service;
    /** @brief expiry time of the bundle */
    uint64_t expiry;
    /** @brief Index in the storage.
     * In case the above do not uniquely identify a bundle.
     * It is only an uint8 because we assume that there won't be more than 256 bundles with
     * the same expiry time to the same node at any given time. */
    uint8_t index;
} cache_list_node_t;

/**
 * @brief Bundle cache.
 *
 * Should be instantiated per contact, such that every list contains bundles that
 * match the contact's filter.
 */
typedef struct {
    /** @brief Head pointer of the clist */
    clist_node_t list;
    /** @brief Collection of possible cache nodes */
    cache_list_node_t nodes[CONFIG_BPLIB_EGRESS_CACHE_LEN];
    /** @brief Whether all bundles in the storage are in the queue */
    bool all_bundles_queued;
} cache_list_t;

/**
 * @brief Add a bundle to the lookup cache.
 *
 * It will not be added if the cache is full with more urgent bundles.
 * If a bundle in the cache is less urgent, it will be replaced.
 *
 * @param cache Cache instance
 * @param node Destinatione node ID of the bundle
 * @param service Destinatione service ID of the bundle
 * @param expiry Expiry DTN time of the bundle
 * @param index Index in the storage, in case the above do not uniquely identify a bundle.
 *
 * @retval 1 if the new entry was NOT stored, since the cache is full with more urgent bundles
 * @retval 2 if the new entry was stored and replaced another entry
 * @retval 0 if the new entry was stored but without replacement
 */
int bplib_cache_add(cache_list_t* cache,
    uint64_t node, uint64_t service, uint64_t expiry, uint8_t index);

/**
 * @brief Peek the given cache queue.
 *
 * Does not consume the front element, that has to be done with bplib_cache_mark_front_consumed()
 *
 * @param cache cache instance
 * @param bundle_path buffer for the string of the file path. Has to hold BPLIB_STOR_PATHLEN bytes.
 * @return 0 If the cache is non empty and bundle_path has been updated
 */
int bplib_cache_get(cache_list_t* cache, char* bundle_path);

/**
 * @brief Actually pop the front of the loadbuffer / cache.
 *
 * @param cache Cache instance
 */
void bplib_cache_mark_front_consumed(cache_list_t* cache);

/**
 * @brief Check if the cache is empty
 *
 * @param cache Cache instance
 */
bool bplib_cache_is_empty(cache_list_t* cache);

#ifdef __cplusplus
}
#endif

/** @} */

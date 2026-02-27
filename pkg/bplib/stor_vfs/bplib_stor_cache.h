/*
 * SPDX-FileCopyrightText: 2026 Hamburg University of Technology (TUHH)
 * SPDX-License-Identifier: LGPL-2.1-only
 */
#pragma once

#include "clist.h"

#include <stdint.h>
#include <stdbool.h>

#include "bplib_mem.h"

#ifdef __cplusplus
extern "C" {
#endif

/* The length of the cache in memory, to reduce frequent and inefficient vfs iterations.
 * Since the list is iterated to find an unused element this should not be huge. */
#define BUNDLE_CACHE_LEN    (8)

/**
 * @brief Bundle Cache element. Contains the filepath (the node/allocator and service),
 * its expiry time and the (storage internal) duplicate index. With this information a
 * bundle in storage is uniquely identified. The list should be maintained sorted by
 * the expiry DTN time, so that egress can be sped up. A node of 0 is reserved for non
 * used cache elements
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
     * In case the above do not uniquely identify a bundle
     * is only a uint8 so we assume that there wont be more than 256 bundles with
     * the same expiry time to the same node at any time. */
    uint8_t index;
} cache_list_node_t;

/* Full cache, should be instantiated per channel and contact. */

/**
 * @brief Bundle cache.
 *
 * Should be instantiated per contact, such that every list contains bundles that
 * mach the contact's filter.
 */
typedef struct {
    /** @brief Head pointer of the clist */
    clist_node_t list;
    /** @brief Collection of possible cache nodes */
    cache_list_node_t nodes[BUNDLE_CACHE_LEN];
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
 */
void bplib_cache_add(cache_list_t* cache,
    uint64_t node, uint64_t service, uint64_t expiry, uint8_t index);

/**
 * @brief Peek the given cache queue.
 *
 * Does not consume the front element, has to be done with bplib_cache_mark_front_consumed()
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

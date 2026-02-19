#pragma once

#include "clist.h"

#include <stdint.h>
#include <stdbool.h>

#include "bplib_mem.h"

/* The length of the cache in memory, to reduce frequence and inefficient vfs iterations.
 * Since the list is iterated to find an unused element this should not be huge. */
#define BUNDLE_CACHE_LEN (8)

/* Bundle Cache element. Contains the filepath (the node/allocator and service), its
 * expiry time and the (storage internal) duplicate index. With this information a
 * bundle in storage is uniquely identified. The list should be maintained sorted by
 * the expiry DTN time, so that egress can be sped up. A node of 0 is reseved for non
 * used cache elements*/
typedef struct {
    clist_node_t list;
    uint64_t node;
    uint64_t service;
    uint64_t expiry;
    uint8_t index; /* Deduplication */
} cache_list_node_t;

/* Full cache, should be instanciated per channel and contact. */
typedef struct {
    clist_node_t list;
    cache_list_node_t nodes[BUNDLE_CACHE_LEN];
} cache_list_t;

/**
 * @brief Add a bundle to the lookup cache. Will not be added if the cache is full with more urgent bundles.
 * If a bundle in the cache is less urgent, it will be replaced.
 */
void bplib_cache_add(cache_list_t* cache, uint64_t node, uint64_t service, uint64_t expiry, uint8_t index);

/**
 * @brief Peek the given cache queue.
 * 
 * @param cache cache instance
 * @param bundle_path buffer for the string of the file path. Has to hold BPLIB_STOR_PATHLEN.
 * @return int 0 If the cache is non empty and bundle_path has been updated
 */
int bplib_cache_get(cache_list_t* cache, char* bundle_path);

/**
 * @brief Actually pop the front of the loadbuffer (after the bundle has been put the the egrees queue)
 */
void bplib_cache_mark_front_consumed(cache_list_t* cache);

bool bplib_cache_is_empty(cache_list_t* cache);
/*
 * SPDX-FileCopyrightText: 2026 Hamburg University of Technology (TUHH)
 * SPDX-License-Identifier: LGPL-2.1-only
 */
/**
 * @ingroup     pkg_bplib_storage_vfs
 * 
 * @{
 *
 * @file
 * @brief       Cache for VFS Based storage, ordered egress
 * @author      Simon Grund <mail@simongrund.de>
 *
 * @}
 */
#include "cache.h"

#include <stdio.h>

#include "list.h"

static cache_list_node_t* get_free_cache_elem(cache_list_t* cache)
{
    /* TODO: move empty elements in own list to avoid this O(n) search */
    for (unsigned i = 0; i < CONFIG_BPLIB_EGRESS_CACHE_LEN; i++) {
        if (cache->nodes[i].node == 0) {
            return &cache->nodes[i];
        }
    }
    return NULL;
}

/* Insert into that point, which is the last node
 * that has an expiry time strictly smaller than expiry */
static void bplib_cache_insert(clist_node_t* list, cache_list_node_t* new_node)
{
    /* Case: Empty cache */
    if (list->next == NULL) {
        clist_lpush(list, &new_node->list);
        return;
    }

    /* Case: Before head */
    if (container_of(list->next->next, cache_list_node_t, list)->expiry >= new_node->expiry) {
        clist_lpush(list, &new_node->list);
        return;
    }

    /* Case: In between the head and the tail */
    clist_node_t *node = list->next->next;

    while (node != list->next) {
        /* ->next may be the tail, but never be past the tail */
        if (container_of(node->next, cache_list_node_t, list)->expiry >= new_node->expiry) {
            list_add(node, &new_node->list);
            return;
        }
        node = node->next;
    }

    /* Else: After tail */
    clist_rpush(list, &new_node->list);
    return;
}

int bplib_cache_add(cache_list_t* cache,
    uint64_t node, uint64_t service, uint64_t expiry, uint8_t index)
{
    cache_list_node_t* elem = get_free_cache_elem(cache);

    if (elem == NULL) {
        /* Cache is full. Then the last element will be removed IF the new element
         * has a more urgent expiry time. Otherwise this new bundle will not be cached */
        clist_node_t * old = clist_rpeek(&cache->list);
        if (container_of(old, cache_list_node_t, list)->expiry > expiry) {
            clist_rpop(&cache->list);
            elem = container_of(old, cache_list_node_t, list);
        }
        else {
            return 1;
        }
    }

    elem->expiry = expiry;
    elem->index = index;
    elem->node = node;
    elem->service = service;

    bplib_cache_insert(&cache->list, elem);

    if (elem == NULL) {
        return 2;
    }
    else {
        return 0;
    }
}

int bplib_cache_get(cache_list_t* cache, char* bundle_path)
{
    /* Assume bundle_path can fit BPLIB_STOR_PATHLEN characters */
    clist_node_t * node = clist_lpeek(&cache->list);
    if (node == NULL) {
        bundle_path[0] = '\0';
        return 1;
    }
    cache_list_node_t * entry = container_of(node, cache_list_node_t, list);
    sprintf(bundle_path, CONFIG_BPLIB_STOR_BASE 
        "/%"PRIx64"/%"PRIx64"/%"PRIx64"_%02"PRIx8,
        entry->node, entry->service, entry->expiry, entry->index);
    return 0;
}

void bplib_cache_mark_front_consumed(cache_list_t* cache)
{
    clist_node_t * node = clist_lpop(&cache->list);
    cache_list_node_t * entry = container_of(node, cache_list_node_t, list);
    entry->node = 0;
}

bool bplib_cache_is_empty(cache_list_t* cache)
{
    return clist_is_empty(&cache->list);
}

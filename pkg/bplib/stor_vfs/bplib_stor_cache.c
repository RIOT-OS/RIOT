#include "bplib_stor_cache.h"

#include <stdio.h>

#include "list.h"

static cache_list_node_t* get_free_cache_elem(cache_list_t* cache) {
    // TODO: maybe move empty elements in own list to avoid this O(n) search
    for (unsigned i = 0; i < BUNDLE_CACHE_LEN; i++) {
        if (cache->nodes[i].node == 0) {
            return &cache->nodes[i];
        }
    }
    return NULL;
}

/* Insert into that point, which is the last node which has an expiry time strictly smaller than expiry */
static void bplib_cache_insert(clist_node_t* list, cache_list_node_t* new_node) {
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

void bplib_cache_add(cache_list_t* cache, uint64_t node, uint64_t service, uint64_t expiry, uint8_t index) {
    cache_list_node_t* elem = get_free_cache_elem(cache);

    if (elem == NULL) {
        /* Cache is full. Then the last element will be removed IF the new element
         * has a more urgent expiry time. Otherwise this new bundle will not be cached */
        clist_node_t * old = clist_rpeek(&cache->list);
        if (container_of(old, cache_list_node_t, list)->expiry > expiry) {
            clist_rpop(&cache->list);
            elem = container_of(old, cache_list_node_t, list);
        } else {
            return;
        }
    }

    elem->expiry = expiry;
    elem->index = index;
    elem->node = node;
    elem->service = service;

    bplib_cache_insert(&cache->list, elem);
}

int bplib_cache_get(cache_list_t* cache, char* bundle_path) {
    /* Assume bundle_path can fit BPLIB_STOR_PATHLEN characters */
    /* Assume that the channel / contact tables reachable EIDs have not changes since the time
     * the bundles ids were loaded into the cache. If lifecycle management of the channels / contacts 
     * is done through BPLib_NC_RemoveApplication / BPLib_NC_ContactTeardown (or the PI version which
     * does the same minus some event sending) bundles in the egress queue are pushed back to storage
     * but not in the LoadBatches. In the FWP provided by the port this is done through the
     * bplib_stor_vfs_contact_changed and bplib_stor_vfs_channel_changed functions */
    clist_node_t * node = clist_lpeek(&cache->list);
    if (node == NULL) {
        bundle_path[0] = '\0';
        return 1;
    }
    cache_list_node_t * entry = container_of(node, cache_list_node_t, list);
    sprintf(bundle_path, BPLIB_STOR_MOUNTPOINT BPLIB_STOR_DIRNAME "/%"PRIx64"/%"PRIx64"/%"PRIx64"_%02"PRIx8,
        entry->node, entry->service, entry->expiry, entry->index);
    return 0;
}

void bplib_cache_mark_front_consumed(cache_list_t* cache) {
    (void) clist_lpop(&cache->list);
}

bool bplib_cache_is_empty(cache_list_t* cache) {
    return clist_is_empty(&cache->list);
}
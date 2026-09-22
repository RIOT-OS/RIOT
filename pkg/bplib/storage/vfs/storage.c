/*
 * NASA Docket No. GSC-19,559-1, and identified as "Delay/Disruption Tolerant Networking
 * (DTN) bundle Protocol (BP) v7 Core Flight System (cFS) Application Build 7.0
 *
 * SPDX-FileCopyrightText: 2025 United States Government as represented by the Administrator of the
 * SPDX-FileCopyrightText: National Aeronautics and Space Administration.
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: Apache-2.0
 *
 * Based on bplib's sqlite storage implementation in [bplib]/bpa/stor/
 */

/**
 * @ingroup     pkg_bplib_storage_vfs
 *
 * @{
 *
 * @file
 * @brief       VFS Based storage, ordered egress
 * @author      Simon Grund <mail@simongrund.de>
 *
 * @}
 */
#include "bplib_stor_vfs.h"

#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <inttypes.h>

#include "mutex.h"
#include "vfs.h"

#include "bplib.h"

#include "cache.h"

static mutex_t caches_lock = MUTEX_INIT;
static cache_list_t contact_caches[BPLIB_MAX_NUM_CONTACTS];
static cache_list_t channel_caches[BPLIB_MAX_NUM_CHANNELS];

#define STORAGE_HEADER_FLAG_DELETE      0x01    /* == egress_attempted */
#define STORAGE_HEADER_FLAG_CUSTODIAL   0x02    /* == is_custodial */

/**
 * @brief Header including fields from the sqlite based implementation
 */
typedef struct {
    uint64_t retransmit_timestamp;
    uint32_t retransmit_trigger;
    uint8_t  flags;
} bundle_file_header_t;

/**
 * @brief Helper to mirror the layout of bundles in a file (hence packed)
 *
 * This packed struct should only be used for offset calculation and not for
 * storing data
 */
typedef struct __attribute__((packed)) {
    bundle_file_header_t header;
    BPLib_BundleMetaData_t meta;
    BPLib_BBlocks_t bblocks;
    uint8_t payload;
} bundle_file_structure_t;

// TODO remove, replace with bundle ID
#ifndef CONFIG_BPLIB_STOR_MAX_DUPLICATE_CHECKS
#  define CONFIG_BPLIB_STOR_MAX_DUPLICATE_CHECKS 64
#endif

/* 16 chars each for 64 bit hex numbers, 1 for all / and the last 1 + 2 + 1 is used
 * for the _xx deduplicator in case of identical times + the null terminator */
#define BPLIB_STOR_PATHLEN (BPLIB_STOR_DATA_LEN + 1 + 16 + 1 + 16 + 1 + 16 + 1 + 2 + 1)

BPLib_Status_t BPLib_STOR_Init(BPLib_Instance_t* inst)
{
    (void) inst;

    mutex_init(&caches_lock);
    memset(contact_caches, 0, sizeof(contact_caches));
    memset(channel_caches, 0, sizeof(channel_caches));

    int res;

    /* Create bplib subfolder and its two subfolders */
    res = vfs_mkdir(CONFIG_BPLIB_STOR_BASE, 0777);
    if (res < 0 && res != -EEXIST) {
        return BPLIB_OS_ERROR;
    }

    res = vfs_mkdir(BPLIB_STOR_PATH_DATA, 0777);
    if (res < 0 && res != -EEXIST) {
        return BPLIB_OS_ERROR;
    }

    res = vfs_mkdir(BPLIB_STOR_PATH_INDEX, 0777);
    if (res < 0 && res != -EEXIST) {
        return BPLIB_OS_ERROR;
    }

    return BPLIB_SUCCESS;
}

void BPLib_STOR_Destroy(BPLib_Instance_t* inst)
{
    (void) inst;
    return;
}

/**
 * @brief Stores the bundle, does not free it
 *
 * @param bundle The bundle to store
 * @retval BPLIB_SUCCESS on success
 * @retval other on error
 */
static BPLib_Status_t _bplib_stor_impl(BPLib_Bundle_t* bundle)
{
    /* Since both node and service are 64 bit, one might have to increase the max path length
     * for this to work with truly all. Each needs 16 digits of hexadecimal numbers plus the
     * path separators. The file name itself might also be 16 characters long, as it is the delivery
     * timestamp. */
    int res = 0;
    ssize_t written = 0;
    bool failed = false;
    char path[BPLIB_STOR_PATHLEN] = BPLIB_STOR_PATH_DATA;
    int len = BPLIB_STOR_DATA_LEN;
    int fd = -1;
    BPLib_MEM_Block_t* curr_mem_block;
    bundle_file_header_t header;

    /* Create node directory */
    len += sprintf(path + len, "/%" PRIx64, (int64_t) bundle->blocks.PrimaryBlock.DestEID.Node);
    res = vfs_mkdir(path, 0777);
    if (res < 0 && res != -EEXIST) {
        failed = true;
        goto end_storage;
    }

    /* Create service directory */
    len += sprintf(path + len, "/%" PRIx64, (int64_t) bundle->blocks.PrimaryBlock.DestEID.Service);
    res = vfs_mkdir(path, 0777);
    if (res < 0 && res != -EEXIST) {
        failed = true;
        goto end_storage;
    }

    /* Write bundle */
    len += sprintf(path + len, "/%" PRIx64 "_",
        (int64_t) bundle->blocks.PrimaryBlock.Timestamp.CreateTime +
        (int64_t) bundle->blocks.PrimaryBlock.Lifetime);

    /* See if the file already exists */
    for (uint8_t i = 0; i < CONFIG_BPLIB_STOR_MAX_DUPLICATE_CHECKS; i++) {
        sprintf(path + len, "%02" PRIx8, i);
        fd = vfs_open(path, O_RDONLY, 0777);
        if (fd == -ENOENT) {
            break;
        } else if (fd < 0) {
            goto end_storage;
        } else {
            vfs_close(fd);
        }
    }
    len += 2;
    fd = vfs_open(path, O_CREAT | O_TRUNC | O_WRONLY, 0777);
    if (fd < 0) {
        failed = true;
        goto end_storage;
    }

    header.flags = 0;
    header.flags |= bundle->Meta.IsCustodial ? STORAGE_HEADER_FLAG_CUSTODIAL : 0;

    header.retransmit_timestamp = (int64_t)BPLib_TIME_GetMonotonicTime()
                                + bundle->Meta.RetransmitTime;
    header.retransmit_trigger = bundle->Meta.RetransmitTime;

    /* Write header specific to this storage implementation */
    written = vfs_write(fd, &header, sizeof(bundle_file_header_t));
    if (written < 0 || written != (ssize_t) sizeof(bundle_file_header_t)) {
        failed = true;
        goto close_file;
    }

    /* Write bundle metadata */
    written = vfs_write(fd, &bundle->Meta, sizeof(BPLib_BundleMetaData_t));
    if (written < 0 || written != (ssize_t) sizeof(BPLib_BundleMetaData_t)) {
        failed = true;
        goto close_file;
    }

    /* Write bundle blocks */
    written = vfs_write(fd, &bundle->blocks, sizeof(BPLib_BBlocks_t));
    if (written < 0 || written != (ssize_t) sizeof(BPLib_BBlocks_t)) {
        failed = true;
        goto close_file;
    }

    /* Write chunks */
    curr_mem_block = bundle->blob;
    while (curr_mem_block != NULL) {
        written = vfs_write(fd, curr_mem_block->user_data.BigData, curr_mem_block->used_len);
        if (written < 0 || written != (ssize_t) curr_mem_block->used_len) {
            failed = true;
            goto close_file;
        }
        curr_mem_block = curr_mem_block->next;
    }

    res = vfs_fsync(fd);
    if (res < 0) {
        failed = true;
        goto close_file;
    }
close_file:
    res = vfs_close(fd);

end_storage:
    if (res < 0 || failed) {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_COUNT_DELETED, 1);
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_COUNT_DISCARDED, 1);
        BPLib_EM_SendEvent(BPLIB_STOR_SQL_STORE_ERR_EID, BPLib_EM_EventType_INFORMATION,
            "Bunde Storage failed.");
        return BPLIB_OS_ERROR;
    }

    return BPLIB_SUCCESS;
}

static void _bplib_stor_finalize_custody(BPLib_Instance_t* inst, BPLib_Bundle_t* bundle,
                                         BPLib_CT_DispositionCode_t disp_code)
{
    BPLib_CLA_ContactRunState_t con_state;
    bool pushed_bundle = true;
    bool bundle_stored = true;

    if (disp_code == BPLib_CT_CustodyRefused) {
        bundle_stored = false;
    }

    /* Finalize custodial transfer for custodial bundles */
    (void) BPLib_CT_SignalCustody(inst, bundle, disp_code, bundle_stored);

    /* Custodial bundles with an egress path should get sent out instead of freed */
    (void) BPLib_CLA_GetContactRunState(bundle->Meta.EgressID, &con_state);
    if (bundle->Meta.EgressID < BPLIB_MAX_NUM_CONTACTS &&
        con_state == BPLIB_CLA_STARTED && disp_code == BPLib_CT_CustodyAccepted) {
        pushed_bundle = BPLib_QM_WaitQueueTryPush(&(inst->ContactEgressJobs[bundle->Meta.EgressID]), 
                                    &bundle, QM_WAIT_FOREVER);
    }
    /* There's no egress path, free memory */
    else {
        pushed_bundle = false;
    }

    if (pushed_bundle == false) {
        BPLib_MEM_BundleFree(&inst->pool, bundle);
    }

    return;
}

BPLib_Status_t BPLib_STOR_StoreBundle(BPLib_Instance_t* inst, BPLib_Bundle_t* bundle)
{
    // TODO add size check (ll 284 in bplib_stor.c) and update the size counter correctly
    BPLib_Status_t status;

    // This should be on the else branch of the above already.

    status = _bplib_stor_impl(bundle);
    
    /* Free bundle since it is now persistent. Custodial bundles should not be
     * freed. This is copied from the bplib storage implementation. */
    if (!bundle->Meta.IsCustodial) {
        BPLib_MEM_BundleFree(&inst->pool, bundle);
    }
    else {
        if (status == BPLIB_SUCCESS) {
            /* Storage successful, custody accepted */
            _bplib_stor_finalize_custody(inst, bundle, BPLib_CT_CustodyAccepted);
        }
        else {
            _bplib_stor_finalize_custody(inst, bundle, BPLib_CT_CustodyRefused);
        }
    }

    return status;
}

typedef struct {
    char path[BPLIB_STOR_PATHLEN];
    unsigned node_len;
    unsigned service_len;
    vfs_DIR node_dir;
    vfs_DIR service_dir;
    vfs_DIR bundle_dir;
    bool node_open;
    bool service_open;
    bool bundle_open;
    uint64_t node_val;
    uint64_t service_val;
    uint64_t expiry_val;
    uint8_t index_val;
} bundle_path_iterator_t;

static bundle_path_iterator_t BUNDLE_PATH_ITER_INIT = {
    .path = BPLIB_STOR_PATH_DATA,
    .node_len = 0,
    .service_len = 0,
    .node_open = false,
    .service_open = false,
    .bundle_open = false
};

static BPLib_Status_t _destroy_iterator(bundle_path_iterator_t* iterator)
{
    int res;

    if (iterator == NULL) {
        return BPLIB_STOR_PARAM_ERR;
    }

    if (iterator->node_open) {
        res = vfs_closedir(&iterator->node_dir);
        if (res < 0) {
            return BPLIB_OS_ERROR;
        }
        iterator->node_open = false;
    }

    if (iterator->service_open) {
        res = vfs_closedir(&iterator->service_dir);
        if (res < 0) {
            return BPLIB_OS_ERROR;
        }
        iterator->service_open = false;
    }

    if (iterator->bundle_open) {
        res = vfs_closedir(&iterator->bundle_dir);
        if (res < 0) {
            return BPLIB_OS_ERROR;
        }
        iterator->bundle_open = false;
    }

    return BPLIB_SUCCESS;
}

/**
 * @brief Advance the iterator to the next bundle path
 *
 * These bundles will already be filtered by the dest_eids, but will not yet be
 * filtered by the other conditions of the original SQL query, like the custodial
 * retransmit times. This has to happen outside of this function
 *
 * @param[inout] iterator Iterator over the whole storage
 * @param[in] dest_eids Destination EID patterns to filter by
 * @param num_eids Number of EID entries in dest_eids
 * @retval 0 when one bundle has been read
 * @retval -EAGAIN when one subdirectory is fully traversed but no bundle has been read
 * @retval -EINVAL when the state is invalid or null pointers are passed
 * @retval -ERRNO respective negative errors from vfs_opendir if this fails
 * @retval 1 when traversal of the whole path is completed
 */
static int _next_bundle_path(bundle_path_iterator_t* iterator,
    const BPLib_EID_Pattern_t* dest_eids, size_t num_eids)
{
    int res;
    vfs_dirent_t entry;
    unsigned acc_len;
    size_t i;
    bool match;
    uint64_t decoded;

    if (iterator == NULL || dest_eids == NULL) {
        return -EINVAL;
    }

    if (!iterator->node_open) {
        if (iterator->service_open) {
            return -EINVAL;
        }

        res = vfs_opendir(&iterator->node_dir, BPLIB_STOR_PATH_DATA);
        if (res < 0) {
            return res;
        }

        iterator->node_open = true;
    }

    /* If the service dir iterator is not yet open or the directory is done */
    while (!iterator->service_open) {
        res = vfs_readdir(&iterator->node_dir, &entry);
        if (res < 0) {
            if (res == -EAGAIN) {
                continue;
            }
            else {
                return res;
            }
        }
        if (res == 0) {
            /* All node_id directories have been read */
            return 1;
        }
        if (entry.d_name[0] == '.') {
            continue;
        }

        decoded = strtoull(entry.d_name, NULL, 16);
        match = false;
        for (i = 0; i < num_eids; i++) {
            if (decoded >= dest_eids[i].MinNode && decoded <= dest_eids[i].MaxNode) {
                match = true;
                break;
            }
        }
        if (!match) {
            continue;
        }

        snprintf(iterator->path + BPLIB_STOR_DATA_LEN,
            BPLIB_STOR_PATHLEN - BPLIB_STOR_DATA_LEN,
            "/%s", entry.d_name);
        iterator->node_val = strtoull(entry.d_name, NULL, 16);
        res = vfs_opendir(&iterator->service_dir, iterator->path);
        if (res < 0) {
            return res;
        }
        iterator->service_open = true;
        iterator->node_len = strlen(entry.d_name);
    }

    while (!iterator->bundle_open) {
        res = vfs_readdir(&iterator->service_dir, &entry);
        if (res < 0) {
            if (res == -EAGAIN) {
                continue;
            }
            else {
                return res;
            }
        }
        if (res == 0) {
            res = vfs_closedir(&iterator->service_dir);
            if (res < 0) {
                return res;
            }
            iterator->service_open = false;
            return -EAGAIN;
        }
        if (entry.d_name[0] == '.') {
            continue;
        }

        decoded = strtoull(entry.d_name, NULL, 16);
        match = false;
        for (i = 0; i < num_eids; i++) {
            if (decoded >= dest_eids[i].MinService && decoded <= dest_eids[i].MaxService) {
                match = true;
                break;
            }
        }
        if (!match) {
            continue;
        }

        acc_len = BPLIB_STOR_DATA_LEN + 1 + iterator->node_len;
        snprintf(iterator->path + acc_len,
            BPLIB_STOR_PATHLEN - acc_len,
            "/%s", entry.d_name);
        iterator->service_val = strtoull(entry.d_name, NULL, 16);
        res = vfs_opendir(&iterator->bundle_dir, iterator->path);
        if (res < 0) {
            return res;
        }
        iterator->bundle_open = true;
        iterator->service_len = strlen(entry.d_name);
    }

    while (1) {
        res = vfs_readdir(&iterator->bundle_dir, &entry);
        if (res < 0) {
            if (res == -EAGAIN) {
                continue;
            }
            else {
                return res;
            }
        }
        if (res == 0) {
            vfs_closedir(&iterator->bundle_dir);
            if (res < 0) {
                return res;
            }
            iterator->bundle_open = false;
            return -EAGAIN;
        }
        if (entry.d_name[0] == '.') {
            continue;
        }

        /* Also ignore all bundles that have already expired */
        iterator->expiry_val = strtoull(entry.d_name, NULL, 16);
        if (iterator->expiry_val <= (uint64_t) BPLib_TIME_GetMonotonicTime()) {
            continue;
        }
        // TODO test how this behaves with restarts

        acc_len = BPLIB_STOR_DATA_LEN + 1 + iterator->node_len + 1 + iterator->service_len;
        snprintf(iterator->path + acc_len,
            BPLIB_STOR_PATHLEN - acc_len,
            "/%s", entry.d_name);
        int len = strlen(entry.d_name);
        iterator->index_val = strtol(entry.d_name + len - 2, NULL, 16);

        return 0;
    }
}

/**
 * @brief Check second part of conditions; if custodial bundles should be retransmitted
 *
 * This check is different for bundles that are forwarded and bundles that are
 * locally delivered (channels vs contacts).
 *
 * @param[in] iter Valid iterator result from _next_bundle_path()
 * @param local_delivery true if this bundle is local (this is a channel not a contact)
 *
 * @retval 0  If the bundle SHOULD NOT be loaded
 * @retval >0 If the bundle SHOULD be loaded
 * @retval -ERRNO On vfs errors, it should also not be loaded in that case
 */
static int _should_bundle_be_loaded(bundle_path_iterator_t* iter, bool local_delivery)
{
    int fd;
    int res = 1;
    bundle_file_header_t header;
    ssize_t bytes_read;

    fd = vfs_open(iter->path, O_RDONLY, 0777);
    if (fd < 0) {
        return fd;
    }

    if (!local_delivery) {
        /* The header is the first thing, so no seek is needed. Local delivery
         * does not care about the retransmit intervals, just deliver it as fast
         * as possible. */
        bytes_read = vfs_read(fd, &header, sizeof(bundle_file_header_t));
        if (bytes_read != sizeof(bundle_file_header_t)) {
            res = -EINVAL;
            goto close_file;
        }

        if (!((header.flags & STORAGE_HEADER_FLAG_CUSTODIAL) &&
            (header.retransmit_trigger != BPLIB_NO_RETRANSMIT_TRIGGER) &&
            (header.retransmit_timestamp <= (uint64_t)BPLib_TIME_GetMonotonicTime())))
        {
            res = 0;
        }
    }
    

close_file:
    fd = vfs_close(fd);
    if (fd < 0) {
        res = fd;
    }

    return res;
}

/**
 * @brief Fills the bundle cache, iterating over all bundles that match the filter
 *
 * This fills the bundle cache (which sorts by urgency). It iterates over all
 * bundles in storage, filtered by the EID patterns.
 *
 * @param[in] dest_eids The patterns by which to filter
 * @param num_eids Number of pattern provided
 * @param[out] cache The cache to fill
 * @param local_delivery true if this egresses for a channel, false for a contact
 */
static void _fill_bundle_cache(const BPLib_EID_Pattern_t* dest_eids,
    size_t num_eids, cache_list_t* cache, bool local_delivery)
{
    bundle_path_iterator_t iterator = BUNDLE_PATH_ITER_INIT;
    int res;
    cache->all_bundles_queued = true;

    while (1) {
        do {
            res = _next_bundle_path(&iterator, dest_eids, num_eids);
        } while (res == -EAGAIN);
        /* bundle was found, is in the iterator or nothing was found */
        if (res == 0) {
            res = _should_bundle_be_loaded(&iterator, local_delivery);
            if (res <= 0) {
                continue;
            }

            res = bplib_cache_add(cache, iterator.node_val, iterator.service_val,
                iterator.expiry_val, iterator.index_val);

            if (res > 0) {
                /* Some bundle is now not in cache anymore */
                cache->all_bundles_queued = false;
            }
        }
        else {
            if (res != 1) {
                /* Some other error but not full traversal */
                cache->all_bundles_queued = false;
            }
            break;
        }
    }

    _destroy_iterator(&iterator);
}

/**
 * @brief Allocate and load the bundle data from vfs from the cache values.
 *
 * @param[in] inst bplib instance
 * @param[out] bundle output bundle
 * @param[in] cache bundle storage cache
 * @param[out] path path of the bundle on vfs, to unlink outside of the function
 * @retval BPLIB_SUCCESS on success
 * @retval BPLIB_ERROR when all bundles have been read
 */
static BPLib_Status_t _load_next_bundle(BPLib_Instance_t* inst, BPLib_Bundle_t** bundle,
    cache_list_t* cache, char* path)
{
    BPLib_Status_t ret = BPLIB_SUCCESS;
    int fd;
    int res;
    ssize_t bytes_read;
    BPLib_MEM_Block_t* bundle_head = NULL;
    BPLib_MEM_Block_t* curr_block = NULL;
    BPLib_MEM_Block_t* next_block = NULL;
    BPLib_Bundle_t* ret_bundle;
    BPLib_MEM_Pool_t* pool = &inst->pool;

    res = bplib_cache_get(cache, path);
    if (res == 1) {
        return BPLIB_ERROR;   /* There is no next bundle */
    }

    fd = vfs_open(path, O_RDONLY, 0777);
    if (fd < 0) {
        return BPLIB_OS_ERROR;
    }

    /* Allocate bundle blocks */
    bundle_head = BPLib_MEM_BlockAlloc(pool, BPLIB_MEM_BIG_BLK_SIZE);
    if (bundle_head == NULL) {
        ret = BPLIB_STOR_NO_MEM_ERR;
        goto close_file;
    }

    /* Skip over the header. It is not needed here where the actual bundle is read */
    vfs_lseek(fd, offsetof(bundle_file_structure_t, meta), SEEK_SET);

    /* Read bundle metadata */
    bytes_read = vfs_read(fd, &bundle_head->user_data.Bundle.Meta, sizeof(BPLib_BundleMetaData_t));
    if (bytes_read != sizeof(BPLib_BundleMetaData_t)) {
        ret = BPLIB_OS_ERROR;
        goto close_file;
    }

    /* Read bundle blocks */
    bytes_read = vfs_read(fd, &bundle_head->user_data.Bundle.blocks, sizeof(BPLib_BBlocks_t));
    if (bytes_read != sizeof(BPLib_BBlocks_t)) {
        ret = BPLIB_OS_ERROR;
        goto close_file;
    }

    curr_block = bundle_head;
    curr_block->used_len = bytes_read;

    while (1) {
        /* Read all the remaining actual data blocks */
        next_block = BPLib_MEM_BlockAlloc(pool, BPLIB_MEM_BIG_BLK_SIZE);
        if (next_block == NULL) {
            ret = BPLIB_STOR_NO_MEM_ERR;
            goto close_file;
        }
        curr_block->next = next_block;

        bytes_read = vfs_read(fd, &next_block->user_data.BigData, sizeof(next_block->user_data.BigData));
        if (bytes_read < 0) {
            ret = BPLIB_OS_ERROR;
            goto close_file;
        }

        curr_block = next_block;
        curr_block->used_len = bytes_read;

        if (bytes_read < (ssize_t) sizeof(next_block->user_data.BigData)) {
            break;
        }
    }

close_file:
    if (vfs_close(fd) < 0) {
        ret = BPLIB_OS_ERROR;
    }

    if (ret != BPLIB_SUCCESS) {
        *bundle = NULL;

        if (bundle_head != NULL) {
            BPLib_MEM_BlockListFree(pool, bundle_head);
        }
    } else {
        ret_bundle = (BPLib_Bundle_t*)(&bundle_head->user_data.Bundle);
        ret_bundle->blob = bundle_head->next;
        *bundle = ret_bundle;
    }

    return ret;
}

BPLib_Status_t BPLib_STOR_EgressForID(BPLib_Instance_t* inst, uint32_t egress_id,
    bool local_delivery, size_t* num_egressed)
{
    BPLib_Bundle_t* curr_bundle = NULL;
    BPLib_EID_Pattern_t local_eid;
    BPLib_EID_Pattern_t* dest_eids;
    BPLib_QM_WaitQueue_t* egress_queue;
    cache_list_t* cache; /* Loadbatch equivalent */
    size_t egress_count = 0;
    size_t num_eids;

    if ((inst == NULL) || (num_egressed == NULL)) {
        return BPLIB_NULL_PTR_ERROR;
    }
    if (local_delivery && egress_id >= BPLIB_MAX_NUM_CHANNELS) {
        return BPLIB_STOR_PARAM_ERR;
    }
    if (!local_delivery && egress_id >= BPLIB_MAX_NUM_CONTACTS) {
        return BPLIB_STOR_PARAM_ERR;
    }

    if (BPLib_QM_IsIngressIdle(inst) == false) {
        /* Avoid searching the DB if the unsorted jobs queue (which is the ingress queue) isn't empty.
         * Note: this is a pretty critical performance optimization that allows bplib
         * to use all of its CPU resources for ingress. */
        *num_egressed = 0;
        return BPLIB_SUCCESS;
    }

    /* Determine which channel or contact's batch we're examining */
    BPLib_NC_ReaderLock();
    if (local_delivery) {
        local_eid.Scheme = BPLIB_EID_INSTANCE.Scheme;
        local_eid.IpnSspFormat = BPLIB_EID_INSTANCE.IpnSspFormat;
        local_eid.MaxAllocator = BPLIB_EID_INSTANCE.Allocator;
        local_eid.MinAllocator = BPLIB_EID_INSTANCE.Allocator;
        local_eid.MaxNode = BPLIB_EID_INSTANCE.Node;
        local_eid.MinNode = BPLIB_EID_INSTANCE.Node;
        local_eid.MaxService = BPLib_NC_ConfigPtrs.ChanConfigPtr->Configs[egress_id].LocalServiceNumber;
        local_eid.MinService = BPLib_NC_ConfigPtrs.ChanConfigPtr->Configs[egress_id].LocalServiceNumber;
        dest_eids = &local_eid;
        num_eids = 1;
        egress_queue = &(inst->ChannelEgressJobs[egress_id]);
        cache = &channel_caches[egress_id];
    } else {
        dest_eids = BPLib_NC_ConfigPtrs.ContactsConfigPtr->ContactSet[egress_id].DestEIDs;
        num_eids = BPLIB_MAX_CONTACT_DEST_EIDS;
        egress_queue = &(inst->ContactEgressJobs[egress_id]);
        cache = &contact_caches[egress_id];
    }
    BPLib_NC_ReaderUnlock();

    mutex_lock(&caches_lock);

    char path[BPLIB_STOR_PATHLEN];
    if (!cache->all_bundles_queued && bplib_cache_is_empty(cache)) {
        _fill_bundle_cache(dest_eids, num_eids, cache, local_delivery);
    }

    while (_load_next_bundle(inst, &curr_bundle, cache, path) == BPLIB_SUCCESS) {
        curr_bundle->Meta.EgressID = egress_id;
        if (BPLib_QM_WaitQueueTryPush(egress_queue, &curr_bundle, QM_NO_WAIT) == false)
        {
            /* If QM couldn't accept the bundle, free it. It will be reloaded next time. */
            BPLib_MEM_BundleFree(&inst->pool, curr_bundle);
            break;
        }

        /* Technically it is not yet sent and could be lost in a power-off after this point
         * Since for RIOT both the Storage and the CLAs can be user defined, this could be
         * prevented by only unlinking after the CLA has really sent the bundle. */
        bplib_cache_mark_front_consumed(cache);
        vfs_unlink(path);
        egress_count++;
    }

    mutex_unlock(&caches_lock);

    *num_egressed = egress_count;
    return BPLIB_SUCCESS;
}

BPLib_Status_t BPLib_STOR_GarbageCollect(BPLib_Instance_t* inst)
{
    (void) inst;
    bundle_path_iterator_t iterator = BUNDLE_PATH_ITER_INIT;
    int res;
    int fd;
    ssize_t bytes_read;
    BPLib_EID_Pattern_t wildcard = {
        .Scheme       = BPLIB_EID_SCHEME_IPN,
        .IpnSspFormat = BPLIB_EID_IPN_SSP_FORMAT_TWO_DIGIT,
        .MinAllocator = 0,
        .MaxAllocator = 0xFFFFFFFFFFFFFFFF,
        .MinNode      = 0,
        .MaxNode      = 0xFFFFFFFFFFFFFFFF,
        .MinService   = 0,
        .MaxService   = 0xFFFFFFFFFFFFFFFF,
    };

    // TODO update GC

    uint64_t time_ref_dtn = BPLib_TIME_GetCurrentDtnTime();
    uint64_t lifetime;
    BPLib_TIME_MonotonicTime_t bundle_creation_time;
    BPLib_TIME_MonotonicTime_t curr_time;
    BPLib_Status_t bplib_status;
    int64_t delta;

    curr_time.Time = BPLib_TIME_GetMonotonicTime();
    curr_time.BootEra = BPLib_TIME_GetBootEra();

    /* Since bundles might be deleted all caches have to be invalidated */
    mutex_lock(&caches_lock);

    while (1) {
        bool delete = false;

        do {
            res = _next_bundle_path(&iterator, &wildcard, 1);
        } while (res == -EAGAIN);
        if (res == 0) {
            fd = vfs_open(iterator.path, O_RDONLY, 0777);
            if (fd < 0) {
                /* Since we just found it by the iterator but it can't seem to open
                 * try to remove it */
                vfs_unlink(iterator.path);
                continue;
            }
            /* Read relevant bundle info.
             * Since 7.0.5 each bundle has a BPLib_BundleMetaData_t in front, but the containing
             * BPLib_Bundle_t might not be packed, but it is on disk. */
            vfs_lseek(fd, offsetof(BPLib_BBlocks_t, PrimaryBlock.Lifetime) +
                          sizeof(BPLib_BundleMetaData_t), SEEK_SET);
            bytes_read = vfs_read(fd, &lifetime, sizeof(uint64_t));
            vfs_lseek(fd, offsetof(BPLib_BBlocks_t, PrimaryBlock.MonoTime) +
                          sizeof(BPLib_BundleMetaData_t), SEEK_SET);
            bytes_read += vfs_read(fd, &bundle_creation_time, sizeof(BPLib_TIME_MonotonicTime_t));

            if (vfs_close(fd) < 0 ||
                bytes_read != sizeof(BPLib_TIME_MonotonicTime_t) + sizeof(uint64_t)) {
                continue;
            }

            bplib_status = BPLib_TIME_GetTimeDelta(curr_time, bundle_creation_time, &delta);

            if (bplib_status == BPLIB_SUCCESS &&
                delta > (int64_t) lifetime) {
                delete = true;
            }

            /* Generally, if the DTN time is available all bundles should be deleted
             * that expired previously */
            if (time_ref_dtn != 0 &&
                time_ref_dtn >= iterator.expiry_val) {
                delete = true;
            }

            /* Delete */
            if (delete) {
                vfs_unlink(iterator.path);
            }
        }
        else {
            break;
        }
    }
    // TODO add deletion of partial bundles, i.e. bundles where only the metadata
    // has been written to a file
    // TODO deletion AS

    memset(contact_caches, 0, sizeof(contact_caches));
    memset(channel_caches, 0, sizeof(channel_caches));

    mutex_unlock(&caches_lock);
    _destroy_iterator(&iterator);

    return BPLIB_SUCCESS;
}

BPLib_Status_t BPLib_STOR_FlushPending(BPLib_Instance_t* inst)
{
    (void) inst;
    /* All bundles stores are directly written to vfs, no need to flush */
    return BPLIB_SUCCESS;
}

void BPLib_STOR_UpdateHkPkt(BPLib_Instance_t* inst)
{
    (void) inst;
    return;
}

/* Validate Storage table data */
BPLib_Status_t BPLib_STOR_StorageTblValidateFunc(void *tbl_data)
{
    (void) tbl_data;
    return BPLIB_SUCCESS;
}

static void _bplib_stor_update_custodial_unlocked(BPLib_Instance_t* inst,
                                BPLib_STOR_CtUpdateBatch_t* custody_batch)
{
    BPLib_Status_t status;
    //status = BPLib_SQL_UpdateCustodialBundles(inst, custody_batch);
    (void) status;
    (void) inst;
    status = BPLIB_SUCCESS;

    if (status != BPLIB_SUCCESS) {
        BPLib_EM_SendEvent(BPLIB_STOR_CCS_ERR_EID, BPLib_EM_EventType_ERROR,
                "Error performing CCS storage operations, Status = %d.", status);
    }

    custody_batch->Size = 0;
    return;
}

void BPLib_STOR_AddToCustodialUpdateBatch(BPLib_Instance_t *inst,
                                          uint32_t bundle_id,
                                          BPLib_CT_StorOp_t op)
{
    BPLib_STOR_CtUpdateBatch_t *custody_batch;

    if (inst == NULL || inst->BundleStorage.CustodyUpdateBatch.Size >= BPLIB_STOR_CT_BATCH_SIZE)
    {
        return;
    }

    mutex_lock(&(inst->BundleStorage.lock));

    custody_batch = &(inst->BundleStorage.CustodyUpdateBatch);

    custody_batch->BundleIDs[custody_batch->Size] = bundle_id;
    custody_batch->Ops[custody_batch->Size] = op;
    custody_batch->Size++;

    if (custody_batch->Size >= BPLIB_STOR_CT_BATCH_SIZE)
    {
        _bplib_stor_update_custodial_unlocked(inst, custody_batch);
    }

    mutex_unlock(&(inst->BundleStorage.lock));

    return;
}

void BPLib_STOR_UpdateCustodialBundles(BPLib_Instance_t* inst)
{
    BPLib_STOR_CtUpdateBatch_t *custody_batch;

    if (inst == NULL) {
        return;
    }

    mutex_lock(&(inst->BundleStorage.lock));

    custody_batch = &(inst->BundleStorage.CustodyUpdateBatch);

    if ((custody_batch->Size <= BPLIB_STOR_CT_BATCH_SIZE) && (custody_batch->Size > 0)) {
        _bplib_stor_update_custodial_unlocked(inst, custody_batch);
    }

    mutex_unlock(&(inst->BundleStorage.lock));
    
    return;
}

BPLib_Status_t BPLib_STOR_SetNewRetransmitTrigger(BPLib_Instance_t *inst,
                                                  uint32_t contact_id)
{
    (void) inst;
    (void) contact_id;
    return BPLIB_SUCCESS;
}

BPLib_Status_t BPLib_STOR_Egress(BPLib_Instance_t *instance, size_t max_bundles)
{
    BPLib_Status_t status;
    uint32_t c;
    size_t num_loaded;
    BPLib_CLA_ContactRunState_t con_state;

    for (c = 0; c < BPLIB_MAX_NUM_CHANNELS; c++) {
        if (BPLib_NC_GetAppState(c) == BPLIB_NC_APP_STATE_STARTED &&
            BPLib_PI_GetRegistrationState(instance, c) == BPLIB_PI_ACTIVE)
        {
            status = BPLib_STOR_EgressForID(instance, c, true, &num_loaded);
            if (status != BPLIB_SUCCESS || num_loaded >= max_bundles) {
                return status;
            }
        }
    }

    for (c = 0; c < BPLIB_MAX_NUM_CONTACTS; c++) {
        (void) BPLib_CLA_GetContactRunState(c, &con_state);
        if (con_state == BPLIB_CLA_STARTED) {
            status = BPLib_STOR_EgressForID(instance, c, false, &num_loaded);
            if (status != BPLIB_SUCCESS || num_loaded >= max_bundles) {
                return status;
            }
        }
    }

    return BPLIB_SUCCESS;
}

void bplib_stor_vfs_contact_changed(uint32_t contact_index)
{
    mutex_lock(&caches_lock);
    memset(&contact_caches[contact_index], 0, sizeof(cache_list_t));
    mutex_unlock(&caches_lock);
}

void bplib_stor_vfs_channel_changed(uint32_t channel_index)
{
    mutex_lock(&caches_lock);
    memset(&channel_caches[channel_index], 0, sizeof(cache_list_t));
    mutex_unlock(&caches_lock);
}

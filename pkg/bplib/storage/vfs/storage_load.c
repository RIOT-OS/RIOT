/*
 * NASA Docket No. GSC-19,559-1, and identified as "Delay/Disruption Tolerant Networking
 * (DTN) bundle Protocol (BP) v7 Core Flight System (cFS) Application Build 7.0
 *
 * SPDX-FileCopyrightText: 2025 United States Government as represented by the Administrator of the
 * SPDX-FileCopyrightText: National Aeronautics and Space Administration.
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @ingroup     pkg_bplib_storage_vfs
 *
 * @{
 *
 * @file
 * @brief       Functions loading bundles from storage
 * @author      Simon Grund <mail@simongrund.de>
 *
 * Based loosely on bplib's sqlite storage implementation in [bplib]/bpa/stor/
 *
 * @}
 */
#include "bplib_stor_vfs.h"
#include "storage_common.h"
#include "cache.h"

#include <fcntl.h>
#include <inttypes.h>
#include <stdlib.h>
#include <errno.h>

#include "bplib.h"

#include "vfs.h"

/** @brief A bundle has been read and is in the iterator */
#define NEXT_BUNDLE_PATH_BUNDLE_READ            0
/** @brief Iteration completed without errors, no bundle is in the iterator */
#define NEXT_BUNDLE_PATH_ITERATION_COMPLETED    1

typedef struct {
    char path[BPLIB_STOR_PATHLEN_DAT];
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
    uint32_t id_val;
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
 * retransmit times. This has to happen outside of this function.
 *
 * @param[inout] iterator Iterator over the whole storage
 * @param[in] dest_eids Destination EID patterns to filter by
 * @param[in] num_eids Number of EID entries in dest_eids
 * @retval NEXT_BUNDLE_PATH_BUNDLE_READ when one bundle has been read
 * @retval -EAGAIN when one subdirectory is fully traversed but no bundle has been read
 * @retval -EINVAL when the state is invalid or null pointers are passed
 * @retval -ERRNO respective negative errors from vfs_opendir if this fails
 * @retval NEXT_BUNDLE_PATH_ITERATION_COMPLETED when traversal of the whole path is completed
 */
static int _next_bundle_path(bundle_path_iterator_t* iterator,
    const BPLib_EID_Pattern_t* dest_eids, size_t num_eids)
{
    int res;
    vfs_dirent_t entry;
    unsigned acc_len;
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
            return NEXT_BUNDLE_PATH_ITERATION_COMPLETED;
        }
        if (entry.d_name[0] == '.') {
            continue;
        }

        decoded = strtoull(entry.d_name, NULL, 16);
        match = false;
        for (size_t i = 0; i < num_eids; i++) {
            if ((decoded >= dest_eids[i].MinNode) && (decoded <= dest_eids[i].MaxNode)) {
                match = true;
                break;
            }
        }
        if (!match) {
            continue;
        }

        snprintf(iterator->path + BPLIB_STOR_DATA_LEN,
            BPLIB_STOR_PATHLEN_DAT - BPLIB_STOR_DATA_LEN,
            "/%s", entry.d_name);
        iterator->node_val = decoded;
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
        for (size_t i = 0; i < num_eids; i++) {
            if ((decoded >= dest_eids[i].MinService) && (decoded <= dest_eids[i].MaxService)) {
                match = true;
                break;
            }
        }
        if (!match) {
            continue;
        }

        acc_len = BPLIB_STOR_DATA_LEN + 1 + iterator->node_len;
        snprintf(iterator->path + acc_len,
            BPLIB_STOR_PATHLEN_DAT - acc_len,
            "/%s", entry.d_name);
        iterator->service_val = decoded;
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
        if (iterator->expiry_val <= (uint64_t)BPLib_TIME_GetMonotonicTime()) {
            continue;
        }

        acc_len = BPLIB_STOR_DATA_LEN + 1 + iterator->node_len + 1 + iterator->service_len;
        snprintf(iterator->path + acc_len,
            BPLIB_STOR_PATHLEN_DAT - acc_len,
            "/%s", entry.d_name);
        iterator->id_val = strtoul(strchr(entry.d_name, '_') + 1, NULL, 16);

        return NEXT_BUNDLE_PATH_BUNDLE_READ;
    }
}

/**
 * @brief Check second part of conditions; if custodial bundles should be retransmitted
 *
 * This check is different for bundles that are forwarded and bundles that are
 * locally delivered (channels vs contacts).
 *
 * @param[in] iter Valid iterator result from _next_bundle_path()
 * @param[in] local_delivery true if this bundle is local (this is a channel not a contact)
 *
 * @retval 0  If the bundle SHOULD NOT be loaded
 * @retval >0 If the bundle SHOULD be loaded
 * @retval -ERRNO On vfs errors, it should also not be loaded in that case
 */
static int _should_bundle_be_loaded(bundle_path_iterator_t* iter, bool local_delivery)
{
    int fd;
    int res = 0;
    bundle_file_header_t header;
    ssize_t bytes_read;

    fd = vfs_open(iter->path, O_RDONLY, 0777);
    if (fd < 0) {
        return fd;
    }

    if (!local_delivery) {
        /* The header is the first thing, so no seek is needed. */
        bytes_read = vfs_read(fd, &header, sizeof(bundle_file_header_t));
        if (bytes_read != sizeof(bundle_file_header_t)) {
            res = -EINVAL;
            goto close_file;
        }

        if (header.flags & STORAGE_HEADER_FLAG_CUSTODIAL) {
            if (!(header.flags & STORAGE_HEADER_FLAG_RETRANS_OFF) &&
                (header.retransmit_timestamp <= (uint64_t)BPLib_TIME_GetMonotonicTime())) {
                /* Load custodial bundles only if retransmissions are not turned off
                 * and the time for a retransmission is reached */
                res = 1;
            }
        }
        else {
            /* Always load non-custodial bundles */
            res = 1;
        }
    }
    else {
        /* Always load local delivery bundles*/
        res = 1;
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
 * @param[in] num_eids Number of pattern provided
 * @param[out] cache The cache to fill
 * @param[in] local_delivery true if this egresses for a channel, false for a contact
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

        /* Error or full traversal */
        if (res != NEXT_BUNDLE_PATH_BUNDLE_READ) {
            if (res != NEXT_BUNDLE_PATH_ITERATION_COMPLETED) {
                /* On any error some bundles probably remain */
                cache->all_bundles_queued = false;
            }
            break;
        }

        /* A bundle was found and is in the iterator */
        res = _should_bundle_be_loaded(&iterator, local_delivery);
        if (res <= 0) {
            /* A custodial bundle should not yet be retransmitted. */
            cache->all_bundles_queued = false;
            continue;
        }

        res = bplib_cache_add(cache, iterator.node_val, iterator.service_val,
            iterator.expiry_val, iterator.id_val);

        if ((res == BPLIB_CACHE_ADD_IGNORED) || (res == BPLIB_CACHE_ADD_REPLACED)) {
            /* Some bundle is now not in cache anymore */
            cache->all_bundles_queued = false;
        }

        /* TODO for future efforts: Add early return option so that not all
            * bundles in storage are searched and ordered, but only N, where N
            * is the size of the cache. This should speed up egress, while
            * giving up on the orderedness. */
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
    ssize_t status;
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
    status = vfs_lseek(fd, offsetof(bundle_file_structure_t, meta), SEEK_SET);
    if (status < 0) {
        ret = BPLIB_OS_ERROR;
        goto close_file;
    }

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

        if (bytes_read < (ssize_t)sizeof(next_block->user_data.BigData)) {
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
    }
    else {
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
        cache = &bplib_stor_common_data.channel_caches[egress_id];
    }
    else {
        dest_eids = BPLib_NC_ConfigPtrs.ContactsConfigPtr->ContactSet[egress_id].DestEIDs;
        num_eids = BPLIB_MAX_CONTACT_DEST_EIDS;
        egress_queue = &(inst->ContactEgressJobs[egress_id]);
        cache = &bplib_stor_common_data.contact_caches[egress_id];
    }
    BPLib_NC_ReaderUnlock();

    mutex_lock(&bplib_stor_common_data.caches_lock);

    char path[BPLIB_STOR_PATHLEN_DAT];
    if (!cache->all_bundles_queued && bplib_cache_is_empty(cache)) {
        _fill_bundle_cache(dest_eids, num_eids, cache, local_delivery);
    }

    while (_load_next_bundle(inst, &curr_bundle, cache, path) == BPLIB_SUCCESS) {
        curr_bundle->Meta.EgressID = egress_id;
        if (!BPLib_QM_WaitQueueTryPush(egress_queue, &curr_bundle, QM_NO_WAIT)) {
            /* If QM couldn't accept the bundle, free it. It will be reloaded next time. */
            BPLib_MEM_BundleFree(&inst->pool, curr_bundle);
            break;
        }

        /* Technically it is not yet sent and could be lost in a power-off after this point
         * Since for RIOT both the Storage and the CLAs can be user defined, this could be
         * prevented by only unlinking after the CLA has really sent the bundle. */
        bplib_cache_mark_front_consumed(cache);

        /* In contrast to the SQLite based implementation of bplib, just delete the bundle
         * here directly and not mark it as deletable. */
        if (local_delivery || !curr_bundle->Meta.IsCustodial) {
            bplib_stor_vfs_delete_bundle(curr_bundle->blocks.PrimaryBlock.BundleId);
        }

        /* Update the retransmission timestamp. Local delivery custody bundles should
         * be instantly delivered and dont have a timeout. */
        if (curr_bundle->Meta.IsCustodial && !local_delivery) {
            bplib_stor_vfs_update_retransmission_time(path,
                BPLib_NC_ConfigPtrs.ContactsConfigPtr->ContactSet[egress_id].RetransmitTimeout);
        }
        egress_count++;
    }

    mutex_unlock(&bplib_stor_common_data.caches_lock);

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

    /* TODO for follow up PR: Update this GC function
     * It should additionally now
     * - delete the index files, use _delete_bundle, no direct vfs_unlink here
     * - integrate other updates to the function from upstream
     * - check size of files to delete bundles that are less than expected (write failed)
     * - increment AS on deletion
     * - handle errors in vfs_read and vfs_lseek
     * - call BPLib_CT_DeleteBundleFromCtdb for custodial bundles */

    uint64_t time_ref_dtn = BPLib_TIME_GetCurrentDtnTime();
    uint64_t lifetime;
    BPLib_TIME_MonotonicTime_t bundle_creation_time;
    BPLib_TIME_MonotonicTime_t curr_time;
    BPLib_Status_t bplib_status;
    int64_t delta;

    curr_time.Time = BPLib_TIME_GetMonotonicTime();
    curr_time.BootEra = BPLib_TIME_GetBootEra();

    /* Since bundles might be deleted all caches have to be invalidated */
    mutex_lock(&bplib_stor_common_data.caches_lock);

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

            if ((vfs_close(fd) < 0) ||
                (bytes_read != (sizeof(BPLib_TIME_MonotonicTime_t) + sizeof(uint64_t)))) {
                continue;
            }

            bplib_status = BPLib_TIME_GetTimeDelta(curr_time, bundle_creation_time, &delta);

            if ((bplib_status == BPLIB_SUCCESS) && (delta > (int64_t)lifetime)) {
                delete = true;
            }

            /* Generally, if the DTN time is available all bundles should be deleted
             * that expired previously */
            if ((time_ref_dtn != 0) && (time_ref_dtn >= iterator.expiry_val)) {
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

    memset(bplib_stor_common_data.contact_caches, 0, sizeof(bplib_stor_common_data.contact_caches));
    memset(bplib_stor_common_data.channel_caches, 0, sizeof(bplib_stor_common_data.channel_caches));

    mutex_unlock(&bplib_stor_common_data.caches_lock);
    _destroy_iterator(&iterator);

    return BPLIB_SUCCESS;
}

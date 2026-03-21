/*
 * NASA Docket No. GSC-19,559-1, and identified as "Delay/Disruption Tolerant Networking
 * (DTN) bundle Protocol (BP) v7 Core Flight System (cFS) Application Build 7.0
 *
 * SPDX-FileCopyrightText: 2025 United States Government as represented by the Administrator of the
 * SPDX-FileCopyrightText: National Aeronautics and Space Administration.
 * SPDX-FileCopyrightText: 2026 Hamburg University of Technology (TUHH)
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

#include "vfs.h"

#include "bplib.h"

#include "cache.h"

static mutex_t caches_lock = MUTEX_INIT;
static cache_list_t contact_caches[BPLIB_MAX_NUM_CONTACTS];
static cache_list_t channel_caches[BPLIB_MAX_NUM_CHANNELS];

#ifndef CONFIG_BPLIB_STOR_MAX_DUPLICATE_CHECKS
#  define CONFIG_BPLIB_STOR_MAX_DUPLICATE_CHECKS 64
#endif

/* 16 chars each for 64 bit hex numbers, 1 for all / and the last 1 + 2 + 1 is used
 * for the _xx deduplicator in case of identical times + the null terminator */
#define BPLIB_STOR_PATHLEN (BPLIB_STOR_BASELEN + 1 + 16 + 1 + 16 + 1 + 16 + 1 + 2 + 1)

BPLib_Status_t BPLib_STOR_Init(BPLib_Instance_t* inst)
{
    (void) inst;

    mutex_init(&caches_lock);
    memset(contact_caches, 0, sizeof(contact_caches));
    memset(channel_caches, 0, sizeof(channel_caches));

    char *path = CONFIG_BPLIB_STOR_BASE;
    int res;

    /* Create bplib subfolder */
    res = vfs_mkdir(path, 0777);
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

BPLib_Status_t BPLib_STOR_StoreBundle(BPLib_Instance_t* inst, BPLib_Bundle_t* bundle)
{
    /* Since both node and service are 64 bit, one might have to increase the max path length
    *  for this to work with truly all. Each needs 16 digits of hexadecimal numbers plus the
    *  path separators. The file name itself might also be 16 characters long, as it is the delivery
    *  timestamp.
    **/
    int res = 0;
    ssize_t written = 0;
    bool failed = false;
    char path[BPLIB_STOR_PATHLEN] = CONFIG_BPLIB_STOR_BASE;
    int len = BPLIB_STOR_BASELEN;
    int fd = -1;
    BPLib_MEM_Block_t* curr_mem_block;

    /* Create node directory */
    len += sprintf(path + len, "/%" PRIx64, (int64_t) bundle->blocks.PrimaryBlock.DestEID.Node);
    res = vfs_mkdir(path, 0777);
    if (res < 0 && res != -EEXIST) {
        failed = true;
        goto free_bundle;
    }

    /* Create service directory */
    len += sprintf(path + len, "/%" PRIx64, (int64_t) bundle->blocks.PrimaryBlock.DestEID.Service);
    res = vfs_mkdir(path, 0777);
    if (res < 0 && res != -EEXIST) {
        failed = true;
        goto free_bundle;
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
            goto free_bundle;
        } else {
            vfs_close(fd);
        }
    }
    len += 2;
    fd = vfs_open(path, O_CREAT | O_TRUNC | O_WRONLY, 0777);
    if (fd < 0) {
        failed = true;
        goto free_bundle;
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
        written = vfs_write(fd, curr_mem_block->user_data.raw_bytes, curr_mem_block->used_len);
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

free_bundle:
    /* Free bundle since it is now persistent */
    BPLib_MEM_BundleFree(&inst->pool, bundle);

    if (res < 0 || failed) {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_COUNT_DELETED, 1);
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_COUNT_DISCARDED, 1);
        BPLib_EM_SendEvent(BPLIB_STOR_SQL_STORE_ERR_EID, BPLib_EM_EventType_INFORMATION,
            "Bunde Storage failed.");
        return BPLIB_OS_ERROR;
    }

    return BPLIB_SUCCESS;
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
    .path = CONFIG_BPLIB_STOR_BASE,
    .node_len = 0,
    .service_len = 0,
    .node_open = false,
    .service_open = false,
    .bundle_open = false
};

static BPLib_Status_t destroy_iterator(bundle_path_iterator_t* iterator)
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
 * @brief
 *
 * @param iterator
 * @param dest_eids
 * @param num_eids
 * @return - 0 when one bundle has been read
 *         - -EAGAIN when one subdirectory is fully traversed but no bundle has been read
 *         - -EINVAL when the state is invalid or null pointers are passed
 *         - respective negative errors from vfs_opendir if this fails
 *         - 1 when traversal of the whole path is completed
 */
static int next_bundle_path(bundle_path_iterator_t* iterator,
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

        res = vfs_opendir(&iterator->node_dir, CONFIG_BPLIB_STOR_BASE);
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

        snprintf(iterator->path + BPLIB_STOR_BASELEN,
            BPLIB_STOR_PATHLEN - BPLIB_STOR_BASELEN,
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

        acc_len = BPLIB_STOR_BASELEN + 1 + iterator->node_len;
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

        acc_len = BPLIB_STOR_BASELEN + 1 + iterator->node_len + 1 + iterator->service_len;
        snprintf(iterator->path + acc_len,
            BPLIB_STOR_PATHLEN - acc_len,
            "/%s", entry.d_name);
        iterator->expiry_val = strtoull(entry.d_name, NULL, 16);
        int len = strlen(entry.d_name);
        iterator->index_val = strtol(entry.d_name + len - 2, NULL, 16);

        return 0;
    }
}

static void fill_bundle_cache(const BPLib_EID_Pattern_t* dest_eids,
    size_t num_eids, cache_list_t* cache)
{
    bundle_path_iterator_t iterator = BUNDLE_PATH_ITER_INIT;
    int res;
    cache->all_bundles_queued = true;

    while (1) {
        do {
            res = next_bundle_path(&iterator, dest_eids, num_eids);
        } while (res == -EAGAIN);
        /* bundle was found, is in the iterator or nothing was found */
        if (res == 0) {
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

    destroy_iterator(&iterator);
}

/**
 * @brief Allocate and load the bundle data from vfs.
 *
 *
 * @param inst bplib instance
 * @param bundle output bundle
 * @param cache bundle storage cache
 * @param path path of the bundle on vfs, to unlink outside of the function
 * @return BPLIB_SUCCESS on success
 *         BPLIB_ERROR when all bundles have been read
 */
static BPLib_Status_t load_next_bundle(BPLib_Instance_t* inst, BPLib_Bundle_t** bundle,
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
    bundle_head = BPLib_MEM_BlockAlloc(pool);
    if (bundle_head == NULL) {
        ret = BPLIB_STOR_NO_MEM_ERR;
        goto close_file;
    }

    /* Read bundle blocks */
    bytes_read = vfs_read(fd, &bundle_head->user_data.bundle.blocks, sizeof(BPLib_BBlocks_t));
    if (bytes_read != sizeof(BPLib_BBlocks_t)) {
        ret = BPLIB_OS_ERROR;
        goto close_file;
    }

    curr_block = bundle_head;
    curr_block->used_len = bytes_read;

    while (1) {
        /* Read all the remaining actual data blocks */
        next_block = BPLib_MEM_BlockAlloc(pool);
        if (next_block == NULL) {
            ret = BPLIB_STOR_NO_MEM_ERR;
            goto close_file;
        }
        curr_block->next = next_block;

        bytes_read = vfs_read(fd, &next_block->user_data.raw_bytes, sizeof(next_block->user_data.raw_bytes));
        if (bytes_read < 0) {
            ret = BPLIB_OS_ERROR;
            goto close_file;
        }

        curr_block = next_block;
        curr_block->used_len = bytes_read;

        if (bytes_read < (ssize_t) sizeof(next_block->user_data.raw_bytes)) {
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
        ret_bundle = (BPLib_Bundle_t*)(bundle_head);
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
        ** Note: this is a pretty critical performance optimization that allows bplib
        ** to use all of its CPU resources for ingress.
        */
        *num_egressed = 0;
        return BPLIB_SUCCESS;
    }

    /* Determine which channel or contact's batch we're examining */
    BPLib_NC_ReaderLock();
    if (local_delivery) {
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
        fill_bundle_cache(dest_eids, num_eids, cache);
    }

    while (load_next_bundle(inst, &curr_bundle, cache, path) == BPLIB_SUCCESS) {
        curr_bundle->Meta.EgressID = egress_id;
        if (BPLib_QM_WaitQueueTryPush(egress_queue, &curr_bundle, QM_NO_WAIT) == false)
        {
            /* If QM couldn't accept the bundle, free it. It will be reloaded
            ** next time.
            */
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
            res = next_bundle_path(&iterator, &wildcard, 1);
        } while (res == -EAGAIN);
        if (res == 0) {
            fd = vfs_open(iterator.path, O_RDONLY, 0777);
            if (fd < 0) {
                /* Since we just found it by the iterator but it cant seem to open
                 * try to remove it */
                vfs_unlink(iterator.path);
                continue;
            }
            /* Read relevant bundle info */
            vfs_lseek(fd, offsetof(BPLib_BBlocks_t, PrimaryBlock.Lifetime), SEEK_SET);
            bytes_read = vfs_read(fd, &lifetime, sizeof(uint64_t));
            vfs_lseek(fd, offsetof(BPLib_BBlocks_t, PrimaryBlock.MonoTime), SEEK_SET);
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

    memset(contact_caches, 0, sizeof(contact_caches));
    memset(channel_caches, 0, sizeof(channel_caches));

    mutex_unlock(&caches_lock);
    destroy_iterator(&iterator);
    
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
/*
 * NASA Docket No. GSC-19,559-1, and identified as "Delay/Disruption Tolerant Networking
 * (DTN) Bundle Protocol (BP) v7 Core Flight System (cFS) Application Build 7.0
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
 * @brief       VFS Based storage, unordered egress
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

typedef struct {
    /**
     * @brief When true, all bundles in the storage that the channel
     * or contact can reach are in this queue.
     */
    bool all_bundles_queued;
    uint16_t rb[CONFIG_BPLIB_EGRESS_CACHE_LEN];
    unsigned start;
    unsigned length;
} bundle_queue_t;

static mutex_t queues_lock = MUTEX_INIT;
static bundle_queue_t contact_queues[BPLIB_MAX_NUM_CONTACTS];
static bundle_queue_t channel_queues[BPLIB_MAX_NUM_CHANNELS];

static uint16_t next_id;

#define BPLIB_STOR_PATHLEN (BPLIB_STOR_BASELEN + 1 + 4 + 1)

BPLib_Status_t BPLib_STOR_Init(BPLib_Instance_t* inst)
{
    (void) inst;

    mutex_init(&queues_lock);
    memset(contact_queues, 0, sizeof(contact_queues));
    memset(channel_queues, 0, sizeof(channel_queues));
    next_id = 0;

    char *path = CONFIG_BPLIB_STOR_BASE;
    int res;

    /* Create bplib subfolder */
    res = vfs_mkdir(path, 0777);
    if (res < 0 && res != -EEXIST) {
        return BPLIB_OS_ERROR;
    }

    return BPLIB_SUCCESS;
}

static int open_next_free(void)
{
    int fd;
    char path[BPLIB_STOR_PATHLEN] = CONFIG_BPLIB_STOR_BASE "/";
    uint16_t mod = 8;

    while (1) {
        sprintf(path + BPLIB_STOR_BASELEN + 1, "%" PRIx16, next_id);
        fd = vfs_open(path, O_WRONLY | O_CREAT | O_EXCL, 0777);
        if (fd == -EEXIST) {
            /* Compute the next slot id to try */
            /* a better strategy could be used to find a free slot.
             * Either use linear search here and live with the possible runtime
             * Or use a faster search and hope that a slot is found without loops */
            next_id += mod;
            mod = mod * 2 + 1;
            continue;
        } 
        else {
            /* Return in failure case and success case */
            /* In this case always try the next slot on the next bundle */
            next_id++;
            return fd;
        } 
    }
}

BPLib_Status_t BPLib_STOR_StoreBundle(BPLib_Instance_t* inst, BPLib_Bundle_t* bundle)
{
    int res = 0;
    ssize_t written = 0;
    bool failed = false;
    int fd = -1;
    BPLib_MEM_Block_t* curr_mem_block;

    fd = open_next_free();
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
    /* Free bundle since it is now persistent (or could not be stored) */
    BPLib_MEM_BundleFree(&inst->pool, bundle);

    if (res < 0 || failed) {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_COUNT_DELETED, 1);
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_COUNT_DISCARDED, 1);
        BPLib_EM_SendEvent(BPLIB_STOR_SQL_STORE_ERR_EID, BPLib_EM_EventType_INFORMATION,
            "Bunde Storage failed.");
        return BPLIB_OS_ERROR;
    }

    /* The contact / channel cache is cleared when the contact / channel is started.
     * This means repetetive starting / stopping has to rebuild the cache (queue) every
     * time. It should be enough to only clear on removal / teardown but then here the
     * queues would have to be updated (possibly filtered) when a new bundle is stored
     * and all_bundles_queued has to reflect this, otherwise the storage might not be searched. */
    return BPLIB_SUCCESS;
}

typedef struct {
    char path[BPLIB_STOR_PATHLEN];
    vfs_DIR dir;
    bool dir_open;
    uint16_t entry;
} bundle_path_iterator_t;

static bundle_path_iterator_t BUNDLE_PATH_ITER_INIT = {
    .path = CONFIG_BPLIB_STOR_BASE "/",
    .dir_open = false,
    .entry = 0
};

static BPLib_Status_t destroy_iterator(bundle_path_iterator_t* iterator)
{
    int res;

    if (iterator == NULL) {
        return BPLIB_STOR_PARAM_ERR;
    }

    if (iterator->dir_open) {
        res = vfs_closedir(&iterator->dir);
        if (res < 0) {
            return BPLIB_OS_ERROR;
        }
        iterator->dir_open = false;
    }

    return BPLIB_SUCCESS;
}

/**
 * @brief Get the next bundle from the vfs.
 *
 * @param iterator
 * @return 0 if iterator has been updated
 *         1 if search is completed
 *         other negative errno errors
 */
static int next_bundle_path(bundle_path_iterator_t* iterator)
{
    int res;
    vfs_dirent_t entry;

    if (!iterator->dir_open) {
        res = vfs_opendir(&iterator->dir, CONFIG_BPLIB_STOR_BASE);
        if (res < 0) {
            return res;
        }

        iterator->dir_open = true;
    }

    while (1) {
        res = vfs_readdir(&iterator->dir, &entry);
        if (res < 0) {
            if (res == -EAGAIN) {
                continue;
            }
            else return res;
        }
        if (res == 0) {
            /* All bundles have been read */
            return 1;
        }
        if (entry.d_name[0] == '.') {
            continue;
        }

        strncpy(iterator->path + BPLIB_STOR_BASELEN + 1, entry.d_name, 4);
        iterator->path[BPLIB_STOR_PATHLEN - 1] = '\0';
        sscanf(entry.d_name, "%"SCNx16, &iterator->entry);

        return 0;
    }
}

/**
 * @brief Fill the bundle queue from the storage.
 * 
 * Note: mutex is acquired
 *
 * @param dest_eids EID filter
 * @param num_eids EID filter count
 * @param queue queue to insert to
 */
static void fill_bundle_queue(BPLib_EID_Pattern_t* dest_eids,
    size_t num_eids, bundle_queue_t* queue)
{
    bundle_path_iterator_t iterator = BUNDLE_PATH_ITER_INIT;
    BPLib_EID_t dest;
    int res;
    int fd;
    ssize_t bytes_read;

    while (queue->length < CONFIG_BPLIB_EGRESS_CACHE_LEN) {
        res = next_bundle_path(&iterator);

        if (res == 0) {
            /* Check if the contact / channel can serve this EID */
            fd = vfs_open(iterator.path, O_RDONLY, 0777);
            if (fd < 0) {
                continue;
            }

            vfs_lseek(fd, offsetof(BPLib_BBlocks_t, PrimaryBlock.DestEID), SEEK_SET);

            /* Read bundle blocks */
            bytes_read = vfs_read(fd, &dest, sizeof(BPLib_EID_t));
            if (vfs_close(fd) < 0 || bytes_read != sizeof(BPLib_EID_t)) {
                continue;
            }

            /* Check if the destination is served by this contact / channel */
            bool eid_match = false;
            for (unsigned i = 0; i < num_eids; i++) {
                /* This might behave oddly for channels / local deliveries
                 * since the IpnSspFormat is also compared */
                if (BPLib_EID_PatternIsMatch(&dest, &dest_eids[i])) {
                    eid_match = true;
                    break;
                }
            }

            if (!eid_match) {
                continue;
            }

            /* Bundle can be served by the current contact / channel */
            queue->rb[(queue->start + queue->length) % CONFIG_BPLIB_EGRESS_CACHE_LEN] = iterator.entry;
            queue->length++;
        }
        else {
            break;
        }
    }

    if (res == 1) {
        /* All bundles have been read */
        queue->all_bundles_queued = true;
    } else {
        queue->all_bundles_queued = false;
    }

    destroy_iterator(&iterator);
}

/**
 * @brief Allocate and load the bundle data from vfs.
 *
 * Note: The queues_lock is aquired.
 *
 * @param inst bplib instance
 * @param bundle output bundle
 * @param queue bundle storage queue (cache)
 * @param path path of the bundle on vfs, to unlink outside of the function
 * @return BPLIB_SUCCESS on success
 *         BPLIB_ERROR when all bundles have been read
 */
static BPLib_Status_t load_next_bundle(BPLib_Instance_t* inst, BPLib_Bundle_t** bundle,
    bundle_queue_t* queue, char* path)
{
    BPLib_Status_t ret = BPLIB_SUCCESS;
    int fd;
    ssize_t bytes_read;
    BPLib_MEM_Block_t* bundle_head = NULL;
    BPLib_MEM_Block_t* curr_block = NULL;
    BPLib_MEM_Block_t* next_block = NULL;
    BPLib_Bundle_t* ret_bundle;
    BPLib_MEM_Pool_t* pool = &inst->pool;

    if (queue->length == 0) {
        return BPLIB_ERROR; /* All bundles of the queue consumed */
    }

    /* Only peek the queue */
    sprintf(path + BPLIB_STOR_BASELEN + 1, "%"PRIx16, queue->rb[queue->start]);

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
    /* Blocks completed, payload remains */

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
            /* The file must have ended */
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
    bundle_queue_t* queue;
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
        queue = &channel_queues[egress_id];
    } else {
        dest_eids = BPLib_NC_ConfigPtrs.ContactsConfigPtr->ContactSet[egress_id].DestEIDs;
        num_eids = BPLIB_MAX_CONTACT_DEST_EIDS;
        egress_queue = &(inst->ContactEgressJobs[egress_id]);
        queue = &contact_queues[egress_id];
    }
    BPLib_NC_ReaderUnlock();

    mutex_lock(&queues_lock);

    if (!queue->all_bundles_queued && queue->length == 0) {
        fill_bundle_queue(dest_eids, num_eids, queue);
    }

    char path[BPLIB_STOR_PATHLEN] = CONFIG_BPLIB_STOR_BASE "/";
    while (load_next_bundle(inst, &curr_bundle, queue, path) == BPLIB_SUCCESS) {
        curr_bundle->Meta.EgressID = egress_id;
        if (BPLib_QM_WaitQueueTryPush(egress_queue, &curr_bundle, QM_NO_WAIT) == false)
        {
            /* If QM couldn't accept the bundle, free it. It will be reloaded
            ** next time.
            */
            BPLib_MEM_BundleFree(&inst->pool, curr_bundle);

            break;
        }

        /* Pop the queue and delete the file on vfs */
        queue->length--;
        queue->start = (queue->start + 1) % CONFIG_BPLIB_EGRESS_CACHE_LEN;
        vfs_unlink(path);
        egress_count++;
    }

    mutex_unlock(&queues_lock);

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

    uint64_t time_ref_dtn = BPLib_TIME_GetCurrentDtnTime();
    uint64_t bundle_creation_time_dtn;
    uint64_t lifetime;
    BPLib_TIME_MonotonicTime_t bundle_creation_time;
    BPLib_TIME_MonotonicTime_t curr_time;
    BPLib_Status_t bplib_status;
    int64_t delta;

    curr_time.Time = BPLib_TIME_GetMonotonicTime();
    curr_time.BootEra = BPLib_TIME_GetBootEra();

    /* Since bundles might be deleted all caches have to be invalidated */
    mutex_lock(&queues_lock);

    while (1) {
        bool delete = false;

        do {
            res = next_bundle_path(&iterator);
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
            vfs_lseek(fd, offsetof(BPLib_BBlocks_t, PrimaryBlock.Timestamp.CreateTime), SEEK_SET);
            bytes_read = vfs_read(fd, &bundle_creation_time_dtn, sizeof(uint64_t));
            vfs_lseek(fd, offsetof(BPLib_BBlocks_t, PrimaryBlock.Lifetime), SEEK_SET);
            bytes_read += vfs_read(fd, &lifetime, sizeof(uint64_t));
            vfs_lseek(fd, offsetof(BPLib_BBlocks_t, PrimaryBlock.MonoTime), SEEK_SET);
            bytes_read += vfs_read(fd, &bundle_creation_time, sizeof(BPLib_TIME_MonotonicTime_t));
            

            if (vfs_close(fd) < 0 ||
                bytes_read != sizeof(BPLib_TIME_MonotonicTime_t) +
                              sizeof(uint64_t) +
                              sizeof(uint64_t)) {
                continue;
            }

            bplib_status = BPLib_TIME_GetTimeDelta(curr_time, bundle_creation_time, &delta);

            if (bplib_status == BPLIB_SUCCESS &&
                delta > (int64_t) lifetime) {
                delete = true;
            }

            /* Generally, if the DTN time is available all bundles should be deleted
             * that expired previously */
            if (time_ref_dtn != 0 && bundle_creation_time_dtn != 0 &&
                time_ref_dtn >= bundle_creation_time_dtn + lifetime) {
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

    memset(contact_queues, 0, sizeof(contact_queues));
    memset(channel_queues, 0, sizeof(channel_queues));

    mutex_unlock(&queues_lock);
    destroy_iterator(&iterator);
    
    return BPLIB_SUCCESS;
}

void bplib_stor_vfs_contact_changed(uint32_t contact_index)
{
    mutex_lock(&queues_lock);
    memset(&contact_queues[contact_index], 0, sizeof(bundle_queue_t));
    mutex_unlock(&queues_lock);
}

void bplib_stor_vfs_channel_changed(uint32_t channel_index)
{
    mutex_lock(&queues_lock);
    memset(&channel_queues[channel_index], 0, sizeof(bundle_queue_t));
    mutex_unlock(&queues_lock);
}
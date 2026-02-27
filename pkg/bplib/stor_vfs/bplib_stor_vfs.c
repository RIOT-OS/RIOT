#include "bplib_stor_vfs.h"

#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <inttypes.h>

#include "vfs.h"

#include "bplib_stor.h"
#include "bplib_qm.h"
#include "bplib_em.h"
#include "bplib_eventids.h"
#include "bplib_fwp.h"
#include "bplib_nc.h"
#include "bplib_eid.h"
#include "bplib_as.h"

#include "bplib_stor_cache.h"

static cache_list_t contact_caches[BPLIB_MAX_NUM_CONTACTS];
static cache_list_t channel_caches[BPLIB_MAX_NUM_CHANNELS];


#ifndef BPLIB_STOR_MOUNTPOINT
#define BPLIB_STOR_MOUNTPOINT "/nvm0/"
#endif

#ifndef BPLIB_STOR_DIRNAME
#define BPLIB_STOR_DIRNAME "bp"
#endif

#ifndef BPLIB_STOR_MAX_DUPLICATE_CHECKS
#define BPLIB_STOR_MAX_DUPLICATE_CHECKS 8
#endif

// Length of both, minus the terminators
#define BPLIB_STOR_BASELEN (sizeof(BPLIB_STOR_MOUNTPOINT) + sizeof(BPLIB_STOR_DIRNAME) - 2)
#define BPLIB_STOR_BASE (BPLIB_STOR_MOUNTPOINT BPLIB_STOR_DIRNAME)

#define BPLIB_STOR_PATHLEN (BPLIB_STOR_BASELEN + 1 + 16 + 1 + 16 + 1 + 16 + 1)

BPLib_Status_t BPLib_STOR_Init(BPLib_Instance_t* Inst) {
    (void) Inst;

    memset(contact_caches, 0, sizeof(contact_caches));
    memset(channel_caches, 0, sizeof(channel_caches));

    char *path = BPLIB_STOR_MOUNTPOINT BPLIB_STOR_DIRNAME;
    int res;

    // Create bplib subfolder
    res = vfs_mkdir(path, 0777);
    if (res < 0 && res != -EEXIST) {
        return BPLIB_OS_ERROR;
    }

    return BPLIB_SUCCESS;
}

void BPLib_STOR_Destroy(BPLib_Instance_t* Inst) {
    (void) Inst;
    return;
}

BPLib_Status_t BPLib_STOR_StoreBundle(BPLib_Instance_t* Inst, BPLib_Bundle_t* Bundle) {
    /* Since both node and service are 64 bit, one might have to increase the max path length
    *  for this to work with truly all. Each needs 16 digits of hexadecimal numbers plus the
    *  path separators. The file name itself might also be 16 characters long, as it is the delivery
    *  timestamp.
    **/
    int res = 0;
    size_t written = 0;
    bool failed = false;
    char path[BPLIB_STOR_PATHLEN] = BPLIB_STOR_MOUNTPOINT BPLIB_STOR_DIRNAME;
    int len = BPLIB_STOR_BASELEN;
    int fd = -1;
    BPLib_MEM_Block_t* CurrMemBlock;

    // Create node directory
    len += sprintf(path + len, "/%" PRIx64, (int64_t) Bundle->blocks.PrimaryBlock.DestEID.Node);
    res = vfs_mkdir(path, 0777);
    if (res < 0 && res != -EEXIST) {
        failed = true;
        goto free_bundle;
    }

    // Create service directory
    len += sprintf(path + len, "/%" PRIx64, (int64_t) Bundle->blocks.PrimaryBlock.DestEID.Service);
    res = vfs_mkdir(path, 0777);
    if (res < 0 && res != -EEXIST) {
        failed = true;
        goto free_bundle;
    }

    // Write bundle
    len += sprintf(path + len, "/%" PRIx64 "_", (int64_t) Bundle->blocks.PrimaryBlock.Timestamp.CreateTime +
                                       (int64_t) Bundle->blocks.PrimaryBlock.Lifetime);
    // See if the file already exists
    for (uint8_t i = 0; i < BPLIB_STOR_MAX_DUPLICATE_CHECKS; i++) {
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

    // Write bundle blocks
    written = vfs_write(fd, &Bundle->blocks, sizeof(BPLib_BBlocks_t));
    if (written < 0 || written != sizeof(BPLib_BBlocks_t)) {
        failed = true;
        goto close_file;
    }

    // Write chunks
    CurrMemBlock = Bundle->blob;
    while (CurrMemBlock != NULL) {
        written = vfs_write(fd, CurrMemBlock->user_data.raw_bytes, CurrMemBlock->used_len);
        if (written < 0 || written != CurrMemBlock->used_len) {
            failed = true;
            goto close_file;
        }
        CurrMemBlock = CurrMemBlock->next;
    }

    res = vfs_fsync(fd);
    if (res < 0) {
        failed = true;
        goto close_file;
    }
close_file:
    res = vfs_close(fd);

free_bundle:
    // Free bundle since it is now persistent
    BPLib_MEM_BundleFree(&Inst->pool, Bundle);

    if (res < 0 || failed) {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_COUNT_DELETED, 1);
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_COUNT_DISCARDED, 1);
        BPLib_EM_SendEvent(BPLIB_STOR_SQL_STORE_ERR_EID, BPLib_EM_EventType_INFORMATION,
            "Bunde Storage failed.");
        return BPLIB_OS_ERROR;
    }

    return BPLIB_SUCCESS;
}

BPLib_Status_t BPLib_STOR_FlushPending(BPLib_Instance_t* Inst) {
    (void) Inst;
    /* All bundles stores are directly written to vfs, no need to flush */
    return BPLIB_SUCCESS;
}

struct BundlePathIterator {
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
};

static struct BundlePathIterator BUNDLE_PATH_ITER_INIT = {
    .path = BPLIB_STOR_BASE,
    .node_len = 0,
    .service_len = 0,
    .node_open = false,
    .service_open = false,
    .bundle_open = false
};

static BPLib_Status_t DestroyIterator(struct BundlePathIterator* iterator) {
    int res;

    if (iterator == NULL) return BPLIB_STOR_PARAM_ERR;

    if (iterator->node_open) {
        res = vfs_closedir(&iterator->node_dir);
        if (res < 0) return BPLIB_OS_ERROR;
        iterator->node_open = false;
    }

    if (iterator->service_open) {
        res = vfs_closedir(&iterator->service_dir);
        if (res < 0) return BPLIB_OS_ERROR;
        iterator->service_open = false;
    }

    if (iterator->bundle_open) {
        res = vfs_closedir(&iterator->bundle_dir);
        if (res < 0) return BPLIB_OS_ERROR;
        iterator->bundle_open = false;
    }

    return BPLIB_SUCCESS;
}

/**
 * @brief
 *
 * @param iterator
 * @param DestEIDs
 * @param NumEIDs
 * @return - 0 when one bundle has been read
 *         - -EAGAIN when one subdirectory is fully traversed but no bundle has been read
 *         - -EINVAL when the state is invalid or null pointers are passed
 *         - respective negative errors from vfs_opendir if this fails
 *         - 1 when traversal of the whole path is completed
 */
static int NextBundlePath(struct BundlePathIterator* iterator,
    const BPLib_EID_Pattern_t* DestEIDs, size_t NumEIDs)
    {
    int res;
    vfs_dirent_t entry;
    unsigned acc_len;
    size_t i;
    bool match;
    uint64_t decoded;

    // Assume correct format for the EIDs

    if (iterator == NULL || DestEIDs == NULL) return -EINVAL;

    if (!iterator->node_open) {
        if (iterator->service_open) return -EINVAL;

        // Create iterator
        res = vfs_opendir(&iterator->node_dir, BPLIB_STOR_BASE);
        if (res < 0) return res;

        iterator->node_open = true;
    }

    // If the service dir iterator is not yet open or the directory is done
    // After the loop the dir iterator is guaranteed to be open
    while (!iterator->service_open) {
        res = vfs_readdir(&iterator->node_dir, &entry);
        if (res < 0) {
            if (res == -EAGAIN) continue;
            else return res;
        }
        if (res == 0) {
            // All node directories have been read
            return 1;
        }
        if (entry.d_name[0] == '.') continue;

        decoded = strtoull(entry.d_name, NULL, 16);
        match = false;
        for (i = 0; i < NumEIDs; i++) {
            if (decoded >= DestEIDs[i].MinNode && decoded <= DestEIDs[i].MaxNode) {
                match = true;
                break;
            }
        }
        if (!match) continue;

        snprintf(iterator->path + BPLIB_STOR_BASELEN,
            BPLIB_STOR_PATHLEN - BPLIB_STOR_BASELEN,
            "/%s", entry.d_name);
        iterator->node_val = strtoull(entry.d_name, NULL, 16);
        res = vfs_opendir(&iterator->service_dir, iterator->path);
        if (res < 0) return res;
        iterator->service_open = true;
        iterator->node_len = strlen(entry.d_name);
    }

    while (!iterator->bundle_open) {
        res = vfs_readdir(&iterator->service_dir, &entry);
        if (res < 0) {
            if (res == -EAGAIN) continue;
            else return res;
        }
        if (res == 0) {
            res = vfs_closedir(&iterator->service_dir);
            if (res < 0) return res;
            iterator->service_open = false;
            return -EAGAIN;
        }
        if (entry.d_name[0] == '.') continue;

        decoded = strtoull(entry.d_name, NULL, 16);
        match = false;
        for (i = 0; i < NumEIDs; i++) {
            if (decoded >= DestEIDs[i].MinService && decoded <= DestEIDs[i].MaxService) {
                match = true;
                break;
            }
        }
        if (!match) continue;

        acc_len = BPLIB_STOR_BASELEN + 1 + iterator->node_len;
        snprintf(iterator->path + acc_len,
            BPLIB_STOR_PATHLEN - acc_len,
            "/%s", entry.d_name);
        iterator->service_val = strtoull(entry.d_name, NULL, 16);
        res = vfs_opendir(&iterator->bundle_dir, iterator->path);
        if (res < 0) return res;
        iterator->bundle_open = true;
        iterator->service_len = strlen(entry.d_name);
    }

    while (1) {
        res = vfs_readdir(&iterator->bundle_dir, &entry);
        if (res < 0) {
            if (res == -EAGAIN) continue;
            else return res;
        }
        if (res == 0) {
            vfs_closedir(&iterator->bundle_dir);
            if (res < 0) return res;
            iterator->bundle_open = false;
            return -EAGAIN;
        }
        if (entry.d_name[0] == '.') continue;

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

static void FillBundleCache(const BPLib_EID_Pattern_t* DestEIDs,
    size_t NumEIDs, cache_list_t* cache)
{
    struct BundlePathIterator iterator = BUNDLE_PATH_ITER_INIT;
    int res;

    while(1) {
        do {
            res = NextBundlePath(&iterator, DestEIDs, NumEIDs);
        } while (res == -EAGAIN);
        /* Bundle was found, is in the iterator or nothing was found */
        if (res == 0) {
            bplib_cache_add(cache, iterator.node_val, iterator.service_val, iterator.expiry_val, iterator.index_val);
        } else {
            break;
        }
    }

    DestroyIterator(&iterator);
}

static BPLib_Status_t LoadNextBundle(BPLib_Instance_t* Inst, BPLib_Bundle_t** Bundle,
    cache_list_t* cache, char* path)
    {
    BPLib_Status_t ret = BPLIB_SUCCESS;
    int fd;
    int res;
    ssize_t bytes_read;
    BPLib_MEM_Block_t* BundleHead = NULL;
    BPLib_MEM_Block_t* CurrBlock = NULL;
    BPLib_MEM_Block_t* NextBlock = NULL;
    BPLib_Bundle_t* RetBundle;
    BPLib_MEM_Pool_t* Pool = &Inst->pool;

    res = bplib_cache_get(cache, path);
    if (res == 1) return BPLIB_ERROR;   // There is no next bundle

    fd = vfs_open(path, O_RDONLY, 0777);
    if (fd < 0) {
        return BPLIB_OS_ERROR;
    }

    // Allocate bundle blocks
    BundleHead = BPLib_MEM_BlockAlloc(Pool);
    if (BundleHead == NULL) {
        ret = BPLIB_STOR_NO_MEM_ERR;
        goto close_file;
    }

    // Read bundle blocks
    bytes_read = vfs_read(fd, &BundleHead->user_data.bundle.blocks, sizeof(BPLib_BBlocks_t));
    if (bytes_read != sizeof(BPLib_BBlocks_t)) {
        ret = BPLIB_OS_ERROR;
        goto close_file;
    }

    CurrBlock = BundleHead;
    CurrBlock->used_len = bytes_read;
    // The bundle blocks (=header kind of) has been loaded successfully

    while (1) {
        // Read all the remaining actual data blocks
        NextBlock = BPLib_MEM_BlockAlloc(Pool);
        if (NextBlock == NULL) {
            ret = BPLIB_STOR_NO_MEM_ERR;
            goto close_file;
        }
        CurrBlock->next = NextBlock;

        bytes_read = vfs_read(fd, &NextBlock->user_data.raw_bytes, sizeof(NextBlock->user_data.raw_bytes));
        if (bytes_read < 0) {
            ret = BPLIB_OS_ERROR;
            goto close_file;
        }

        CurrBlock = NextBlock;
        CurrBlock->used_len = bytes_read;

        if (bytes_read < (ssize_t) sizeof(NextBlock->user_data.raw_bytes)) {
            // The file must have ended
            break;
        }
    }

close_file:
    if (vfs_close(fd) < 0) {
        // Say file things are more important than no memory
        ret = BPLIB_OS_ERROR;
    }

    if (ret != BPLIB_SUCCESS) {
        *Bundle = NULL;

        // Dealloc
        if (BundleHead != NULL) {
            BPLib_MEM_BlockListFree(Pool, BundleHead);
        }
    } else {
        RetBundle = (BPLib_Bundle_t*)(BundleHead);
        RetBundle->blob = BundleHead->next;
        *Bundle = RetBundle;
    }

    return ret;
}

BPLib_Status_t BPLib_STOR_EgressForID(BPLib_Instance_t* Inst, uint32_t EgressID, bool LocalDelivery,
    size_t* NumEgressed) {

    BPLib_BundleCache_t* CacheInst;
    BPLib_Bundle_t* CurrBundle = NULL;
    BPLib_EID_Pattern_t LocalEID;
    BPLib_EID_Pattern_t* DestEIDs;
    BPLib_QM_WaitQueue_t* EgressQueue;
    cache_list_t* cache; /* Loadbatch equivalent */
    size_t EgressCnt = 0;
    size_t NumEIDs;

    if ((Inst == NULL) || (NumEgressed == NULL)) {
        return BPLIB_NULL_PTR_ERROR;
    }
    if (LocalDelivery && EgressID >= BPLIB_MAX_NUM_CHANNELS) {
        return BPLIB_STOR_PARAM_ERR;
    }
    if (!LocalDelivery && EgressID >= BPLIB_MAX_NUM_CONTACTS) {
        return BPLIB_STOR_PARAM_ERR;
    }

    if (BPLib_QM_IsIngressIdle(Inst) == false) {
        /* Avoid searching the DB if the unsorted jobs queue (which is the ingress queue) isn't empty.
        ** Note: this is a pretty critical performance optimization that allows bplib
        ** to use all of its CPU resources for ingress.
        */
        *NumEgressed = 0;
        return BPLIB_SUCCESS;
    }

    /* Determine which channel or contact's batch we're examining */
    BPLib_NC_ReaderLock();
    CacheInst = &Inst->BundleStorage;
    if (LocalDelivery) {
        LocalEID.MaxNode = BPLIB_EID_INSTANCE.Node;
        LocalEID.MinNode = BPLIB_EID_INSTANCE.Node;
        LocalEID.MaxService = BPLib_NC_ConfigPtrs.ChanConfigPtr->Configs[EgressID].LocalServiceNumber;
        LocalEID.MinService = BPLib_NC_ConfigPtrs.ChanConfigPtr->Configs[EgressID].LocalServiceNumber;
        DestEIDs = &LocalEID;
        NumEIDs = 1;
        EgressQueue = &(Inst->ChannelEgressJobs[EgressID]);
        cache = &channel_caches[EgressID];
    } else {
        DestEIDs = BPLib_NC_ConfigPtrs.ContactsConfigPtr->ContactSet[EgressID].DestEIDs;
        NumEIDs = BPLIB_MAX_CONTACT_DEST_EIDS;
        EgressQueue = &(Inst->ContactEgressJobs[EgressID]);
        cache = &contact_caches[EgressID];
    }
    BPLib_NC_ReaderUnlock();

    mutex_lock(&CacheInst->lock);

    char path[BPLIB_STOR_PATHLEN];
    if (bplib_cache_is_empty(cache)) {
        FillBundleCache(DestEIDs, NumEIDs, cache);
    }

    while (LoadNextBundle(Inst, &CurrBundle, cache, path) == BPLIB_SUCCESS) {
        CurrBundle->Meta.EgressID = EgressID;
        if (BPLib_QM_WaitQueueTryPush(EgressQueue, &CurrBundle, QM_NO_WAIT) == false)
        {
            /* If QM couldn't accept the bundle, free it. It will be reloaded
            ** next time.
            */
            BPLib_MEM_BundleFree(&Inst->pool, CurrBundle);

            break;
        }

        /* Technically it is not yet sent and could be lost in a power-off after this point
         * Since for RIOT both the Storage and the CLAs can be user defined, this could be
         * prevented by only unlinking after the CLA has really sent the bundle. */
        bplib_cache_mark_front_consumed(cache);
        vfs_unlink(path);
        EgressCnt++;
    }

    mutex_unlock(&CacheInst->lock);

    *NumEgressed = EgressCnt;
    return BPLIB_SUCCESS;
}

BPLib_Status_t BPLib_STOR_GarbageCollect(BPLib_Instance_t* Inst) {
    (void) Inst;
    return BPLIB_SUCCESS;
}

void BPLib_STOR_UpdateHkPkt(BPLib_Instance_t* Inst) {
    (void) Inst;
    return;
}

/* Validate Storage table data */
BPLib_Status_t BPLib_STOR_StorageTblValidateFunc(void *TblData)
{
    (void) TblData;
    // TODO: Implement the use of the storage configuration table. Verification could be copied from bplib storage

    return BPLIB_SUCCESS;
}

void bplib_stor_vfs_contact_changed(uint32_t contact_index) {
    memset(&contact_caches[contact_index], 0, sizeof(cache_list_t));
}

void bplib_stor_vfs_channel_changed(uint32_t channel_index) {
    memset(&channel_caches[channel_index], 0, sizeof(cache_list_t));
}
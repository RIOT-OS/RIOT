/*
 * NASA Docket No. GSC-19,559-1, and identified as "Delay/Disruption Tolerant Networking
 * (DTN) bundle Protocol (BP) v7 Core Flight System (cFS) Application Build 7.0
 *
 * SPDX-FileCopyrightText: 2025 United States Government as represented by the Administrator of the
 * SPDX-FileCopyrightText: National Aeronautics and Space Administration.
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: Apache-2.0
 *
 * Based loosely on bplib's sqlite storage implementation in [bplib]/bpa/stor/
 */

/**
 * @ingroup     pkg_bplib_storage_vfs
 *
 * @{
 *
 * @file
 * @brief       Functions saving bundles to storage
 * @author      Simon Grund <mail@simongrund.de>
 *
 * @}
 */
#include "bplib_stor_vfs.h"
#include "storage_common.h"

#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdlib.h>

#include "vfs.h"

#include "bplib.h"

/**
 * @brief Creates the index file for the bundle to store
 *
 * If this fails, no index file will have been created (or it has been deleted again).
 *
 * @param[in] bundle Bundle to work with
 * @param expiration_timestamp Timestamp where the bundle expires
 * @retval 0 on success
 * @retval -ERRNO on vfs errors
 */
static int _create_index_file(BPLib_Bundle_t* bundle, uint64_t expiration_timestamp)
{
    char path[BPLIB_STOR_PATHLEN_IDX] = BPLIB_STOR_PATH_INDEX;
    int fd;
    ssize_t written = 0;
    bool failed = false;

    /* First create the index file / check if it exists */
    sprintf(path + BPLIB_STOR_INDEX_LEN, "/%" PRIx32, bundle->blocks.PrimaryBlock.BundleId);
    fd = vfs_open(path, O_CREAT | O_EXCL | O_WRONLY, 0777);
    if (fd == -EEXIST) {
        return -EEXIST;
    }

    written = vfs_write(fd, &bundle->blocks.PrimaryBlock.DestEID.Node, sizeof(uint64_t));
    if (written < 0 || written != (ssize_t) sizeof(uint64_t)) {
        failed = true;
        goto close_file;
    }

    written = vfs_write(fd, &bundle->blocks.PrimaryBlock.DestEID.Service, sizeof(uint64_t));
    if (written < 0 || written != (ssize_t) sizeof(uint64_t)) {
        failed = true;
        goto close_file;
    }

    written = vfs_write(fd, &expiration_timestamp, sizeof(uint64_t));
    if (written < 0 || written != (ssize_t) sizeof(uint64_t)) {
        failed = true;
        goto close_file;
    }

close_file:
    fd = vfs_close(fd);

    /* Atomic, if something failed delete the file again */
    if (failed || fd < 0) {
        vfs_unlink(path);
        return -ENOENT;
    }

    return 0;
}

/**
 * @brief Stores the bundle, does not free it
 *
 * This creates the bundle on vfs. It stores the bundle data under the correct
 * subdirectory in the /dat subdirectory of CONFIG_BPLIB_STOR_BASE.
 * Moreover, it also stores a single file in the /idx subdirectory. This file
 * contains the mapping from the 32 bit bundle_id to the actual file in the /dat
 * subdirectory. The index file contains 24 Bytes (node, service and deletion
 * timestamp), which uniquely identifies the bundle in the /dat directory.
 *
 * The /idx is assumed to be unique and storage will fail if a bundle with the
 * same is is already present in the index.
 *
 * This function will increment the counters regarding storage space.
 *
 * @param inst bplib instance
 * @param bundle The bundle to store
 *
 * @retval BPLIB_SUCCESS on success
 * @retval other on error
 */
static BPLib_Status_t _bplib_stor_impl(BPLib_Instance_t* inst, BPLib_Bundle_t* bundle)
{
    int res = 0;
    int res_idx;
    ssize_t written = 0;
    bool failed = false;
    char path[BPLIB_STOR_PATHLEN_DAT] = BPLIB_STOR_PATH_DATA;
    int len = BPLIB_STOR_DATA_LEN;
    int fd = -1;
    BPLib_MEM_Block_t* curr_mem_block;
    bundle_file_header_t header;
    uint64_t expiration_timestamp;

    if (bundle->blocks.PrimaryBlock.Timestamp.CreateTime == 0) {
        /* Unknown DTN time */
        expiration_timestamp = bundle->blocks.PrimaryBlock.Lifetime + BPLib_TIME_GetMonotonicTime();
    }
    else {
        /* Known DTN time */
        expiration_timestamp = bundle->blocks.PrimaryBlock.Timestamp.CreateTime +
                               bundle->blocks.PrimaryBlock.Lifetime;
    }

    res_idx = _create_index_file(bundle, expiration_timestamp);
    if (res_idx != 0) {
        failed = true;
        goto end_storage;
    }

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
    len += sprintf(path + len, "/%" PRIx64 "_%" PRIx32,
        expiration_timestamp, bundle->blocks.PrimaryBlock.BundleId);

    /* Assume now that the ID does not exist, because it did not exist in the index.
     * If it does wrongly exist, just overwrite it. */
    fd = vfs_open(path, O_CREAT | O_TRUNC | O_WRONLY, 0777);
    if (fd < 0) {
        failed = true;
        goto end_storage;
    }

    header.flags = 0;
    header.flags |= bundle->Meta.IsCustodial ? STORAGE_HEADER_FLAG_CUSTODIAL : 0;

    header.retransmit_timestamp = (int64_t)BPLib_TIME_GetMonotonicTime()
                                + bundle->Meta.RetransmitTime;

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
        /* On any error, delete both the index and the bundle again */
        bplib_stor_vfs_delete_bundle(bundle->blocks.PrimaryBlock.BundleId);

        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_COUNT_DELETED, 1);
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_COUNT_DISCARDED, 1);
        BPLib_EM_SendEvent(BPLIB_STOR_SQL_STORE_ERR_EID, BPLib_EM_EventType_INFORMATION,
            "Bunde Storage failed.");
        return BPLIB_OS_ERROR;
    }

    /* The TotalBytes fields already includes the BPLib_BBlocks_t, don't count it twice */
    inst->BundleStorage.BytesStorageInUse +=
        BUNDLE_STORAGE_USAGE_FROM_TOTAL_BYTES(bundle->Meta.TotalBytes);
    inst->BundleStorage.BundleCountStored++;

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
    if (inst->BundleStorage.BytesStorageInUse
        + BUNDLE_STORAGE_USAGE_FROM_TOTAL_BYTES(bundle->Meta.TotalBytes)
        >= BPLIB_MAX_STORED_BUNDLE_BYTES) {
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_COUNT_DELETED_NO_STORAGE, 1);
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_COUNT_DELETED, 1);
        BPLib_AS_Increment(BPLIB_EID_INSTANCE, BUNDLE_COUNT_DISCARDED, 1);

        BPLib_MEM_BundleFree(&(inst->pool), bundle);

        return BPLIB_NO_STOR_ERR;
    }

    BPLib_Status_t status = _bplib_stor_impl(inst, bundle);

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

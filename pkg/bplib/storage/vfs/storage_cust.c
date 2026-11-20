/*
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     pkg_bplib_storage_vfs
 *
 * @{
 *
 * @file
 * @brief       Functions related to Custody Transfer
 * @author      Simon Grund <mail@simongrund.de>
 *
 * @}
 */
#include "storage_common.h"

#include "bplib.h"

static void _bplib_stor_update_custodial_unlocked(BPLib_Instance_t* inst,
                                BPLib_STOR_CtUpdateBatch_t* custody_batch)
{
    char path[BPLIB_STOR_PATHLEN_DAT];

    for (size_t i = 0; i < custody_batch->Size; i++) {
        if (custody_batch->Ops[i] == BPLIB_CT_MARK_DELETE) {
            /* This gets called when custody on a next node was accepted */
            bplib_stor_vfs_delete_bundle(custody_batch->BundleIDs[i]);
            BPLib_CT_DeleteBundleFromCtdb(inst, custody_batch->BundleIDs[i]);
        }
    }

    for (size_t i = 0; i < custody_batch->Size; i++) {
        if (!bplib_stor_vfs_bundle_path_from_id(custody_batch->BundleIDs[i], path)) {
            /* Path could not be loaded, not much we can do. */
            break;
        }

        if (custody_batch->Ops[i] == BPLIB_CT_STOP_RETRANSMIT) {
            /* This gets called when custody on a next node was actively rejected,
             * there is no reason to continue retransmissions to the node.
             * When a CLA is reconfigured this bundle might be retransmitted again. */
            bplib_stor_vfs_update_retransmission_time(path, 0);
        }
        else if (custody_batch->Ops[i] == BPLIB_CT_START_RETRANSMIT) {
            /* I am not too sure when this gets called but retransmissen shall
             * start ASAP again. 1 value to not fall into the 0 case. */
            bplib_stor_vfs_update_retransmission_time(path, 1);
        }
    }

    custody_batch->Size = 0;
    return;
}

void BPLib_STOR_AddToCustodialUpdateBatch(BPLib_Instance_t *inst,
                                          uint32_t bundle_id,
                                          BPLib_CT_StorOp_t op)
{
    BPLib_STOR_CtUpdateBatch_t *custody_batch;

    if (inst == NULL || inst->BundleStorage.CustodyUpdateBatch.Size >= BPLIB_STOR_CT_BATCH_SIZE) {
        return;
    }

    mutex_lock(&(inst->BundleStorage.lock));

    custody_batch = &(inst->BundleStorage.CustodyUpdateBatch);

    custody_batch->BundleIDs[custody_batch->Size] = bundle_id;
    custody_batch->Ops[custody_batch->Size] = op;
    custody_batch->Size++;

    if (custody_batch->Size >= BPLIB_STOR_CT_BATCH_SIZE) {
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
    /* This function SHOULD update all of the bundles retransmit times and is
     * called when a CLA is setup. In practice this only decreases the time to
     * the next retransmission, so it is not super important for now. TODO */
    (void) inst;
    (void) contact_id;
    return BPLIB_SUCCESS;
}

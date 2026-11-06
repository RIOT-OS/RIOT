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
 * @brief       General, initialization and NOOP functions
 * @author      Simon Grund <mail@simongrund.de>
 *
 * @}
 */
#include "bplib_stor_vfs.h"
#include "storage_common.h"

#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <inttypes.h>

#include "vfs.h"

#include "bplib.h"

BPLib_Status_t BPLib_STOR_Init(BPLib_Instance_t* inst)
{
    (void) inst;

    mutex_init(&bplib_stor_common_data.caches_lock);
    memset(bplib_stor_common_data.contact_caches, 0, sizeof(bplib_stor_common_data.contact_caches));
    memset(bplib_stor_common_data.channel_caches, 0, sizeof(bplib_stor_common_data.channel_caches));

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

    /* TODOs for future efforts: Iterate over all bundles to:
     * - Read the current storage usage of bundles that remained in storage
     *   across restarts
     * - Somehow build this CTDB from storage bundles. Currently, custodial bundles
     *   in storage after a restart will be ignored since there is no reference
     *   in RAM. Upstream bplib 7.0.5 also currently does not support this.
     *   For this possibly use BPLib_STOR_Destroy to write the CTDB to VFS such
     *   that at least in planned shutdowns nothing is lost. */

    return BPLIB_SUCCESS;
}

void BPLib_STOR_Destroy(BPLib_Instance_t* inst)
{
    (void) inst;
    return;
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

BPLib_Status_t BPLib_STOR_StorageTblValidateFunc(void *tbl_data)
{
    (void) tbl_data;
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
    mutex_lock(&bplib_stor_common_data.caches_lock);
    memset(&bplib_stor_common_data.contact_caches[contact_index], 0, sizeof(cache_list_t));
    mutex_unlock(&bplib_stor_common_data.caches_lock);
}

void bplib_stor_vfs_channel_changed(uint32_t channel_index)
{
    mutex_lock(&bplib_stor_common_data.caches_lock);
    memset(&bplib_stor_common_data.channel_caches[channel_index], 0, sizeof(cache_list_t));
    mutex_unlock(&bplib_stor_common_data.caches_lock);
}

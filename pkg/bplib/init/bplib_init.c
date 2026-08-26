/*
 * NASA Docket No. GSC-19,559-1, and identified as "Delay/Disruption Tolerant Networking
 * (DTN) Bundle Protocol (BP) v7 Core Flight System (cFS) Application Build 7.0
 *
 * SPDX-FileCopyrightText: 2025 United States Government as represented by the Administrator of the
 * SPDX-FileCopyrightText: National Aeronautics and Space Administration.
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: Apache-2.0
 *
 * Based on bplib's example implementation in [bplib]/app/bpcat.c
 */

/**
 * @ingroup     pkg_bplib_init
 * @{
 *
 * @file
 * @brief       Initialization wrapper for bplib.
 *
 * @author      Simon Grund <mail@simongrund.de>
 *
 * @}
 */
#include "bplib_init.h"

#include "bplib_riot_fwp.h"
#include "bplib_riot_nc.h"

#include <inttypes.h>

static char generic_worker_stack[CONFIG_BPLIB_GENERIC_STACK_SIZE];
static char mem_pool[CONFIG_BPLIB_MEMPOOL_LEN] __attribute__ ((aligned (8)));

bplib_instance_data_t bplib_instance_data;

#define ENABLE_DEBUG 0
#include "debug.h"

static void* generic_worker(void *arg)
{
    (void) arg;
    int32_t worker_id;
    BPLib_Status_t rv = BPLib_QM_RegisterWorker(&bplib_instance_data.BPLibInst, &worker_id);
    if (rv != BPLIB_SUCCESS) {
        DEBUG("bplib QM_RegisterWorker failed with %"PRId32"\n", rv);
        return NULL;
    }

    while (bplib_instance_data.running) {
        BPLib_QM_WorkerRunJob(&bplib_instance_data.BPLibInst, worker_id,
            CONFIG_BPLIB_GEN_WORKER_TIMEOUT);
    }
    return NULL;
}

BPLib_Status_t bplib_init(void)
{
    BPLib_Status_t bplib_status;

    bplib_instance_data.running = 1;
    
    bplib_riot_nc_init(&bplib_instance_data.ConfigPtrs);

    /* Note: The 1 here could be any value, it is ignored in the patches. It gets
     * passed on to QM, but there the compile time constant BPLIB_QM_MAX_JOBS is
     * used instead to prevent dynamic memory allocation */
    bplib_status = BPLib_NC_Init(&bplib_instance_data.ConfigPtrs,
                                 &bplib_fwp_callbacks,
                                 &bplib_instance_data.BPLibInst,
                                 1, mem_pool, (size_t)CONFIG_BPLIB_MEMPOOL_LEN);
    if (bplib_status != BPLIB_SUCCESS) {
        return bplib_status;
    }

    /* Start Generic Worker */
    int rc = thread_create(generic_worker_stack, CONFIG_BPLIB_GENERIC_STACK_SIZE,
        THREAD_PRIORITY_MAIN - 1, 0, generic_worker,
        NULL, "bplib-generic");
    if (rc < 0) {
        return BPLIB_ERROR;
    }

    return BPLIB_SUCCESS;
}

void bplib_terminate(void) {
    bplib_instance_data.running = 0;
    BPLib_QM_QueueTableDestroy(&bplib_instance_data.BPLibInst);
    BPLib_MEM_PoolDestroy(&bplib_instance_data.BPLibInst.pool);
}

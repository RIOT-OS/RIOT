/*
 * NASA Docket No. GSC-19,559-1, and identified as "Delay/Disruption Tolerant Networking
 * (DTN) Bundle Protocol (BP) v7 Core Flight System (cFS) Application Build 7.0
 *
 * SPDX-FileCopyrightText: 2025 United States Government as represented by the Administrator of the
 * SPDX-FileCopyrightText: National Aeronautics and Space Administration.
 * SPDX-FileCopyrightText: 2026 Hamburg University of Technology (TUHH)
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

#include "ztimer.h"

#include <inttypes.h>

static char _generic_worker_stack[CONFIG_BPLIB_GENERIC_STACK_SIZE];
static char _maintenance_stack[CONFIG_BPLIB_MAINTENANCE_STACK_SIZE];

static char _mem_pool[CONFIG_BPLIB_MEMPOOL_LEN] __attribute__ ((aligned (8)));

bplib_instance_data_t bplib_instance_data;

#define ENABLE_DEBUG 0
#include "debug.h"

static void* _generic_worker(void *arg)
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

static void* _maintenance_worker(void *arg)
{
    (void) arg;
    BPLib_Status_t status;

    while (bplib_instance_data.running)
    {
        ztimer_sleep(ZTIMER_MSEC, CONFIG_BPLIB_MAINTENANCE_INTERVAL);

        status = BPLib_STOR_Egress(&bplib_instance_data.BPLibInst,
                                   CONFIG_BPLIB_MAINTENANCE_MAX_BUNDLES);

        if (status != BPLIB_SUCCESS) {
            DEBUG_PUTS("bplib Error egressing from storage");
        }
        
        BPLib_NC_RunMaintenanceActivities(&bplib_instance_data.BPLibInst);
    }

    return NULL;
}

int bplib_init(void)
{
    BPLib_Status_t bplib_status;

    bplib_instance_data.running = 1;

    /* FWP */
    //bplib_status = bplib_riot_fwp_init();
    //if (bplib_status != BPLIB_SUCCESS) {
    //    return 1;
    //}

    /* EM */
    //bplib_status = BPLib_EM_Init();
    //if (bplib_status != BPLIB_SUCCESS) {
    //    return 2;
    //}

    /* Time Management */
    //bplib_status = BPLib_TIME_Init();
    //if (bplib_status != BPLIB_SUCCESS) {
    //    return 3;
    //}

    /* Node Config */
    //bplib_status = bplib_riot_nc_init(&bplib_instance_data.ConfigPtrs);
    //if (bplib_status != BPLIB_SUCCESS) {
    //    return 4;
    //}
    
    bplib_riot_nc_init(&bplib_instance_data.ConfigPtrs);

    // TODO the 1 magic number
    bplib_status = BPLib_NC_Init(&bplib_instance_data.ConfigPtrs,
                                 &bplib_fwp_callbacks,
                                 &bplib_instance_data.BPLibInst,
                                 1, _mem_pool, (size_t)CONFIG_BPLIB_MEMPOOL_LEN);
    if (bplib_status != BPLIB_SUCCESS) {
        return bplib_status;
    }

    /* MEM */
    //bplib_status = BPLib_MEM_PoolInit(&bplib_instance_data.BPLibInst.pool, _mem_pool,
    //    (size_t)CONFIG_BPLIB_MEMPOOL_LEN);
    //if (bplib_status != BPLIB_SUCCESS) {
    //    return 5;
    //}

    /* QM. The last arg is 0 since it (BPLIB_QM_MAX_JOBS) is now a compile time constant */
    //bplib_status = BPLib_QM_QueueTableInit(&bplib_instance_data.BPLibInst, 0);
    //if (bplib_status != BPLIB_SUCCESS) {
    //    return 6;
    //}

    /* Start Generic Worker */
    int rc = thread_create(_generic_worker_stack, CONFIG_BPLIB_GENERIC_STACK_SIZE,
        THREAD_PRIORITY_MAIN - 1, 0, _generic_worker,
        NULL, "bplib-generic");
    if (rc < 0) {
        return rc;
    }

    rc = thread_create(_maintenance_stack, CONFIG_BPLIB_MAINTENANCE_STACK_SIZE,
        THREAD_PRIORITY_MAIN - 1, 0, _maintenance_worker,
        NULL, "bplib-maintenance");
    if (rc < 0) {
        return rc;
    }

    return 0;
}

void bplib_terminate(void) {
    bplib_instance_data.running = 0;
    BPLib_QM_QueueTableDestroy(&bplib_instance_data.BPLibInst);
    BPLib_MEM_PoolDestroy(&bplib_instance_data.BPLibInst.pool);
}

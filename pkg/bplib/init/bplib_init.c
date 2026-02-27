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
#include "bplib_init.h"

#include "bplib_riot_fwp.h"
#include "bplib_riot_nc.h"

#include <inttypes.h>

static char generic_worker_stack[BPLIB_GENERIC_STACK_SIZE];
static char mem_pool[BPLIB_MEMPOOL_LEN] __attribute__ ((aligned (8)));

bplib_instance_data_t bplib_instance_data;

#define ENABLE_DEBUG 0
#include "debug.h"

static void* generic_worker(void * arg)
{
    (void) arg;
    int32_t worker_id; // THIS ONLY WORKS IF THERE'S ONE WORKER ID!!!
    BPLib_Status_t rv = BPLib_QM_RegisterWorker(&bplib_instance_data.BPLibInst, &worker_id);
    if (rv != BPLIB_SUCCESS) {
        DEBUG("bplib QM_RegisterWorker failed with %"PRId32"\n", rv);
        return NULL;
    }

    while (bplib_instance_data.running) {
        BPLib_QM_WorkerRunJob(&bplib_instance_data.BPLibInst, 0, BPLIB_GEN_WORKER_TIMEOUT);
    }
    return NULL;
}

int bplib_init(void)
{
    BPLib_Status_t bplib_status;

    bplib_instance_data.running = 1;

    /* FWP */
    bplib_status = bplib_riot_fwp_init();
    if (bplib_status != BPLIB_SUCCESS) {
        return 1;
    }

    /* EM */
    bplib_status = BPLib_EM_Init();
    if (bplib_status != BPLIB_SUCCESS) {
        return 2;
    }

    /* Time Management */
    bplib_status = BPLib_TIME_Init();
    if (bplib_status != BPLIB_SUCCESS) {
        return 3;
    }

    /* Node Config */
    bplib_status = bplib_riot_nc_init(&bplib_instance_data.ConfigPtrs);
    if (bplib_status != BPLIB_SUCCESS) {
        return 4;
    }

    /* MEM */
    bplib_status = BPLib_MEM_PoolInit(&bplib_instance_data.BPLibInst.pool, mem_pool,
        (size_t)BPLIB_MEMPOOL_LEN);
    if (bplib_status != BPLIB_SUCCESS) {
        return 5;
    }

    /* QM */
    bplib_status = BPLib_QM_QueueTableInit(&bplib_instance_data.BPLibInst, BPLIB_QM_MAX_JOBS);
    if (bplib_status != BPLIB_SUCCESS) {
        return 6;
    }

    /* Start Generic Worker */
    int rc = thread_create(generic_worker_stack, BPLIB_GENERIC_STACK_SIZE,
        THREAD_PRIORITY_MAIN - 1, 0, generic_worker,
        NULL, "bplib-generic");
    if (rc < 0) {
        return 7;
    }

    return 0;
}

void bplib_terminate(void) {
    /* This stops the gen worker after its timeout
     * Might be unsafe since the thread is not joined. But terminate is
     * only called at the end of main() and does not really have any functionality
     * connected to it as of now.
    **/
    bplib_instance_data.running = 0;

    BPLib_QM_QueueTableDestroy(&bplib_instance_data.BPLibInst);
}

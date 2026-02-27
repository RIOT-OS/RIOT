/*
 * SPDX-FileCopyrightText: 2026 Hamburg University of Technology (TUHH)
 * SPDX-License-Identifier: LGPL-2.1-only
 */
#pragma once

#include "bplib.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Size of bplib's memory pool. Used to cache all bundles currently in
 * bplib but not in storage, for example in egress queues. Each bundle however
 * needs the raw bundle size (sizeof BPLib_BBlocks_t), which is not CBOR encoded,
 * and is quite big.
 */
#ifndef BPLIB_MEMPOOL_LEN
#  define BPLIB_MEMPOOL_LEN         20000u
#endif

/**
 * @brief Length of the job queue. The job queue together with it's worker thread
 * is used to move bundles to the different ingress / egress points.
 */
#ifndef BPLIB_QM_MAX_JOBS
#  define BPLIB_QM_MAX_JOBS         32
#endif

/**
 * @brief Time after which the job queue worker (generic worker) times out. If bplib
 * is terminated this amount of time [in ms] may pass, before this thread is terminated.
 */
#ifndef BPLIB_GEN_WORKER_TIMEOUT
#  define BPLIB_GEN_WORKER_TIMEOUT  1000u
#endif

/**
 * @brief Stack size of the generic bplib worker thread.
 * From experiments this can be pretty small.
 */
#ifndef BPLIB_GENERIC_STACK_SIZE
#  define BPLIB_GENERIC_STACK_SIZE  THREAD_STACKSIZE_SMALL
#endif

/**
 * @brief State of the bplib instance.
 */
typedef struct {
    /** @brief Run state of bplib.
     * Should not be changed directly, use bplib_init() and bplib_terminate() */
    volatile int running;
    /** @brief Instance (bplib type) */
    BPLib_Instance_t BPLibInst;
    /** @brief Config pointers, to the config tables, set in NC (bplib type) */
    BPLib_NC_ConfigPtrs_t ConfigPtrs;
} bplib_instance_data_t;

/**
 * @brief Global, containing the bplib instance and relevant information
 */
extern bplib_instance_data_t bplib_instance_data;

/**
 * @brief Initializes bplib, with all of its modules.
 *
 * Before calling this init function, if walltime is available, it should
 * be set correctly.
 *
 * @return 0 on success, values 1-7 for failure in init of the modules
 *         [FWP, EM, TIME, NC, MEM, QM, thread creation] respectively
 */
int bplib_init(void);

/**
 * @brief Terminates bplib.
 *
 * The BPLib_CLA_ContactStop and BPLib_PI_StopApplication and respective teardown funcions
 * should also be called, before this function.
 *
 * Ensures all of the CLAs terminate as well.
 */
void bplib_terminate(void);

#ifdef __cplusplus
}
#endif

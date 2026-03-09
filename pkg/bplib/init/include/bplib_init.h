/*
 * SPDX-FileCopyrightText: 2026 Hamburg University of Technology (TUHH)
 * SPDX-License-Identifier: LGPL-2.1-only
 */
#pragma once

/**
 * @defgroup pkg_bplib_init Initialization
 * @ingroup pkg_bplib
 * @brief Initialization wrapper for bplib.
 *
 * # About
 * Allows initializing and terminating the bplib stack using just two functions
 * bplib_init() and bplib_terminate(). Initialization will init all of bplib's
 * modules and start the generic worker thread.
 *
 * This module owns the memory pool used for bundle allocation in a static
 * array. Its size can be configured with CONFIG_BPLIB_MEMPOOL_LEN.
 *
 * All defines in this module can be overwritten by CFLAGS definitions.
 *
 * @note Since this module initializes also bplib's time module, if an accurate
 *       time source is available (through walltime using the provided
 *       @ref pkg_bplib_fwp, or a custom NC implementation), it has to be set
 *       BEFORE the call of bplib_init().
 *
 * @{
 *
 * @file
 * @brief       Initialization wrapper for bplib.
 *
 * @author      Simon Grund <mail@simongrund.de>
 */

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
#ifndef CONFIG_BPLIB_MEMPOOL_LEN
#  define CONFIG_BPLIB_MEMPOOL_LEN         8192
#endif

/**
 * @brief Time after which the job queue worker (generic worker) times out. If bplib
 * is terminated this amount of time [in ms] may pass, before this thread is terminated.
 */
#ifndef CONFIG_BPLIB_GEN_WORKER_TIMEOUT
#  define CONFIG_BPLIB_GEN_WORKER_TIMEOUT  1000u
#endif

/**
 * @brief Stack size of the generic bplib worker thread.
 * From experiments this can be pretty small.
 *
 * This is not currently configurable through Kconfig.
 */
#ifndef CONFIG_BPLIB_GENERIC_STACK_SIZE
#  define CONFIG_BPLIB_GENERIC_STACK_SIZE  THREAD_STACKSIZE_SMALL
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
 * The BPLib_CLA_ContactStop and BPLib_PI_StopApplication and respective teardown functions
 * should also be called, before this function.
 *
 * Ensures all of the CLAs terminate as well.
 */
void bplib_terminate(void);

#ifdef __cplusplus
}
#endif

/** @} */

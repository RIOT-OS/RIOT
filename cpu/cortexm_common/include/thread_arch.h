/*
 * SPDX-FileCopyrightText: 2021 Koen Zandberg <koen@bergzand.net>
 * SPDX-FileCopyrightText: 2021 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     cpu_cortexm_common
 * @{
 *
 * @file
 * @brief       Implementation of the kernels thread interface
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#include "cpu_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

#define THREAD_API_INLINED

#ifndef DOXYGEN /* Doxygen is in core/include/thread.h */

static inline __attribute__((always_inline)) void thread_yield_higher(void)
{
    /* trigger the PENDSV interrupt to run scheduler and schedule new thread if
     * applicable */
    SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
    /* flush the pipeline. Otherwise we risk that subsequent instructions are
     * executed before the IRQ has actually triggered */
    __ISB();
}

#endif /* DOXYGEN */

/**
 * @brief All svc dispatch handlers have this type.
 */
typedef int (*svc_dispatch_handler_t)(
    unsigned int svc_number, unsigned int *svc_args);

#ifdef NDEBUG
#  define assert_svc_dispatch_manage_handler() do {} while (0)
#else

/**
 * @brief Last file that asserted svc dispatch handler was free.
 */
extern const char *_free_svc_dispatch_handler_last_file;

/**
 * @brief Asserts that SVC dispatch handler is free.
 *
 * @param file[in] Calling file.
 * @param line[in] Line in calling file.
 *
 * @see assert_free_svc_dispatch_handler
 */
void assert_free_svc_dispatch_handler_ex(const char *file, int line);

/**
 * @brief Asserts that SVC dispatch handler is free.
 *
 * To be used in files using set_memory_manage_handler.
 *
 * @see set_svc_dispatch_handler
 * @see assert_free_svc_dispatch_handler_ex
 */
#define assert_free_svc_dispatch_handler()                        \
    assert_free_svc_dispatch_handler_ex(__FILE__, __LINE__); \
    _free_svc_dispatch_handler_last_file = __FILE__

#endif /* NDEBUG */

/**
 * @brief SVC dispatch handler setter.
 *
 * @param handler Handler to set
 *
 * @retval < 0 on NULL handler or if a handler has already been set
 * @retval >= 0 otherwise
 */
int set_svc_dispatch_handler(svc_dispatch_handler_t handler);

/**
 * @brief SVC dispatch handler cleaner.
 */
void remove_svc_dispatch_handler(void);

#ifdef __cplusplus
}
#endif

/** @} */

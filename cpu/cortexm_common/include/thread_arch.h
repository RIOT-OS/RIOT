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

#ifdef __cplusplus
}
#endif

/** @} */

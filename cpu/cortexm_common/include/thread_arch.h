/*
 * Copyright (C) 2021 Koen Zandberg <koen@bergzand.net>
 *               2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
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

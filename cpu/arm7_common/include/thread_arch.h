/*
 * SPDX-FileCopyrightText: 2008-2009 Heiko Will <hwill@inf.fu-berlin.de>
 * SPDX-FileCopyrightText: 2009 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     cpu_arm7_common
 * @{
 *
 * @file
 * @brief       Implementation of the kernels thread interface
 *
 * @author Kaspar Schleiser <kaspar@schleiser.de>
 * @author Heiko Will <heiko.will@fu-berlin.de>
 */

#include "irq.h"

#ifdef __cplusplus
extern "C" {
#endif

#define THREAD_API_INLINED

#ifndef DOXYGEN /* Doxygen is in core/include/thread.h */

static inline __attribute__((always_inline)) void thread_yield_higher(void)
{
    if (irq_is_in()) {
        sched_context_switch_request = 1;
    }
    else {
        __asm__("svc 0\n");
    }
}

#endif /* DOXYGEN */

#ifdef __cplusplus
}
#endif

/** @} */

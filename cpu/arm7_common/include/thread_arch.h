/*
 * Copyright (C) 2008, 2009  Heiko Will <hwill@inf.fu-berlin.de>
 * Copyright (C) 2009 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_arm7_common
 * @{
 *
 * @file
 * @brief       Implementation of the kernels thread interface
 *
 * @author Kaspar Schleiser <kaspar@schleiser.de>
 * @author Heiko Will <heiko.will@fu-berlin.de>
 *
 * @}
 */
#ifndef THREAD_ARCH_H
#define THREAD_ARCH_H

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

#endif /* THREAD_ARCH_H */
/** @} */

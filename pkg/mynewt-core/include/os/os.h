/**
 * Apache Mynewt
 * Copyright 2015-2021 The Apache Software Foundation
 *
 * This product includes software developed at
 * The Apache Software Foundation (http://www.apache.org/).
 *
 * Portions of this software were developed at
 * Runtime Inc, copyright 2015.
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

/**
 * @ingroup     pkg_mynewt_core
 * @{
 *
 * @file
 * @brief       mynewt-core error types
 *
 * @}
 */

#ifndef OS_OS_H
#define OS_OS_H

#include <assert.h>
#include <stdint.h>

#include "irq.h"
#include "os/os_types.h"
#include "os/os_error.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    MyNewt os/%.h required macros
 * @{
 *
 * PLEASE NOTE: Following macro definitions where copied directly from
 *              apache/mynewt-core and are under the copyright specified in
 *              the header.
 *
 */
#ifndef min
#define min(a, b) ((a)<(b)?(a):(b))
#endif

#ifndef max
#define max(a, b) ((a)>(b)?(a):(b))
#endif

#define OS_ALIGN(__n, __a) (                             \
        (((__n) & ((__a) - 1)) == 0)                   ? \
            (__n)                                      : \
            ((__n) + ((__a) - ((__n) & ((__a) - 1))))    \
        )
#define OS_ALIGNMENT    (4)
/** @} */

/**
 * @brief   CPU status register
 */
typedef uint32_t os_sr_t;

/**
 * @name    Entering and exiting critical section defines
 * @{
 */
#define OS_ENTER_CRITICAL(_sr) (_sr = os_hw_enter_critical())
#define OS_EXIT_CRITICAL(_sr) (os_hw_exit_critical(_sr))
#define OS_ASSERT_CRITICAL() assert(os_hw_is_in_critical())
/** @} */

/**
 * @brief   Disable ISRs
 *
 * @return  current isr context
 */
static inline uint32_t os_hw_enter_critical(void)
{
    uint32_t ctx = irq_disable();
    return ctx;
}

/**
 * @brief   Restores ISR context
 *
 * @param[in]   ctx      ISR context to restore.
 */
static inline void os_hw_exit_critical(uint32_t ctx)
{
    irq_restore((unsigned)ctx);
}

/**
 * @brief Check if is in critical section
 *
 * @return true, if in critical section, false otherwise
 */
static inline bool os_hw_is_in_critical(void)
{
    return (irq_is_in() || __get_PRIMASK());
}

/* Mynewt components (not abstracted in NPL or DPL) */
#include "os/endian.h"
#include "os/os_callout.h"
#include "os/os_cputime.h"
#include "os/os_dev.h"
#include "os/os_eventq.h"
#include "os/os_mbuf.h"
#include "os/os_mempool.h"
#include "os/os_mutex.h"
#include "os/os_sem.h"
#include "os/os_task.h"
#include "os/os_time.h"
#include "os/os_trace_api.h"
#include "os/queue.h"

#if IS_USED(MODULE_NIMBLE)
#include "nimble/nimble_npl.h"
#endif

#ifdef __cplusplus
}
#endif

#endif /* OS_OS_H */

/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_uwb_core
 * @{
 *
 * @file
 * @brief       uwb-core DPL (Decawave Porting Layer) error types
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#ifndef DPL_DPL_OS_H
#define DPL_DPL_OS_H

#include <assert.h>
#include <stdint.h>
#include <stdatomic.h>

#include "irq.h"
#include "dpl/dpl_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Entering and exiting critical section defines
 * @{
 */
#define DPL_ENTER_CRITICAL(_sr) (_sr = dpl_hw_enter_critical())
#define DPL_EXIT_CRITICAL(_sr) (dpl_hw_exit_critical(_sr))
#define DPL_ASSERT_CRITICAL() assert(dpl_hw_is_in_critical())
/** @} */

/**
 * @brief   variable to check if ISR are disabled
 */
extern atomic_uint dpl_in_critical;

/**
 * @brief   CPU status register
 */
typedef uint32_t dpl_sr_t;

/**
 * @brief   Disable ISRs
 *
 * @return  current isr context
 */
static inline uint32_t dpl_hw_enter_critical(void)
{
    uint32_t ctx = irq_disable();
    unsigned int count = atomic_load(&dpl_in_critical);
    atomic_store(&dpl_in_critical, count + 1);
    return ctx;
}

/**
 * @brief   Restores ISR context
 *
 * @param[in]   ctx      ISR context to restore.
 */
static inline void dpl_hw_exit_critical(uint32_t ctx)
{
    unsigned int count = atomic_load(&dpl_in_critical);
    atomic_store(&dpl_in_critical, count - 1);
    irq_restore((unsigned)ctx);
}

/**
 * @brief Check if is in critical section
 *
 * @return true, if in critical section, false otherwise
 */
static inline bool dpl_hw_is_in_critical(void)
{
    /*
     * XXX Currently RIOT does not support an API for finding out if interrupts
     *     are currently disabled, hence in a critical section in this context.
     *     So for now, we use this global variable to keep this state for us.
     */
    return (atomic_load(&dpl_in_critical) > 0);
}

#ifdef __cplusplus
}
#endif

#endif /* DPL_DPL_OS_H */

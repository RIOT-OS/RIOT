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
 * @brief       uwb-core DPL (Decawave Porting Layer) os abstraction layer
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#ifndef DPL_DPL_OS_H
#define DPL_DPL_OS_H

#include "os/os.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Entering and exiting critical section defines
 * @{
 */
#define DPL_ENTER_CRITICAL(_sr) (_sr = os_hw_enter_critical())
#define DPL_EXIT_CRITICAL(_sr) (os_hw_exit_critical(_sr))
#define DPL_ASSERT_CRITICAL() assert(os_hw_is_in_critical())
/** @} */

/**
 * @brief   CPU status register
 */
typedef os_sr_t dpl_sr_t;

/**
 * @brief   Disable ISRs
 *
 * @return  current isr context
 */
static inline uint32_t dpl_hw_enter_critical(void)
{
    return os_hw_enter_critical();
}

/**
 * @brief   Restores ISR context
 *
 * @param[in]   ctx      ISR context to restore.
 */
static inline void dpl_hw_exit_critical(uint32_t ctx)
{
    os_hw_exit_critical(ctx);
}

/**
 * @brief Check if is in critical section
 *
 * @return true, if in critical section, false otherwise
 */
static inline bool dpl_hw_is_in_critical(void)
{
    return os_hw_is_in_critical();
}

#ifdef __cplusplus
}
#endif

#endif /* DPL_DPL_OS_H */

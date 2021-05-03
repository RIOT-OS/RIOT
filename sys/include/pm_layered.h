/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_pm_layered Layered PM Infrastructure
 * @ingroup     sys
 * @{
 *
 * This module provides a base infrastructure that MCU's may use to implement
 * periph/pm.
 *
 * This simple power management interface is based on the following assumptions:
 *
 * - CPUs define up to 4 power modes (from zero, the lowest power mode, to
 *   PM_NUM_MODES-1, the highest)
 * - there is an implicit extra idle mode (which has the number PM_NUM_MODES)
 * - individual power modes can be blocked/unblocked, e.g., by peripherals
 * - if a mode is blocked, so are implicitly all lower modes
 * - the idle thread automatically selects and sets the lowest unblocked mode
 *
 * In order to use this module, you'll need to implement pm_set().
 *
 * @file
 * @brief       Layered low power mode infrastructure
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef PM_LAYERED_H
#define PM_LAYERED_H

#include <stdint.h>
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef PROVIDES_PM_OFF
#define PROVIDES_PM_OFF
#endif

#ifndef PROVIDES_PM_SET_LOWEST
#define PROVIDES_PM_SET_LOWEST
#endif

/**
 * @brief Power Management mode blocker typedef
 */
typedef union {
    uint32_t val_u32;                   /**< power mode blockers u32 */
    uint8_t val_u8[PM_NUM_MODES];       /**< power mode blockers u8 */
} pm_blocker_t;

/**
 * @brief   Block a power mode
 *
 * @param[in]   mode      power mode to block
 */
#ifdef MODULE_PM_LAYERED
void pm_block(unsigned mode);
#else
static inline void pm_block(unsigned mode) { (void)mode; }
#endif

/**
 * @brief   Unblock a power mode
 *
 * @param[in]   mode      power mode to unblock
 */
#ifdef MODULE_PM_LAYERED
void pm_unblock(unsigned mode);
#else
static inline void pm_unblock(unsigned mode) { (void)mode; }
#endif

/**
 * @brief   Switches the MCU to a new power mode
 *
 * This function will be called by @ref pm_set_lowest() after determining the
 * lowest non-blocked mode.
 *
 * It needs to be implemented for each MCU using this module.
 *
 * @param[in]   mode      Target power mode
 */
void pm_set(unsigned mode);

/**
 * @brief   Get currently blocked PM modes
 *
 * @return  The current blocker state
 *
 * This function atomically retrieves the currently blocked PM modes.
 */
pm_blocker_t pm_get_blocker(void);

#ifdef __cplusplus
}
#endif

#endif /* PM_LAYERED_H */
/** @} */

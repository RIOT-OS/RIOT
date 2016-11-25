/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    core_pm Power Management
 * @ingroup     core
 * @brief       The kernels power management interface
 * @{
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
 * The following functions need to be implemented for each platform:
 *
 * pm_init()
 * pm_set()
 *
 *
 * @file
 * @brief       Power management interface
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef PM_H_
#define PM_H_

#ifdef MODULE_PM

#include "assert.h"
#include "cpu_pm.h"

#ifdef __cplusplus
 extern "C" {
#endif

#ifndef PM_BLOCKER_INITIAL
#define PM_BLOCKER_INITIAL { .val_u32=0x01010101 }
#endif

/**
 * @brief Power Management mode typedef
 */
typedef union {
    uint32_t val_u32;
    uint8_t val_u8[PM_NUM_MODES];
} pm_blocker_t;

/**
 * @brief Global variable for keeping track of blocked modes
 */
extern volatile pm_blocker_t pm_blocker;

/**
 * @brief   Initialization of power management
 *
 * This function is invoked once during boot.
 * It needs to be implemented for each cpu.
 */
void pm_init(void);

/**
 * @brief   Switches the MCU to a new power mode
 *
 * This function will be called by the idle thread after determining the lowest
 * non-blocked mode.
 *
 * It needs to be implemented for each cpu.
 *
 * @param[in]   mode      Target power mode
 */
void pm_set(unsigned mode);

/**
 * @brief   Block a power mode
 *
 * @param[in]   mode      power mode to block
 */
static inline void pm_block(unsigned mode)
{
    assert(pm_blocker.val_u8[mode] != 255);

    unsigned state = irq_disable();
    pm_blocker.val_u8[mode]++;
    irq_restore(state);
}

/**
 * @brief   Unblock a power mode
 *
 * @param[in]   mode      power mode to unblock
 */
static inline void pm_unblock(unsigned mode)
{
    assert(pm_blocker.val_u8[mode] > 0);

    unsigned state = irq_disable();
    pm_blocker.val_u8[mode]--;
    irq_restore(state);
}

/**
 * @brief   Switches the MCU to the lowest possible power mode
 *
 * This function will be called by the idle thread.
 */
void pm_set_lowest(void);

#ifdef __cplusplus
}
#endif

#endif /* MODULE_PM */

#endif /* __PM_H_ */
/** @} */

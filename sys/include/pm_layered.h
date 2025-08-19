/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    sys_pm_layered Layered PM Infrastructure
 * @ingroup     sys
 * @{
 *
 * # Abstract
 *
 * This module provides a base infrastructure that MCU's may use to implement
 * periph/pm. It assumes that power modes are strictly ordered like a ladder.
 *
 * In order to use this module, you'll need to implement pm_set().
 *
 * # Power Mangament Model in Detail
 *
 * This simple power management interface is based on the following assumptions:
 *
 * - CPUs define a number of power modes (from zero, the lowest power mode, to
 *   PM_NUM_MODES, the highest)
 * - there is an implicit extra busy-wait mode (which has the number PM_NUM_MODES)
 *   where the CPU is kept spinning if all modes are blocked.
 * - individual power modes can be blocked/unblocked, e.g., by peripherals
 * - if a mode is blocked, so are implicitly all lower modes
 * - the idle thread automatically selects and sets the lowest unblocked mode
 *
 * An MCU with 4 power modes (3 explicit, one implicit) would have a value of 3
 * for @ref PM_NUM_MODES and could look like this:
 *
 * \dot
 *  digraph G {
 *      l_3[label=<<B>Level 3</B><BR/>CPU: Active, High Speed Clock: Active, Low Speed Clock: Active<BR/><I>implicit</I>>, shape=rectangle]
 *      l_2[label=<<B>Level 2</B><BR/>CPU: Inactive, High Speed Clock: Active, Low Speed Clock: Active>, shape=rectangle]
 *      l_1[label=<<B>Level 1</B><BR/>CPU: Inactive, High Speed Clock: Inactive, Low Speed Clock: Active>, shape=rectangle]
 *      l_0[label=<<B>Level 0</B><BR/>CPU: Inactive, High Speed Clock: Inactive, Low Speed Clock: Inactive>, shape=rectangle]
 *
 *      l_3 -> l_2 [label=<   depends on>];
 *      l_2 -> l_1 [label=<   depends on>];
 *      l_1 -> l_0 [label=<   depends on>];
 *
 *
 *      // Hack: invisible notes to draw y-axis
 *      top[style=invisible];
 *      bottom[style=invisible];
 *
 *      bottom->top [label=<   higher power<BR/>   consumption>]
 *
 *      // Hack: Force top and bottom at same level as highest and lowest level
 *      subgraph {
 *          rank=same; top; l_3
 *      }
 *
 *      subgraph {
 *          rank=same; bottom; l_0
 *      }
 *  }
 * \enddot
 *
 * @note    If the power management does not match the model used here, it
 *          can just implement @ref pm_set_lowest directly rather than using
 *          the common infrastructure provided here and just implement a custom
 *          book keeping of which power domains may be safely turned off and
 *          implement @ref pm_set_lowest by hand. (E.g. the MSP430 port does
 *          so.)
 *
 * # Implementation Hints
 *
 * All peripheral drivers need to make sure to that they contain matching pairs
 * of @ref pm_block and @ref pm_unblock to prevent the MCU from entering power
 * modes that are incompatible with correct operation of the peripheral as
 * currently configured. If e.g. the timer peripheral in the example above is
 * configured to run from the high speed clock, it should call `pm_block(1)`
 * during a call to @ref timer_init to prevent both Level 1 and Level 0 to be
 * entered and call `pm_unblock(1)` on calls to @ref timer_stop. In calls to
 * @ref timer_start it should only call `pm_block(1)` if the timer was indeed
 * previously stopped.
 *
 * If the timer periphal is instead configured to run from the low speed clock,
 * only Level 0 needs to be blocked and `pm_block(0)` / `pm_unblock(1)` needs
 * to be used.
 *
 * The implementation of @ref pm_set should assume that it is only called via
 * @ref pm_set_lowest, which in turn is automatically called whenever the
 * scheduler runs out of runnable threads (or, in case the idle thread is used,
 * whenever the idle thread is run). As a result it safely can omit checks
 * whether a given power mode is safe to enter and directly enter the requested
 * power mode.
 *
 * @ref pm_set must ignore any power mode that does not have full RAM retention,
 * as loosing data whenever no (non-idle) thread is runnable is not acceptable.
 * A RIOT port however may provide CPU specific interfaces to allow an
 * application to explicitly enter deep sleep modes without RAM retention.
 *
 * @file
 * @brief       Layered low power mode infrastructure
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#include <stdint.h>
#include "periph_cpu.h"
#include "architecture.h"

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
typedef struct {
    uint8_t blockers[PM_NUM_MODES];     /**< number of blockers for the mode */
} WORD_ALIGNED pm_blocker_t;

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

/** @} */

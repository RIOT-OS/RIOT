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

#include "assert.h"
#include "periph/pm.h"
#include "periph_cpu.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @brief   Block a power mode
 *
 * @param[in]   mode      power mode to block
 */
void pm_block(unsigned mode);

/**
 * @brief   Unblock a power mode
 *
 * @param[in]   mode      power mode to unblock
 */
void pm_unblock(unsigned mode);

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

#ifdef __cplusplus
}
#endif

#endif /* __PM_LAYERED_H */
/** @} */

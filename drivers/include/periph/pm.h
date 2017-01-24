/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_periph_pm Power Management
 * @ingroup     drivers_periph
 * @brief       The kernels power management interface
 * @{
 *
 * This interface *must* be implemented for every platform in RIOT.
 *
 * @file
 * @brief       Power management interface
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef PM_H
#define PM_H

#include "assert.h"
#include "periph_cpu.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @brief   Reboot MCU
 */
void pm_reboot(void);

/**
 * @brief   Turn off MCU completely
 */
void pm_off(void);

/**
 * @brief   Switches the MCU to the lowest possible power mode
 *
 * This function will be called by the idle thread.
 */
void pm_set_lowest(void);

#ifdef __cplusplus
}
#endif

#endif /* __PM_H */
/** @} */

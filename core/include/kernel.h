/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  core_internal
 * @{
 *
 * @file
 * @brief       Kernel compile time configuration
 *
 *              A reboot() function is also provided
 *              (and used by core_panic() when needed).
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef KERNEL_H_
#define KERNEL_H_

#include <stdbool.h>
#include <stdint.h>

#include "attributes.h"
#include "config.h"
#include "tcb.h"
#include "cpu.h"
#include "sched.h"
#include "cpu_conf.h"

#ifdef __cplusplus
 extern "C" {
#endif

/* ------------------------------------------------------------------------- */

/**
 * @def LPM_PREVENT_SLEEP_UART
 * @brief This flag tells the kernel that the deepest power saving
 *        mode that currently can be used must still allow UART
 *        communication. Bitmask to use with `lpm_prevent_sleep`
 *        in power management.
 */
#define LPM_PREVENT_SLEEP_UART    BIT2

/**
 * @brief This bitfield is used to configure which modules are
 *        currently active and prevent the kernel to go to the
 *        deepest power modes. It is used with `LPM_PREVENT_SLEEP_HWTIMER`
 *        and/or `LPM_PREVENT_SLEEP_UART`.
 */
extern volatile int lpm_prevent_sleep;

/**
 * @brief Variable used to store system configuration
 *
 * @details This contains e.g. the node ID, name, default channel and so on
 */
extern config_t sysconfig;

/* ------------------------------------------------------------------------- */

/**
 * @brief Immediately reboots the system.
 *
 * This function is used by core_panic() when the DEVELHELP macro is not defined.
 *
 * @param mode  The reboot mode (unused for now)
 *
 * @return This call never returns when successful. -1 is returned otherwise.
 */
int reboot(int mode);

/**
 * @def RB_AUTOBOOT
 * @brief Reboot the system in the usual fashion
 */
#define RB_AUTOBOOT 0

#ifdef __cplusplus
}
#endif

#endif /* KERNEL_H_ */
/** @} */

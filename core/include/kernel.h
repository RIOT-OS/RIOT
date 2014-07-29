/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @addtogroup  core_internal
 * @{
 *
 * @file        kernel.h
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

#include "attributes.h"
#include "config.h"
#include "tcb.h"
#include "cpu.h"
#include "flags.h"
#include "sched.h"
#include "cpu-conf.h"

/**
 * @def KERNEL_CONF_STACKSIZE_DEFAULT
 * @brief A reasonable default stack size that will suffice most smaller tasks
 */
#ifndef KERNEL_CONF_STACKSIZE_DEFAULT
#error KERNEL_CONF_STACKSIZE_DEFAULT must be defined per CPU
#endif

/**
 * @def KERNEL_CONF_STACKSIZE_IDLE
 * @brief Size of the idle task's stack in bytes
 */
#ifndef KERNEL_CONF_STACKSIZE_IDLE
#error KERNEL_CONF_STACKSIZE_IDLE must be defined per CPU
#endif

/**
 * @def KERNEL_CONF_STACKSIZE_PRINTF
 * @ingroup conf
 * @brief Size of the task's printf stack in bytes
 */
#ifndef KERNEL_CONF_STACKSIZE_PRINTF
#error KERNEL_CONF_STACKSIZE_PRINTF must be defined per CPU
#endif

/**
 * @def KERNEL_CONF_STACKSIZE_MAIN
 * @brief Size of the main task's stack in bytes
 */
#ifndef KERNEL_CONF_STACKSIZE_MAIN
#define KERNEL_CONF_STACKSIZE_MAIN      (KERNEL_CONF_STACKSIZE_DEFAULT + KERNEL_CONF_STACKSIZE_PRINTF)
#endif

/* ------------------------------------------------------------------------- */

/**
 * @def PID_NULL
 * @brief Identifier to detect an invalid PID
 */
#define PID_NULL -1

/**
 * @def PRIORITY_MIN
 * @brief Least priority a thread can have
 */
#define PRIORITY_MIN            (SCHED_PRIO_LEVELS-1)

/**
 * @def PRIORITY_IDLE
 * @brief Priority of the idle thread
 */
#define PRIORITY_IDLE           PRIORITY_MIN

/**
 * @def PRIORITY_MAIN
 * @brief Priority of the main thread
 */
#define PRIORITY_MAIN           (PRIORITY_MIN - (SCHED_PRIO_LEVELS/2))

/**
 * @def LPM_PREVENT_SLEEP_UART
 * @brief This flag tells the kernel that the deepest power saving
 *        mode that currently can be used must still allow UART
 *        communication. Bitmask to use with `lpm_prevent_sleep`
 *        in power management.
 */
#define LPM_PREVENT_SLEEP_UART    BIT2

/**
 * @def LPM_PREVENT_SLEEP_HWTIMER
 * @brief This flag tells the kernel that the deepest power saving
 *        mode that currently can be used must still allow the hwtimer
 *        to run. Bitmask to use with `lpm_prevent_sleep` in power management.
 */
#define LPM_PREVENT_SLEEP_HWTIMER    BIT1

/**
 * @brief This bitfield is used to configure which modules are
 *        currently active and prevent the kernel to go to the
 *        deepest power modes. It is used with `LPM_PREVENT_SLEEP_HWTIMER`
 *        and/or `LPM_PREVENT_SLEEP_UART`.
 */
extern volatile int lpm_prevent_sleep;

/**
 * @brief Variable used to store system configurationi
 *
 * @detail This contains e.g. the node ID, name, default channel and so on
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

#endif /* KERNEL_H_ */
/** @} */

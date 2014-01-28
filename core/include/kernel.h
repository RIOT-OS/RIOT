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
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef KERNEL_H_
#define KERNEL_H_

#include <stdbool.h>
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


#define PID_NULL -1

#define PRIORITY_MIN            SCHED_PRIO_LEVELS-1

#define PRIORITY_IDLE           PRIORITY_MIN
#define PRIORITY_MAIN           (PRIORITY_MIN - (SCHED_PRIO_LEVELS/2))

/**
 * @brief   Check whether called from interrupt service routine
 *
 * @return  true if called from within interrupt
 * @return  false if not.
 */
int inISR(void);

#define LPM_PREVENT_SLEEP_UART    BIT2
#define LPM_PREVENT_SLEEP_HWTIMER    BIT1

extern volatile int lpm_prevent_sleep;

extern config_t sysconfig;

/** @} */
#endif /* KERNEL_H_ */

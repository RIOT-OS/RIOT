/**
 * Copyright (C) 2014 Oliver Hahm <oliver.hahm@inria.fr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef CPU_H
#define CPU_H

/**
 * @defgroup    lpc1768 NXP LPC1768
 * @brief       NXP LPC1768 specific code
 * @ingroup     cpu
 * @{
 * @author Oliver Hahm <oliver.hahm@inria.fr>
 */

#include "LPC17xx.h"
#include "core_cm3.h"
#include "core_cmFunc.h"

extern void dINT(void);
extern void eINT(void);

/**
 * @brief Save the thread's context
 */
void save_context(void);

/**
 * @brief Restores the before saved context of a thread
 */
void restore_context(void);

/**
 * @brief Let the scheduler yield
 */
void thread_yield_higher(void);

/** @} */

#endif /* CPU_H */

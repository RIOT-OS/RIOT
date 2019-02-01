/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp8266
 * @{
 *
 * @file
 * @brief       Implementation of the kernels irq interface
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#ifndef IRQ_ARCH_H
#define IRQ_ARCH_H

#include "irq.h"
#include "sched.h"
#include "thread.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Indicates the interrupt nesting depth
 *
 * The variable is increment on entry into and decremented on exit from an ISR.
 */
extern uint32_t irq_interrupt_nesting;

/**
 * @name   Macros to enter and exit an ISR
 *
 * In non-SDK interrupt handling all stuff is done in `_frxt_int_enter`
 * and `_frxt_int_exit`. These macros do therefore nothing and are kept only
 * for source code compatibility.
 *
 * @{
 */
#define irq_isr_enter()
#define irq_isr_exit()
/** @} */

/**
 * @name   Macros to enter and exit a critical region
 *
 * @note since they use a local variable they can be used only in same function
 *
 * @{
 */
#define critical_enter()   int _irq_state = irq_disable ()
#define critical_exit()    irq_restore(_irq_state)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* IRQ_ARCH_H */

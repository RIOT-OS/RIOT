/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32
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
extern volatile uint32_t irq_interrupt_nesting;

/**
  * @brief fixed allocated CPU interrupt numbers that are used by RIOT
  * @{
  */
#define CPU_INUM_GPIO       2   /* level interrupt, low priority = 1 */
#define CPU_INUM_CAN        3   /* level interrupt, low priority = 1 */
#define CPU_INUM_UART       5   /* level interrupt, low priority = 1 */
#define CPU_INUM_RTC        9   /* level interrupt, low priority = 1 */
#define CPU_INUM_I2C        12  /* level interrupt, low priority = 1 */
#define CPU_INUM_WDT        13  /* level interrupt, low priority = 1 */
#define CPU_INUM_SOFTWARE   17  /* level interrupt, low priority = 1 */
#define CPU_INUM_ETH        18  /* level interrupt, low priority = 1 */
#define CPU_INUM_TIMER      19  /* level interrupt, medium priority = 2 */
/** @} */

#if defined(SDK_INT_HANDLING) || defined(DOXYGEN)
/**
 * @brief   Macros that have to be used on entry into and reset from an ISR
 *
 * NOTE: since they use a local variable they can be used only in same function
 * @{
 */
/** Macro that has to be used at the entry point of an ISR */
#define irq_isr_enter()    int _irq_state = irq_disable (); \
                           irq_interrupt_nesting++;

/** Macro that has to be used at the exit point of an ISR */
#define irq_isr_exit()     if (irq_interrupt_nesting) \
                               irq_interrupt_nesting--; \
                           irq_restore (_irq_state); \
                           if (sched_context_switch_request) \
                               thread_yield();

#else /* SDK_INT_HANDLING */

/* in non SDK task handling all the stuff is done in _frxt_int_enter and _frxt_int_exit */
#define irq_isr_enter() /* int _irq_state = irq_disable (); \
                           irq_interrupt_nesting++; */

#define irq_isr_exit()  /* if (irq_interrupt_nesting) \
                               irq_interrupt_nesting--; \
                           irq_restore (_irq_state); */

#endif /* SDK_INT_HANDLING */

/**
 * @brief   Macros to enter and exit from critical region
 *
 * NOTE: since they use a local variable they can be used only in same function
 * @{
 */
#define critical_enter()   int _irq_state = irq_disable ()
#define critical_exit()    irq_restore(_irq_state)

/**
 * @brief   Macros to enter and exit from critical region with state variable
 */
#define critical_enter_var(m)   m = irq_disable ()
#define critical_exit_var(m)    irq_restore(m)

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* IRQ_ARCH_H */

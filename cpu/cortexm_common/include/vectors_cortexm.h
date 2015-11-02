/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cortexm_common
 * @{
 *
 * @file
 * @brief       Default interrupt vectors shared by Cortex-M based CPUs
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef VECTORS_DEFAULT_H_
#define VECTORS_DEFAULT_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Use this macro to make interrupt functions overridable with the
 *          dummy_handler as fallback in case they are not implemented
 */
#define WEAK_DEFAULT    __attribute__((weak,alias("dummy_handler")))

/**
 * @brief   Put this macro in front of the array holding the interrupt vectors
 */
#define ISR_VECTORS     __attribute__((used,section(".vectors")))

/**
 * @brief   This function is the default entry point after a system reset
 *
 * After a system reset, the following steps are necessary and carried out:
 * 1. load data section from flash to ram
 * 2. overwrite uninitialized data section (BSS) with zeros
 * 3. initialize the board (sync clock, setup std-IO)
 * 4. initialize the newlib (optional, on when newlib is used)
 * 5. initialize and start RIOTs kernel
 */
void reset_handler_default(void);

/**
 * @brief   Non-maskable interrupt handler
 *
 * Non-maskable interrupts have the highest priority other than the reset event
 * and can not be masked (surprise surprise...). They can be triggered by
 * software and some peripherals. So far, they are not used in RIOT.
 */
void nmi_default(void);

/**
 * @brief   Hard fault exception handler
 *
 * Hard faults are triggered on errors during exception processing. Typical
 * causes of hard faults are access to un-aligned pointers on Cortex-M0 CPUs
 * and calls of function pointers that are set to NULL.
 */
void hard_fault_default(void);

/* The following four exceptions are only present for Cortex-M3 and -M4 CPUs */
#if defined(CPU_ARCH_CORTEX_M3) || defined(CPU_ARCH_CORTEX_M4) || \
    defined(CPU_ARCH_CORTEX_M4F)
/**
 * @brief   Memory management exception handler
 *
 * Memory management exceptions are triggered on access to protected memory
 * regions.
 */
void mem_manage_default(void);

/**
 * @brief   Bus fault exception handler
 *
 * Bus faults are triggered on errors that are related to memory transactions.
 */
void bus_fault_default(void);

/**
 * @brief   Usage fault exception handler
 *
 * Usage fault exceptions are triggered by faults related to the execution of
 * instructions, e.g. execution of undefined opcodes, illegal alignment, or
 * or invalid exception return codes.
 */
void usage_fault_default(void);

/**
 * @brief   Debug monitor exception handler
 *
 * The debug monitor exception is triggered, when a software debug event occurs
 * while in debug mode.
 */
void debug_mon_default(void);
#endif

/**
 * @brief   Default handler used as weak alias for not implemented ISR vectors
 *
 * Per default, all interrupt handlers are mapped to the dummy handler using a
 * weak symbol. This means the handlers can be (should be) overwritten in the
 * RIOT code by just implementing a function with the name of the targeted
 * interrupt routine.
 */
void dummy_handler_default(void);

#ifdef __cplusplus
}
#endif

#endif /* VECTORS_DEFAULT_H_ */
/** @} */

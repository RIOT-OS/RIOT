/*
 * SPDX-FileCopyrightText: 2015 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     cpu_cortexm_common
 * @{
 *
 * @file
 * @brief       Default interrupt vectors shared by Cortex-M based CPUs
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "cpu_conf.h"

/**
 * @brief   Use this macro to make interrupt functions overridable with the
 *          dummy_handler as fallback in case they are not implemented
 */
#define WEAK_DEFAULT    __attribute__((weak,alias("dummy_handler")))

/**
 * @brief   Use this macro to define the parts of the vector table
 *
 * The entries in the vector table are sorted in ascending order defined by the
 * (numeric) value given for `x`. The Cortex-M base vectors are always defined
 * with `ISR_VECTOR(0)`, so the CPU specific vector(s) **must** start from 1.
 */
#define ISR_VECTOR(x)   __attribute__((used,section(".vectors." # x )))

/**
 * @brief   Number of Cortex-M non-ISR exceptions
 *
 * This means those that are no hardware interrupts, or the ones with a
 * negative interrupt number.
 */
#define CPU_NONISR_EXCEPTIONS   (15)

/**
 * @brief   All ISR functions have this type
 */
typedef void (*isr_t)(void);

/**
 * @brief   Structure of Cortex-M basic vector table
 */
typedef struct {
    void* _estack;                          /**< exception stack pointer */
    isr_t vectors[CPU_NONISR_EXCEPTIONS];   /**< shared Cortex-M vectors */
} cortexm_base_t;

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
void nmi_handler(void);

/**
 * @brief   Hard fault exception handler
 *
 * Hard faults are triggered on errors during exception processing. Typical
 * causes of hard faults are access to un-aligned pointers on Cortex-M0 CPUs
 * and calls of function pointers that are set to NULL.
 */
void hard_fault_default(void);

/* The following four exceptions are only present for Cortex-M3 and -M4 CPUs */
#if defined(CPU_CORE_CORTEX_M3) || defined(CPU_CORE_CORTEX_M4) || \
    defined(CPU_CORE_CORTEX_M4F) || defined(CPU_CORE_CORTEX_M7)

/**
 * @brief All mem manage handlers have this type.
 */
typedef int (*mem_manage_handler_t)(void);

#ifdef NDEBUG
#  define assert_free_mem_manage_handler() do {} while (0)
#else

/**
 * @brief Last file that asserted memory manage handler was free.
 */
extern const char *_free_mem_manage_handler_last_file;

/**
 * @brief Asserts that memory manage handler is free.
 *
 * @param[in] file Calling file.
 * @param[in] line Line in calling file.
 *
 * @see assert_free_mem_manage_handler
 */
void assert_free_mem_manage_handler_ex(const char *file, int line);

/**
 * @brief Asserts that memory manage handler is free.
 *
 * To be used in files using set_memory_manage_handler.
 *
 * @see set_memory_manage_handler
 * @see assert_free_mem_manage_handler_ex
 */
#define assert_free_mem_manage_handler()                   \
    assert_free_mem_manage_handler_ex(__FILE__, __LINE__); \
    _free_mem_manage_handler_last_file = __FILE__

#endif /* NDEBUG */

/**
 * @brief Memory manage handler setter.
 *
 * @param handler Handler to set
 *
 * @retval < 0 on NULL handler or if a handler has already been set
 * @retval >= 0 otherwise
 */
int set_memory_manage_handler(mem_manage_handler_t handler);

/**
 * @brief Memory manage handler cleaner.
 */
void remove_memory_manage_handler(void);

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

/** @} */

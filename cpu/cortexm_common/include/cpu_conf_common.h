/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     cpu_cortexm_common
 * @{
 *
 * @file
 * @brief       Common CPU definitione for Cortex-M family based MCUs
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    Configuration of default stack sizes
 *
 * As all members of the Cortex-M family behave identical in terms of stack
 * usage, we define the default stack size values here centrally for all CPU
 * implementations.
 *
 * If needed, you can overwrite these values the `cpu_conf.h` file of the
 * specific CPU implementation.
 *
 * @todo Adjust values for Cortex-M4F with FPU?
 * @todo Configure second set if no newlib nano.specs are available?
 * @{
 */
#ifndef THREAD_EXTRA_STACKSIZE_PRINTF
#define THREAD_EXTRA_STACKSIZE_PRINTF   (512)
#endif
#ifndef THREAD_STACKSIZE_DEFAULT
#define THREAD_STACKSIZE_DEFAULT        (1024)
#endif
#ifndef THREAD_STACKSIZE_IDLE
#define THREAD_STACKSIZE_IDLE           (256)
#endif
/** @} */

/**
 * @brief   Stack size used for the exception (ISR) stack
 * @{
 */
#ifndef ISR_STACKSIZE
#define ISR_STACKSIZE                   (512U)
#endif
/** @} */

/**
 * @brief   Select fastest bitarithm_lsb implementation
 * @{
 */
#ifdef __ARM_FEATURE_CLZ
#define BITARITHM_LSB_BUILTIN
#define BITARITHM_HAS_CLZ
#else
#define BITARITHM_LSB_LOOKUP
#endif
/** @} */

/**
 * @name    ARM Cortex-M interrupt sub-priorities and PendSV priority
 * @{
 */

/**
 * @brief Enable Cortex-M sub-priorities, with the given number of bits
 *
 * Cortex-M CPUs allow interrupt priorities to be arranged in subgroups,
 * meaning that any interrupts on the same subgroup will not
 * preempt each other, but those on a lower sub-priority will run
 * only once all on the higher sub-priorities have been completed.
 *
 * The usual practice on Cortex-M is to run the PendSV interrupt
 * as the last one, just before returning to the user context,
 * running the scheduler from PendSV.  However, in RIOT we don't want
 * the scheduler to be interrupted by any "normal" interrupts, which
 * may change the mutexes or something else that would affect the
 * scheduler.  At the same time, we don't want to explicitly
 * disable all interrupts while in the scheduler, as that would
 * increase scheduling latency.
 *
 * A currently experimental way to make PendSV on Cortex-M to run
 * last is to
 * - make sure @ref CPU_DEFAULT_IRQ_PRIO is even (e.g. `6U`),
 * - set @ref CPU_CORTEXM_PENDSV_IRQ_PRIO to one higher, and
 * - set this one
 *
 * For example, as follows:
 * @code{.c}
 * # define CPU_CORTEXM_PRIORITY_GROUPING (1U)
 * # define CPU_CORTEXM_PENDSV_IRQ_PRIO   (CPU_DEFAULT_IRQ_PRIO + 1U)
 * @endcode
 *
 * See cpu/cortexm_common/cortexm_init.c how these are used.
 *
 * If you want to set this, define it in your `cpu_conf.h`.
 */
#ifndef CPU_CORTEXM_PRIORITY_GROUPING
#define CPU_CORTEXM_PRIORITY_GROUPING   (0)
#endif

/**
 * @brief Define a separate priority for the PendSV interrupt
 *
 * According to the ARM Cortex-M documentation, the recommended best
 * practice is to place the PendSV at the lowest interrupt priority.
 * By default, RIOT runs PendSV on the same interrupt priority with all
 * other interrupts.
 *
 * For efficiency (or other reasons), one may want to run the PendSV as
 * the last one, just before returning to the (next) thread.  However,
 * since PendSV triggers the RIOT scheduler _without_ interrupts being
 * disabled, any interrupts that preempt the scheduler, including the
 * timer interrupts, must not call anything that may affect the
 * scheduler, such as mutex or scheduler functions.  With the current
 * design of RIOT, writing interrupt handlers in such a manner is not
 * exactly trivial.
 *
 * An experimental way to to run PendSV as the last thing before
 * returning to the user thread context is to enable Cortex-M
 * sub-priorities with @ref CPU_CORTEXM_PRIORITY_GROUPING and then
 * make the PendSV interrupt sub-priority lower than the default.
 * (Remember, on Cortex-M lower urgency means higher priority number.)
 *
 * For now, by default, we preserve the traditional RIOT behaviour, but
 * allow specific CPUs, boards, or apps to change this.
 *
 * See cpu/cortexm_common/cortexm_init.c how these are used.
 *
 * If you want to set this, define it in your `cpu_conf.h`.
 */
#ifndef CPU_CORTEXM_PENDSV_IRQ_PRIO
#define CPU_CORTEXM_PENDSV_IRQ_PRIO (UINT8_MAX)
#endif
/** @} */

/**
 * @brief   Attribute for memory sections required by SRAM PUF
 */
#define PUF_SRAM_ATTRIBUTES __attribute__((used, section(".puf")))

#if CPU_HAS_BACKUP_RAM || DOXYGEN
/**
 * @brief   Memory marked with this attribute is retained during deep sleep
 *          and initialized with 0 on cold boot.
 */
#define BACKUP_RAM      __attribute__((section(".backup.bss")))

/**
 * @brief   Memory marked with this attribute is retained during deep sleep
 *          and initialized with user provided data on cold boot.
 */
#define BACKUP_RAM_DATA __attribute__((section(".backup.data")))
#endif /* CPU_HAS_BACKUP_RAM */

/**
 * @brief   This arch uses the inlined irq API.
 */
#define IRQ_API_INLINED     (1)

#if defined(CPU_CORE_CORTEX_M0) || defined(CPU_CORE_CORTEX_M0PLUS) \
 || defined(CPU_CORE_CORTEX_M23)
/**
 * @brief   CPU cycles per busy wait loop
 */
#define CPU_CYCLES_PER_LOOP (4)
#endif

#ifdef __cplusplus
}
#endif

/** @} */

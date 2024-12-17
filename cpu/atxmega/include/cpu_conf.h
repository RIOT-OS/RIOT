/*
 * Copyright (C) 2021 Gerson Fernando Budke
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_atxmega
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Gerson Fernando Budke <nandojve@gmail.com>
 */

#ifndef CPU_CONF_H
#define CPU_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

#define THREAD_EXTRA_STACKSIZE_PRINTF   (128)

/**
 * @name           Kernel configuration
 *
 *                 Since printf seems to get memory allocated by the
 *                 linker/avr-libc the stack size tested successfully
 *                 even with pretty small stacks.
 * @{
 */
#ifndef THREAD_STACKSIZE_DEFAULT
#define THREAD_STACKSIZE_DEFAULT        (512)
#endif

/* keep THREAD_STACKSIZE_IDLE > THREAD_EXTRA_STACKSIZE_PRINTF
 * to avoid not printing of debug in interrupts
 */
#ifndef THREAD_STACKSIZE_IDLE
#if MODULE_XTIMER || MODULE_ZTIMER || MODULE_ZTIMER64
/* For AVR no ISR stack is used, hence an IRQ will victimize the stack of
 * whatever thread happens to be running with the IRQ kicks in. If more than
 * trivial stuff is needed to be done in ISRs (e.g. when soft timers are used),
 * the idle stack will overflow.
 */
#define THREAD_STACKSIZE_IDLE           (384)
#else
#define THREAD_STACKSIZE_IDLE           (192)
#endif
#endif
/** @} */

/**
 * @brief   Declare the heap_stats function as available
 */
#define HAVE_HEAP_STATS

/**
 * @brief   This arch uses the inlined IRQ API.
 */
#define IRQ_API_INLINED                 (1)

#ifndef DOXYGEN
#define HAS_FLASH_UTILS_ARCH    1
#endif

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
/** @} */

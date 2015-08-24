/*
 * Original work Copyright (c) 2014-2015 Freie Universität Berlin
 * Modified work Copyright 2015 Attilio Dona'
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    cpu_cc3200 ARM CC3200 Cortex-M
 * @ingroup     cpu
 * @brief       Common implementations and headers for CC3200
 *              micro-controller
 * @{
 *
 * @file
 * @brief       Basic definitions for the CC3200 MCU
 *
 * When ever you want to do something hardware related, that is accessing MCUs
 * registers, just include this file. It will then make sure that the MCU
 * specific headers are included.
 *
 * @author      Stefan Pfeiffer <stefan.pfeiffer@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Joakim Gebart <joakim.gebart@eistec.se>
 * @author      Attilio Dona' <@attiliodona>
 *
 * @todo        remove include irq.h once core was adjusted
 */

#ifndef CPU_H_
#define CPU_H_

#include "cpu_conf.h"
#include "irq.h"

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
 * If needed, you can overwrite these values the the `cpu_conf.h` file of the
 * specific CPU implementation.
 *
 *
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
 * @brief   UART0 buffer size definition for compatibility reasons
 * 
 * Needed by ng_networking example shell_init() for configuring shell->shell_buffer_size
 * @todo remove once the remodeling of the uart0 driver is done
 * @{
 */
#ifndef UART0_BUFSIZE
#define UART0_BUFSIZE                   (128)
#endif
/** @} */



/**
 * @brief   Deprecated interrupt control function for backward compatibility
 * @{
 */
#define eINT                            enableIRQ
#define dINT                            disableIRQ
/** @} */

/**
 * @brief   Some members of the Cortex-M family have architecture specific
 *          atomic operations in atomic_arch.c
 */
#if defined(CPU_ARCH_CORTEX_M3) || defined(CPU_ARCH_CORTEX_M4) || \
    defined(CPU_ARCH_CORTEX_M4F)
#define ARCH_HAS_ATOMIC_COMPARE_AND_SWAP 1
#endif

/**
 * @brief Definition of available panic modes
 */
typedef enum {
    PANIC_NMI_HANDLER,       /**< non maskable interrupt */
    PANIC_HARD_FAULT,        /**< hard fault */
#if defined(CPU_ARCH_CORTEX_M3) || defined(CPU_ARCH_CORTEX_M4) || \
    defined(CPU_ARCH_CORTEX_M4F)
    PANIC_MEM_MANAGE,        /**< memory controller interrupt */
    PANIC_BUS_FAULT,         /**< bus fault */
    PANIC_USAGE_FAULT,       /**< undefined instruction or unaligned access */
    PANIC_DEBUG_MON,         /**< debug interrupt */
#endif
    PANIC_DUMMY_HANDLER,     /**< unhandled interrupt */
} panic_t;

/**
 * @brief   Initialization of the CPU
 */
void cpu_init(void);


#ifdef __cplusplus
}
#endif

#endif /* CPU_H_ */
/** @} */

/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    cpu_cortexm_common ARM Cortex-M common
 * @ingroup     cpu
 * @brief       Common implementations and headers for Cortex-M family based
 *              micro-controllers
 * @{
 *
 * @file
 * @brief       Basic definitions for the Cortex-M common module
 *
 * When ever you want to do something hardware related, that is accessing MCUs
 * registers, just include this file. It will then make sure that the MCU
 * specific headers are included.
 *
 * @author      Stefan Pfeiffer <stefan.pfeiffer@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @todo        remove include irq.h once core was adjusted
 */

#ifndef CPU_H_
#define CPU_H_

#include <stdio.h>

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
 * @brief   Some members of the Cortex-M family have architecture specific
 *          atomic operations in atomic_arch.c
 */
#if defined(CPU_ARCH_CORTEX_M3) || defined(CPU_ARCH_CORTEX_M4) || \
    defined(CPU_ARCH_CORTEX_M4F)
#define ARCH_HAS_ATOMIC_COMPARE_AND_SWAP 1
#endif

/**
 * @brief   Initialization of the CPU
 */
void cpu_init(void);

/**
 * @brief   Initialize Cortex-M specific core parts of the CPU
 */
void cortexm_init(void);

/**
 * @brief   Prints the current content of the link register (lr)
 */
static inline void cpu_print_last_instruction(void)
{
    register uint32_t *lr_ptr;
    __asm__ __volatile__("mov %0, lr" : "=r"(lr_ptr));
    printf("%p\n", (void*) lr_ptr);
}

#ifdef __cplusplus
}
#endif

#endif /* CPU_H_ */
/** @} */

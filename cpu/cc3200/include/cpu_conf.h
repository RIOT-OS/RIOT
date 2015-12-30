/*
 * Copyright (C) 2015 Rakendra Thapa <rakendrathapa@gmail.com
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup        cpu_lm4f120 LM4F
 * @ingroup         cpu
 * @brief           CPU specific implementations for LM4F120
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Rakendra Thapa <rakendrathapa@gmail.com>
 */

#ifndef CPU_CONF_H
#define CPU_CONF_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "cc3200.h"

/**
 * @brief   ARM Cortex-M specific CPU configuration
 * @{
 */
#define CPU_DEFAULT_IRQ_PRIO            (1U)
#define CPU_IRQ_NUMOF                   (48U)
#define CPU_FLASH_BASE                  FLASH_BASE
/** @} */

/**
 * @brief Length for reading CPU_ID
 * @{
 */
#define CPUID_ID_LEN                    (12)
#define CPUID_ADDR                      NVIC_CPUID
/** @} */

// #define THREAD_EXTRA_STACKSIZE_PRINTF   (512)
// #define THREAD_STACKSIZE_DEFAULT        (1024)
// #define THREAD_STACKSIZE_IDLE           (256)
// #define ISR_STACKSIZE                   (512U) 	
// #define ARCH_HAS_ATOMIC_COMPARE_AND_SWAP 1
/**
 * @name cpu functions
 * @{
 */
extern void setup_fpu(void);
extern void cpu_clock_init(int);
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
/** @} */

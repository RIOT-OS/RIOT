/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup        cpu_stm32f0 STM32F0
 * @brief           STM32F0 specific code
 * @ingroup         cpu
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Hauke Petersen <hauke.peterse@fu-berlin.de>
 */

#ifndef __CPU_CONF_H
#define __CPU_CONF_H

#ifdef CPU_MODEL_STM32F051R8
#include "stm32f051x8.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Kernel configuration
 *
 * The absolute minimum stack size is 140 byte (68 byte for the tcb + 72 byte
 * for a complete context save).
 *
 * TODO: measure and adjust for the Cortex-M0
 * @{
 */
#define KERNEL_CONF_STACKSIZE_PRINTF    (512)

#ifndef KERNEL_CONF_STACKSIZE_DEFAULT
#define KERNEL_CONF_STACKSIZE_DEFAULT   (512)
#endif

#define KERNEL_CONF_STACKSIZE_IDLE      (192)
/** @} */

/**
 * @name UART0 buffer size definition for compatibility reasons
 *
 * TODO: remove once the remodeling of the uart0 driver is done
 * @{
 */
#ifndef UART0_BUFSIZE
#define UART0_BUFSIZE                   (128)
#endif
/** @} */

/**
 * @brief Length for reading CPU_ID
 */
#define CPUID_ID_LEN                    (12)

/**
 * @brief Definition of different panic modes
 */
typedef enum {
    HARD_FAULT,             /**< hard fault */
    NMI_HANDLER,            /**< non maskable interrupt */
    DUMMY_HANDLER           /**< dummy interrupt handler */
} panic_t;

#ifdef __cplusplus
}
#endif

#endif /* __CPU_CONF_H */
/** @} */

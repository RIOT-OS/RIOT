/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup        cpu_stm32f3 STM32F3
 * @ingroup         cpu
 * @brief           CPU specific implementations for the STM32F3
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __CPU_CONF_H
#define __CPU_CONF_H

#ifdef CPU_MODEL_STM32F303VC
#include "stm32f303xc.h"
#endif
#ifdef CPU_MODEL_STM32F334R8
#include "stm32f334x8.h"
#endif

/**
 * @name Kernel configuration
 *
 * TODO: measure and adjust for the Cortex-M4f
 * @{
 */
#define KERNEL_CONF_STACKSIZE_PRINTF    (1024)

#ifndef KERNEL_CONF_STACKSIZE_DEFAULT
#define KERNEL_CONF_STACKSIZE_DEFAULT   (1024)
#endif

#define KERNEL_CONF_STACKSIZE_IDLE      (256)
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

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* __CPU_CONF_H */
/** @} */

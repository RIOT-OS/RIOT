/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup        cpu_stm32l1 STM32L1
 * @brief           CPU specific implementations for the STM32F1
 * @ingroup         cpu
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 */

#ifndef CPUCONF_H_
#define CPUCONF_H_

#include "stm32l1xx.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   ARM Cortex-M specific CPU configuration
 * @{
 */
#define CPU_DEFAULT_IRQ_PRIO            (1U)
#define CPU_IRQ_NUMOF                   (57U)
#define CPU_FLASH_BASE                  FLASH_BASE
/** @} */

/**
 * @name Length for reading CPU_ID
 */
#define CPUID_ID_LEN                      (12)

#define TRANSCEIVER_BUFFER_SIZE (3)

#ifdef __cplusplus
}
#endif

#endif /* __CPU_CONF_H */
/** @} */
/** @} */

/*
* Copyright (C) 2015 Freie Universität Berlin
*
* This file is subject to the terms and conditions of the GNU Lesser
* General Public License v2.1. See the file LICENSE in the top level
* directory for more details.
*/

/**
* @defgroup    cpu_efm32gg Silicon Labs EFM32GG
* @ingroup     cpu
* @brief       Support for the Silicon Labs EFM32GG CPU
* @{
*
* @file
* @brief       Implementation specific CPU configuration options
*/

#ifndef CPU_CONF_H
#define CPU_CONF_H

#include "em_device.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* @brief   ARM Cortex-M3 specific CPU configuration
* @{
*/
#define CPU_DEFAULT_IRQ_PRIO            (1U)
#define CPU_IRQ_NUMOF                   (EMU_IRQn + 1)
#define CPU_FLASH_BASE                  FLASH_BASE
/** @} */

/**
* @brief   CPUID_ID_LEN length of cpuid in bytes
*/
#define CPUID_ID_LEN                    (8U)    /* 64-bit unique ID */

#ifdef __cplusplus
}
#endif

#endif /* __CPU_CONF_H */
/** @} */
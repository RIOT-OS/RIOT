/*
 * Copyright (C) 2015 Engineering-Spirit
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup        cpu_stm32f2 STM32F2
 * @ingroup         cpu
 * @brief           CPU specific implementations for the STM32F2
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Hauke Petersen <hauke.peterse@fu-berlin.de>
 * @author          Nick v. IJzendoorn <nijzendoorn@engineering-spirit.nl
 */

#ifndef __CPU_CONF_H
#define __CPU_CONF_H

#include "stm32f2xx.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   ARM Cortex-M specific CPU configuration
 * @{
 */
#define CPU_DEFAULT_IRQ_PRIO            (1U)
#define CPU_IRQ_NUMOF                   (81U)
#define CPU_FLASH_BASE                  FLASH_BASE
/** @} */

/**
 * @brief Length for reading CPU_ID
 */
#define CPUID_ID_LEN                    (12)

/**
 * @brief Configure the CPU's clock system
 *
 * @param[in] source    source clock frequency
 * @param[in] target    target clock frequency
 * @param[in] prescale  prescaler to use
 */
void cpu_clock_scale(uint32_t source, uint32_t target, uint32_t *prescale);

#ifdef __cplusplus
}
#endif

#endif /* __CPU_CONF_H */
/** @} */

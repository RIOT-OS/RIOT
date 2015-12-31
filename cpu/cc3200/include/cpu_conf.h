/*
 * Copyright (C) 2015 Dinh Nguyen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc3200
 * @{
 *
 * @file
 * @brief           cc3200 MCU configuration
 *
 * @author          Dinh Nguyen <nqdinhddt@gmail.com>
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

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
/** @} */

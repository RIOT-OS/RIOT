/*
 * Copyright (C) 2023 BISSELL Homecare, Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    cpu_gd32e23x
 * @ingroup     cpu
 * @brief       CPU specific implementations for GD32E23x
 * @{
 *
 * @file
 * @brief       Implementation specific CPU configuration options
 *
 * @author      Jason Parker <jason.parker@bissell.com>
 */

#ifndef CPU_CONF_H
#define CPU_CONF_H

#include "cpu_conf_common.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CPU_MODEL_GD32E230C
#include "gd32e23x.h"
#endif

/**
 * @brief   ARM Cortex-M specific CPU configuration
 * @{
 */
#define CPU_DEFAULT_IRQ_PRIO            (1U)
#define CPU_IRQ_NUMOF                   (139U)
#define CPU_FLASH_BASE                  FLASH_BASE
#define CPU_HAS_BITBAND                 (1)
// /** @} */

// /**
//  * @name cpu functions
//  * @{
//  */
// extern void setup_fpu(void);
// extern void cpu_clock_init(int);
// /** @} */

/**
 * @name Flashpage settings
 * @{
 */
// #define FLASHPAGE_SIZE                  (1024U)
// #define FLASHPAGE_NUMOF                 (128U)
// #define FLASHPAGE_WRITE_BLOCK_SIZE      (2U)
// #define FLASHPAGE_WRITE_BLOCK_ALIGNMENT (4U)
// #define CPU_FLASH_BASE                  0x08000000

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
/** @} */

/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup        cpu_lpc1768 NXP LPC1768
 * @ingroup         cpu
 * @brief           CPU specific implementations for the NXP LPC1768 cpu
 * @{
 *
 * @file
 * @brief           CPU specific configuration options
 *
 * @author          Hauke Petersen <hauke.peterse@fu-berlin.de>
 */

#ifndef __CPU_CONF_H
#define __CPU_CONF_H

#include "cpu_conf_common.h"

#include "LPC17xx.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   ARM Cortex-M specific CPU configuration
 * @{
 */
#define CPU_DEFAULT_IRQ_PRIO            (1U)
#define CPU_IRQ_NUMOF                   (35U)
#define CPU_FLASH_BASE                  LPC_FLASH_BASE
/** @} */

/**
 * @brief   CPU ID configuration
 * @{
 */
#define CPUID_LEN                       (16U)
/* @} */

#ifdef __cplusplus
}
#endif

#endif /* __CPU_CONF_H */
/** @} */

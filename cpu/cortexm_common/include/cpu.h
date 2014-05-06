/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    cpu_cortexm_common STM32F303VG
 * @ingroup     cpu
 * @brief       Common implementations for Cortex-M family based micro-controllers
 * @{
 *
 * @file        cortexm_common.h
 * @brief       Basic definitions for the Cortex-M common module
 *
 * @author      Stefan Pfeiffer <stefan.pfeiffer@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __CORTEXM_COMMON_H
#define __CORTEXM_COMMON_H

#include "cpu_conf.h"

/**
 * @brief Macro has to be called in the beginning of each ISR
 */
#define ISR_ENTER()         asm("push {LR}")

/**
 * @brief Macro has to be called on each exit of an ISR
 */
#define ISR_EXIT()          asm("pop {r0}"); asm("bx r0")

/**
 * @brief Initialization of the CPU
 */
void cpu_init(void);


#endif /* __CORTEXM_COMMON_H */
/** @} */

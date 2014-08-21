/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_cortexm3_common
 * @{
 *
 * @file
 * @brief       CMSIS system header definitions for the Cortex-M3
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __CMSIS_SYSTEM_H
#define __CMSIS_SYSTEM_H

#include <stdint.h>

/**
 * @brief This variable holds the current CPU core clock frequency in Hz
 */
extern uint32_t SystemCoreClock;

/**
 * @brief Initialize the system's clock system
 *
 * This function sets up the system's clock tree, concerning all options
 * regarding PLL setup, external clock source configuration and prescaler
 * setup for peripheral buses.
 */
void SystemInit(void);

/**
 * @brief Update the `SystemCoreClock` variable with the current core clock value
 */
void SystemCoreClockUpdate(void);


#endif /* __CMSIS_SYSTEM_H */

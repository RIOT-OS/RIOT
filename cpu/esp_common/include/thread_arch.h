/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     cpu_esp_common
 * @{
 *
 * @file
 * @brief       Implementation of the kernel's architecture dependent thread
 *              interface for ESP SoCS
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initializes the ISR stack
 * Initializes the ISR stack with predefined values (address value) to be able to
 * measure used ISR stack size.
 */
extern void thread_isr_stack_init(void);

#ifdef __cplusplus
}
#endif

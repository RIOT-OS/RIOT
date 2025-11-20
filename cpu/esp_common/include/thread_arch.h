/*
 * SPDX-FileCopyrightText: 2019 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
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

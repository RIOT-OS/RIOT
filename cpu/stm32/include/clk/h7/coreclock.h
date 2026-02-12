/*
 * SPDX-FileCopyrightText: 2024 Prime Controls, Inc.(R)
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     cpu_stm32
 * @{
 *
 * @file
 * @brief       Core clock configuration for STM32H7
 *
 * @author      Joshua DeWeese <jdeweese@primecontrols.com>
 */

#include "clk/h7/ahb.h"
#include "clk/h7/sysclk.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Core clock source frequency
 *
 * This macro defines the frequency of the core clock source
 * before applying any optional divider.
 */
#define CLOCK_CORE_SRC          MHZ((CLOCK_SYSCLK))

/**
 * @brief   Core clock frequency
 *
 * If CONFIG_CLOCK_CORECLOCK_DIV is defined, the core clock
 * frequency is divided accordingly.
 */
#if defined(CONFIG_CLOCK_CORECLOCK_DIV)
#  define CLOCK_CORECLOCK         (CLOCK_CORE_SRC / CONFIG_CLOCK_CORECLOCK_DIV)
#else
#  define CLOCK_CORECLOCK         CLOCK_CORE_SRC
#endif

#ifdef __cplusplus
}
#endif

/** @} */

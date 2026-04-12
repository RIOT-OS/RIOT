/*
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     cpu_stm32
 * @{
 *
 * @file
 * @brief       Default STM32U3 clock configuration (bring-up)
 *
 * This configuration is intentionally conservative for early bring-up.
 * By default it uses HSI (16 MHz) as SYSCLK and disables PLL usage.
 *
 * @author      Adarsh Nair Mullachery
 */

#include "cfg_clock_common_lx_u5_wx.h"
#include "kernel_defines.h"
#include "macros/units.h"

#ifdef __cplusplus
extern "C" {
#endif

/* STM32U3: no main PLL */
#ifndef CONFIG_USE_CLOCK_PLL
#  define CONFIG_USE_CLOCK_PLL 0
#endif

/* Default to HSI for bring-up */
#ifndef CONFIG_USE_CLOCK_HSI
#  define CONFIG_USE_CLOCK_HSI 1
#endif

#ifndef CONFIG_CLOCK_HSI
#  define CONFIG_CLOCK_HSI MHZ(16)
#endif

#ifndef CONFIG_CLOCK_HSE
#  define CONFIG_CLOCK_HSE MHZ(8)
#endif

/* Pick a safe MSIS default if you later enable MSI mode */
#ifndef CONFIG_CLOCK_MSI
#  define CONFIG_CLOCK_MSI MHZ(12)
#endif

#if IS_ACTIVE(CONFIG_USE_CLOCK_HSE)
#  define CLOCK_CORECLOCK CONFIG_CLOCK_HSE
#elif IS_ACTIVE(CONFIG_USE_CLOCK_MSI)
#  define CLOCK_CORECLOCK CONFIG_CLOCK_MSI
#else
#  define CLOCK_CORECLOCK CONFIG_CLOCK_HSI
#endif

#define CLOCK_CORECLOCK_MAX MHZ(96)

#if CLOCK_CORECLOCK > CLOCK_CORECLOCK_MAX
#  error "SYSCLK cannot exceed 96MHz on STM32U3"
#endif

#define CLOCK_AHB CLOCK_CORECLOCK

#ifndef CONFIG_CLOCK_APB1_DIV
#  define CONFIG_CLOCK_APB1_DIV (1)
#endif

#ifndef CONFIG_CLOCK_APB2_DIV
#  define CONFIG_CLOCK_APB2_DIV (1)
#endif

#define CLOCK_APB1 (CLOCK_AHB / CONFIG_CLOCK_APB1_DIV)
#define CLOCK_APB2 (CLOCK_AHB / CONFIG_CLOCK_APB2_DIV)

#ifdef __cplusplus
}
#endif

/** @} */

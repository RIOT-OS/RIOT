/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#pragma once

/**
 * @ingroup         cpu_stm32
 * @{
 *
 * @file
 * @brief           Main header for STM32F2/F4/F7 clock configuration
 *
 * @author          Alexandre Abadie <alexandre.abadie@inria.fr>
*/


#include "cfg_clock_common_fx_gx_mp1_c0.h"
#include "kernel_defines.h"

#if defined(CPU_FAM_STM32F2)
#include "f2f4f7/cfg_clock_default_120.h"
#elif defined(CPU_FAM_STM32F4)
#if defined(CPU_LINE_STM32F401xC) || defined(CPU_LINE_STM32F401xE)
#include "f2f4f7/cfg_clock_default_84.h"
#elif defined(CPU_LINE_STM32F410Cx) || defined(CPU_LINE_STM32F410Rx) || \
      defined(CPU_LINE_STM32F410Tx) || defined(CPU_LINE_STM32F411xE) || \
      defined(CPU_LINE_STM32F412Cx) || defined(CPU_LINE_STM32F412Rx) || \
      defined(CPU_LINE_STM32F412Vx) || defined(CPU_LINE_STM32F412Zx) || \
      defined(CPU_LINE_STM32F413xx) || defined(CPU_LINE_STM32F423xx)
#include "f2f4f7/cfg_clock_default_100.h"
#elif defined(CPU_LINE_STM32F405xx) || defined(CPU_LINE_STM32F407xx) || \
      defined(CPU_LINE_STM32F415xx) || defined(CPU_LINE_STM32F417xx) || \
      defined(CPU_LINE_STM32F427xx) || defined(CPU_LINE_STM32F437xx) || \
      defined(CPU_LINE_STM32F429xx) || defined(CPU_LINE_STM32F439xx) || \
      defined(CPU_LINE_STM32F446xx) || defined(CPU_LINE_STM32F469xx) || \
      defined(CPU_LINE_STM32F479xx)
#include "f2f4f7/cfg_clock_default_180.h"
#else
#error "No clock configuration available for this F4 line"
#endif
#elif defined(CPU_FAM_STM32F7)
#include "f2f4f7/cfg_clock_default_216.h"
#else
#error "No clock configuration available for this family"
#endif

/**
 * @name    Clock values
 * @{
 */
#if IS_ACTIVE(CONFIG_BOARD_HAS_HSE)
#define CLOCK_PLL_SRC                   (CONFIG_CLOCK_HSE)
#else /* CONFIG_CLOCK_HSI */
#define CLOCK_PLL_SRC                   (CONFIG_CLOCK_HSI)
#endif

#if IS_ACTIVE(CONFIG_USE_CLOCK_HSI)
#define CLOCK_CORECLOCK                 (CONFIG_CLOCK_HSI)

#elif IS_ACTIVE(CONFIG_USE_CLOCK_HSE)
#if !IS_ACTIVE(CONFIG_BOARD_HAS_HSE)
#error "The board doesn't provide an HSE oscillator"
#endif
#define CLOCK_CORECLOCK                 (CONFIG_CLOCK_HSE)

#elif IS_ACTIVE(CONFIG_USE_CLOCK_PLL)
#define CLOCK_CORECLOCK                 (((CLOCK_PLL_SRC / CONFIG_CLOCK_PLL_M) * CONFIG_CLOCK_PLL_N) / CONFIG_CLOCK_PLL_P)
#endif /* CONFIG_USE_CLOCK_PLL */

#define CLOCK_PLLQ                      (((CLOCK_PLL_SRC / CONFIG_CLOCK_PLL_M) * CONFIG_CLOCK_PLL_N) / CONFIG_CLOCK_PLL_Q)

#define CLOCK_AHB                       CLOCK_CORECLOCK
#define CLOCK_APB1                      (CLOCK_CORECLOCK / CONFIG_CLOCK_APB1_DIV)
#define CLOCK_APB2                      (CLOCK_CORECLOCK / CONFIG_CLOCK_APB2_DIV)
/** @} */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

/** @} */

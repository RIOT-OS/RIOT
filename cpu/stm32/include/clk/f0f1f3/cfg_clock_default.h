/*
 * Copyright (C) 2015 TriaGnoSys GmbH
 *               2017 Alexander Kurth, Sören Tempel, Tristan Bruns
 *               2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     cpu_stm32
 * @{
 *
 * @file
 * @brief       Default clock configuration for STM32F0/F1/F3
 *
 * @author      Víctor Ariño <victor.arino@triagnosys.com>
 * @author      Sören Tempel <tempel@uni-bremen.de>
 * @author      Tristan Bruns <tbruns@uni-bremen.de>
 * @author      Alexander Kurth <kurth1@uni-bremen.de>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 */


#include "cfg_clock_common_fx_gx_mp1_c0.h"
#include "kernel_defines.h"
#include "macros/units.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    F0/F1/F3 clock settings
 * @{
 */
#if IS_ACTIVE(CONFIG_BOARD_HAS_HSE) && (CONFIG_CLOCK_HSE < MHZ(4) || CONFIG_CLOCK_HSE > MHZ(32))
#error "HSE clock frequency must be between 4MHz and 32MHz"
#endif

/* The following parameters configure:
   - on F0: a 48MHz system clock with HSI (or default HSE) as input clock
   On stm32f031x6 and stm32f042x6 lines, there's no HSE and PREDIV is
   hard-wired to 2, so to reach 48MHz set PLL_PREDIV to 2 and PLL_MUL to 12 so
   system clock = (HSI8 / 2) * 12 = 48MHz
   - on F1/F3: a 72MHz system clock with HSE (8MHz or 16MHz) and HSI (8MHz) as input clock
   On stm32f303x6, stm32f303x8, stm32f303xB, stm32f303xC, stm32f328x8 and
   stm32f358xC lines, PREDIV is hard-wired to 2 (see RM0316 p.126 to p.128).
   To reach the maximum possible system clock (64MHz) set PLL_PREDIV to 2 and
   PLL_MUL to 16, so system clock = (HSI8 / 2) * 16 = 64MHz
*/
#ifndef CONFIG_CLOCK_PLL_PREDIV
#if (IS_ACTIVE(CONFIG_BOARD_HAS_HSE) && (CONFIG_CLOCK_HSE == MHZ(16))) || \
    defined(CPU_LINE_STM32F303x6) || defined(CPU_LINE_STM32F303x8) || \
    defined(CPU_LINE_STM32F303xB) || defined(CPU_LINE_STM32F303xC) || \
    defined(CPU_LINE_STM32F328x8) || defined(CPU_LINE_STM32F358xC) || \
    defined(CPU_LINE_STM32F031x6) || defined(CPU_LINE_STM32F042x6)
#define CONFIG_CLOCK_PLL_PREDIV         (2)
#else
#define CONFIG_CLOCK_PLL_PREDIV         (1)
#endif
#endif
#ifndef CONFIG_CLOCK_PLL_MUL
#ifdef CPU_FAM_STM32F0
#if defined(CPU_LINE_STM32F031x6) || defined(CPU_LINE_STM32F042x6)
#define CONFIG_CLOCK_PLL_MUL            (12)
#else
#define CONFIG_CLOCK_PLL_MUL            (6)
#endif
#else /* CPU_FAM_F1 || CPU_FAM_F3 */
#if defined(CPU_LINE_STM32F303x6) || defined(CPU_LINE_STM32F303x8) || \
    defined(CPU_LINE_STM32F303xB) || defined(CPU_LINE_STM32F303xC) || \
    defined(CPU_LINE_STM32F328x8) || defined(CPU_LINE_STM32F358xC)
#define CONFIG_CLOCK_PLL_MUL            (16)
#else
#define CONFIG_CLOCK_PLL_MUL            (9)
#endif
#endif /* CPU_FAM_STM32F0 */
#endif /* CONFIG_CLOCK_PLL_MUL */

#if IS_ACTIVE(CONFIG_USE_CLOCK_HSI)
#define CLOCK_CORECLOCK                 (CONFIG_CLOCK_HSI)

#elif IS_ACTIVE(CONFIG_USE_CLOCK_HSE)
#if !IS_ACTIVE(CONFIG_BOARD_HAS_HSE)
#error "The board doesn't provide an HSE oscillator"
#endif
#define CLOCK_CORECLOCK                 (CONFIG_CLOCK_HSE)

#elif IS_ACTIVE(CONFIG_USE_CLOCK_PLL)
#if IS_ACTIVE(CONFIG_BOARD_HAS_HSE)
#define CLOCK_PLL_SRC                   (CONFIG_CLOCK_HSE)
#else /* CONFIG_CLOCK_HSI */
#define CLOCK_PLL_SRC                   (CONFIG_CLOCK_HSI)
#endif
/* PLL configuration: make sure your values are legit!
 *
 * compute by: CORECLOCK = ((PLL_IN / PLL_PREDIV) * PLL_MUL)
 * with:
 * PLL_IN:          input clock is HSE if available or HSI otherwise
 * PLL_PREDIV :     pre-divider,  allowed range: [1:16]
 * PLL_MUL:         multiplier,   allowed range: [2:16]
 * CORECLOCK        -> 48MHz Max on F0, 72MHz MAX on F1/F3!
 */
#define CLOCK_CORECLOCK                 ((CLOCK_PLL_SRC / CONFIG_CLOCK_PLL_PREDIV) * CONFIG_CLOCK_PLL_MUL)
#ifdef CPU_FAM_STM32F0
#if CLOCK_CORECLOCK > MHZ(48)
#error "SYSCLK cannot exceed 48MHz"
#endif
#else
#if CLOCK_CORECLOCK > MHZ(72)
#error "SYSCLK cannot exceed 72MHz"
#endif
#endif
#endif /* CONFIG_USE_CLOCK_PLL */

#define CLOCK_AHB                       CLOCK_CORECLOCK  /* HCLK, max: 48MHz (F0), 72MHz (F1/F3)*/

#ifndef CONFIG_CLOCK_APB1_DIV
#ifdef CPU_FAM_STM32F0
#define CONFIG_CLOCK_APB1_DIV           (1)
#else
#define CONFIG_CLOCK_APB1_DIV           (2)
#endif
#endif
#define CLOCK_APB1                      (CLOCK_AHB / CONFIG_CLOCK_APB1_DIV)   /* PCLK1, max: 48MHz (F0), 36MHz (F1/F3)*/
#ifdef CPU_FAM_STM32F0
/* APB2 and APB1 are the same bus but configuration registers still follows the
 * split between APB1 and APB2. Since it's the same bus, APB2 clock is equal to APB1 clock.
 */
#define CLOCK_APB2                      (CLOCK_APB1)
#else
#ifndef CONFIG_CLOCK_APB2_DIV
#define CONFIG_CLOCK_APB2_DIV           (1)
#endif
#define CLOCK_APB2                      (CLOCK_AHB / CONFIG_CLOCK_APB2_DIV)   /* PCLK2, max: 72MHz */
#endif
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */

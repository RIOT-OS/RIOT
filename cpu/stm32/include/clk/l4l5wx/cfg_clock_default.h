/*
 * Copyright (C) 2017 Freie Universität Berlin
 *               2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32
 * @{
 *
 * @file
 * @brief       Default STM32L4 clock configuration
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef CLK_L4L5WX_CFG_CLOCK_DEFAULT_H
#define CLK_L4L5WX_CFG_CLOCK_DEFAULT_H

#include "cfg_clock_common_lx_u5_wx.h"
#include "kernel_defines.h"
#include "macros/units.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    L4/L5/WB clock system configuration
 * @{
 */
#if IS_ACTIVE(CONFIG_BOARD_HAS_HSE) && (CONFIG_CLOCK_HSE < MHZ(4) || CONFIG_CLOCK_HSE > MHZ(48))
#error "HSE clock frequency must be between 4MHz and 48MHz"
#endif

/* The following parameters configure a 80MHz system clock with PLL as input clock */
#ifndef CONFIG_CLOCK_PLL_SRC_MSI
#if IS_ACTIVE(CONFIG_CLOCK_PLL_SRC_HSE) || IS_ACTIVE(CONFIG_CLOCK_PLL_SRC_HSI) || \
    IS_ACTIVE(CONFIG_BOARD_HAS_HSE)
#define CONFIG_CLOCK_PLL_SRC_MSI        0
#else
#define CONFIG_CLOCK_PLL_SRC_MSI        1       /* Use MSI as input clock by default */
#endif
#endif /* CONFIG_CLOCK_PLL_SRC_MSI */
#ifndef CONFIG_CLOCK_PLL_SRC_HSE
#if IS_ACTIVE(CONFIG_BOARD_HAS_HSE) && \
    !IS_ACTIVE(CONFIG_CLOCK_PLL_SRC_HSI) && !IS_ACTIVE(CONFIG_CLOCK_PLL_SRC_MSI)
#define CONFIG_CLOCK_PLL_SRC_HSE        1
#else
#define CONFIG_CLOCK_PLL_SRC_HSE        0
#endif
#endif
#ifndef CONFIG_CLOCK_PLL_SRC_HSI
#define CONFIG_CLOCK_PLL_SRC_HSI        0
#endif
#if IS_ACTIVE(CONFIG_CLOCK_PLL_SRC_MSI)
#define CLOCK_PLL_SRC                   (CONFIG_CLOCK_MSI)
#elif IS_ACTIVE(CONFIG_CLOCK_PLL_SRC_HSE)
#define CLOCK_PLL_SRC                   (CONFIG_CLOCK_HSE)
#else /* CONFIG_CLOCK_PLL_SRC_ */
#define CLOCK_PLL_SRC                   (CONFIG_CLOCK_HSI)
#endif
#ifndef CONFIG_CLOCK_PLL_M
#if IS_ACTIVE(CONFIG_CLOCK_PLL_SRC_MSI)
#define CONFIG_CLOCK_PLL_M              (6)     /* MSI at 48MHz */
#elif IS_ACTIVE(CONFIG_CLOCK_PLL_SRC_HSE) && (CONFIG_CLOCK_HSE == MHZ(8))
#define CONFIG_CLOCK_PLL_M              (1)     /* HSE at 8MHz */
#elif IS_ACTIVE(CONFIG_CLOCK_PLL_SRC_HSE) && (CONFIG_CLOCK_HSE == MHZ(32))
#define CONFIG_CLOCK_PLL_M              (4)     /* HSE at 32MHz */
#else
#define CONFIG_CLOCK_PLL_M              (2)     /* HSI at 16MHz */
#endif
#endif
#ifndef CONFIG_CLOCK_PLL_N
#if IS_ACTIVE(CONFIG_CLOCK_PLL_SRC_HSE) && (CONFIG_CLOCK_HSE == MHZ(32))
/* For STM32WL, VCO output frequency ((PLL input clock frequency / PLLM ) x PLLN )
   must be between 96 and 344 MHz. PLLN can have values <=127 & >=6 */
#if IS_ACTIVE(CPU_FAM_STM32WL)
#define CONFIG_CLOCK_PLL_N              (12)
#else
#define CONFIG_CLOCK_PLL_N              (16)
#endif /* CPU_FAM_STM32WL */
#elif IS_ACTIVE(CONFIG_CLOCK_PLL_SRC_HSI) || \
      (IS_ACTIVE(CONFIG_CLOCK_PLL_SRC_HSE) && (CONFIG_CLOCK_HSE == MHZ(16)))
#define CONFIG_CLOCK_PLL_N              (32)
#else
#if defined(CPU_LINE_STM32L4A6xx) || defined(CPU_LINE_STM32L4P5xx) || \
    defined(CPU_LINE_STM32L4Q5xx) || defined(CPU_LINE_STM32L4R5xx) || \
    defined(CPU_LINE_STM32L4R7xx) || defined(CPU_LINE_STM32L4R9xx) || \
    defined(CPU_LINE_STM32L4S5xx) || defined(CPU_LINE_STM32L4S7xx) || \
    defined(CPU_LINE_STM32L4S9xx)
#define CONFIG_CLOCK_PLL_N              (30)
#elif defined(CPU_FAM_STM32L5)
#define CONFIG_CLOCK_PLL_N              (27)
#else
#define CONFIG_CLOCK_PLL_N              (20)
#endif
#endif
#endif
#ifndef CONFIG_CLOCK_PLL_Q
#define CONFIG_CLOCK_PLL_Q              (2)
#endif
#ifndef CONFIG_CLOCK_PLL_R
#define CONFIG_CLOCK_PLL_R              (2)
#endif

#if IS_ACTIVE(CONFIG_USE_CLOCK_HSI)
#define CLOCK_CORECLOCK                 (CONFIG_CLOCK_HSI)

#elif IS_ACTIVE(CONFIG_USE_CLOCK_HSE)
#define CLOCK_CORECLOCK                 (CONFIG_CLOCK_HSE)

#elif IS_ACTIVE(CONFIG_USE_CLOCK_MSI)
#define CLOCK_CORECLOCK                 (CONFIG_CLOCK_MSI)

#elif IS_ACTIVE(CONFIG_USE_CLOCK_PLL)
/* PLL configuration: make sure your values are legit!
 *
 * compute by: CORECLOCK = (((PLL_IN / M) * N) / R)
 * with:
 * PLL_IN:  input clock, HSE or MSI
 * M:       pre-divider,  allowed range: [1:8]
 * N:       multiplier,   allowed range: [8:86]
 * R:       post-divider, allowed range: [2:8]
 *
 * Also the following constraints need to be met:
 * (PLL_IN / M)     -> [4MHz:16MHz]
 * (PLL_IN / M) * N -> [64MHz:344MHz]
 * CORECLOCK        -> 64MHz, 80MHZ or 120MHz MAX!
 */
#define CLOCK_CORECLOCK                 \
        ((CLOCK_PLL_SRC / CONFIG_CLOCK_PLL_M) * CONFIG_CLOCK_PLL_N) / CONFIG_CLOCK_PLL_R

/* Set max allowed sysclk */
#if defined(CPU_FAM_STM32WL)
#define CLOCK_CORECLOCK_MAX             MHZ(48)
#elif defined(CPU_FAM_STM32WB)
#define CLOCK_CORECLOCK_MAX             MHZ(64)
#elif defined(CPU_FAM_STM32L5)
#define CLOCK_CORECLOCK_MAX             MHZ(110)
#elif defined(CPU_LINE_STM32L4A6xx) || defined(CPU_LINE_STM32L4P5xx) || \
      defined(CPU_LINE_STM32L4Q5xx) || defined(CPU_LINE_STM32L4R5xx) || \
      defined(CPU_LINE_STM32L4R7xx) || defined(CPU_LINE_STM32L4R9xx) || \
      defined(CPU_LINE_STM32L4S5xx) || defined(CPU_LINE_STM32L4S7xx) || \
      defined(CPU_LINE_STM32L4S9xx)
#define CLOCK_CORECLOCK_MAX             MHZ(120)
#else /* all the other L4 */
#define CLOCK_CORECLOCK_MAX             MHZ(80)
#endif

#if CLOCK_CORECLOCK > CLOCK_CORECLOCK_MAX
#if CLOCK_CORECLOCK_MAX == MHZ(48)
#error "SYSCLK cannot exceed 48MHz"
#elif CLOCK_CORECLOCK_MAX == MHZ(64)
#error "SYSCLK cannot exceed 64MHz"
#elif CLOCK_CORECLOCK_MAX == MHZ(80)
#error "SYSCLK cannot exceed 80MHz"
#elif CLOCK_CORECLOCK_MAX == MHZ(110)
#error "SYSCLK cannot exceed 110MHz"
#elif CLOCK_CORECLOCK_MAX == MHZ(120)
#error "SYSCLK cannot exceed 120MHz"
#else
#error "invalid SYSCLK"
#endif
#endif /* CLOCK_CORECLOCK > CLOCK_CORECLOCK_MAX */
#endif /* CONFIG_USE_CLOCK_PLL */

#define CLOCK_AHB                       CLOCK_CORECLOCK /* HCLK, max: 48/64/80/120MHz */

#ifndef CONFIG_CLOCK_APB1_DIV
#define CONFIG_CLOCK_APB1_DIV           (4)
#endif
#define CLOCK_APB1                      (CLOCK_AHB / CONFIG_CLOCK_APB1_DIV)     /* PCLK1, max: 48/64/80/120MHz */
#ifndef CONFIG_CLOCK_APB2_DIV
#define CONFIG_CLOCK_APB2_DIV           (2)
#endif
#define CLOCK_APB2                      (CLOCK_AHB / CONFIG_CLOCK_APB2_DIV)     /* PCLK1, max: 48/64/80/120MHz */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CLK_L4L5WX_CFG_CLOCK_DEFAULT_H */
/** @} */

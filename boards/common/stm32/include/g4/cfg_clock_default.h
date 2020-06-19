/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_stm32
 * @{
 *
 * @file
 * @brief       Configure STM32G4 clock
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef G4_CFG_CLOCK_DEFAULT_H
#define G4_CFG_CLOCK_DEFAULT_H

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Clock settings
 * @{
 */
#define CLOCK_USE_HSI       (0)
#define CLOCK_USE_HSE       (0)
#define CLOCK_USE_PLL       (1)

#define CLOCK_HSI           (16000000U)
#ifndef CLOCK_HSE
#define CLOCK_HSE           (24000000U)
#endif
#ifndef CLOCK_LSE
#define CLOCK_LSE           (1U)
#endif

#if CLOCK_USE_HSI
#define CLOCK_CORECLOCK     (CLOCK_HSI)

#elif CLOCK_USE_HSE
#define CLOCK_CORECLOCK     (CLOCK_HSE)

#elif CLOCK_USE_PLL
/* The following parameters configure a 80MHz system clock with HSE as input clock */
#define CLOCK_PLL_M         (6)
#define CLOCK_PLL_N         (40)
#define CLOCK_PLL_R         (2)
/* Use the following to reach 170MHz
#define CLOCK_PLL_M          (6)
#define CLOCK_PLL_N          (85)
#define CLOCK_PLL_R          (2)
*/
#if CLOCK_HSE
#define CLOCK_PLL_SRC       (CLOCK_HSE)
#else /* CLOCK_HSI */
#define CLOCK_PLL_SRC       (CLOCK_HSI)
#endif
#define CLOCK_CORECLOCK     ((CLOCK_PLL_SRC / CLOCK_PLL_M) * CLOCK_PLL_N) / CLOCK_PLL_R
#endif

#define CLOCK_AHB_DIV       RCC_CFGR_HPRE_DIV1
#define CLOCK_AHB           (CLOCK_CORECLOCK / 1)   /* max 170MHz */
#define CLOCK_APB1_DIV      RCC_CFGR_PPRE1_DIV1
#define CLOCK_APB1          (CLOCK_CORECLOCK / 1)   /* max 170MHz */
#define CLOCK_APB2_DIV      RCC_CFGR_PPRE2_DIV1
#define CLOCK_APB2          (CLOCK_CORECLOCK / 1)   /* max 170MHz */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* G4_CFG_CLOCK_DEFAULT_H */
/** @} */

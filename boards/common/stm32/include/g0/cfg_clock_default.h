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
 * @brief       Configure STM32G0 clock
 *
 * CORECLOCK cannot exceeds 64MHz core clock. LSE is 32768Hz.
 * Default configuration use PLL clock as system clock. PLL input clock is HSI
 * by default.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef G0_CFG_CLOCK_DEFAULT_H
#define G0_CFG_CLOCK_DEFAULT_H

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Clock settings
 *
 * @{
 */
/* Select the desired system clock source between PLL, HSE or HSI */
#define CLOCK_USE_HSI       (0)
#define CLOCK_USE_HSE       (0)
#define CLOCK_USE_PLL       (1)

#define CLOCK_HSI           (16000000U)
#ifndef CLOCK_HSE
#define CLOCK_HSE           (0)
#endif
#ifndef CLOCK_LSE
#define CLOCK_LSE           (1U)
#endif

#if CLOCK_USE_HSI
#define CLOCK_CORECLOCK     (CLOCK_HSI)

#elif CLOCK_USE_HSE
#define CLOCK_CORECLOCK     (CLOCK_HSE)

#elif CLOCK_USE_PLL
/* The following parameters configure a 64MHz system clock with HSI as input clock */
#define CLOCK_PLL_M         (1)
#define CLOCK_PLL_N         (20)
#define CLOCK_PLL_R         (5)
#if CLOCK_HSE
#define CLOCK_PLL_SRC       (CLOCK_HSE)
#else /* CLOCK_HSI */
#define CLOCK_PLL_SRC       (CLOCK_HSI)
#endif
#define CLOCK_CORECLOCK     ((CLOCK_PLL_SRC / CLOCK_PLL_M) * CLOCK_PLL_N) / CLOCK_PLL_R
#endif /* CLOCK_USE_PLL */

#define CLOCK_AHB_DIV       RCC_CFGR_HPRE_0
#define CLOCK_AHB           (CLOCK_CORECLOCK / 1)   /* max: 64MHz */
#define CLOCK_APB1_DIV      RCC_CFGR_PPRE_0
#define CLOCK_APB1          (CLOCK_CORECLOCK / 1)   /* max: 64MHz */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* G0_CFG_CLOCK_DEFAULT_H */
/** @} */

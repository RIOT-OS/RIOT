/*
 * Copyright (C) 2017 Freie Universität Berlin
 *               2019 Inria
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
 * @brief       Default STM32L4 clock configuration
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef L4_CFG_CLOCK_DEFAULT_H
#define L4_CFG_CLOCK_DEFAULT_H

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Clock system configuration
 * @{
 */
#ifndef CONFIG_USE_CLOCK_PLL
#if IS_ACTIVE(CONFIG_USE_CLOCK_HSE) || IS_ACTIVE(CONFIG_USE_CLOCK_HSI) || \
    IS_ACTIVE(CONFIG_USE_CLOCK_MSI)
#define CONFIG_USE_CLOCK_PLL            (0)
#else
#define CONFIG_USE_CLOCK_PLL            (1)     /* Use PLL by default */
#endif
#endif /* CONFIG_USE_CLOCK_PLL */

#ifndef CONFIG_USE_CLOCK_MSI
#define CONFIG_USE_CLOCK_MSI            (0)
#endif /* CONFIG_USE_CLOCK_MSI */

#ifndef CONFIG_USE_CLOCK_HSE
#define CONFIG_USE_CLOCK_HSE            (0)
#endif /* CONFIG_USE_CLOCK_HSE */

#ifndef CONFIG_USE_CLOCK_HSI
#define CONFIG_USE_CLOCK_HSI            (0)
#endif /* CONFIG_USE_CLOCK_HSI */

#if CONFIG_USE_CLOCK_PLL && \
    (CONFIG_USE_CLOCK_MSI || CONFIG_USE_CLOCK_HSE || CONFIG_USE_CLOCK_HSI)
#error "Cannot use PLL as clock source with other clock configurations"
#endif

#if CONFIG_USE_CLOCK_MSI && \
    (CONFIG_USE_CLOCK_PLL || CONFIG_USE_CLOCK_HSI || CONFIG_USE_CLOCK_HSE)
#error "Cannot use MSI as clock source with other clock configurations"
#endif

#if CONFIG_USE_CLOCK_HSE && \
    (CONFIG_USE_CLOCK_PLL || CONFIG_USE_CLOCK_MSI || CONFIG_USE_CLOCK_HSI)
#error "Cannot use HSE as clock source with other clock configurations"
#endif

#if CONFIG_USE_CLOCK_HSI && \
    (CONFIG_USE_CLOCK_PLL || CONFIG_USE_CLOCK_MSI || CONFIG_USE_CLOCK_HSE)
#error "Cannot use HSI as clock source with other clock configurations"
#endif

#ifndef CONFIG_BOARD_HAS_HSE
#define CONFIG_BOARD_HAS_HSE            (0)
#endif
#ifndef CLOCK_HSE
#define CLOCK_HSE                       MHZ(8)
#endif
#if CONFIG_BOARD_HAS_HSE && (CLOCK_HSE < MHZ(4) || CLOCK_HSE > MHZ(48))
#error "HSE clock frequency must be between 4MHz and 48MHz"
#endif

#ifndef CONFIG_BOARD_HAS_LSE
#define CONFIG_BOARD_HAS_LSE            (0)
#endif
#if CONFIG_BOARD_HAS_LSE
#define CLOCK_LSE                       (1)
#else
#define CLOCK_LSE                       (0)
#endif

#define CLOCK_HSI                       MHZ(16)

#ifndef CONFIG_CLOCK_MSI
#define CONFIG_CLOCK_MSI                MHZ(48)
#endif

/* The following parameters configure a 80MHz system clock with PLL as input clock */
#ifndef CONFIG_CLOCK_PLL_SRC_MSI
#if IS_ACTIVE(CONFIG_CLOCK_PLL_SRC_HSE) || IS_ACTIVE(CONFIG_CLOCK_PLL_SRC_HSI) || \
    CONFIG_BOARD_HAS_HSE
#define CONFIG_CLOCK_PLL_SRC_MSI        (0)
#else
#define CONFIG_CLOCK_PLL_SRC_MSI        (1)     /* Use MSI an input clock by default */
#endif
#endif /* CONFIG_CLOCK_PLL_SRC_MSI */
#ifndef CONFIG_CLOCK_PLL_SRC_HSE
#if CONFIG_BOARD_HAS_HSE
#define CONFIG_CLOCK_PLL_SRC_HSE        (1)
#else
#define CONFIG_CLOCK_PLL_SRC_HSE        (0)
#endif
#endif
#ifndef CONFIG_CLOCK_PLL_SRC_HSI
#define CONFIG_CLOCK_PLL_SRC_HSI        (0)
#endif
#ifndef CONFIG_CLOCK_PLL_M
#define CONFIG_CLOCK_PLL_M              (6)
#endif
#ifndef CONFIG_CLOCK_PLL_N
#define CONFIG_CLOCK_PLL_N              (20)
#endif
#ifndef CONFIG_CLOCK_PLL_R
#define CONFIG_CLOCK_PLL_R              (2)
#endif

#if CONFIG_USE_CLOCK_HSI
#define CLOCK_CORECLOCK                 (CLOCK_HSI)

#elif CONFIG_USE_CLOCK_HSE
#if CONFIG_BOARD_HAS_HSE == 0
#error "The board doesn't provide an HSE oscillator"
#endif
#define CLOCK_CORECLOCK                 (CLOCK_HSE)

#elif CONFIG_USE_CLOCK_MSI
#define CLOCK_CORECLOCK                 (CONFIG_CLOCK_MSI)

#elif CONFIG_USE_CLOCK_PLL
#if CONFIG_CLOCK_PLL_SRC_MSI
#define CLOCK_PLL_SRC                   (CONFIG_CLOCK_MSI)
#elif CONFIG_CLOCK_PLL_SRC_HSE
#define CLOCK_PLL_SRC                   (CLOCK_HSE)
#else /* CONFIG_CLOCK_PLL_SRC_ */
#define CLOCK_PLL_SRC                   (CLOCK_HSI)
#endif
#define CLOCK_CORECLOCK                 \
        ((CLOCK_PLL_SRC / CONFIG_CLOCK_PLL_M) * CONFIG_CLOCK_PLL_N) / CONFIG_CLOCK_PLL_R
#ifndef CLOCK_CORECLOCK_MAX
#define CLOCK_CORECLOCK_MAX             MHZ(80)
#endif
#if CLOCK_CORECLOCK > CLOCK_CORECLOCK_MAX
#if CLOCK_CORECLOCK_MAX == MHZ(64)
#error "SYSCLK cannot exceed 64MHz"
#elif CLOCK_CORECLOCK_MAX == MHZ(80)
#error "SYSCLK cannot exceed 80MHz"
#elif CLOCK_CORECLOCK_MAX == MHZ(120)
#error "SYSCLK cannot exceed 120MHz"
#else
#error "invalid SYSCLK"
#endif
#endif /* CLOCK_CORECLOCK > CLOCK_CORECLOCK_MAX */
#endif /* CONFIG_USE_CLOCK_PLL */

#define CLOCK_AHB                       CLOCK_CORECLOCK /* max: 80MHz */

#ifndef CONFIG_CLOCK_APB1_DIV
#define CONFIG_CLOCK_APB1_DIV           (4)
#endif
#define CLOCK_APB1                      (CLOCK_CORECLOCK / CONFIG_CLOCK_APB1_DIV)   /* max: 80MHz */
#ifndef CONFIG_CLOCK_APB2_DIV
#define CONFIG_CLOCK_APB2_DIV           (2)
#endif
#define CLOCK_APB2                      (CLOCK_CORECLOCK / CONFIG_CLOCK_APB2_DIV)   /* max: 80MHz */

#ifdef __cplusplus
}
#endif

#endif /* L4_CFG_CLOCK_DEFAULT_H */
/** @} */

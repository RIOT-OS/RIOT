/*
 * Copyright (C) 2020 Inria
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
 * @brief       Configure STM32G0 clock
 *
 * CORECLOCK cannot exceeds 64MHz core clock. LSE is 32768Hz.
 * Default configuration use PLL clock as system clock. PLL input clock is HSI
 * by default.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef CLK_G0_CFG_CLOCK_DEFAULT_H
#define CLK_G0_CFG_CLOCK_DEFAULT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Clock settings
 *
 * @{
 */
/* Select the desired system clock source between PLL, HSE or HSI */
#ifndef CONFIG_USE_CLOCK_PLL
#if IS_ACTIVE(CONFIG_USE_CLOCK_HSE) || IS_ACTIVE(CONFIG_USE_CLOCK_HSI)
#define CONFIG_USE_CLOCK_PLL            0
#else
#define CONFIG_USE_CLOCK_PLL            1       /* Use PLL by default */
#endif
#endif /* CONFIG_USE_CLOCK_PLL */

#ifndef CONFIG_USE_CLOCK_HSE
#define CONFIG_USE_CLOCK_HSE            0
#endif /* CONFIG_USE_CLOCK_HSE */

#ifndef CONFIG_USE_CLOCK_HSI
#define CONFIG_USE_CLOCK_HSI            0
#endif /* CONFIG_USE_CLOCK_HSI */

#if IS_ACTIVE(CONFIG_USE_CLOCK_PLL) && \
    (IS_ACTIVE(CONFIG_USE_CLOCK_HSE) || IS_ACTIVE(CONFIG_USE_CLOCK_HSI))
#error "Cannot use PLL as clock source with other clock configurations"
#endif

#if IS_ACTIVE(CONFIG_USE_CLOCK_HSE) && \
    (IS_ACTIVE(CONFIG_USE_CLOCK_PLL) || IS_ACTIVE(CONFIG_USE_CLOCK_HSI))
#error "Cannot use HSE as clock source with other clock configurations"
#endif

#if IS_ACTIVE(CONFIG_USE_CLOCK_HSI) && \
    (IS_ACTIVE(CONFIG_USE_CLOCK_PLL) || IS_ACTIVE(CONFIG_USE_CLOCK_HSE))
#error "Cannot use HSI as clock source with other clock configurations"
#endif

#ifndef CONFIG_BOARD_HAS_HSE
#define CONFIG_BOARD_HAS_HSE            0
#endif

#ifndef CLOCK_HSE
#define CLOCK_HSE                       MHZ(24)
#endif
#if IS_ACTIVE(CONFIG_BOARD_HAS_HSE) && (CLOCK_HSE < MHZ(4) || CLOCK_HSE > MHZ(48))
#error "HSE clock frequency must be between 4MHz and 48MHz"
#endif

#ifndef CONFIG_BOARD_HAS_LSE
#define CONFIG_BOARD_HAS_LSE            0
#endif

#define CLOCK_HSI                       MHZ(16)

#ifndef CONFIG_CLOCK_HSISYS_DIV
#define CONFIG_CLOCK_HSISYS_DIV         (1)
#endif

#if IS_ACTIVE(CONFIG_BOARD_HAS_HSE)
#define CLOCK_PLL_SRC                   (CLOCK_HSE)
#else /* CLOCK_HSI */
#define CLOCK_PLL_SRC                   (CLOCK_HSI)
#endif

/* The following parameters configure a 64MHz system clock with HSI as input clock */
#ifndef CONFIG_CLOCK_PLL_M
#define CONFIG_CLOCK_PLL_M              (1)
#endif
#ifndef CONFIG_CLOCK_PLL_N
#define CONFIG_CLOCK_PLL_N              (20)
#endif
#ifndef CONFIG_CLOCK_PLL_R
#define CONFIG_CLOCK_PLL_R              (5)
#endif

#if IS_ACTIVE(CONFIG_USE_CLOCK_HSI)
#define CLOCK_CORECLOCK                 (CLOCK_HSI / CONFIG_CLOCK_HSISYS_DIV)

#elif IS_ACTIVE(CONFIG_USE_CLOCK_HSE)
#if !IS_ACTIVE(CONFIG_BOARD_HAS_HSE)
#error "The board doesn't provide an HSE oscillator"
#endif
#define CLOCK_CORECLOCK                 (CLOCK_HSE)

#elif IS_ACTIVE(CONFIG_USE_CLOCK_PLL)
#define CLOCK_CORECLOCK                 \
        ((CLOCK_PLL_SRC / CONFIG_CLOCK_PLL_M) * CONFIG_CLOCK_PLL_N) / CONFIG_CLOCK_PLL_R
#if CLOCK_CORECLOCK > MHZ(64)
#error "SYSCLK cannot exceed 64MHz"
#endif
#endif /* CONFIG_USE_CLOCK_PLL */

#define CLOCK_AHB                       CLOCK_CORECLOCK  /* max: 64MHz */

#ifndef CONFIG_CLOCK_APB1_DIV
#define CONFIG_CLOCK_APB1_DIV           (1)
#endif
#define CLOCK_APB1                      (CLOCK_CORECLOCK / CONFIG_CLOCK_APB1_DIV)   /* max: 64MHz */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CLK_G0_CFG_CLOCK_DEFAULT_H */
/** @} */

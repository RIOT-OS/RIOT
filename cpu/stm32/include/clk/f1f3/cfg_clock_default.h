/*
 * Copyright (C) 2015 TriaGnoSys GmbH
 *               2017 Alexander Kurth, Sören Tempel, Tristan Bruns
 *               2020 Inria
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
 * @brief       Default clock configuration for STM32F1/F3
 *
 * @author      Víctor Ariño <victor.arino@triagnosys.com>
 * @author      Sören Tempel <tempel@uni-bremen.de>
 * @author      Tristan Bruns <tbruns@uni-bremen.de>
 * @author      Alexander Kurth <kurth1@uni-bremen.de>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 */

#ifndef CLK_F1F3_CFG_CLOCK_DEFAULT_H
#define CLK_F1F3_CFG_CLOCK_DEFAULT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Clock settings
 * @{
 */
/* Select the desired system clock source between PLL, HSE or HSI */
#ifndef CONFIG_USE_CLOCK_PLL
#if IS_ACTIVE(CONFIG_USE_CLOCK_HSE) || IS_ACTIVE(CONFIG_USE_CLOCK_HSI)
#define CONFIG_USE_CLOCK_PLL            0
#else
#define CONFIG_USE_CLOCK_PLL            1     /* Use PLL by default */
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
#define CLOCK_HSE                       MHZ(8)
#endif
#if IS_ACTIVE(CONFIG_BOARD_HAS_HSE) && (CLOCK_HSE < MHZ(4) || CLOCK_HSE > MHZ(32))
#error "HSE clock frequency must be between 4MHz and 32MHz"
#endif

#ifndef CONFIG_BOARD_HAS_LSE
#define CONFIG_BOARD_HAS_LSE            0
#endif

#define CLOCK_HSI                       MHZ(8)

/* The following parameters configure a 72MHz system clock with HSE (8MHz or
   16MHz) and HSI (8MHz) as input clock */
#ifndef CONFIG_CLOCK_PLL_PREDIV
#if IS_ACTIVE(CONFIG_BOARD_HAS_HSE) && (CLOCK_HSE == MHZ(16))
#define CONFIG_CLOCK_PLL_PREDIV         (2)
#else
#define CONFIG_CLOCK_PLL_PREDIV         (1)
#endif
#endif
#ifndef CONFIG_CLOCK_PLL_MUL
#define CONFIG_CLOCK_PLL_MUL            (9)
#endif

#if IS_ACTIVE(CONFIG_USE_CLOCK_HSI)
#define CLOCK_CORECLOCK                 (CLOCK_HSI)

#elif IS_ACTIVE(CONFIG_USE_CLOCK_HSE)
#if !IS_ACTIVE(CONFIG_BOARD_HAS_HSE)
#error "The board doesn't provide an HSE oscillator"
#endif
#define CLOCK_CORECLOCK                 (CLOCK_HSE)

#elif IS_ACTIVE(CONFIG_USE_CLOCK_PLL)
#if IS_ACTIVE(CONFIG_BOARD_HAS_HSE)
#define CLOCK_PLL_SRC                   (CLOCK_HSE)
#else /* CLOCK_HSI */
#define CLOCK_PLL_SRC                   (CLOCK_HSI)
#endif
/* PLL configuration: make sure your values are legit!
 *
 * compute by: CORECLOCK = ((PLL_IN / PLL_PREDIV) * PLL_MUL)
 * with:
 * PLL_IN:          input clock is HSE if available or HSI otherwise
 * PLL_PREDIV :     pre-divider,  allowed range: [1:16]
 * PLL_MUL:         multiplier,   allowed range: [2:16]
 * CORECLOCK        -> 72MHz MAX!
 */
#define CLOCK_CORECLOCK                 ((CLOCK_PLL_SRC / CONFIG_CLOCK_PLL_PREDIV) * CONFIG_CLOCK_PLL_MUL)
#if CLOCK_CORECLOCK > MHZ(72)
#error "SYSCLK cannot exceed 72MHz"
#endif
#endif /* CONFIG_USE_CLOCK_PLL */

#define CLOCK_AHB                       CLOCK_CORECLOCK  /* HCLK, max: 72MHz */

#ifndef CONFIG_CLOCK_APB1_DIV
#define CONFIG_CLOCK_APB1_DIV           (2)
#endif
#define CLOCK_APB1                      (CLOCK_AHB / CONFIG_CLOCK_APB1_DIV)   /* PCLK1, max: 36MHz */
#ifndef CONFIG_CLOCK_APB2_DIV
#define CONFIG_CLOCK_APB2_DIV           (1)
#endif
#define CLOCK_APB2                      (CLOCK_AHB / CONFIG_CLOCK_APB2_DIV)   /* PCLK2, max: 72MHz */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CLK_F1F3_CFG_CLOCK_DEFAULT_H */
/** @} */

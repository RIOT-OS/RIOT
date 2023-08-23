/*
 * Copyright (C) 2023 BISSELL Homecare, Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_gd32e23x
 * @{
 *
 * @file
 * @brief       Default clock configuration for GD32E23x
 *
 * @author      Jason Parker <jason.parker@bissell.com>
 *
 */

#ifndef CFG_CLOCK_DEFAULT_H
#define CFG_CLOCK_DEFAULT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    GD32E23x clock settings
 * @{
 */

// #ifndef CONFIG_CLOCK_PLL_PREDIV
// #define CONFIG_CLOCK_PLL_PREDIV         (1)
// #endif

// #ifndef CONFIG_CLOCK_PLL_MUL
// #define CONFIG_CLOCK_PLL_MUL            (12)
// #endif

// #if IS_ACTIVE(CONFIG_USE_CLOCK_HSI)
// #define CLOCK_CORECLOCK                 (CLOCK_HSI)

// #elif IS_ACTIVE(CONFIG_USE_CLOCK_HSE)
// #if !IS_ACTIVE(CONFIG_BOARD_HAS_HSE)
// #error "The board doesn't provide an HSE oscillator"
// #endif
// #define CLOCK_CORECLOCK                 (CLOCK_HSE)

// #elif IS_ACTIVE(CONFIG_USE_CLOCK_PLL)
// #if IS_ACTIVE(CONFIG_BOARD_HAS_HSE)
// #define CLOCK_PLL_SRC                   (CLOCK_HSE)
// #else /* CLOCK_HSI */
// #define CLOCK_PLL_SRC                   (CLOCK_HSI)
// #endif
/* PLL configuration: make sure your values are legit!
 *
 * compute by: CORECLOCK = ((PLL_IN / PLL_PREDIV) * PLL_MUL)
 * with:
 * PLL_IN:          input clock is HSE if available or HSI otherwise
 * PLL_PREDIV :     pre-divider,  allowed range: [1:16]
 * PLL_MUL:         multiplier,   allowed range: [2:16]
 * CORECLOCK        -> 48MHz Max on F0, 72MHz MAX on F1/F3!
 */
// #define CLOCK_CORECLOCK                 ((CLOCK_PLL_SRC / CONFIG_CLOCK_PLL_PREDIV) * CONFIG_CLOCK_PLL_MUL)
// #if CLOCK_CORECLOCK > MHZ(72)
// #error "SYSCLK cannot exceed 72MHz"
// #endif
// #endif /* CONFIG_USE_CLOCK_PLL */

// #define CLOCK_AHB                       CLOCK_CORECLOCK

// #ifndef CONFIG_CLOCK_APB1_DIV
// #define CONFIG_CLOCK_APB1_DIV           (1)
// #endif
// #define CLOCK_APB1                      (CLOCK_AHB / CONFIG_CLOCK_APB1_DIV)

// #ifndef CONFIG_CLOCK_APB2_DIV
// #define CONFIG_CLOCK_APB2_DIV           (1)
// #endif
// #define CLOCK_APB2                      (CLOCK_AHB / CONFIG_CLOCK_APB2_DIV)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CFG_CLOCK_DEFAULT_H */
/** @} */

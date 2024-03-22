/*
 * Copyright (C) 2018-2020 Inria
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
 * @brief       Default STM32L0/STM32L1 clock configuration
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef CLK_L0L1_CFG_CLOCK_DEFAULT_H
#define CLK_L0L1_CFG_CLOCK_DEFAULT_H

#include "cfg_clock_common_lx_u5_wx.h"
#include "kernel_defines.h"
#include "macros/units.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    L0/L1 clock system configuration
 * @{
 */
#if IS_ACTIVE(CONFIG_BOARD_HAS_HSE) && (CONFIG_CLOCK_HSE < MHZ(1) || CONFIG_CLOCK_HSE > MHZ(24))
#error "HSE clock frequency must be between 1MHz and 24MHz"
#endif

/* The following parameters configure a 32MHz system clock with HSI as input clock */
#ifndef CONFIG_CLOCK_PLL_DIV
#define CONFIG_CLOCK_PLL_DIV            (2)
#endif
#ifndef CONFIG_CLOCK_PLL_MUL
#define CONFIG_CLOCK_PLL_MUL            (4)
#endif

#if IS_ACTIVE(CONFIG_USE_CLOCK_HSI)
#define CLOCK_CORECLOCK                 (CONFIG_CLOCK_HSI)

#elif IS_ACTIVE(CONFIG_USE_CLOCK_HSE)
#if !IS_ACTIVE(CONFIG_BOARD_HAS_HSE)
#error "The board doesn't provide an HSE oscillator"
#endif
#define CLOCK_CORECLOCK                 (CONFIG_CLOCK_HSE)

#elif IS_ACTIVE(CONFIG_USE_CLOCK_MSI)
#define CLOCK_CORECLOCK                 (CONFIG_CLOCK_MSI)

#elif IS_ACTIVE(CONFIG_USE_CLOCK_PLL)
#if IS_ACTIVE(CONFIG_BOARD_HAS_HSE)
#if CONFIG_CLOCK_HSE < MHZ(2)
#error "HSE must be greater than 2MHz when used as PLL input clock"
#endif
#define CLOCK_PLL_SRC                   (CONFIG_CLOCK_HSE)
#else /* CONFIG_CLOCK_HSI */
#define CLOCK_PLL_SRC                   (CONFIG_CLOCK_HSI)
#endif /* CONFIG_BOARD_HAS_HSE */
/* PLL configuration: make sure your values are legit!
 *
 * compute by: CORECLOCK = ((PLL_IN / PLL_PREDIV) * PLL_MUL)
 * with:
 * PLL_IN:          input clock is HSE if available or HSI otherwise
 * PLL_DIV :        divider, allowed values: 2, 3, 4. Default is 2.
 * PLL_MUL:         multiplier, allowed values: 3, 4, 6, 8, 12, 16, 24, 32, 48. Default is 4.
 * CORECLOCK        -> 32MHz MAX!
 */
#define CLOCK_CORECLOCK                 ((CLOCK_PLL_SRC * CONFIG_CLOCK_PLL_MUL) / CONFIG_CLOCK_PLL_DIV)
#if CLOCK_CORECLOCK > MHZ(32)
#error "SYSCLK cannot exceed 32MHz"
#endif
#endif /* CONFIG_USE_CLOCK_PLL */

#define CLOCK_AHB                       CLOCK_CORECLOCK  /* max: 32MHz */

#ifndef CONFIG_CLOCK_APB1_DIV
#define CONFIG_CLOCK_APB1_DIV           (1)
#endif
#define CLOCK_APB1                      (CLOCK_CORECLOCK / CONFIG_CLOCK_APB1_DIV)   /* max: 32MHz */
#ifndef CONFIG_CLOCK_APB2_DIV
#define CONFIG_CLOCK_APB2_DIV           (1)
#endif
#define CLOCK_APB2                      (CLOCK_CORECLOCK / CONFIG_CLOCK_APB2_DIV)   /* max: 32MHz */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CLK_L0L1_CFG_CLOCK_DEFAULT_H */
/** @} */

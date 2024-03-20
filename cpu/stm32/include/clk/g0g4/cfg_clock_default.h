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
 * @brief       Configure STM32G0/G4 clock
 *
 * CORECLOCK cannot exceeds 64MHz core clock. LSE is 32768Hz.
 * Default configuration use PLL clock as system clock. PLL input clock is HSI
 * by default.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef CLK_G0G4_CFG_CLOCK_DEFAULT_H
#define CLK_G0G4_CFG_CLOCK_DEFAULT_H

#include "cfg_clock_common_fx_gx_mp1_c0.h"
#include "kernel_defines.h"
#include "macros/units.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    G0/G4 clock settings
 *
 * @{
 */
#if IS_ACTIVE(CONFIG_BOARD_HAS_HSE) && (CONFIG_CLOCK_HSE < MHZ(4) || CONFIG_CLOCK_HSE > MHZ(48))
#error "HSE clock frequency must be between 4MHz and 48MHz"
#endif

#ifdef CPU_FAM_STM32G0
#ifndef CONFIG_CLOCK_HSISYS_DIV
#define CONFIG_CLOCK_HSISYS_DIV         (1)
#endif
#endif

#if IS_ACTIVE(CONFIG_BOARD_HAS_HSE)
#define CLOCK_PLL_SRC                   (CONFIG_CLOCK_HSE)
#else /* CONFIG_CLOCK_HSI */
#define CLOCK_PLL_SRC                   (CONFIG_CLOCK_HSI)
#endif

/* The following parameters configure a 64MHz system clock with HSI as input clock */
#ifndef CONFIG_CLOCK_PLL_M
#ifdef CPU_FAM_STM32G0
#define CONFIG_CLOCK_PLL_M              (1)
#else
#define CONFIG_CLOCK_PLL_M              (4)
#endif
#endif
#ifndef CONFIG_CLOCK_PLL_N
#ifdef CPU_FAM_STM32G0
#define CONFIG_CLOCK_PLL_N              (20)
#else
#define CONFIG_CLOCK_PLL_N              (85)
#endif
#endif
#ifndef CONFIG_CLOCK_PLL_R
#ifdef CPU_FAM_STM32G0
#define CONFIG_CLOCK_PLL_R              (5)
#else
#define CONFIG_CLOCK_PLL_R              (2)
#endif
#endif

#if IS_ACTIVE(CONFIG_USE_CLOCK_HSI)
#ifdef CPU_FAM_STM32G0
#define CLOCK_CORECLOCK                 (CONFIG_CLOCK_HSI / CONFIG_CLOCK_HSISYS_DIV)
#else
#define CLOCK_CORECLOCK                 (CONFIG_CLOCK_HSI)
#endif

#elif IS_ACTIVE(CONFIG_USE_CLOCK_HSE)
#if !IS_ACTIVE(CONFIG_BOARD_HAS_HSE)
#error "The board doesn't provide an HSE oscillator"
#endif
#define CLOCK_CORECLOCK                 (CONFIG_CLOCK_HSE)

#elif IS_ACTIVE(CONFIG_USE_CLOCK_PLL)
#define CLOCK_CORECLOCK                 \
        ((CLOCK_PLL_SRC / CONFIG_CLOCK_PLL_M) * CONFIG_CLOCK_PLL_N) / CONFIG_CLOCK_PLL_R
#ifdef CPU_FAM_STM32G0
#if CLOCK_CORECLOCK > MHZ(64)
#error "SYSCLK cannot exceed 64MHz"
#endif
#else /* CPU_FAM_STM32G4 */
#if CLOCK_CORECLOCK > MHZ(170)
#error "SYSCLK cannot exceed 170MHz"
#endif
#endif
#endif /* CONFIG_USE_CLOCK_PLL */

#define CLOCK_AHB                       CLOCK_CORECLOCK  /* max: 64MHz (G0), 170MHZ (G4) */

#ifndef CONFIG_CLOCK_APB1_DIV
#define CONFIG_CLOCK_APB1_DIV           (1)
#endif
#define CLOCK_APB1                      (CLOCK_CORECLOCK / CONFIG_CLOCK_APB1_DIV)   /* max: 64MHz (G0), 170MHZ (G4) */
#ifdef CPU_FAM_STM32G4
#ifndef CONFIG_CLOCK_APB2_DIV
#define CONFIG_CLOCK_APB2_DIV           (1)
#endif
#define CLOCK_APB2                      (CLOCK_AHB / CONFIG_CLOCK_APB2_DIV)     /* max: 170MHz (only on G4) */
#endif
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CLK_G0G4_CFG_CLOCK_DEFAULT_H */
/** @} */

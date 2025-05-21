/*
 * Copyright (C) 2020 Savoir-faire Linux
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#pragma once

/**
 * @ingroup         cpu_stm32
 * @{
 *
 * @file
 * @brief           Main header for STM32MP1 clock configuration
 *
 * @author          Gilles DOFFE <gilles.doffe@savoirfairelinux.com>
*/

#include "cfg_clock_common_fx_gx_mp1_c0.h"
#include "kernel_defines.h"
#include "macros/units.h"

/**
 * @name    MP1 clock PLL settings (208MHz)
 * @{
 */
/* The following parameters configure a 208MHz system clock with HSE (24MHz)
 * or HSI (16MHz) as PLL input clock */
#ifndef CONFIG_CLOCK_PLL_M
#define CONFIG_CLOCK_PLL_M              (2)
#endif
#if IS_ACTIVE(CONFIG_BOARD_HAS_HSE) && (CONFIG_CLOCK_HSE == MHZ(24))
#ifndef CONFIG_CLOCK_PLL_N
#define CONFIG_CLOCK_PLL_N              (52)
#endif
#else /* HSI */
#ifndef CONFIG_CLOCK_PLL_N
#define CONFIG_CLOCK_PLL_N              (78)
#endif
#endif
#ifndef CONFIG_CLOCK_PLL_P
#define CONFIG_CLOCK_PLL_P              (3)
#endif
#ifndef CONFIG_CLOCK_PLL_Q
#define CONFIG_CLOCK_PLL_Q              (13)
#endif
#ifndef CONFIG_CLOCK_PLL_R
#define CONFIG_CLOCK_PLL_R              (3)
#endif
/** @} */

/**
 * @name    MP1 clock bus settings (MCU, APB1, APB2 and APB3)
 * @{
 */
#ifndef CONFIG_CLOCK_MCU_DIV
#define CONFIG_CLOCK_MCU_DIV            (1)         /* max 208MHz */
#endif
#ifndef CONFIG_CLOCK_APB1_DIV
#define CONFIG_CLOCK_APB1_DIV           (2)         /* max 104MHz */
#endif
#ifndef CONFIG_CLOCK_APB2_DIV
#define CONFIG_CLOCK_APB2_DIV           (2)         /* max 104MHz */
#endif
#ifndef CONFIG_CLOCK_APB3_DIV
#define CONFIG_CLOCK_APB3_DIV           (2)         /* max 104MHz */
#endif
/** @} */

#if CLOCK_CORECLOCK > MHZ(208)
#error "SYSCLK cannot exceed 208MHz"
#endif

/**
 * @name    MP1 clock values
 * @{
 */
#if IS_ACTIVE(CONFIG_BOARD_HAS_HSE)
#define CLOCK_PLL_SRC                   (CONFIG_CLOCK_HSE)
#else /* CONFIG_CLOCK_HSI */
#define CLOCK_PLL_SRC                   (CONFIG_CLOCK_HSI)
#endif

#if IS_ACTIVE(CONFIG_USE_CLOCK_HSI)
#define CLOCK_CORECLOCK                 (CONFIG_CLOCK_HSI)

#elif IS_ACTIVE(CONFIG_USE_CLOCK_HSE)
#if !IS_ACTIVE(CONFIG_BOARD_HAS_HSE)
#error "The board doesn't provide an HSE oscillator"
#endif
#define CLOCK_CORECLOCK                 (CONFIG_CLOCK_HSE)

#elif IS_ACTIVE(CONFIG_USE_CLOCK_PLL)
#define CLOCK_CORECLOCK                 (((CLOCK_PLL_SRC / CONFIG_CLOCK_PLL_M) \
            * CONFIG_CLOCK_PLL_N) / CONFIG_CLOCK_PLL_P)
#endif /* CONFIG_USE_CLOCK_PLL */

#define CLOCK_PLLQ                      (((CLOCK_PLL_SRC / CONFIG_CLOCK_PLL_M) \
            * CONFIG_CLOCK_PLL_N) / CONFIG_CLOCK_PLL_Q)

#define CLOCK_APB1                      (CLOCK_CORECLOCK \
        / CONFIG_CLOCK_APB1_DIV)
#define CLOCK_APB2                      (CLOCK_CORECLOCK \
        / CONFIG_CLOCK_APB2_DIV)
/** @} */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

/** @} */

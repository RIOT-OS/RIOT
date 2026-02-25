/*
 * SPDX-FileCopyrightText: 2021 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     cpu_stm32
 * @{
 *
 * @file
 * @brief       Default STM32U5 clock configuration
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include "cfg_clock_common_lx_u5_wx.h"
#include "kernel_defines.h"
#include "macros/units.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    U5 clock system configuration
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
#else
#define CONFIG_CLOCK_PLL_M              (2)     /* HSI/HSE at 16MHz */
#endif
#endif
#ifndef CONFIG_CLOCK_PLL_N
#define CONFIG_CLOCK_PLL_N              (40)
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
 * N:       multiplier,   allowed range: [5:512]
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
#define CLOCK_CORECLOCK_MAX             MHZ(160)

#if CLOCK_CORECLOCK > CLOCK_CORECLOCK_MAX
#error "SYSCLK cannot exceed 160MHz"
#endif /* CLOCK_CORECLOCK > CLOCK_CORECLOCK_MAX */
#endif /* CONFIG_USE_CLOCK_PLL */

#define CLOCK_AHB                       CLOCK_CORECLOCK /* HCLK, max: 160MHz */

#ifndef CONFIG_CLOCK_APB1_DIV
#define CONFIG_CLOCK_APB1_DIV           (4)
#endif
#define CLOCK_APB1                      (CLOCK_AHB / CONFIG_CLOCK_APB1_DIV)     /* PCLK1, max: 160MHz */
#ifndef CONFIG_CLOCK_APB2_DIV
#define CONFIG_CLOCK_APB2_DIV           (2)
#endif
#define CLOCK_APB2                      (CLOCK_AHB / CONFIG_CLOCK_APB2_DIV)     /* PCLK1, max: 160MHz */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */

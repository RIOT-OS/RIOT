/*
 * SPDX-FileCopyrightText: 2018 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2017 OTA keys S.A.
 * SPDX-FileCopyrightText: 2018-2020 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     cpu_stm32
 * @{
 *
 * @file
 * @brief       Default STM32F7 clock configuration for 216MHz boards
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include "kernel_defines.h"
#include "macros/units.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Clock PLL settings (216MHz)
 * @{
 */
/* The following parameters configure a 216MHz system clock with HSE (8MHz,
   16MHz or 25MHz) or HSI (16MHz) as PLL input clock */
#ifndef CONFIG_CLOCK_PLL_M
#if IS_ACTIVE(CONFIG_BOARD_HAS_HSE) && (CONFIG_CLOCK_HSE == MHZ(25))
#define CONFIG_CLOCK_PLL_M              (25)
#else
#define CONFIG_CLOCK_PLL_M              (4)
#endif
#endif
#ifndef CONFIG_CLOCK_PLL_N
#if IS_ACTIVE(CONFIG_BOARD_HAS_HSE) && (CONFIG_CLOCK_HSE == MHZ(25))
#define CONFIG_CLOCK_PLL_N              (432)
#elif IS_ACTIVE(CONFIG_BOARD_HAS_HSE) && (CONFIG_CLOCK_HSE == MHZ(8))
#define CONFIG_CLOCK_PLL_N              (216)
#else
#define CONFIG_CLOCK_PLL_N              (108)
#endif
#endif
#ifndef CONFIG_CLOCK_PLL_P
#define CONFIG_CLOCK_PLL_P              (2)
#endif
#ifndef CONFIG_CLOCK_PLL_Q
#define CONFIG_CLOCK_PLL_Q              (9)
#endif
#ifndef CONFIG_CLOCK_PLL_R
#define CONFIG_CLOCK_PLL_R              (8)
#endif
/** @} */

/**
 * @name    Clock bus settings (APB1 and APB2)
 * @{
 */
#ifndef CONFIG_CLOCK_APB1_DIV
#define CONFIG_CLOCK_APB1_DIV           (4)         /* max 54MHz */
#endif
#ifndef CONFIG_CLOCK_APB2_DIV
#define CONFIG_CLOCK_APB2_DIV           (2)         /* max 108MHz */
#endif
/** @} */

#if CLOCK_CORECLOCK > MHZ(216)
#error "SYSCLK cannot exceed 216MHz"
#endif

#ifdef __cplusplus
}
#endif

/** @} */

/*
 * Copyright (C) 2018 Freie Universität Berlin
 *               2017 OTA keys S.A.
 *               2018-2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     cpu_stm32
 * @{
 *
 * @file
 * @brief       Default STM32F2/4/7 clock configuration for 120MHz boards
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
 * @name    Clock PLL settings (120MHz)
 * @{
 */
/* The following parameters configure a 120MHz system clock with HSE (8MHz or
   16MHz) or HSI (16MHz) as PLL input clock */
#ifndef CONFIG_CLOCK_PLL_M
#define CONFIG_CLOCK_PLL_M              (4)
#endif
#ifndef CONFIG_CLOCK_PLL_N
#if IS_ACTIVE(CONFIG_BOARD_HAS_HSE) && (CONFIG_CLOCK_HSE == MHZ(8))
#define CONFIG_CLOCK_PLL_N              (120)
#else
#define CONFIG_CLOCK_PLL_N              (60)
#endif
#endif
#ifndef CONFIG_CLOCK_PLL_P
#define CONFIG_CLOCK_PLL_P              (2)
#endif
#ifndef CONFIG_CLOCK_PLL_Q
#define CONFIG_CLOCK_PLL_Q              (5)
#endif
/** @} */

/**
 * @name    Clock bus settings (APB1 and APB2)
 * @{
 */
#ifndef CONFIG_CLOCK_APB1_DIV
#define CONFIG_CLOCK_APB1_DIV           (4)        /* max 30MHz */
#endif
#ifndef CONFIG_CLOCK_APB2_DIV
#define CONFIG_CLOCK_APB2_DIV           (2)        /* max 60MHz */
#endif
/** @} */

#if CLOCK_CORECLOCK > MHZ(120)
#error "SYSCLK cannot exceed 120MHz"
#endif

#ifdef __cplusplus
}
#endif

/** @} */

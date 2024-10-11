/*
 * Copyright (C) 2018 Freie Universität Berlin
 *               2017 OTA keys S.A.
 *               2018-2020 Inria
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
 * @brief       Default STM32F4 clock configuration for 84MHz boards
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef CLK_F2F4F7_CFG_CLOCK_DEFAULT_84_H
#define CLK_F2F4F7_CFG_CLOCK_DEFAULT_84_H

#include "kernel_defines.h"
#include "macros/units.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Clock PLL settings (84MHz)
 * @{
 */
/* The following parameters configure a 84MHz system clock with HSE (8MHz, 16MHz
   or 25MHz) or HSI (16MHz) as PLL input clock */
#ifndef CONFIG_CLOCK_PLL_M
#if IS_ACTIVE(CONFIG_BOARD_HAS_HSE) && (CONFIG_CLOCK_HSE == MHZ(25))
#define CONFIG_CLOCK_PLL_M              (25)
#else
#define CONFIG_CLOCK_PLL_M              (4)
#endif
#endif
#ifndef CONFIG_CLOCK_PLL_N
#if IS_ACTIVE(CONFIG_BOARD_HAS_HSE) && (CONFIG_CLOCK_HSE == MHZ(8))
#define CONFIG_CLOCK_PLL_N              (168)
#elif IS_ACTIVE(CONFIG_BOARD_HAS_HSE) && (CONFIG_CLOCK_HSE == MHZ(25))
#define CONFIG_CLOCK_PLL_N              (336)
#else
#define CONFIG_CLOCK_PLL_N              (84)
#endif
#endif
#ifndef CONFIG_CLOCK_PLL_P
#define CONFIG_CLOCK_PLL_P              (4)
#endif
#ifndef CONFIG_CLOCK_PLL_Q
#define CONFIG_CLOCK_PLL_Q              (7)
#endif
#ifndef CONFIG_CLOCK_PLL_R
#define CONFIG_CLOCK_PLL_R              (0)
#endif
/** @} */

/**
 * @name    Clock bus settings (APB1 and APB2)
 * @{
 */
#ifndef CONFIG_CLOCK_APB1_DIV
#define CONFIG_CLOCK_APB1_DIV           (2)         /* max 42MHz */
#endif
#ifndef CONFIG_CLOCK_APB2_DIV
#define CONFIG_CLOCK_APB2_DIV           (1)         /* max 84MHz */
#endif
/** @} */

#if CLOCK_CORECLOCK > MHZ(84)
#error "SYSCLK cannot exceed 84MHz"
#endif

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* CLK_F2F4F7_CFG_CLOCK_DEFAULT_84_H */

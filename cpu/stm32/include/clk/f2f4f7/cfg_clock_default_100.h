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
 * @brief       Default STM32F4 clock configuration for 100MHz boards
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef CLK_F2F4F7_CFG_CLOCK_DEFAULT_100_H
#define CLK_F2F4F7_CFG_CLOCK_DEFAULT_100_H

#include "f2f4f7/cfg_clock_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Clock PLL settings (100MHz)
 * @{
 */
/* The following parameters configure a 100MHz system clock with HSE (8MHz, 16MHz or
   25MHz) or HSI (16MHz) as PLL input clock.
   If USB is used and no alternative 48MHz is available, the clock frequency is
   decreased to 96MHZ so the PLLQ can output 48MHz.
   */
#ifndef CONFIG_CLOCK_PLL_M
#if IS_ACTIVE(CONFIG_BOARD_HAS_HSE) && (CLOCK_HSE == MHZ(25))
#define CONFIG_CLOCK_PLL_M              (25)
#else
#define CONFIG_CLOCK_PLL_M              (4)
#endif
#endif
#ifndef CONFIG_CLOCK_PLL_N
#if IS_USED(MODULE_PERIPH_USBDEV) && defined(CPU_LINE_STM32F411xE)
#if IS_ACTIVE(CONFIG_BOARD_HAS_HSE) && (CLOCK_HSE == MHZ(8))
#define CONFIG_CLOCK_PLL_N              (96)
#elif IS_ACTIVE(CONFIG_BOARD_HAS_HSE) && (CLOCK_HSE == MHZ(25))
#define CONFIG_CLOCK_PLL_N              (192)
#else
#define CONFIG_CLOCK_PLL_N              (48)
#endif
#else
#if IS_ACTIVE(CONFIG_BOARD_HAS_HSE) && (CLOCK_HSE == MHZ(8))
#define CONFIG_CLOCK_PLL_N              (100)
#elif IS_ACTIVE(CONFIG_BOARD_HAS_HSE) && (CLOCK_HSE == MHZ(25))
#define CONFIG_CLOCK_PLL_N              (200)
#else
#define CONFIG_CLOCK_PLL_N              (50)
#endif
#endif /* MODULE_PERIPH_USBDEV */
#endif
#ifndef CONFIG_CLOCK_PLL_P
#define CONFIG_CLOCK_PLL_P              (2)
#endif
#ifndef CONFIG_CLOCK_PLL_Q
#define CONFIG_CLOCK_PLL_Q              (4)
#endif
#ifndef CONFIG_CLOCK_PLL_R
#define CONFIG_CLOCK_PLL_R              (4)
#endif
/** @} */

/**
 * @name    Clock bus settings (APB1 and APB2)
 */
#ifndef CONFIG_CLOCK_APB1_DIV
#define CONFIG_CLOCK_APB1_DIV           (2)         /* max 50MHz */
#endif
#ifndef CONFIG_CLOCK_APB2_DIV
#define CONFIG_CLOCK_APB2_DIV           (1)         /* max 100MHz */
#endif
/** @} */

#ifdef __cplusplus
}
#endif

#include "f2f4f7/cfg_clock_values.h"

#if CLOCK_CORECLOCK > MHZ(100)
#error "SYSCLK cannot exceed 100MHz"
#endif

#endif /* CLK_F2F4F7_CFG_CLOCK_DEFAULT_100_H */
/** @} */

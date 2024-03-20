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
 * @brief       Default STM32F4 clock configuration for 180MHz boards
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef CLK_F2F4F7_CFG_CLOCK_DEFAULT_180_H
#define CLK_F2F4F7_CFG_CLOCK_DEFAULT_180_H

#include "kernel_defines.h"
#include "macros/units.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Clock PLL settings (180MHz)
 * @{
 */
/* The following parameters configure a 180MHz system clock with HSE (8MHz,
   12MHz or 16MHz) or HSI (16MHz) as PLL input clock.
   If USB is used and no alternative 48MHz is available, the clock frequency is
   decreased to 168MHZ so the PLLQ can output 48MHz.
   */
#ifndef CONFIG_CLOCK_PLL_M
#if IS_ACTIVE(CONFIG_BOARD_HAS_HSE) && (CONFIG_CLOCK_HSE == MHZ(12))
#define CONFIG_CLOCK_PLL_M              (12)
#elif IS_ACTIVE(CONFIG_BOARD_HAS_HSE) && (CONFIG_CLOCK_HSE == MHZ(16))
#define CONFIG_CLOCK_PLL_M              (16)
#else
#define CONFIG_CLOCK_PLL_M              (4)
#endif
#endif
#ifndef CONFIG_CLOCK_PLL_N
#if (IS_USED(MODULE_PERIPH_USBDEV_CLK) || IS_USED(MODULE_PERIPH_SDMMC_CLK)) && \
    (defined(CPU_LINE_STM32F405xx) || defined(CPU_LINE_STM32F407xx) || \
     defined(CPU_LINE_STM32F415xx) || defined(CPU_LINE_STM32F417xx) || \
     defined(CPU_LINE_STM32F427xx) || defined(CPU_LINE_STM32F429xx) || \
     defined(CPU_LINE_STM32F437xx) || defined(CPU_LINE_STM32F439xx))
#if IS_ACTIVE(CONFIG_BOARD_HAS_HSE) && (CONFIG_CLOCK_HSE == MHZ(8))
#define CONFIG_CLOCK_PLL_N              (168)
#elif IS_ACTIVE(CONFIG_BOARD_HAS_HSE) && (CONFIG_CLOCK_HSE == MHZ(12) || \
    (CONFIG_CLOCK_HSE == MHZ(16)))
#define CONFIG_CLOCK_PLL_N              (336)
#else
#define CONFIG_CLOCK_PLL_N              (84)
#endif
#else
#if IS_ACTIVE(CONFIG_BOARD_HAS_HSE) && (CONFIG_CLOCK_HSE == MHZ(8))
#define CONFIG_CLOCK_PLL_N              (180)
#elif IS_ACTIVE(CONFIG_BOARD_HAS_HSE) && (CONFIG_CLOCK_HSE == MHZ(12) || \
    (CONFIG_CLOCK_HSE == MHZ(16)))
#define CONFIG_CLOCK_PLL_N              (360)
#else
#define CONFIG_CLOCK_PLL_N              (90)
#endif
#endif /* MODULE_PERIPH_USBDEV_CLK || MODULE_PERIPH_SDMMC_CLK */
#endif
#ifndef CONFIG_CLOCK_PLL_P
#define CONFIG_CLOCK_PLL_P              (2)
#endif
#ifndef CONFIG_CLOCK_PLL_Q
#if (IS_USED(MODULE_PERIPH_USBDEV_CLK) || IS_USED(MODULE_PERIPH_SDMMC_CLK)) && \
    (defined(CPU_LINE_STM32F405xx) || defined(CPU_LINE_STM32F407xx) || \
     defined(CPU_LINE_STM32F415xx) || defined(CPU_LINE_STM32F417xx) || \
     defined(CPU_LINE_STM32F427xx) || defined(CPU_LINE_STM32F429xx) || \
     defined(CPU_LINE_STM32F437xx) || defined(CPU_LINE_STM32F439xx))
#define CONFIG_CLOCK_PLL_Q              (7)
#else
#define CONFIG_CLOCK_PLL_Q              (8)
#endif
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
#define CONFIG_CLOCK_APB1_DIV           (4)         /* max 45MHz */
#endif
#ifndef CONFIG_CLOCK_APB2_DIV
#define CONFIG_CLOCK_APB2_DIV           (2)         /* max 90MHz */
#endif
/** @} */

#if CLOCK_CORECLOCK > MHZ(180)
#error "SYSCLK cannot exceed 180MHz"
#endif

#ifdef __cplusplus
}
#endif

#endif /* CLK_F2F4F7_CFG_CLOCK_DEFAULT_180_H */
/** @} */

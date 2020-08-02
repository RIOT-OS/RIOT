/*
 * Copyright (C) 2020 Savoir-faire Linux
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_common_stm32
 * @{
 *
 * @file
 * @brief       Default STM32MP1 clock configuration for 208MHz boards
 *
 * @author      Gilles DOFFE <gilles.doffe@savoirfairelinux.com>
 */

#ifndef CLK_MP1_CFG_CLOCK_DEFAULT_208_H
#define CLK_MP1_CFG_CLOCK_DEFAULT_208_H

#include "mp1/cfg_clock_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Clock PLL settings (208MHz)
 * @{
 */
/* The following parameters configure a 208MHz system clock with HSE (24MHz)
 * or HSI (16MHz) as PLL input clock */
#ifndef CONFIG_CLOCK_PLL_M
#define CONFIG_CLOCK_PLL_M              (2)
#endif
#if IS_ACTIVE(CONFIG_BOARD_HAS_HSE) && (CLOCK_HSE == MHZ(24))
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
 * @name    Clock bus settings (MCU, APB1, APB2 and APB3)
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

#ifdef __cplusplus
}
#endif

#include "mp1/cfg_clock_values.h"

#if CLOCK_CORECLOCK > MHZ(208)
#error "SYSCLK cannot exceed 208MHz"
#endif

#endif /* CLK_MP1_CFG_CLOCK_DEFAULT_208_H */
/** @} */

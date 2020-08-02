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
 * @brief       Base STM32MP1 clock configuration
 *
 * @author      Gilles DOFFE <gilles.doffe@savoirfairelinux.com>
 */

#ifndef CLK_MP1_CFG_CLOCK_COMMON_H
#define CLK_MP1_CFG_CLOCK_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Clock common configuration
 * @{
 */
/* Select the desired system clock source between PLL, HSE or HSI */
#ifndef CONFIG_USE_CLOCK_PLL
#if IS_ACTIVE(CONFIG_USE_CLOCK_HSE) || IS_ACTIVE(CONFIG_USE_CLOCK_HSI)
#define CONFIG_USE_CLOCK_PLL            0
#else
#define CONFIG_USE_CLOCK_PLL            1     /* Use PLL by default */
#endif
#endif /* CONFIG_USE_CLOCK_PLL */

#ifndef CONFIG_USE_CLOCK_HSE
#define CONFIG_USE_CLOCK_HSE            0
#endif /* CONFIG_USE_CLOCK_HSE */

#ifndef CONFIG_USE_CLOCK_HSI
#define CONFIG_USE_CLOCK_HSI            0
#endif /* CONFIG_USE_CLOCK_HSI */

#if IS_ACTIVE(CONFIG_USE_CLOCK_PLL) && \
    (IS_ACTIVE(CONFIG_USE_CLOCK_HSE) || IS_ACTIVE(CONFIG_USE_CLOCK_HSI))
#error "Cannot use PLL as clock source with other clock configurations"
#endif

#if IS_ACTIVE(CONFIG_USE_CLOCK_HSE) && \
    (IS_ACTIVE(CONFIG_USE_CLOCK_PLL) || IS_ACTIVE(CONFIG_USE_CLOCK_HSI))
#error "Cannot use HSE as clock source with other clock configurations"
#endif

#if IS_ACTIVE(CONFIG_USE_CLOCK_HSI) && \
    (IS_ACTIVE(CONFIG_USE_CLOCK_PLL) || IS_ACTIVE(CONFIG_USE_CLOCK_HSE))
#error "Cannot use HSI as clock source with other clock configurations"
#endif

#ifndef CONFIG_BOARD_HAS_HSE
#define CONFIG_BOARD_HAS_HSE            0
#endif

#ifndef CLOCK_HSE
#define CLOCK_HSE                       MHZ(24)
#endif

#define CLOCK_HSI                       MHZ(64)

#ifdef __cplusplus
}
#endif

#endif /* CLK_MP1_CFG_CLOCK_COMMON_H */
/** @} */

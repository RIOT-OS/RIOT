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
 * @brief       STM32MP1 clock values definitions
 *
 * @author      Gilles DOFFE <gilles.doffe@savoirfairelinux.com>
 */

#ifndef CLK_MP1_CFG_CLOCK_VALUES_H
#define CLK_MP1_CFG_CLOCK_VALUES_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Clock values
 * @{
 */
#if IS_ACTIVE(CONFIG_BOARD_HAS_HSE)
#define CLOCK_PLL_SRC                   (CLOCK_HSE)
#else /* CLOCK_HSI */
#define CLOCK_PLL_SRC                   (CLOCK_HSI)
#endif

#if IS_ACTIVE(CONFIG_USE_CLOCK_HSI)
#define CLOCK_CORECLOCK                 (CLOCK_HSI)

#elif IS_ACTIVE(CONFIG_USE_CLOCK_HSE)
#if !IS_ACTIVE(CONFIG_BOARD_HAS_HSE)
#error "The board doesn't provide an HSE oscillator"
#endif
#define CLOCK_CORECLOCK                 (CLOCK_HSE)

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
}
#endif

#endif /* CLK_MP1_CFG_CLOCK_VALUES_H */
/** @} */

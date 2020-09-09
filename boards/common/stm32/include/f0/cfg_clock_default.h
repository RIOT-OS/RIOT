/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_stm32
 * @{
 *
 * @file
 * @brief       Default clock configuration for STM32F0
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      José Ignacio Alamos <jialamos@uc.cl>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef F0_CFG_CLOCK_DEFAULT_H
#define F0_CFG_CLOCK_DEFAULT_H

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Clock settings
 * @{
 */
/* give the target core clock (HCLK) frequency [in Hz],
 * maximum: 48MHz */
#define CLOCK_CORECLOCK         MHZ(48)
/* 0: no external high speed crystal available
 * else: actual crystal frequency [in Hz] */
#ifndef CLOCK_HSE
#define CLOCK_HSE               MHZ(8)
#endif
/* 0: no external low speed crystal available,
 * 1: external crystal available (always 32.768kHz) */
#ifndef CLOCK_LSE
#define CLOCK_LSE               (0)
#endif
/* peripheral clock setup */
#define CLOCK_AHB_DIV           RCC_CFGR_HPRE_DIV1
#define CLOCK_AHB               (CLOCK_CORECLOCK / 1)
#define CLOCK_APB1_DIV          RCC_CFGR_PPRE_DIV1      /* max 48MHz */
#define CLOCK_APB1              (CLOCK_CORECLOCK / 1)
#define CLOCK_APB2              (CLOCK_APB1)

/* PLL factors */
#ifndef CLOCK_PLL_PREDIV
#define CLOCK_PLL_PREDIV        (1)
#endif
#ifndef CLOCK_PLL_MUL
#define CLOCK_PLL_MUL           (6)
#endif
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* F0_CFG_CLOCK_DEFAULT_H */
/** @} */

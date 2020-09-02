/*
 * Copyright (C) 2015 TriaGnoSys GmbH
 *               2017 Alexander Kurth, Sören Tempel, Tristan Bruns
 *               2020 Inria
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
 * @brief       Default clock configuration for STM32F1/F3
 *
 * @author      Víctor Ariño <victor.arino@triagnosys.com>
 * @author      Sören Tempel <tempel@uni-bremen.de>
 * @author      Tristan Bruns <tbruns@uni-bremen.de>
 * @author      Alexander Kurth <kurth1@uni-bremen.de>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 */

#ifndef F1F3_CFG_CLOCK_DEFAULT_H
#define F1F3_CFG_CLOCK_DEFAULT_H

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Clock settings
 * @{
 */
/* give the target core clock (HCLK) frequency [in Hz],
 * maximum: 72MHz */
#define CLOCK_CORECLOCK     MHZ(72)
/* 0: no external high speed crystal available
 * else: actual crystal frequency [in Hz] */
#ifndef CLOCK_HSE
#define CLOCK_HSE           MHZ(8)
#endif
/* 0: no external low speed crystal available,
 * 1: external crystal available (always 32.768kHz) */
#ifndef CLOCK_LSE
#define CLOCK_LSE           (0)
#endif
/* peripheral clock setup */
#define CLOCK_AHB_DIV       RCC_CFGR_HPRE_DIV1
#define CLOCK_AHB           (CLOCK_CORECLOCK / 1)
#define CLOCK_APB1_DIV      RCC_CFGR_PPRE1_DIV2     /* max 36MHz */
#define CLOCK_APB1          (CLOCK_CORECLOCK / 2)
#define CLOCK_APB2_DIV      RCC_CFGR_PPRE2_DIV1     /* max 72MHz */
#define CLOCK_APB2          (CLOCK_CORECLOCK / 1)

/* PLL factors */
#ifndef CLOCK_PLL_PREDIV
#define CLOCK_PLL_PREDIV     (1)
#endif
#ifndef CLOCK_PLL_MUL
#define CLOCK_PLL_MUL        (9)
#endif
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* F1F3_CFG_CLOCK_DEFAULT_H */
/** @} */

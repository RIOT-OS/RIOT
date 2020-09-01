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
 * @brief       Default clock configuration for the STM32L1
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef L1_CFG_CLOCK_DEFAULT_H
#define L1_CFG_CLOCK_DEFAULT_H

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Clock system configuration
 * @{
 */
#define CLOCK_HSI           (16000000U)         /* internal oscillator */
#define CLOCK_CORECLOCK     (32000000U)         /* desired core clock frequency */

#ifndef CLOCK_LSE
#define CLOCK_LSE           (0)                 /* disable low speed external oscillator */
#endif

/* configuration of PLL prescaler and multiply values */
/* CORECLOCK := HSI / CLOCK_PLL_DIV * CLOCK_PLL_MUL */
#define CLOCK_PLL_DIV       RCC_CFGR_PLLDIV2
#define CLOCK_PLL_MUL       RCC_CFGR_PLLMUL4
/* configuration of peripheral bus clock prescalers */
#define CLOCK_AHB_DIV       RCC_CFGR_HPRE_DIV1      /* AHB clock -> 32MHz */
#define CLOCK_APB2_DIV      RCC_CFGR_PPRE2_DIV1     /* APB2 clock -> 32MHz */
#define CLOCK_APB1_DIV      RCC_CFGR_PPRE1_DIV1     /* APB1 clock -> 32MHz */
/* configuration of flash access cycles */
#define CLOCK_FLASH_LATENCY FLASH_ACR_LATENCY

/* bus clocks for simplified peripheral initialization, UPDATE MANUALLY! */
#define CLOCK_AHB           (CLOCK_CORECLOCK / 1)
#define CLOCK_APB2          (CLOCK_CORECLOCK / 1)
#define CLOCK_APB1          (CLOCK_CORECLOCK / 1)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* L1_CFG_CLOCK_DEFAULT_H */
/** @} */

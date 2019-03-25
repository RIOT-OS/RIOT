/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
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
 * @brief       Configure STM32F7 clock to 216MHz and 8MHz HSE using PLL with
 *              LSE
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @note        This is auto-generated from
 *              `cpu/stm32_common/dist/clk_conf/clk_conf.c`
 * @{
 */

#ifndef F7_CFG_CLOCK_216_8_1_H
#define F7_CFG_CLOCK_216_8_1_H

#ifdef __cplusplus
extern "C" {
#endif

/* give the target core clock (HCLK) frequency [in Hz],
 * maximum: 216MHz */
#define CLOCK_CORECLOCK     (216000000U)
/* 0: no external high speed crystal available
 * else: actual crystal frequency [in Hz] */
#define CLOCK_HSE           (8000000U)
/* 0: no external low speed crystal available,
 * 1: external crystal available (always 32.768kHz) */
#define CLOCK_LSE           (1U)
/* peripheral clock setup */
#define CLOCK_AHB_DIV       RCC_CFGR_HPRE_DIV1
#define CLOCK_AHB           (CLOCK_CORECLOCK / 1)
#define CLOCK_APB1_DIV      RCC_CFGR_PPRE1_DIV4     /* max 54MHz */
#define CLOCK_APB1          (CLOCK_CORECLOCK / 4)
#define CLOCK_APB2_DIV      RCC_CFGR_PPRE2_DIV2     /* max 108MHz */
#define CLOCK_APB2          (CLOCK_CORECLOCK / 2)

/* Main PLL factors */
#define CLOCK_PLL_M          (4)
#define CLOCK_PLL_N          (216)
#define CLOCK_PLL_P          (2)
#define CLOCK_PLL_Q          (9)

#ifdef __cplusplus
}
#endif

#endif /* F7_CFG_CLOCK_216_8_1_H */
/** @} */

/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_nucleo
 * @brief       XX
 * @{
 *
 * @file
 * @brief       XXX
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_CLK_F4_180_H
#define PERIPH_CLK_F4_180_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Clock settings
 *
 * @note    This is auto-generated from
 *          `cpu/stm32_common/dist/clk_conf/clk_conf.c`
 * @{
 */
/* give the target core clock (HCLK) frequency [in Hz],
 * maximum: 180MHz */
#define CLOCK_CORECLOCK     (180000000U)
/* 0: no external high speed crystal available
 * else: actual crystal frequency [in Hz] */
#define CLOCK_HSE           (8000000U)
/* 0: no external low speed crystal available,
 * 1: external crystal available (always 32.768kHz) */
#define CLOCK_LSE           (1)
/* peripheral clock setup */
#define CLOCK_AHB_DIV       RCC_CFGR_HPRE_DIV1
#define CLOCK_AHB           (CLOCK_CORECLOCK / 1)
#define CLOCK_APB1_DIV      RCC_CFGR_PPRE1_DIV4     /* max 45MHz */
#define CLOCK_APB1          (CLOCK_CORECLOCK / 4)
#define CLOCK_APB2_DIV      RCC_CFGR_PPRE2_DIV2     /* max 90MHz */
#define CLOCK_APB2          (CLOCK_CORECLOCK / 2)

/* Main PLL factors */
#define CLOCK_PLL_M          (4)
#define CLOCK_PLL_N          (180)
#define CLOCK_PLL_P          (2)
#define CLOCK_PLL_Q          (0)

/* PLL SAI configuration */
#define CLOCK_ENABLE_PLL_SAI (1)
#define CLOCK_PLL_SAI_M      (4)
#define CLOCK_PLL_SAI_N      (192)
#define CLOCK_PLL_SAI_P      (8)
#define CLOCK_PLL_SAI_Q      (0)

/* Use alternative source for 48MHz clock */
#define CLOCK_USE_ALT_48MHZ  (1)
/** @} */

/**
 * @name    SPI clock divisors
 *
 * @note    The spi_divtable is auto-generated from
 *          `cpu/stm32_common/dist/spi_divtable/spi_divtable.c`
 *
 * @{
 */
static const uint8_t spi_divtable[2][5] = {
    {       /* for APB1 @ 90000000Hz */
        7,  /* -> 351562Hz */
        7,  /* -> 351562Hz */
        6,  /* -> 703125Hz */
        3,  /* -> 5625000Hz */
        2   /* -> 11250000Hz */
    },
    {       /* for APB2 @ 180000000Hz */
        7,  /* -> 703125Hz */
        7,  /* -> 703125Hz */
        7,  /* -> 703125Hz */
        4,  /* -> 5625000Hz */
        3   /* -> 11250000Hz */
    }
};
/** @} */


#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CLK_F4_180_H */
/** @} */

/*
 * Copyright (C) 2017 Freie Universität Berlin
 *               2017 OTA keys S.A.
 *               2017 HAW-Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32l4
 * @{
 *
 * @file
 * @brief       Implementation of STM32 clock configuration
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Nick van IJzendoorn <nijzendoorn@engineering-spirit.nl>
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 * @}
 */

#include "cpu.h"
#include "stmclk.h"
#include "periph_conf.h"

/* make sure we have all needed information about the clock configuration */
#ifndef CLOCK_HSE
#error "Please provide CLOCK_HSE in your board's perhip_conf.h"
#endif
#ifndef CLOCK_LSE
#error "Please provide CLOCK_LSE in your board's periph_conf.h"
#endif
#if !defined(CLOCK_PLL_M) || !defined(CLOCK_PLL_N) || !defined(CLOCK_PLL_R)
#error "Please provide the PLL configuration in your board's periph_conf.h"
#endif

/**
 * @name    PLL configuration
 * @{
 */
/* figure out which input to use */
#if (CLOCK_HSE)
#define PLL_IN                      CLOCK_HSE
#define PLL_SRC                     RCC_PLLCFGR_PLLSRC_HSE
#else
#define PLL_IN                      (48000000) /* MSI @ 48MHz */
#define PLL_SRC                     RCC_PLLCFGR_PLLSRC_MSI
#endif

/**check configuration and get the corresponding bitfields */
#if (CLOCK_PLL_M < 1 || CLOCK_PLL_M > 8)
#error "PLL configuration: PLL M value is out of range"
#endif
#define PLL_M                       ((CLOCK_PLL_M - 1) << RCC_PLLCFGR_PLLM_Pos)

#if (CLOCK_PLL_N < 8 || CLOCK_PLL_N > 86)
#error "PLL configuration: PLL N value is out of range"
#endif
#define PLL_N                       (CLOCK_PLL_N << RCC_PLLCFGR_PLLN_Pos)

#if (CLOCK_PLL_R == 2)
#define PLL_R                       (0)
#elif (CLOCK_PLL_R == 4)
#define PLL_R                       (RCC_PLLCFGR_PLLR_0)
#elif (CLOCK_PLL_R == 6)
#define PLL_R                       (RCC_PLLCFGR_PLLR_1)
#elif (CLOCK_PLL_R == 8)
#define PLL_R                       (RCC_PLLCFGR_PLLR_0 | RCC_PLLCFGR_PLLR_1)
#else
#error "PLL configuration: PLL R value is invalid"
#endif
/** @} */

/**
 * @name    Deduct the needed flash wait states from the core clock frequency
 * @{
 */
#if (CLOCK_CORECLOCK <= 16000000)
#define FLASH_WAITSTATES            FLASH_ACR_LATENCY_0WS
#elif (CLOCK_CORECLOCK <= 32000000)
#define FLASH_WAITSTATES            FLASH_ACR_LATENCY_1WS
#elif (CLOCK_CORECLOCK <= 48000000)
#define FLASH_WAITSTATES            FLASH_ACR_LATENCY_2WS
#elif (CLOCK_CORECLOCK <= 64000000)
#define FLASH_WAITSTATES            FLASH_ACR_LATENCY_3WS
#else
#define FLASH_WAITSTATES            FLASH_ACR_LATENCY_4WS
#endif
/** @} */

void stmclk_init_sysclk(void)
{
    /* disable any interrupts. Global interrupts could be enabled if this is
     * called from some kind of bootloader...  */
    unsigned is = irq_disable();
    RCC->CIER = 0;

    /* enable HSI clock for the duration of initialization */
    stmclk_enable_hsi();

    /* use HSI as system clock while we do any further configuration and
     * configure the AHB and APB clock dividers as configure by the board */
    RCC->CFGR = (RCC_CFGR_SW_HSI | CLOCK_AHB_DIV |
                 CLOCK_APB1_DIV | CLOCK_APB2_DIV);
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI) {}

    /* we enable I+D cashes, pre-fetch, and we set the actual number of
     * needed flash wait states */
    FLASH->ACR = (FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN |
                  FLASH_WAITSTATES);

    /* disable all active clocks except HSI -> resets the clk configuration
     * Note: on STM32L4x5 & STM32L4x6 this disables the following:
             PLLSAI2, PLLSAI1, Main PLL (via PLLON),
             Clock security system (via CSSON), MSI clock PLL (via MSIPLLEN),
             HSE crystal oscillator bypass (via HSEBYP), HSE,
             HSI16 automatic start from Stop (via HSIASFS),
             HSI16 always enable for peripheral kernels (via HSIKERON).

             Additionally it configures the MSI clock range (MSIRANGE) to
             ~100 kHz and the MSI clock to be based on MSISRANGE in RCC_CSR
             (instead of MSIRANGE in the RCC_CR) */
    RCC->CR = (RCC_CR_HSION);

#if (CLOCK_HSE)
    /* if configured, we need to enable the HSE clock now */
    RCC->CR |= (RCC_CR_HSEON);
    while (!(RCC->CR & RCC_CR_HSERDY)) {}
#endif

#if ((CLOCK_HSE == 0) || CLOCK_MSI_ENABLE)
    /* reset clock to MSI with 48MHz, disables all other clocks */
    RCC->CR = (RCC_CR_MSIRANGE_11 | RCC_CR_MSION | RCC_CR_MSIRGSEL);
    while (!(RCC->CR & RCC_CR_MSIRDY)) {}
    /* select the MSI clock for the 48MHz clock tree (USB, RNG) */
    RCC->CCIPR = (RCC_CCIPR_CLK48SEL_0 | RCC_CCIPR_CLK48SEL_1);
#if CLOCK_MSI_LSE_PLL
    /* configure the low speed clock domain */
    stmclk_enable_lfclk();
    /* now we can enable the MSI PLL mode to enhance accuracy of the MSI*/
    RCC->CR |= RCC_CR_MSIPLLEN;
    while (!(RCC->CR & RCC_CR_MSIRDY)) {}
#endif /* CLOCK_MSI_LSE_PLL */
#endif /* ((CLOCK_HSE == 0) || CLOCK_MSI_ENABLE) */

    /* now we can safely configure and start the PLL */
    RCC->PLLCFGR = (PLL_SRC | PLL_M | PLL_N | PLL_R | RCC_PLLCFGR_PLLREN);
    RCC->CR |= (RCC_CR_PLLON);
    while (!(RCC->CR & RCC_CR_PLLRDY)) {}

    /* now that the PLL is running, we use it as system clock */
    RCC->CFGR |= (RCC_CFGR_SW_PLL);
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {}

    stmclk_disable_hsi();
    irq_restore(is);
}

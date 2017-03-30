/*
 * Copyright (C) 2017 Freie Universität Berlin
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
 * @brief       Implementation of the CPU initialization
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Nick van IJzendoorn <nijzendoorn@engineering-spirit.nl>
 * @}
 */

#include <stdint.h>
#include "cpu.h"
#include "irq.h"
#include "periph_conf.h"
#include "periph/init.h"

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
#define PLL_IN                      (48000000)      /* MSI @ 48MHz */
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

/**
 * @brief   Configure the STM32L4's clock system
 *
 * We use the following configuration:
 * - we always enable the 32kHz low speed clock (LSI or LSE)
 * - we configure the MSI clock to 48MHz (for USB and RNG) and enable it
 * - if LSE present, we use it to stabilize the 48MHz MSI clock (MSIPLLEN)
 * - use either MSI @ 48MHz or HSE (4 to 48MHZ) as base clock
 * - we use the PLL as main clock provider
 * - we don't enable any ASI clock
 *
 * For the computation of the PLL configuration, see defines above.
 */
static void cpu_clock_init(void)
{
    /* disable any interrupts. Global interrupts could be enabled if this is
     * called from some kind of bootloader...  */
    unsigned is = irq_disable();
    RCC->CIER = 0;

    /* for the duration of the configuration, we fall-back to the maximum number
     * of flash wait states */
    FLASH->ACR = (FLASH_ACR_LATENCY_4WS);

    /* reset clock to MSI with 48MHz, disables all other clocks */
    RCC->CR = (RCC_CR_MSIRANGE_11 | RCC_CR_MSION | RCC_CR_MSIRGSEL);
    while (!(RCC->CR & RCC_CR_MSIRDY)) {}

    /* use MSI as system clock while we do any further configuration and
     * configure the AHB and APB clock dividers as configure by the board */
    RCC->CFGR = (RCC_CFGR_SW_MSI | CLOCK_AHB_DIV |
                 CLOCK_APB1_DIV | CLOCK_APB2_DIV);
    while ((RCC->CFGR & RCC_CFGR_SWS_Msk) != RCC_CFGR_SWS_MSI) {}

    /* configure the low speed clock domain (LSE vs LSI) */
#if CLOCK_LSE
    /* allow write access to backup domain */
    periph_clk_en(APB1, RCC_APB1ENR1_PWREN);
    PWR->CR1 |= PWR_CR1_DBP;
    /* enable LSE */
    RCC->BDCR = RCC_BDCR_LSEON;
    while (!(RCC->BDCR & RCC_BDCR_LSERDY)) {}
    /* disable write access to back domain when done */
    PWR->CR1 &= ~(PWR_CR1_DBP);
    periph_clk_dis(APB1, RCC_APB1ENR1_PWREN);

    /* now we can enable the MSI PLL mode */
    RCC->CR |= RCC_CR_MSIPLLEN;
    while (!(RCC->CR & RCC_CR_MSIRDY)) {}
#else
    RCC->CSR = RCC_CSR_LSION;
    while (!(RCC->CSR & RCC_CSR_LSIRDY)) {}
#endif

    /* select the MSI clock for the 48MHz clock tree (USB, RNG) */
    RCC->CCIPR = (RCC_CCIPR_CLK48SEL_0 | RCC_CCIPR_CLK48SEL_1);

    /* if configured: enable the HSE clock */
#if CLOCK_HSE
    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY)) {}
#endif

    /* next we configure and enable the PLL */
    RCC->PLLCFGR = (PLL_SRC | PLL_M | PLL_N | PLL_R | RCC_PLLCFGR_PLLREN);
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY)) {}

    /* now tell the system to use the PLL as main clock */
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS_Msk) != RCC_CFGR_SWS_PLL) {}

    /* finally we enable I+D cashes, pre-fetch, and we set the actual number of
     * needed flash wait states */
    FLASH->ACR = (FLASH_ACR_ICEN | FLASH_ACR_DCEN |
                  FLASH_ACR_PRFTEN | FLASH_WAITSTATES);

    irq_restore(is);
}

/**
 * @brief   Initialize the CPU, set IRQ priorities
 */
void cpu_init(void)
{
    /* initialize the Cortex-M core */
    cortexm_init();
    /* initialize the clock system */
    cpu_clock_init();
    /* trigger static peripheral initialization */
    periph_init();
}

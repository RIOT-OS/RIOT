/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32
 * @{
 *
 * @file
 * @brief       Implementation of STM32 clock configuration for the G4 family
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @}
 */

#include "cpu.h"
#include "stmclk.h"
#include "periph_conf.h"

#if CLOCK_USE_HSE && CLOCK_HSE == 0
#error "HSE is selected as input clock source but CLOCK_HSE is not set"
#endif

#if CLOCK_USE_PLL
#if (CLOCK_PLL_M < 1 || CLOCK_PLL_M > 16)
#error "PLL configuration: PLL M value is out of range"
#endif
#define PLL_M                       ((CLOCK_PLL_M - 1) << RCC_PLLCFGR_PLLM_Pos)

#if (CLOCK_PLL_N < 8 || CLOCK_PLL_N > 127)
#error "PLL configuration: PLL N value is out of range"
#endif
#define PLL_N                       (CLOCK_PLL_N << RCC_PLLCFGR_PLLN_Pos)

#if (CLOCK_PLL_R < 1 || CLOCK_PLL_R > 8)
#error "PLL configuration: PLL R value is out of range"
#endif
#define PLL_R                       (((CLOCK_PLL_R >> 1) - 1) << RCC_PLLCFGR_PLLR_Pos)

#if CLOCK_HSE
#define PLL_IN                      CLOCK_HSE
#define PLL_SRC                     RCC_PLLCFGR_PLLSRC_HSE
#else
#define PLL_IN                      CLOCK_HSI
#define PLL_SRC                     RCC_PLLCFGR_PLLSRC_HSI
#endif
#endif /* CLOCK_USE_PLL */

/** Determine the required flash wait states from the core clock frequency */
#if CLOCK_AHB >= 136
#define FLASH_WAITSTATES            (FLASH_ACR_LATENCY_4WS) /* 4 ws */
#elif CLOCK_AHB >= 102
#define FLASH_WAITSTATES            (FLASH_ACR_LATENCY_3WS) /* 3 ws */
#elif CLOCK_AHB >= 68
#define FLASH_WAITSTATES            (FLASH_ACR_LATENCY_2WS) /* 2 ws */
#elif CLOCK_AHB >= 34
#define FLASH_WAITSTATES            (FLASH_ACR_LATENCY_1WS) /* 1 ws */
#else
#define FLASH_WAITSTATES            (0)                     /* 0 ws */
#endif

void stmclk_init_sysclk(void)
{
    /* disable any interrupts. Global interrupts could be enabled if this is
     * called from some kind of bootloader...  */
    unsigned is = irq_disable();
    /* enable HSI clock for the duration of initialization */
    stmclk_enable_hsi();

    RCC->CIER = 0;

    /* use HSI as system clock while we do any further configuration and
     * configure the AHB and APB clock dividers as configured by the board */
    RCC->CFGR = (RCC_CFGR_SW_HSI | CLOCK_AHB_DIV | CLOCK_APB1_DIV | CLOCK_APB2_DIV);
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI) {}

    /* we enable I+D caches, pre-fetch, and we set the actual number of
     * needed flash wait states */
    FLASH->ACR |= (FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN |
                   FLASH_WAITSTATES);

    /* disable all active clocks except HSI -> resets the clk configuration */
    RCC->CR = RCC_CR_HSION;

#if CLOCK_LSE
    stmclk_enable_lfclk();
#endif

#if CLOCK_USE_HSE
    /* if configured, we need to enable the HSE clock now */
    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY)) {}

    RCC->CFGR = (RCC_CFGR_SW_HSE | CLOCK_AHB_DIV | CLOCK_APB1_DIV | CLOCK_APB2_DIV);
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSE) {}
#elif CLOCK_USE_PLL
#if CLOCK_HSE
    /* if configured, we need to enable the HSE clock now */
    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY)) {}
#endif
    /* now we can safely configure and start the PLL */
    RCC->PLLCFGR = (PLL_SRC | PLL_M | PLL_N | PLL_R | RCC_PLLCFGR_PLLREN);
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY)) {}

    /* now that the PLL is running, we use it as system clock */
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {}
#endif

    stmclk_disable_hsi();
    irq_restore(is);

#ifdef MODULE_PERIPH_HWRNG
    /* HWRNG is clocked by HSI48 so enable this clock when the peripheral is used */
    RCC->CRRCR |= RCC_CRRCR_HSI48ON;
    while (!(RCC->CRRCR & RCC_CRRCR_HSI48RDY)) {}
#endif

#ifdef MODULE_PERIPH_RTT
    /* Ensure LPTIM1 clock source (LSI or LSE) is correctly reset when initializing
       the clock, this is particularly useful after waking up from deep sleep */
#if CLOCK_LSE
    RCC->CCIPR |= RCC_CCIPR_LPTIM1SEL_0 | RCC_CCIPR_LPTIM1SEL_1;
#else
    RCC->CCIPR |= RCC_CCIPR_LPTIM1SEL_0;
#endif /* CLOCK_LSE */
#endif /* MODULE_PERIPH_RTT */
}

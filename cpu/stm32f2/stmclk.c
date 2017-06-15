/*
 * Copyright (C) 2017 Freie Universität Berlin
 *               2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32f2
 * @{
 *
 * @file
 * @brief       Implementation of STM32 clock configuration
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Vincent Dupont <vincent@otakeys.com>
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

/**
 * @name    PLL configuration
 * @{
 */
/* figure out which input to use */
#if (CLOCK_HSE)
#define PLL_IN                      CLOCK_HSE
#define PLL_SRC                     RCC_PLLCFGR_PLLSRC_HSE
#else
#define PLL_IN                      (16000000U)         /* HSI fixed @ 16MHz */
#define PLL_SRC                     RCC_PLLCFGR_PLLSRC_HSI
#endif

#ifndef P
/* we fix P to 2 (so the PLL output equals 2 * CLOCK_CORECLOCK) */
#define P                       (2U)
#if ((P != 2) && (P != 4) && (P != 6) && (P != 8))
#error "PLL configuration: PLL P value is invalid"
#endif
#endif /* P */
/* the recommended input clock for the PLL should be 2MHz */
#define M                       (PLL_IN / 2000000U)
#if ((M < 2) || (M > 63))
#error "PLL configuration: PLL M value is out of range"
#endif
/* next we multiply the input freq to 2 * CORECLOCK */
#define N                       (P * CLOCK_CORECLOCK / 2000000U)
#if ((N < 50) || (N > 432))
#error "PLL configuration: PLL N value is out of range"
#endif
/* finally we need to set Q, so that the USB clock is 48MHz */
#define Q                       ((P * CLOCK_CORECLOCK) / 48000000U)
#if ((Q * 48000000U) != (P * CLOCK_CORECLOCK))
#error "PLL configuration: USB frequency is not 48MHz"
#endif

#define RCC_PLLCFGR_PLLP_Pos    (16U)
#define RCC_PLLCFGR_PLLM_Pos    (0U)
#define RCC_PLLCFGR_PLLN_Pos    (6U)
#define RCC_PLLCFGR_PLLQ_Pos    (24U)
/* now we get the actual bitfields */
#define PLL_P                   (((P / 2) - 1) << RCC_PLLCFGR_PLLP_Pos)
#define PLL_M                   (M << RCC_PLLCFGR_PLLM_Pos)
#define PLL_N                   (N << RCC_PLLCFGR_PLLN_Pos)
#define PLL_Q                   (Q << RCC_PLLCFGR_PLLQ_Pos)
/** @} */

/**
 * @name    Deduct the needed flash wait states from the core clock frequency
 * @{
 */
#define FLASH_WAITSTATES        (CLOCK_CORECLOCK / 30000000U)
/** @} */

void stmclk_init_sysclk(void)
{
    /* disable any interrupts. Global interrupts could be enabled if this is
     * called from some kind of bootloader...  */
    unsigned is = irq_disable();
    RCC->CIR = 0;

    /* enable HSI clock for the duration of initialization */
    stmclk_enable_hsi();

    /* use HSI as system clock while we do any further configuration and
     * configure the AHB and APB clock dividers as configure by the board */
    RCC->CFGR = (RCC_CFGR_SW_HSI | CLOCK_AHB_DIV |
                 CLOCK_APB1_DIV | CLOCK_APB2_DIV);
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI) {}

    /* we enable I+D cashes, pre-fetch, and we set the actual number of
     * needed flash wait states */
    FLASH->ACR = (FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN | FLASH_WAITSTATES);

    /* disable all active clocks except HSI -> resets the clk configuration */
    RCC->CR = (RCC_CR_HSION | RCC_CR_HSITRIM_4);

    /* if configured, we need to enable the HSE clock now */
#if (CLOCK_HSE)
    RCC->CR |= (RCC_CR_HSEON);
    while (!(RCC->CR & RCC_CR_HSERDY)) {}
#endif

#ifdef ENABLE_PLLI2S_MCO2
    /* reset PLL I2S config register */
    RCC->PLLI2SCFGR = 0x00000000U;
    /* set PLL I2S division factor */
    RCC->PLLI2SCFGR |= (CLOCK_PLL_I2S_R & 0x07) << 28;
    /* set PLL I2S multiplication factor */
    RCC->PLLI2SCFGR |= (CLOCK_PLL_I2S_N & 0x1FF) << 6;

    /* MCO2 output is PLLI2S */
    RCC->CFGR |= (uint32_t) RCC_CFGR_MCO2_0;
    RCC->CFGR &= ~(uint32_t) RCC_CFGR_MCO2_1;
    /* MCO2 prescaler div by 5 */
    RCC->CFGR |= (uint32_t) ((CLOCK_MC02_PRE + 4 - 2) & 0x7) << 27;
    /* enable PLL I2S clock */
    RCC->CR |= RCC_CR_PLLI2SON;
    /* wait till PLL I2S clock is ready */
    while ((RCC->CR & RCC_CR_PLLI2SRDY) == 0) {}
#endif

    /* now we can safely configure and start the PLL */
    RCC->PLLCFGR = (PLL_SRC | PLL_M | PLL_N | PLL_P | PLL_Q);
    RCC->CR |= (RCC_CR_PLLON);
    while (!(RCC->CR & RCC_CR_PLLRDY)) {}

    /* now that the PLL is running, we use it as system clock */
    RCC->CFGR |= (RCC_CFGR_SW_PLL);
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {}

    stmclk_disable_hsi();
    irq_restore(is);
}

void stmclk_enable_hsi(void)
{
    RCC->CR |= (RCC_CR_HSION);
    while (!(RCC->CR & RCC_CR_HSIRDY)) {}
}

void stmclk_disable_hsi(void)
{
    if ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI) {
        RCC->CR &= ~(RCC_CR_HSION);
    }
}

void stmclk_enable_lfclk(void)
{
    /* configure the low speed clock domain (LSE vs LSI) */
#if CLOCK_LSE
    /* allow write access to backup domain */
    stmclk_bdp_unlock();
    /* enable LSE */
    RCC->BDCR |= RCC_BDCR_LSEON;
    while (!(RCC->BDCR & RCC_BDCR_LSERDY)) {}
    /* disable write access to back domain when done */
    stmclk_bdp_lock();
#else
    RCC->CSR |= RCC_CSR_LSION;
    while (!(RCC->CSR & RCC_CSR_LSIRDY)) {}
#endif
}

void stmclk_disable_lfclk(void)
{
#if CLOCK_LSE
    stmclk_bdp_unlock();
    RCC->BDCR &= ~(RCC_BDCR_LSEON);
    stmclk_bdp_lock();
#else
    RCC->CSR &= ~(RCC_CSR_LSION);
#endif
}

void stmclk_bdp_unlock(void)
{
    periph_clk_en(APB1, RCC_APB1ENR_PWREN);
    PWR->CR |= PWR_CR_DBP;
}

void stmclk_bdp_lock(void)
{
    PWR->CR &= ~(PWR_CR_DBP);
    periph_clk_dis(APB1, RCC_APB1ENR_PWREN);
}

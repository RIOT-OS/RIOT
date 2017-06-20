/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32f1
 * @{
 *
 * @file
 * @brief       Implementation of STM32F1 clock configuration
 *
 * @author      Stefan Pfeiffer <stefan.pfeiffer@fu-berlin.de>
 * @author      Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Nick van IJzendoorn <nijzendoorn@engineering-spirit.nl>
 * @author      Víctor Ariño <victor.arino@zii.aero>
 *
 * @}
 */

#include "irq.h"
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
#ifndef CLOCK_CORECLOCK
#error "Please provide CLOCK_CORECLOCK in your board's periph_conf.h"
#endif
#if !defined(CLOCK_PLL_MUL) || !defined(CLOCK_PLL_DIV)
#error "Please provide a valid PLL configuration in your board's periph_conf.h"
#endif
#if !defined(CLOCK_AHB_DIV) || !defined(CLOCK_AHB) || \
    !defined(CLOCK_APB1_DIV) || !defined(CLOCK_APB1) || \
    !defined(CLOCK_APB2_DIV) || !defined(CLOCK_APB2)
#error "Please provide a AHB and APBx configuration in your board configuration"
#endif

/* make sure the selected system clock is valid */
#if (CLOCK_CORECLOCK > 72000000)
#error "clock config: the selected system clock exceeds 72MHz"
#endif

/* figure out which base block to use */
#if CLOCK_HSE
#if (CLOCK_HSE < 4000000) || (CLOCK_HSE > 16000000)
#error "clock config: HSE value is out of valid range"
#endif
#define BASECLK                 (CLOCK_HSE)
#else
#define BASECLK                 (8000000)       /* HSI is alway 8MHz */
#endif

/* if PLL is configured, verify its parameters */
#if (CLOCK_PLL_DIV && CLOCK_PLL_MUL)
#define USEPLL
/* check clock config */
#if (CLOCK_CORECLOCK != ((BASECLK / CLOCK_PLL_DIV) * CLOCK_PLL_MUL))
#error "clock config: PLL configuration does not yield expected system clock"
#endif
/* make sure PLL_MUL is in range */
#if (CLOCK_PLL_MUL < 2) || (CLOCK_PLL_MUL > 17)
#error "clock config: CLOCK_PLL_MUL is out of range"
#endif
/* make sure PLL_DIV is 2 when using HSI as input */
#if (!CLOCK_HSE) && (CLOCK_PLL_DIV != 2)
#error "clock config: CLOCK_PLL_DIV must be 2 when using HSI oscillator"
#endif
/* and produce the actual PLL configuration */
#if CLOCK_HSE
#define PLLSRC                  (RCC_CFGR_PLLSRC)
#if (CLOCK_PLL_DIV == 2)
#define PLLDIV                  (RCC_CFGR_PLLXTPRE)
#else
#define PLLDIV                  (0)
#endif
#else
#define PLLSRC                  (0)
#define PLLDIV                  (0)
#endif
#define PLLMUL                  ((CLOCK_PLL_MUL - 2) << 18)
/* and join it for writing the the CFGR register */
#define PLLCFG                  (PLLMUL | PLLDIV | PLLSRC)
#else
#define PLLCFG                  (0)
#endif

/* now we need to select the system clock source configuration */
#ifdef USEPLL
#define SYSCLK_SRC              RCC_CFGR_SW_PLL
#define SYSCLK_BSY              RCC_CFGR_SWS_PLL
#elif CLK_HSE
#define SYSCLK_SRC              RCC_CFGR_SW_HSE
#define SYSCLK_BSY              RCC_CFGR_SWS_HSE
#else
#define SYSCLK_SRC              RCC_CFGR_SW_HSI
#define SYSCLK_BSY              RCC_CFGR_SWS_HSI
#endif

/* Configuration of flash access cycles */
#define FLASH_WAITSTATES        ((CLOCK_CORECLOCK - 1) / 24000000U)

/* define some bitfields */
#define HSITRIM                 (1 << 7)
#define SWSHSI                  (0)


void stmclk_init_sysclk(void)
{
    /* disable any IRQs */
    unsigned is = irq_disable();
    RCC->CIR = 0;

    /* enable HSI and use it as system clock */
    stmclk_enable_hsi();
    RCC->CFGR &= ~(RCC_CFGR_SW);
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI) {}

    /* its safe now to program the flash wait states */
    FLASH->ACR = (FLASH_ACR_PRFTBE | FLASH_WAITSTATES);
    /* now we are in a defined state and can stop all other clocks */
    RCC->CR = (HSITRIM | RCC_CR_HSION);
    /* next we put in the desired PLL and peripheral bus configuration */
    RCC->CFGR = (CLOCK_AHB_DIV | CLOCK_APB1_DIV | CLOCK_APB2_DIV | PLLCFG);

    /* now we need to (re-)enable the used clocks */
#if CLOCK_HSE
    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY)) {}
#endif
#ifdef USEPLL
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY)) {}
#endif

    /* leaves switching the system clock */
    RCC->CFGR |= SYSCLK_SRC;
    while ((RCC->CFGR & RCC_CFGR_SWS) != SYSCLK_BSY) {}

    /* disable HSI (if not used) */
    stmclk_disable_hsi();

    /* re-enable IRQs */
    irq_restore(is);
}

void stmclk_enable_hsi(void)
{
    RCC->CR |= RCC_CR_HSION;
    while (!(RCC->CR & RCC_CR_HSIRDY)) {}
}

void stmclk_disable_hsi(void)
{
    /* we only disable the HSI clock if not used as input for the PLL and if
     * not used directly as system clock */
#ifdef CLOCK_HSE
    if ((RCC->CFGR & RCC_CFGR_SWS) != SWSHSI) {
        RCC->CR &= ~(RCC_CR_HSION);
    }
#endif
}

void stmclk_enable_lfclk(void)
{
#if CLOCK_LSE
    stmclk_bdp_unlock();
    RCC->BDCR |= RCC_BDCR_LSEON;
    while (!(RCC->BDCR & RCC_BDCR_LSERDY)) {}
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

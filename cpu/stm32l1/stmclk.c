/*
 * Copyright (C) 2014 Freie Universität Berlin
 *               2017 Fundacion Inria Chile
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32l1
 * @{
 *
 * @file
 * @brief       Implementation of the cpu clock functions
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Nick van IJzendoorn <nijzendoorn@engineering-spirit.nl>
 * @author      Víctor Ariño <victor.arino@zii.aero>
 * @author      Oleg Artamonov <oleg@unwds.com>
 * @author      Francisco Molina <francisco.molinas@inria.cl>
 *
 * @}
 */

#include <string.h>
#include "cpu.h"
#include "stmclk.h"
#include "periph_conf.h"

/* See if we want to use the PLL */
/* If HSE and HSI are both defined, then different defines for HSx DIV and MUL*/
#if defined(CLOCK_PLL_DIV) || defined(CLOCK_PLL_MUL) || \
    defined(CLOCK_PLL_DIV_HSE) || defined(CLOCK_PLL_MUL_HSE) || \
    defined(CLOCK_PLL_DIV_HSI) || defined(CLOCK_PLL_MUL_HSI)
#define CLOCK_USE_PLL              1
#else
#define CLOCK_USE_PLL              0
#endif

/* Make sure we have all needed information about the clock configuration */
#if !defined(CLOCK_HSI) && !defined(CLOCK_HSE)
#error "Please provide CLOCK_HSE or CLOCK_HSI in your board's perhip_conf.h"
#endif
#ifndef CLOCK_CORECLOCK
#error "Please provide CLOCK_CORECLOCK in your board's periph_conf.h"
#endif

/* Check the source to be used for the PLL, can be HSE, HSI or both*/
#if defined(CLOCK_PLL_DIV_HSE) && defined(CLOCK_PLL_MUL_HSE) && \
    defined(CLOCK_PLL_DIV_HSI) && defined(CLOCK_PLL_MUL_HSI)
#define CLOCK_PLL_MULTI
#elif defined(CLOCK_HSI)
#define CLOCK_PLL_SOURCE           RCC_CFGR_PLLSRC_HSI
#elif defined(CLOCK_HSE)
#define CLOCK_PLL_SOURCE           RCC_CFGR_PLLSRC_HSE
#endif

/* Set core voltage according to core clock*/
#if (CLOCK_CORECLOCK > 16000000U)
#define CORE_VOLTAGE PWR_CR_VOS_0
#elif (CLOCK_CORECLOCK > 8000000U)
#define CORE_VOLTAGE PWR_CR_VOS_1
#else
#define CORE_VOLTAGE (PWR_CR_VOS_1 | PWR_CR_VOS_0)
#endif

static uint32_t tmpreg;
static volatile uint32_t clock_source_rdy = 0;

/**
 * @brief Configure the clock system of the stm32l1
 */
void stmclk_init_sysclk(void)
{
    /* Reset the RCC clock configuration to the default reset state(for debug purpose) */
    /* Set MSION bit */
    RCC->CR |= RCC_CR_MSION;
    /* Reset SW, HPRE, PPRE1, PPRE2, MCOSEL and MCOPRE bits */
    RCC->CFGR &= ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2);
    RCC->CFGR &= ~(RCC_CFGR_MCOSEL | RCC_CFGR_MCOPRE);
    RCC->CFGR &= ~(RCC_CFGR_SW);
    /* Reset PLL bits */
    RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLDIV | RCC_CFGR_PLLMUL);
    /* Reset HSION, HSEON, CSSON and PLLON bits */
    RCC->CR &= ~(RCC_CR_HSION | RCC_CR_HSEON | RCC_CR_HSEBYP | RCC_CR_CSSON | RCC_CR_PLLON);
    /* Disable any interrupts. Global interrupts could be enabled if this is
     * called from some kind of bootloader...  */
    unsigned state = irq_disable();
    /* Clear all interrupts */
    RCC->CIR = 0x0;

    /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration */
#if defined(CLOCK_HSE)
    RCC->CR |= (RCC_CR_HSION | RCC_CR_HSEON);

    /* MCU after reboot or poweron */
    if (clock_source_rdy == 0) {
        clock_source_rdy = RCC_CR_HSERDY;
    }

    /* Timeout for HSE configuration, fall back to HSI*/
    volatile int timeout = 0;
    while (!(RCC->CR & clock_source_rdy)) {
        timeout++;
        if (timeout > 10000) {
            clock_source_rdy = RCC_CR_HSIRDY;
            timeout = 0;
        }
    }
#else
    /* If HSE is not defined then run directly HSI*/
    stmclk_enable_hsi();
#endif

    /* Choose the most efficient flash configuration */
#if (CLOCK_CORECLOCK > 8000000U)
    /* Enable 64-bit access, prefetch and 1 wait state */
    /* (at F > 8MHz/16MHz WS must be 1) */
    FLASH->ACR |= FLASH_ACR_ACC64;
    FLASH->ACR |= CLOCK_FLASH_LATENCY;
    FLASH->ACR |= FLASH_ACR_PRFTEN;
    /* Wait for flash to become ready */
    while (!(FLASH->SR & FLASH_SR_READY)) {}
#else
    /* Set 0 wait state, 32-bit access and no prefetch */
    /* LATENCY and PRFTEN can be changed with 64-bit access enabled only */
    FLASH->ACR |= FLASH_ACR_ACC64;
    FLASH->ACR &= ~FLASH_ACR_LATENCY;
    FLASH->ACR &= ~FLASH_ACR_PRFTEN;
    FLASH->ACR &= ~FLASH_ACR_ACC64;
    /* Wait for flash to become ready */
    while (!(FLASH->SR & FLASH_SR_READY)) {}
#endif

    /* Power domain enable */
    periph_clk_en(APB1, RCC_APB1ENR_PWREN);
    /* Select the Voltage Range */
    tmpreg = PWR->CR;
    tmpreg &= ~PWR_CR_VOS;
    tmpreg |= CORE_VOLTAGE;
    PWR->CR = tmpreg;
    /* Wait until the Voltage Regulator is ready */
    while((PWR->CSR & PWR_CSR_VOSF) != 0) {}

    /* set AHB, APB1 and APB2 clock dividers */
    tmpreg = RCC->CFGR;
    tmpreg &= ~RCC_CFGR_HPRE;
    tmpreg |= (uint32_t)CLOCK_AHB_DIV;
    tmpreg &= ~RCC_CFGR_PPRE1;
    tmpreg |= (uint32_t)CLOCK_APB1_DIV;
    tmpreg &= ~RCC_CFGR_PPRE2;
    tmpreg |= (uint32_t)CLOCK_APB2_DIV;
    RCC->CFGR = tmpreg;

#if CLOCK_USE_PLL
    /*  PLL configuration: PLLCLK = CLOCK_SOURCE / PLL_DIV * PLL_MUL */
#if defined(CLOCK_PLL_MULTI)
    if (clock_source_rdy == RCC_CR_HSERDY) {
        RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSE | CLOCK_PLL_DIV_HSE | CLOCK_PLL_MUL_HSE);
    } else {
        RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSI | CLOCK_PLL_DIV_HSI | CLOCK_PLL_MUL_HSI);
    }
#else
    RCC->CFGR |= (uint32_t)(CLOCK_PLL_SOURCE | CLOCK_PLL_DIV | CLOCK_PLL_MUL);
#endif
    /* Enable PLL */
    RCC->CR |= RCC_CR_PLLON;
    /* Wait till PLL is ready */
    while ((RCC->CR & RCC_CR_PLLRDY) == 0) {}
#endif

    /* Select system clock source and turn of other clock sources*/
    tmpreg = RCC->CFGR;
    tmpreg &= ~RCC_CFGR_SW;

    uint32_t clock_cfgr_sw;
    uint32_t clock_cfgr_sw_rdy;
    uint32_t clock_disable_clocks;

    if (clock_source_rdy == RCC_CR_HSERDY) {
        clock_cfgr_sw = RCC_CFGR_SW_HSE;
        clock_cfgr_sw_rdy = RCC_CFGR_SWS_HSE;
        clock_disable_clocks = RCC_CR_HSION | RCC_CR_MSION;
    } else {
        clock_cfgr_sw = RCC_CFGR_SW_HSI;
        clock_cfgr_sw_rdy = RCC_CFGR_SWS_HSI;
        clock_disable_clocks = RCC_CR_HSEON | RCC_CR_MSION;
    }
    if (CLOCK_USE_PLL) {
        clock_cfgr_sw = RCC_CFGR_SW_PLL;
        clock_cfgr_sw_rdy = RCC_CFGR_SWS_PLL;
    }

    tmpreg |= (uint32_t)clock_cfgr_sw;
    RCC->CFGR = tmpreg;

    /* Wait for sysyem clock to be ready before desabling other clocks*/
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != clock_cfgr_sw_rdy) {}
    RCC->CR &= ~(clock_disable_clocks);

    /* Restore isr state*/
    irq_restore(state);
}

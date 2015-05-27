/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32f4
 * @{
 *
 * @file
 * @brief       Implementation of the CPU initialization
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @}
 */

#include <stdint.h>
#include "cpu.h"
#include "periph_conf.h"



static void cpu_clock_init(void);

/**
 * @brief Initialize the CPU, set IRQ priorities
 */
void cpu_init(void)
{
    /* initialize the Cortex-M core */
    cortexm_init();
    /* initialize the clock system */
    cpu_clock_init();
}

/**
 * @brief Configure the controllers clock system
 *
 * The clock initialization make the following assumptions:
 * - the external HSE clock from an external oscillator is used as base clock
 * - the internal PLL circuit is used for clock refinement
 *
 * Use the following formulas to calculate the needed values:
 *
 * SYSCLK = ((HSE_VALUE / CLOCK_PLL_M) * CLOCK_PLL_N) / CLOCK_PLL_P
 * USB, SDIO and RNG Clock =  ((HSE_VALUE / CLOCK_PLL_M) * CLOCK_PLL_N) / CLOCK_PLL_Q
 *
 * The actual used values are specified in the board's `periph_conf.h` file.
 *
 * NOTE: currently there is not timeout for initialization of PLL and other locks
 *       -> when wrong values are chosen, the initialization could stall
 */
static void cpu_clock_init(void)
{
    /* configure the HSE clock */

    /* enable the HSI clock */
    RCC->CR |= RCC_CR_HSION;

    /* reset clock configuration register */
    RCC->CFGR = 0;

    /* disable HSE, CSS and PLL */
    RCC->CR &= ~(RCC_CR_HSEON | RCC_CR_HSEBYP | RCC_CR_CSSON | RCC_CR_PLLON);

    /* disable all clock interrupts */
    RCC->CIR = 0;

    /* enable the HSE clock */
    RCC->CR |= RCC_CR_HSEON;

    /* wait for HSE to be ready */
    while (!(RCC->CR & RCC_CR_HSERDY));

    /* setup power module */

    /* enable the power module */
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    /* set the voltage scaling to 1 to enable the maximum frequency */
    PWR->CR |= PWR_CR_VOS_1;

    /* setup the peripheral bus prescalers */

    /* set the AHB clock divider */
    RCC->CFGR &= ~RCC_CFGR_HPRE;
    RCC->CFGR |= CLOCK_AHB_DIV;
    /* set the APB2 (high speed) bus clock divider */
    RCC->CFGR &= ~RCC_CFGR_PPRE2;
    RCC->CFGR |= CLOCK_APB2_DIV;
    /* set the APB1 (low speed) bus clock divider */
    RCC->CFGR &= ~RCC_CFGR_PPRE1;
    RCC->CFGR |= CLOCK_APB1_DIV;

    /* configure the PLL */

    /* reset PLL config register */
    RCC->PLLCFGR = 0;
    /* set HSE as source for the PLL */
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSE;
    /* set division factor for main PLL input clock */
    RCC->PLLCFGR |= (CLOCK_PLL_M & 0x3F);
    /* set main PLL multiplication factor for VCO */
    RCC->PLLCFGR |= (CLOCK_PLL_N & 0x1FF) << 6;
    /* set main PLL division factor for main system clock */
    RCC->PLLCFGR |= (((CLOCK_PLL_P & 0x03) >> 1) - 1) << 16;
    /* set main PLL division factor for USB OTG FS, SDIO and RNG clocks */
    RCC->PLLCFGR |= (CLOCK_PLL_Q & 0x0F) << 24;

    /* enable PLL again */
    RCC->CR |= RCC_CR_PLLON;
    /* wait until PLL is stable */
    while(!(RCC->CR & RCC_CR_PLLRDY));

    /* configure flash latency */

    /* reset flash access control register */
    FLASH->ACR = 0;
    /* enable instruction cache */
    FLASH->ACR |= FLASH_ACR_ICEN;
    /* enable data cache */
    FLASH->ACR |= FLASH_ACR_DCEN;
    /* enable pre-fetch buffer */
    // FLASH->ACR |= FLASH_ACR_PRFTEN;
    /* set flash latency */
    FLASH->ACR &= ~FLASH_ACR_LATENCY;
    FLASH->ACR |= CLOCK_FLASH_LATENCY;

    /* configure the sysclock and the peripheral clocks */

    /* set sysclock to be driven by the PLL clock */
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;

    /* wait for sysclock to be stable */
    while (!(RCC->CFGR & RCC_CFGR_SWS_PLL));
}

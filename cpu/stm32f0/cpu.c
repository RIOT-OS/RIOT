/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32f0
 * @{
 *
 * @file
 * @brief       Implementation of the CPU initialization
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @}
 */

#include "cpu.h"
#include "periph_conf.h"


static void clock_init(void);

/**
 * @brief Initialize the CPU, set IRQ priorities
 */
void cpu_init(void)
{
    /* initialize the Cortex-M core */
    cortexm_init();
    /* initialize the clock system */
    clock_init();
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
static void clock_init(void)
{
    /* configure the HSE clock */

    /* enable the HSI clock */
    RCC->CR |= RCC_CR_HSION;

    /* reset clock configuration register */
    RCC->CFGR = 0;
    RCC->CFGR2 = 0;

    /* disable HSE, CSS and PLL */
    RCC->CR &= ~(RCC_CR_HSEON | RCC_CR_HSEBYP | RCC_CR_CSSON | RCC_CR_PLLON);

    /* disable all clock interrupts */
    RCC->CIR = 0;

    /* enable the HSE clock */
    RCC->CR |= RCC_CR_HSEON;

    /* wait for HSE to be ready */
    while (!(RCC->CR & RCC_CR_HSERDY));

    /* setup the peripheral bus prescalers */

    /* set HCLK = SYSCLK, so no clock division here */
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
    /* set PCLK = HCLK, so its not divided */
    RCC->CFGR |= RCC_CFGR_PPRE_DIV1;

    /* configure the PLL */

    /* reset PLL configuration bits */
    RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMUL);
    /* set PLL configuration */
    RCC->CFGR |= RCC_CFGR_PLLSRC_HSE_PREDIV | RCC_CFGR_PLLXTPRE_HSE_PREDIV_DIV1 |
                 (((CLOCK_PLL_MUL - 2) & 0xf) << 18);

    /* enable PLL again */
    RCC->CR |= RCC_CR_PLLON;
    /* wait until PLL is stable */
    while(!(RCC->CR & RCC_CR_PLLRDY));

    /* configure flash latency */

    /* enable pre-fetch buffer and set flash latency to 1 cycle*/
    FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY;

    /* configure the sysclock and the peripheral clocks */

    /* set sysclock to be driven by the PLL clock */
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;

    /* wait for sysclock to be stable */
    while (!(RCC->CFGR & RCC_CFGR_SWS_PLL));
}

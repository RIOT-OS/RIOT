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
 * - the internal HSI clock is used as base clock
 * - each board defines how to configure HSE and PLL clocks after cpu_init()
 */
static void clock_init(void)
{
    /* configure the HSE clock */

    /* enable the HSI clock */
    RCC->CR |= RCC_CR_HSION;

    /* wait until HSI is stable */
    while (!(RCC->CR & RCC_CR_HSIRDY));

    /* reset clock configuration register */
    RCC->CFGR = 0;
    RCC->CFGR2 = 0;

    /* disable HSE, CSS and PLL */
    RCC->CR &= ~(RCC_CR_HSEON | RCC_CR_HSEBYP | RCC_CR_CSSON | RCC_CR_PLLON);

    /* disable all clock interrupts */
    RCC->CIR = 0;


    /* setup the peripheral bus prescalers */

    /* set HCLK = SYSCLK, so no clock division here */
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
    /* set PCLK = HCLK, so its not divided */
    RCC->CFGR |= RCC_CFGR_PPRE_DIV1;


    /* configure flash latency */

    /* enable pre-fetch buffer and set flash latency to 1 cycle*/
    FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY;

}

/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32f3
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

/**
 * @name Pattern to write into the Coprocessor Access Control Register to allow full FPU access
 */
#define FULL_FPU_ACCESS         (0x00f00000)

static void cpu_clock_init(void);

/**
 * @brief Initialize the CPU, set IRQ priorities
 */
void cpu_init(void)
{
    /* give full access to the FPU */
    SCB->CPACR |= (uint32_t)FULL_FPU_ACCESS;

    /* configure the vector table location to internal flash */
    SCB->VTOR = FLASH_BASE;

    /* initialize the clock system */
    cpu_clock_init();

    /* set pendSV interrupt to lowest possible priority */
    NVIC_SetPriority(PendSV_IRQn, 0xff);
}

/**
 * @brief Configure the controllers clock system
 *
 * The clock initialization make the following assumptions:
 * - the external HSE clock from an external oscillator is used as base clock
 * - the internal PLL circuit is used for clock refinement
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

    /* reset PLL configuration */
    RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMUL);
    /* set PLL to use HSE clock with prescaler 1 as input */
    RCC->CFGR |= RCC_CFGR_PLLSRC_HSE_PREDIV | RCC_CFGR_PLLXTPRE_HSE_PREDIV_DIV1 |
                 (((CLOCK_PLL_MUL - 2) & 0xf) << 18);

    /* enable PLL again */
    RCC->CR |= RCC_CR_PLLON;
    /* wait until PLL is stable */
    while(!(RCC->CR & RCC_CR_PLLRDY));

    /* configure flash latency */

    /* reset flash access control register */
    FLASH->ACR = 0;
    /* enable pre-fetch buffer */
    FLASH->ACR |= FLASH_ACR_PRFTBE;
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

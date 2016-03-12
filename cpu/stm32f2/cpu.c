/*
 * Copyright (C) 2015 Engineering-Spirit
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32f2
 * @{
 *
 * @file
 * @brief       Implementation of the kernel cpu functions
 *
 * @author      Nick v. IJzendoorn <nijzendoorn@engineering-spirit.nl>
 * @}
 */

#include <stdint.h>
#include "cpu.h"
#include "periph_conf.h"

#ifdef HSI_VALUE
# define RCC_CR_SOURCE          RCC_CR_HSION
# define RCC_CR_SOURCE_RDY      RCC_CR_HSIRDY
# define RCC_PLL_SOURCE         RCC_PLLCFGR_PLLSRC_HSI
#else
# define RCC_CR_SOURCE          RCC_CR_HSEON
# define RCC_CR_SOURCE_RDY      RCC_CR_HSERDY
# define RCC_PLL_SOURCE         RCC_PLLCFGR_PLLSRC_HSE
#endif

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

    /* Enable the high speed clock source */
    RCC->CR |= RCC_CR_SOURCE;

    /* Wait till hish speed clock source is ready */
    while ((RCC->CR & RCC_CR_SOURCE_RDY) == 0);

    /* Configure Flash prefetch, Instruction cache, Data cache and wait state */
    FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN | FLASH_ACR_DCEN;
    /* Flash 2 wait state */
    FLASH->ACR &= ~((uint32_t)FLASH_ACR_LATENCY);
    FLASH->ACR |= (uint32_t)CLOCK_FLASH_LATENCY;

    /* HCLK = SYSCLK */
    RCC->CFGR |= (uint32_t)CLOCK_AHB_DIV;
    /* PCLK2 = HCLK */
    RCC->CFGR |= (uint32_t)CLOCK_APB2_DIV;
    /* PCLK1 = HCLK */
    RCC->CFGR |= (uint32_t)CLOCK_APB1_DIV;

    /*  PLL configuration: PLLCLK = SOURCE_CLOCK / SOURCE_CLOCK_DIV * SOURCE_CLOCK_MUL */
    RCC->PLLCFGR &= ~((uint32_t)(RCC_PLLCFGR_PLLSRC | RCC_PLLCFGR_PLLN | RCC_PLLCFGR_PLLM | RCC_PLLCFGR_PLLP | RCC_PLLCFGR_PLLQ));
    RCC->PLLCFGR |= (uint32_t)(RCC_PLL_SOURCE | CLOCK_PLL_DIV | (CLOCK_PLL_MUL << 6) | (CLOCK_PLL_Q_DIV << 24));

    /* Enable PLL */
    RCC->CR |= RCC_CR_PLLON;
    /* Wait till PLL is ready */
    while ((RCC->CR & RCC_CR_PLLRDY) == 0);

    /* Select PLL as system clock source */
    RCC->CFGR &= ~((uint32_t)(RCC_CFGR_SW));
    RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;

    /* Wait till PLL is used as system clock source */
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
}

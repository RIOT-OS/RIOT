/*
 * Copyright (C) 2014 Freie Universität Berlin
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
 * @brief       Implementation of the kernel cpu functions
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"
#include "periph_conf.h"

static void clk_init(void);

void cpu_init(void)
{
    /* set PendSV priority to the lowest possible priority */
    NVIC_SetPriority(PendSV_IRQn, 0xff);

    /* initialize system clocks */
    clk_init();

    /* configure the vector table location to internal flash */
    SCB->VTOR = FLASH_BASE;
}

/**
 * @brief Configure the clock system of the stm32f1
 *
 */
static void clk_init(void)
{
    /* Reset the RCC clock configuration to the default reset state(for debug purpose) */
    /* Set MSION bit */
    RCC->CR |= RCC_CR_MSION;
    /* Reset SW, HPRE, PPRE1, PPRE2, MCOSEL and MCOPRE bits */
    RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLDIV | RCC_CFGR_PLLMUL);
    /* Reset HSION, HSEON, CSSON and PLLON bits */
    RCC->CR &= ~(RCC_CR_HSION | RCC_CR_HSEON | RCC_CR_HSEBYP | RCC_CR_CSSON | RCC_CR_PLLON);
    /* Disable all interruptss  */
    RCC->CIR = 0x0;

    /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration */
    /* Enable HSE */
    RCC->CR |= RCC_CR_HSION;
    /* Wait till HSE is ready,
     * NOTE: the MCU will stay here forever if no HSE clock is connected */
    while (!(RCC->CR & RCC_CR_HSIRDY));
    FLASH->ACR |= FLASH_ACR_ACC64;
    /* Enable Prefetch Buffer */
    FLASH->ACR |= FLASH_ACR_PRFTEN;
    /* Flash 1 wait state */
    FLASH->ACR |= CLOCK_FLASH_LATENCY;
    /* Power enable */
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    /* Select the Voltage Range 1 (1.8 V) */
    PWR->CR = PWR_CR_VOS_0;
    /* Wait Until the Voltage Regulator is ready */
    while((PWR->CSR & PWR_CSR_VOSF) != 0);
    /* HCLK = SYSCLK */
    RCC->CFGR |= (uint32_t)CLOCK_AHB_DIV;

    /* PCLK2 = HCLK */
    RCC->CFGR |= (uint32_t)CLOCK_APB2_DIV;
    /* PCLK1 = HCLK */
    RCC->CFGR |= (uint32_t)CLOCK_APB1_DIV;
    /*  PLL configuration: PLLCLK = HSE / HSE_DIV * HSE_MUL */
    RCC->CFGR &= ~((uint32_t)(RCC_CFGR_PLLSRC | RCC_CFGR_PLLDIV | RCC_CFGR_PLLMUL));
    RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSI | CLOCK_PLL_HSE_DIV | CLOCK_PLL_HSE_MUL);
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

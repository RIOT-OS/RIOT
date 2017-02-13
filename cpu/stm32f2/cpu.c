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

#include "cpu.h"
#include "periph_conf.h"
#include "periph/init.h"

#ifdef HSI_VALUE
# define RCC_CR_SOURCE          RCC_CR_HSION
# define RCC_CR_SOURCE_RDY      RCC_CR_HSIRDY
# define RCC_PLL_SOURCE         RCC_PLLCFGR_PLLSRC_HSI
#else
# define RCC_CR_SOURCE          RCC_CR_HSEON
# define RCC_CR_SOURCE_RDY      RCC_CR_HSERDY
# define RCC_PLL_SOURCE         RCC_PLLCFGR_PLLSRC_HSE
#endif

static void clk_init(void);

void cpu_init(void)
{
    /* initialize the Cortex-M core */
    cortexm_init();
    /* initialize system clocks */
    clk_init();
    /* trigger static peripheral initialization */
    periph_init();
}

/**
 * @brief Configure the clock system of the stm32f2
 *
 */
static void clk_init(void)
{
    /* Reset the RCC clock configuration to the default reset state(for debug purpose) */
    /* Set HSION bit */
    RCC->CR |= 0x00000001U;
    /* Reset SW, HPRE, PPRE1, PPRE2, ADCPRE and MCO bits */
    RCC->CFGR = 0x00000000U;
    /* Reset HSEON, CSSON and PLLON bits */
    RCC->CR &= 0xFEF6FFFFU;
    /* Reset PLLCFGR register */
    RCC->PLLCFGR = 0x24003010U;
    /* Reset HSEBYP bit */
    RCC->CR &= 0xFFFBFFFFU;
    /* Disable all interrupts and clear pending bits  */
    RCC->CIR = 0x00000000U;

    /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration */
    /* Enable the high speed clock source */
    RCC->CR |= RCC_CR_SOURCE;
    /* Wait till hish speed clock source is ready,
     * NOTE: the MCU will stay here forever if no HSE clock is connected */
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

    /* reset PLL config register */
    RCC->PLLCFGR &= ~((uint32_t)(RCC_PLLCFGR_PLLSRC | RCC_PLLCFGR_PLLN | RCC_PLLCFGR_PLLM | RCC_PLLCFGR_PLLP | RCC_PLLCFGR_PLLQ));
    /* set HSE as source for the PLL */
    RCC->PLLCFGR |= RCC_PLL_SOURCE;
    /* set division factor for main PLL input clock */
    RCC->PLLCFGR |= (CLOCK_PLL_M & 0x3F);
    /* set main PLL multiplication factor for VCO */
    RCC->PLLCFGR |= (CLOCK_PLL_N & 0x1FF) << 6;
    /* set main PLL division factor for main system clock */
    RCC->PLLCFGR |= (((CLOCK_PLL_P & 0x03) >> 1) - 1) << 16;
    /* set main PLL division factor for USB OTG FS, SDIO and RNG clocks */
    RCC->PLLCFGR |= (CLOCK_PLL_Q & 0x0F) << 24;

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

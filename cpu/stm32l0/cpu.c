/*
 * Copyright (C) 2014 Freie Universität Berlin
 *               2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32l0
 * @{
 *
 * @file
 * @brief       Implementation of the CPU initialization
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include "cpu.h"
#include "periph_conf.h"
#include "periph/init.h"


/* Check the source to be used for the PLL */
#if defined(CLOCK_HSI) && defined(CLOCK_HSE)
#error "Only provide one of two CLOCK_HSI/CLOCK_HSE"
#elif CLOCK_HSI
#define CLOCK_CR_SOURCE            RCC_CR_HSION
#define CLOCK_CR_SOURCE_RDY        RCC_CR_HSIRDY
#define CLOCK_PLL_SOURCE           RCC_CFGR_PLLSRC_HSI
#elif CLOCK_HSE
#define CLOCK_CR_SOURCE            RCC_CR_HSEON
#define CLOCK_CR_SOURCE_RDY        RCC_CR_HSERDY
#define CLOCK_PLL_SOURCE           RCC_CFGR_PLLSRC_HSE
#else
#error "Please provide CLOCK_HSI or CLOCK_HSE in boards/NAME/includes/perhip_cpu.h"
#endif

static void clk_init(void);

/**
 * @brief Initialize the CPU, set IRQ priorities
 */
void cpu_init(void)
{
    /* initialize the Cortex-M core */
    cortexm_init();
    /* initialize the clock system */
    clk_init();
    /* trigger static peripheral initialization */
    periph_init();
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
static void clk_init(void)
{
    /* Reset the RCC clock configuration to the default reset state(for debug purpose) */
    /* Set MSION bit */
    RCC->CR |= RCC_CR_MSION;
    /* Reset SW, HPRE, PPRE1, PPRE2, MCOSEL and MCOPRE bits */
    RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLDIV | RCC_CFGR_PLLMUL);
    /* Reset HSION, HSEON, CSSON and PLLON bits */
    RCC->CR &= ~(RCC_CR_HSION | RCC_CR_HSEON | RCC_CR_HSEBYP | RCC_CR_CSSON | RCC_CR_PLLON);
    /* Disable all interrupts */
    RCC->CICR = 0x0;

    /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration */
    /* Enable high speed clock source */
    RCC->CR |= CLOCK_CR_SOURCE;
    /* Wait till the high speed clock source is ready
     * NOTE: the MCU will stay here forever if you use an external clock source and it's not connected */
    while (!(RCC->CR & CLOCK_CR_SOURCE_RDY)) {}
    /* Enable Prefetch Buffer */
    FLASH->ACR |= FLASH_ACR_PRFTEN;
    /* Flash 1 wait state */
    FLASH->ACR |= CLOCK_FLASH_LATENCY;
    /* Power enable */
    periph_clk_en(APB1, RCC_APB1ENR_PWREN);
    /* Select the Voltage Range 1 (1.8 V) */
    PWR->CR = PWR_CR_VOS_0;
    /* Wait Until the Voltage Regulator is ready */
    while((PWR->CSR & PWR_CSR_VOSF) != 0) {}
    /* HCLK = SYSCLK */
    RCC->CFGR |= (uint32_t)CLOCK_AHB_DIV;
    /* PCLK2 = HCLK */
    RCC->CFGR |= (uint32_t)CLOCK_APB2_DIV;
    /* PCLK1 = HCLK */
    RCC->CFGR |= (uint32_t)CLOCK_APB1_DIV;
    /*  PLL configuration: PLLCLK = CLOCK_SOURCE / PLL_DIV * PLL_MUL */
    RCC->CFGR &= ~((uint32_t)(RCC_CFGR_PLLSRC | RCC_CFGR_PLLDIV | RCC_CFGR_PLLMUL));
    RCC->CFGR |= (uint32_t)(CLOCK_PLL_SOURCE | CLOCK_PLL_DIV | CLOCK_PLL_MUL);
    /* Enable PLL */
    RCC->CR |= RCC_CR_PLLON;
    /* Wait till PLL is ready */
    while ((RCC->CR & RCC_CR_PLLRDY) == 0) {}
    /* Select PLL as system clock source */
    RCC->CFGR &= ~((uint32_t)(RCC_CFGR_SW));
    RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;
    /* Wait till PLL is used as system clock source */
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {}
}

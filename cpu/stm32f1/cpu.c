/*
 * Copyright (C) 2013 INRIA
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2016 TriaGnoSys GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32f1
 * @{
 *
 * @file
 * @brief       Implementation of the kernel cpu functions
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

#include "cpu.h"
#include "periph_conf.h"
#include "periph/init.h"

/* Configuration of flash access cycles */
#if CLOCK_CORECLOCK <= 24000000
#define FLASH_LATENCY (0) /* Zero wait state, if 0 < SYSCLK≤ 24 MHz */
#elif CLOCK_CORECLOCK <= 48000000
#define FLASH_LATENCY (1) /* One wait states, if 24 MHz < SYSCLK ≤ 48 MHz */
#elif CLOCK_CORECLOCK <= 72000000
#define FLASH_LATENCY (2) /* Two wait states, if 48 MHz < SYSCLK ≤ 72 MHz */
#endif

/* See if we want to use the PLL */
#if defined(CLOCK_PLL_DIV) || defined(CLOCK_PLL_MUL)
#define CLOCK_USE_PLL              1
#else
#define CLOCK_USE_PLL              0
#endif

/* Check the source to be used and parameters */
#if defined(CLOCK_HSI) && defined(CLOCK_HSE)
#error "Only provide one of two CLOCK_HSI/CLOCK_HSE"
#elif (CLOCK_USE_PLL == 1) && (!defined(CLOCK_PLL_MUL) || !defined(CLOCK_PLL_DIV))
#error "When using PLL both CLOCK_PLL_DIV and CLOCK_PLL_MUL must be provided"

#elif CLOCK_HSI
#define CLOCK_CR_SOURCE            RCC_CR_HSION
#define CLOCK_CR_SOURCE_RDY        RCC_CR_HSIRDY
#define CLOCK_PLL_DIVMSK           0
#define CLOCK_PLL_SOURCE           0
#define CLOCK_DISABLE_HSI          0

#if (CLOCK_USE_PLL == 0)
#define CLOCK_CFGR_SW              RCC_CFGR_SW_HSI
#define CLOCK_CFGR_SW_RDY          RCC_CFGR_SWS_HSI
#elif (CLOCK_PLL_DIV != 2)
#error "CLOCK_PLL_DIV can only be 2 for the HSI"
#endif

#elif CLOCK_HSE
#define CLOCK_CR_SOURCE            RCC_CR_HSEON
#define CLOCK_CR_SOURCE_RDY        RCC_CR_HSERDY
#define CLOCK_PLL_SOURCE           RCC_CFGR_PLLSRC
#define CLOCK_DISABLE_HSI          1

#if (CLOCK_USE_PLL == 0)
#define CLOCK_CFGR_SW              RCC_CFGR_SW_HSE
#define CLOCK_CFGR_SW_RDY          RCC_CFGR_SWS_HSE
#elif (CLOCK_PLL_DIV == 2)
#define CLOCK_PLL_DIVMSK           RCC_CFGR_PLLXTPRE
#elif (CLOCK_PLL_DIV == 1)
#define CLOCK_PLL_DIVMSK           0
#elif defined(CLOCK_PLL_DIV)
#error "HSE divider must be 1 or 2"
#endif

#else
#error "Please provide CLOCK_HSI or CLOCK_HSE in boards/NAME/includes/perhip_cpu.h"
#endif

#if (CLOCK_USE_PLL == 1)
#define CLOCK_CFGR_SW              RCC_CFGR_SW_PLL
#define CLOCK_CFGR_SW_RDY          RCC_CFGR_SWS_PLL
#if CLOCK_PLL_MUL > 16
#error "PLL multiplier cannot exceed 16 times"
#elif CLOCK_PLL_MUL < 2
#error "PLL multiplier cannot be set to 1 or lower"
#endif
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
 * @brief Configure the clock system of the stm32f1
 */
static void clk_init(void)
{
    /* Reset the RCC clock configuration to the default reset state(for debug purpose) */
    /* Set HSION bit */
    RCC->CR |= (uint32_t)0x00000001;
    /* Reset SW, HPRE, PPRE1, PPRE2, ADCPRE and MCO bits */
    RCC->CFGR &= (uint32_t)0xF0FF0000;
    /* Reset HSEON, CSSON and PLLON bits */
    RCC->CR &= (uint32_t)0xFEF6FFFF;
    /* Reset HSEBYP bit */
    RCC->CR &= (uint32_t)0xFFFBFFFF;
    /* Reset PLLSRC, PLLXTPRE, PLLMUL and USBPRE/OTGFSPRE bits */
    RCC->CFGR &= (uint32_t)0xFF80FFFF;
    /* Disable all interrupts and clear pending bits  */
    RCC->CIR = (uint32_t)0x009F0000;

    /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration */
    /* Enable high speed clock source */
    RCC->CR |= ((uint32_t)CLOCK_CR_SOURCE);
    /* Wait till the high speed clock source is ready
     * NOTE: the MCU will stay here forever if you use an external clock source and it's not connected */
    while ((RCC->CR & CLOCK_CR_SOURCE_RDY) == 0) {}
    /* Enable prefetch buffer and set flash wait state */
    FLASH->ACR = (uint32_t)(FLASH_ACR_PRFTBE | FLASH_LATENCY);
    /* HCLK = SYSCLK */
    RCC->CFGR |= (uint32_t)CLOCK_AHB_DIV;
    /* PCLK2 = HCLK */
    RCC->CFGR |= (uint32_t)CLOCK_APB2_DIV;
    /* PCLK1 = HCLK */
    RCC->CFGR |= (uint32_t)CLOCK_APB1_DIV;

#if (CLOCK_USE_PLL == 1)
    /*  PLL configuration: PLLCLK = CLOCK_SOURCE / PLL_DIV * PLL_MUL */
    RCC->CFGR |= (uint32_t)(CLOCK_PLL_SOURCE | CLOCK_PLL_DIVMSK | ((CLOCK_PLL_MUL - 2) << 18));
    /* Enable PLL */
    RCC->CR |= RCC_CR_PLLON;
    /* Wait till PLL is ready */
    while ((RCC->CR & RCC_CR_PLLRDY) == 0) {}
#endif

    /* Select the system clock source */
    RCC->CFGR &= ~((uint32_t)(RCC_CFGR_SW));
    RCC->CFGR |= (uint32_t)CLOCK_CFGR_SW;
    /* Wait till selected system clock source is ready */
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != CLOCK_CFGR_SW_RDY) {}

#if CLOCK_DISABLE_HSI
    RCC->CR &= ~(RCC_CR_HSION);
    while ((RCC->CR & RCC_CR_HSIRDY) != 0) {}
#endif
}

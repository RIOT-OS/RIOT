/*
 * Copyright (C) 2016 TriaGnoSys GmbH
 *               2014 Freie Universität Berlin
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
 * @author      Nick van IJzendoorn <nijzendoorn@engineering-spirit.nl>
 * @author      Víctor Ariño <victor.arino@zii.aero>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"
#include "periph_conf.h"
#include "periph/init.h"

/* See if we want to use the PLL */
#if defined(CLOCK_PLL_DIV) || defined(CLOCK_PLL_MUL)
#define CLOCK_USE_PLL              1
#else
#define CLOCK_USE_PLL              0
#endif

/* Check the source to be used for the PLL */
#if defined(CLOCK_HSI) && defined(CLOCK_HSE)
#error "Only provide one of two CLOCK_HSI/CLOCK_HSE"
#elif (CLOCK_USE_PLL == 1) && (!defined(CLOCK_PLL_MUL) || !defined(CLOCK_PLL_DIV))
#error "When using PLL both CLOCK_PLL_DIV and CLOCK_PLL_MUL must be provided"

#elif CLOCK_HSI
#define CLOCK_CR_SOURCE            RCC_CR_HSION
#define CLOCK_CR_SOURCE_RDY        RCC_CR_HSIRDY
#define CLOCK_PLL_SOURCE           RCC_CFGR_PLLSRC_HSI
#define CLOCK_DISABLE_OTHERS       (RCC_CR_HSEON | RCC_CR_MSION)

#if (CLOCK_USE_PLL == 0)
#define CLOCK_CFGR_SW              RCC_CFGR_SW_HSI
#define CLOCK_CFGR_SW_RDY          RCC_CFGR_SWS_HSI
#endif

#elif CLOCK_HSE
#define CLOCK_CR_SOURCE            RCC_CR_HSEON
#define CLOCK_CR_SOURCE_RDY        RCC_CR_HSERDY
#define CLOCK_PLL_SOURCE           RCC_CFGR_PLLSRC_HSE
#define CLOCK_DISABLE_OTHERS       (RCC_CR_HSION | RCC_CR_MSION)

#if (CLOCK_USE_PLL == 0)
#define CLOCK_CFGR_SW              RCC_CFGR_SW_HSE
#define CLOCK_CFGR_SW_RDY          RCC_CFGR_SWS_HSE
#endif

#elif CLOCK_MSI
#define CLOCK_CR_SOURCE            RCC_CR_MSION
#define CLOCK_CR_SOURCE_RDY        RCC_CR_MSIRDY
#define CLOCK_CFGR_SW              RCC_CFGR_SW_MSI
#define CLOCK_CFGR_SW_RDY          RCC_CFGR_SWS_MSI
#define CLOCK_DISABLE_OTHERS       (RCC_CR_HSEON | RCC_CR_HSION)

#if (CLOCK_USE_PLL == 1)
#error "PLL can't be used with MSI"
#endif

#else
#error "Please provide CLOCK_HSI or CLOCK_HSE in boards/NAME/includes/perhip_cpu.h"
#endif

#if (CLOCK_USE_PLL == 1)
#define CLOCK_CFGR_SW              RCC_CFGR_SW_PLL
#define CLOCK_CFGR_SW_RDY          RCC_CFGR_SWS_PLL
#endif

#if (CLOCK_CORECLOCK > 16000000U)
#define CORE_VOLTAGE PWR_CR_VOS_0
#elif (CLOCK_CORECLOCK > 8000000U)
#define CORE_VOLTAGE PWR_CR_VOS_1
#else
#define CORE_VOLTAGE (PWR_CR_VOS_1 | PWR_CR_VOS_0)
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
    /* Set MSION bit */
    RCC->CR |= RCC_CR_MSION;
    /* Reset SW, HPRE, PPRE1, PPRE2, MCOSEL and MCOPRE bits */
    RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLDIV | RCC_CFGR_PLLMUL);
    /* Reset HSION, HSEON, CSSON and PLLON bits */
    RCC->CR &= ~(RCC_CR_HSION | RCC_CR_HSEON | RCC_CR_HSEBYP | RCC_CR_CSSON | RCC_CR_PLLON);
    /* Disable all interrupts */
    RCC->CIR = 0x0;

    /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration */
    /* Enable high speed clock source */
    RCC->CR |= CLOCK_CR_SOURCE;
    /* Wait till the high speed clock source is ready
     * NOTE: the MCU will stay here forever if you use an external clock source and it's not connected */
    while (!(RCC->CR & CLOCK_CR_SOURCE_RDY)) {}
    FLASH->ACR |= FLASH_ACR_ACC64 | FLASH_ACR_SLEEP_PD;
    /* Enable Prefetch Buffer */
    FLASH->ACR |= FLASH_ACR_PRFTEN;
    /* Flash 1 wait state */
    FLASH->ACR |= CLOCK_FLASH_LATENCY;
    /* Power enable */
    periph_clk_en(APB1, RCC_APB1ENR_PWREN);
    /* Select the Voltage Range */
    PWR->CR = CORE_VOLTAGE;
    /* Wait Until the Voltage Regulator is ready */
    while((PWR->CSR & PWR_CSR_VOSF) != 0) {}
#if CLOCK_MSI
    PWR->CR |= PWR_CR_LPSDSR | PWR_CR_LPRUN;
#endif
    /* HCLK = SYSCLK */
    RCC->CFGR |= (uint32_t)CLOCK_AHB_DIV;
    /* PCLK2 = HCLK */
    RCC->CFGR |= (uint32_t)CLOCK_APB2_DIV;
    /* PCLK1 = HCLK */
    RCC->CFGR |= (uint32_t)CLOCK_APB1_DIV;

#if CLOCK_USE_PLL
    /*  PLL configuration: PLLCLK = CLOCK_SOURCE / PLL_DIV * PLL_MUL */
    RCC->CFGR |= (uint32_t)(CLOCK_PLL_SOURCE | CLOCK_PLL_DIV | CLOCK_PLL_MUL);
    /* Enable PLL */
    RCC->CR |= RCC_CR_PLLON;
    /* Wait till PLL is ready */
    while ((RCC->CR & RCC_CR_PLLRDY) == 0) {}
#elif CLOCK_MSI
    RCC->ICSCR &= ~(RCC_ICSCR_MSIRANGE);
    RCC->ICSCR |= (CLOCK_MSI << 13);
#endif  

    /* Select system clock source */
    RCC->CFGR &= ~((uint32_t)(RCC_CFGR_SW));
    RCC->CFGR |= (uint32_t)CLOCK_CFGR_SW;
    /* Wait until selected system clock is ready */
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != CLOCK_CFGR_SW_RDY) {}

    RCC->CR &= ~(CLOCK_DISABLE_OTHERS);
}

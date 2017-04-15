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
 * @author      Nick van IJzendoorn <nijzendoorn@engineering-spirit.nl>
 * @}
 */

#include <stdint.h>
#include "cpu.h"
#include "periph_conf.h"
#include "periph/init.h"

/* Check the source to be used for the PLL */
#if defined(CLOCK_HSI) && defined(CLOCK_HSE)
#error "Only provide one of two CLOCK_HSI/CLOCK_HSE"
#elif CLOCK_HSI
#define CLOCK_CR_SOURCE             RCC_CR_HSION
#define CLOCK_CR_SOURCE_RDY         RCC_CR_HSIRDY
#ifdef RCC_CFGR_PLLSRC_HSI_DIV2
#define CLOCK_PLL_SOURCE            (RCC_CFGR_PLLSRC_HSI_DIV2)
#define CLOCK_PLL_MUL_MULTIPLIER    2
#else
#define CLOCK_PLL_SOURCE            (RCC_CFGR_PLLSRC_HSI_PREDIV)
#define CLOCK_PLL_MUL_MULTIPLIER    1
#endif
#define CLOCK_DISABLE_HSI           0
#elif CLOCK_HSE
#define CLOCK_CR_SOURCE             RCC_CR_HSEON
#define CLOCK_CR_SOURCE_RDY         RCC_CR_HSERDY
#define CLOCK_PLL_SOURCE            (RCC_CFGR_PLLSRC_HSE_PREDIV | RCC_CFGR_PLLXTPRE_HSE_PREDIV_DIV1)
#define CLOCK_PLL_MUL_MULTIPLIER    1
#define CLOCK_DISABLE_HSI           1
#else
#error "Please provide CLOCK_HSI or CLOCK_HSE in boards/NAME/includes/perhip_cpu.h"
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
    /* trigger static peripheral initialization */
    periph_init();
}

/**
 * @brief Configure the controllers clock system
 *
 * The clock initialization make the following assumptions:
 * - the HSI and HSE selection is based on the CLOCK_HSI or CLOCK_HSE define in the periph_conf.h
 * - the internal PLL circuit is used for clock refinement
 *
 * The actual used values are specified in the board's `periph_conf.h` file.
 *
 * NOTE: currently there is not timeout for initialization of PLL and other locks
 *       -> when wrong values are chosen, the initialization could stall
 */
static void cpu_clock_init(void)
{
    /* configure the high speed clock */

    /* reset clock configuration register */
    RCC->CFGR = 0;

    /* disable HSE, CSS and PLL */
    RCC->CR &= ~(RCC_CR_HSEON | RCC_CR_HSEBYP | RCC_CR_CSSON | RCC_CR_PLLON);

    /* disable all clock interrupts */
    RCC->CIR = 0;

    /* enable the high speed clock */
    RCC->CR |= CLOCK_CR_SOURCE;

    /* wait for high speed clock to be ready */
    while (!(RCC->CR & CLOCK_CR_SOURCE_RDY)) {}

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
    /* set PLL to use high speed clock with prescaler 1 as input */
    RCC->CFGR |= CLOCK_PLL_SOURCE | ((((CLOCK_PLL_MUL * CLOCK_PLL_MUL_MULTIPLIER) - 2) & 0xf) << 18);

    /* enable PLL again */
    RCC->CR |= RCC_CR_PLLON;
    /* wait until PLL is stable */
    while(!(RCC->CR & RCC_CR_PLLRDY)) {}

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
    while (!(RCC->CFGR & RCC_CFGR_SWS_PLL)) {}

#if CLOCK_DISABLE_HSI
    /* disable the HSI if we use the HSE */
    RCC->CR &= ~(RCC_CR_HSION);
    while (RCC->CR & RCC_CR_HSIRDY) {}
#endif
}

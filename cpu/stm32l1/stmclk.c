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
 * @author      Nick van IJzendoorn <nijzendoorn@engineering-spirit.nl>
 * @author      Víctor Ariño <victor.arino@zii.aero>
 * @author      Oleg Artamonov <info@unwds.com>
 *
 * @}
 */

#include <string.h>
#include "cpu.h"
#include "stmclk.h"
#include "periph_conf.h"

/* See if we want to use the PLL */
#if defined(CLOCK_PLL_DIV) || defined(CLOCK_PLL_MUL) || \
defined(CLOCK_PLL_DIV_HSE) || defined(CLOCK_PLL_MUL_HSE) || \
defined(CLOCK_PLL_DIV_HSI) || defined(CLOCK_PLL_MUL_HSI)

#define CLOCK_USE_PLL              1
#else
#define CLOCK_USE_PLL              0
#endif

#if !defined(CLOCK_HSI) && !defined(CLOCK_HSE) && !defined(CLOCK_MSI)
#error "Please provide clock source in board.h or periph_conf.h file"
#endif

/* Check the source to be used for the PLL */
#if defined(CLOCK_HSI) && defined(CLOCK_HSE)
#define CLOCK_HS_MULTI
#endif

#if !defined(CLOCK_HS_MULTI)
#if defined(CLOCK_HSI)
#define CLOCK_CR_SOURCE            RCC_CR_HSION
#define CLOCK_CR_SOURCE_RDY        RCC_CR_HSIRDY
#define CLOCK_PLL_SOURCE           RCC_CFGR_PLLSRC_HSI
#define CLOCK_DISABLE_OTHERS       (RCC_CR_HSEON | RCC_CR_MSION)

#if (CLOCK_USE_PLL == 0)
#define CLOCK_CFGR_SW              RCC_CFGR_SW_HSI
#define CLOCK_CFGR_SW_RDY          RCC_CFGR_SWS_HSI
#endif
#elif defined(CLOCK_HSE)
#define CLOCK_CR_SOURCE            RCC_CR_HSEON
#define CLOCK_CR_SOURCE_RDY        RCC_CR_HSERDY
#define CLOCK_PLL_SOURCE           RCC_CFGR_PLLSRC_HSE
#define CLOCK_DISABLE_OTHERS       (RCC_CR_HSION | RCC_CR_MSION)

#if (CLOCK_USE_PLL == 0)
#define CLOCK_CFGR_SW              RCC_CFGR_SW_HSE
#define CLOCK_CFGR_SW_RDY          RCC_CFGR_SWS_HSE
#endif
#endif
#endif

#if defined(CLOCK_MSI)
#define CLOCK_CR_SOURCE            RCC_CR_MSION
#define CLOCK_CR_SOURCE_RDY        RCC_CR_MSIRDY
#define CLOCK_CFGR_SW              RCC_CFGR_SW_MSI
#define CLOCK_CFGR_SW_RDY          RCC_CFGR_SWS_MSI
#define CLOCK_DISABLE_OTHERS       (RCC_CR_HSEON | RCC_CR_HSION)
#define CLOCK_MSIRANGE             RCC_ICSCR_MSIRANGE_6

#if (CLOCK_USE_PLL == 1)
#error "PLL can't be used with MSI"
#endif
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

static uint32_t tmpreg;
static volatile uint32_t clock_source_rdy = 0;
volatile uint32_t cpu_clock_global;
volatile uint32_t cpu_ports_number;
char cpu_clock_source[10] = { 0 };

/**
 * @brief Configure the clock system of the stm32l1
 */
void stmclk_init_sysclk(void)
{
    /* Reset the RCC clock configuration to the default reset state(for debug purpose) */
    /* Set MSION bit */
    RCC->CR |= RCC_CR_MSION;
    /* Reset SW, HPRE, PPRE1, PPRE2, MCOSEL and MCOPRE bits */
    RCC->CFGR &= ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2);
    RCC->CFGR &= ~(RCC_CFGR_MCOSEL | RCC_CFGR_MCOPRE);
    RCC->CFGR &= ~(RCC_CFGR_SW);
    /* Reset PLL bits */
    RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLDIV | RCC_CFGR_PLLMUL);
    /* Reset HSION, HSEON, CSSON and PLLON bits */
    RCC->CR &= ~(RCC_CR_HSION | RCC_CR_HSEON | RCC_CR_HSEBYP | RCC_CR_CSSON | RCC_CR_PLLON);
    /* Clear all interrupts */
    RCC->CIR = 0x0;

    /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration */
#if defined(CLOCK_HS_MULTI)
    RCC->CR |= (RCC_CR_HSION | RCC_CR_HSEON);

    /* MCU after reboot or poweron */
    if (clock_source_rdy == 0) {
        clock_source_rdy = RCC_CR_HSERDY;
    }

    volatile int timeout = 0;
    while (!(RCC->CR & clock_source_rdy)) {
        timeout++;
        if (timeout > 10000) {
            clock_source_rdy = RCC_CR_HSIRDY;
            timeout = 0;
        }
    }
#else
    /* Enable high speed clock source */
    RCC->CR |= CLOCK_CR_SOURCE;
    /* Wait till the clock source is ready
     * NOTE: the MCU will stay here forever if you use an external clock source and it's not connected */
    while (!(RCC->CR & CLOCK_CR_SOURCE_RDY)) {}
#endif

    /* Choose the most efficient flash configuration */
#if (CLOCK_CORECLOCK > 8000000U)
    /* Enable 64-bit access, prefetch and 1 wait state */
    /* (at F > 8MHz/16MHz WS must be 1) */
    FLASH->ACR |= FLASH_ACR_ACC64;
    FLASH->ACR |= CLOCK_FLASH_LATENCY;
    FLASH->ACR |= FLASH_ACR_PRFTEN;
    /* Wait for flash to become ready */
    while (!(FLASH->SR & FLASH_SR_READY)) {}
#else
    /* Set 0 wait state, 32-bit access and no prefetch */
    /* LATENCY and PRFTEN can be changed with 64-bit access enabled only */
    FLASH->ACR |= FLASH_ACR_ACC64;
    FLASH->ACR &= ~FLASH_ACR_LATENCY;
    FLASH->ACR &= ~FLASH_ACR_PRFTEN;
    FLASH->ACR &= ~FLASH_ACR_ACC64;
    /* Wait for flash to become ready */
    while (!(FLASH->SR & FLASH_SR_READY)) {}
#endif

    /* Power domain enable */
    periph_clk_en(APB1, RCC_APB1ENR_PWREN);
    /* Select the Voltage Range */
    tmpreg = PWR->CR;
    tmpreg &= ~PWR_CR_VOS;
    tmpreg |= CORE_VOLTAGE;
    PWR->CR = tmpreg;
    /* Wait until the Voltage Regulator is ready */
    while((PWR->CSR & PWR_CSR_VOSF) != 0) {}

    /* Enable low-power run if permitted */
#if CLOCK_MSI
    if ((CLOCK_MSIRANGE == RCC_ICSCR_MSIRANGE_1) || (msi_range == RCC_ICSCR_MSIRANGE_0)) {
        PWR->CR |= PWR_CR_LPSDSR | PWR_CR_LPRUN;
    }
#endif

    /* set AHB, APB1 and APB2 clock dividers */
    tmpreg = RCC->CFGR;
    tmpreg &= ~RCC_CFGR_HPRE;
    tmpreg |= (uint32_t)CLOCK_AHB_DIV;
    tmpreg &= ~RCC_CFGR_PPRE1;
    tmpreg |= (uint32_t)CLOCK_APB1_DIV;
    tmpreg &= ~RCC_CFGR_PPRE2;
    tmpreg |= (uint32_t)CLOCK_APB2_DIV;
    RCC->CFGR = tmpreg;

#if CLOCK_USE_PLL
    /*  PLL configuration: PLLCLK = CLOCK_SOURCE / PLL_DIV * PLL_MUL */
#if defined(CLOCK_HS_MULTI)
    if (clock_source_rdy == RCC_CR_HSERDY) {
        RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSE | CLOCK_PLL_DIV_HSE | CLOCK_PLL_MUL_HSE);
    } else {
        RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSI | CLOCK_PLL_DIV_HSI | CLOCK_PLL_MUL_HSI);
    }
#else
    RCC->CFGR |= (uint32_t)(CLOCK_PLL_SOURCE | CLOCK_PLL_DIV | CLOCK_PLL_MUL);
#endif
    /* Enable PLL */
    RCC->CR |= RCC_CR_PLLON;
    /* Wait till PLL is ready */
    while ((RCC->CR & RCC_CR_PLLRDY) == 0) {}
#elif CLOCK_MSI
    tmpreg = RCC->ICSCR;
    tmpreg &= ~(RCC_ICSCR_MSIRANGE);
    tmpreg |= CLOCK_MSIRANGE;
    RCC->ICSCR = tmpreg;
#endif

    /* Select system clock source */
    tmpreg = RCC->CFGR;
    tmpreg &= ~RCC_CFGR_SW;

#if defined(CLOCK_HS_MULTI)
    uint32_t clock_cfgr_sw;
    uint32_t clock_cfgr_sw_rdy;
    uint32_t clock_disable_clocks;

    if (clock_source_rdy == RCC_CR_HSERDY) {
        clock_cfgr_sw = RCC_CFGR_SW_HSE;
        clock_cfgr_sw_rdy = RCC_CFGR_SWS_HSE;
        clock_disable_clocks = RCC_CR_HSION | RCC_CR_MSION;
    } else {
        clock_cfgr_sw = RCC_CFGR_SW_HSI;
        clock_cfgr_sw_rdy = RCC_CFGR_SWS_HSI;
        clock_disable_clocks = RCC_CR_HSEON | RCC_CR_MSION;
    }

    if (CLOCK_USE_PLL) {
        clock_cfgr_sw = RCC_CFGR_SW_PLL;
        clock_cfgr_sw_rdy = RCC_CFGR_SWS_PLL;
    }

    tmpreg |= (uint32_t)clock_cfgr_sw;
    RCC->CFGR = tmpreg;
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != clock_cfgr_sw_rdy) {}
    RCC->CR &= ~(clock_disable_clocks);
#else
    tmpreg |= (uint32_t)CLOCK_CFGR_SW;

    RCC->CFGR = tmpreg;
    /* Wait till clock is used as system clock source */
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != CLOCK_CFGR_SW_RDY) {}

    /* Disable other clock sources */
    RCC->CR &= ~(CLOCK_DISABLE_OTHERS);
#endif

    cpu_clock_global = CLOCK_CORECLOCK;

#if CLOCK_MSI
    memcpy(cpu_clock_source, "MSI", 3);
#elif defined(CLOCK_HS_MULTI)
    uint32_t n = 0;
    if (CLOCK_USE_PLL) {
        memcpy(cpu_clock_source, "PLL", 3);
        n += 3;
    }
    if (clock_source_rdy == RCC_CR_HSERDY) {
        memcpy(cpu_clock_source + n, "/HSE", 4);
    } else {
        memcpy(cpu_clock_source + n, "/HSI", 4);
    }
#elif defined(CLOCK_HSI)
#if CLOCK_USE_PLL
    memcpy(cpu_clock_source, "PLL/HSI", 7);
#elif
    memcpy(cpu_clock_source, "HSI", 3);
#endif
#elif defined(CLOCK_HSE)
#if CLOCK_USE_PLL
    memcpy(cpu_clock_source, "PLL/HSE", 7);
#elif
    memcpy(cpu_clock_source, "HSE", 3);
#endif
#endif
}

void stmclk_switch_msi(uint32_t msi_range, uint32_t ahb_divider)
{
    RCC->CR |= RCC_CR_MSION;
    while (!(RCC->CR & RCC_CR_MSIRDY)) {}

    tmpreg = RCC->ICSCR;
    tmpreg &= ~RCC_ICSCR_MSIRANGE;
    tmpreg |= msi_range;
    RCC->ICSCR = tmpreg;

    tmpreg = RCC->CFGR;
    tmpreg &= ~RCC_CFGR_HPRE;
    tmpreg |= ahb_divider;
    tmpreg &= ~RCC_CFGR_SW;
    tmpreg |= RCC_CFGR_SW_MSI;
    RCC->CFGR = tmpreg;

    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != RCC_CFGR_SWS_MSI) {}

    if ((msi_range == RCC_ICSCR_MSIRANGE_1) || (msi_range == RCC_ICSCR_MSIRANGE_0)) {
        /* Low-power run is only allowed at MSI Range 0 and 1 */
        PWR->CR |= PWR_CR_LPSDSR | PWR_CR_LPRUN;
    } else {
        /* set Voltage Range 3 (1.2V) */
        PWR->CR |= (PWR_CR_VOS_1 | PWR_CR_VOS_0);
        while((PWR->CSR & PWR_CSR_VOSF) != 0) {}
    }

    /* Set latency = 0, disable prefetch and 64-bit access */
    FLASH->ACR &= ~FLASH_ACR_LATENCY;
    FLASH->ACR &= ~FLASH_ACR_PRFTEN;
    FLASH->ACR &= ~FLASH_ACR_ACC64;
    while (!(FLASH->SR & FLASH_SR_READY)) {}

    /* Disable high speed clock sources and PLL */
    tmpreg = RCC->CR;
    tmpreg &= ~(RCC_CR_HSION | RCC_CR_HSEON);
    tmpreg &= ~(RCC_CR_HSEBYP | RCC_CR_CSSON | RCC_CR_PLLON);
    RCC->CR = tmpreg;

    cpu_clock_global = 65536 * (1 << (msi_range >> 13));
}

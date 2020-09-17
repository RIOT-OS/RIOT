/*
 * Copyright (C) 2014 Freie Universität Berlin
 *               2017-2020 Inria
 *               2018 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32
 * @{
 *
 * @file
 * @brief       Implementation of STM32 clock configuration for L0 and L1 families
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include "cpu.h"
#include "stmclk.h"
#include "periph_conf.h"

#if defined(CPU_FAM_STM32L1)
#define REG_CIR     (RCC->CIR)
#else /* CPU_FAM_STM32L0 */
#define REG_CIR     (RCC->CICR)
#endif

/* configuration of flash access cycles */
#define CLOCK_FLASH_LATENCY         (FLASH_ACR_LATENCY)

/* Configure the prescalers */
#define CLOCK_AHB_DIV               (RCC_CFGR_HPRE_DIV1)  /* HCLK = SYSCLK */

#if CONFIG_CLOCK_APB1_DIV == 1
#define CLOCK_APB1_DIV              (RCC_CFGR_PPRE1_DIV1)
#elif CONFIG_CLOCK_APB1_DIV == 2
#define CLOCK_APB1_DIV              (RCC_CFGR_PPRE1_DIV2)
#elif CONFIG_CLOCK_APB1_DIV == 4
#define CLOCK_APB1_DIV              (RCC_CFGR_PPRE1_DIV4)
#elif CONFIG_CLOCK_APB1_DIV == 8
#define CLOCK_APB1_DIV              (RCC_CFGR_PPRE1_DIV8)
#elif CONFIG_CLOCK_APB1_DIV == 16
#define CLOCK_APB1_DIV              (RCC_CFGR_PPRE1_DIV16)
#endif

#if CONFIG_CLOCK_APB2_DIV == 1
#define CLOCK_APB2_DIV              (RCC_CFGR_PPRE2_DIV1)
#elif CONFIG_CLOCK_APB2_DIV == 2
#define CLOCK_APB2_DIV              (RCC_CFGR_PPRE2_DIV2)
#elif CONFIG_CLOCK_APB2_DIV == 4
#define CLOCK_APB2_DIV              (RCC_CFGR_PPRE2_DIV4)
#elif CONFIG_CLOCK_APB2_DIV == 8
#define CLOCK_APB2_DIV              (RCC_CFGR_PPRE2_DIV8)
#elif CONFIG_CLOCK_APB2_DIV == 16
#define CLOCK_APB2_DIV              (RCC_CFGR_PPRE2_DIV16)
#endif

/* Check the source to be used for the PLL */
#if IS_ACTIVE(CONFIG_BOARD_HAS_HSE)
#define CLOCK_PLL_SOURCE            (RCC_CFGR_PLLSRC_HSE)
#else /* Use HSI as PLL input */
#define CLOCK_PLL_SOURCE            (RCC_CFGR_PLLSRC_HSI)
#endif

#if CONFIG_CLOCK_PLL_DIV == 2
#define CLOCK_PLL_DIV               (RCC_CFGR_PLLDIV2)
#elif CONFIG_CLOCK_PLL_DIV == 3
#define CLOCK_PLL_DIV               (RCC_CFGR_PLLDIV3)
#elif CONFIG_CLOCK_PLL_DIV == 4
#define CLOCK_PLL_DIV               (RCC_CFGR_PLLDIV4)
#else
#error "Invalid PLL DIV value, only 2, 3, and 4 values are allowed."
#endif

#if CONFIG_CLOCK_PLL_MUL == 3
#define CLOCK_PLL_MUL               (RCC_CFGR_PLLMUL3)
#elif CONFIG_CLOCK_PLL_MUL == 4
#define CLOCK_PLL_MUL               (RCC_CFGR_PLLMUL4)
#elif CONFIG_CLOCK_PLL_MUL == 6
#define CLOCK_PLL_MUL               (RCC_CFGR_PLLMUL6)
#elif CONFIG_CLOCK_PLL_MUL == 8
#define CLOCK_PLL_MUL               (RCC_CFGR_PLLMUL8)
#elif CONFIG_CLOCK_PLL_MUL == 12
#define CLOCK_PLL_MUL               (RCC_CFGR_PLLMUL12)
#elif CONFIG_CLOCK_PLL_MUL == 16
#define CLOCK_PLL_MUL               (RCC_CFGR_PLLMUL16)
#elif CONFIG_CLOCK_PLL_MUL == 24
#define CLOCK_PLL_MUL               (RCC_CFGR_PLLMUL24)
#elif CONFIG_CLOCK_PLL_MUL == 32
#define CLOCK_PLL_MUL               (RCC_CFGR_PLLMUL32)
#elif CONFIG_CLOCK_PLL_MUL == 48
#define CLOCK_PLL_MUL               (RCC_CFGR_PLLMUL48)
#else
#error "Invalid PLL MUL value, only 3, 4, 6, 8, 12, 16, 24, 32 and 48 values are allowed."
#endif

#if CONFIG_CLOCK_MSI == 65536UL
#define CLOCK_MSIRANGE              (RCC_ICSCR_MSIRANGE_0)
#elif CONFIG_CLOCK_MSI == 131072UL
#define CLOCK_MSIRANGE              (RCC_ICSCR_MSIRANGE_1)
#elif CONFIG_CLOCK_MSI == 262144UL
#define CLOCK_MSIRANGE              (RCC_ICSCR_MSIRANGE_2)
#elif CONFIG_CLOCK_MSI == 524288UL
#define CLOCK_MSIRANGE              (RCC_ICSCR_MSIRANGE_3)
#elif CONFIG_CLOCK_MSI == KHZ(1048)
#define CLOCK_MSIRANGE              (RCC_ICSCR_MSIRANGE_4)
#elif CONFIG_CLOCK_MSI == KHZ(2097)
#define CLOCK_MSIRANGE              (RCC_ICSCR_MSIRANGE_5)
#elif CONFIG_CLOCK_MSI == KHZ(4194)
#define CLOCK_MSIRANGE              (RCC_ICSCR_MSIRANGE_6)
#else
#error "Invalid MSI clock value"
#endif

/* Check whether PLL must be enabled:
  - When PLLCLK is used as SYSCLK
  - When HWRNG feature is used (for the 48MHz clock)
*/
#if IS_ACTIVE(CONFIG_USE_CLOCK_PLL) || IS_USED(MODULE_PERIPH_HWRNG)
#define CLOCK_ENABLE_PLL            1
#else
#define CLOCK_ENABLE_PLL            0
#endif

/* Check whether HSE must be enabled:
  - When HSE is used as SYSCLK
  - When PLL is used as SYSCLK and the board provides HSE (since HSE will be
    used as PLL input clock)
*/
#if IS_ACTIVE(CONFIG_USE_CLOCK_HSE) || \
    (IS_ACTIVE(CONFIG_BOARD_HAS_HSE) && IS_ACTIVE(CONFIG_USE_CLOCK_PLL))
#define CLOCK_ENABLE_HSE            1
#else
#define CLOCK_ENABLE_HSE            0
#endif

/* Check whether HSI must be enabled:
  - When HSI is used as SYSCLK
  - When PLL is used as SYSCLK and the board doesn't provide HSE (since HSI will be
    used as PLL input clock)
*/
#if IS_ACTIVE(CONFIG_USE_CLOCK_HSI) || \
    (!IS_ACTIVE(CONFIG_BOARD_HAS_HSE) && IS_ACTIVE(CONFIG_USE_CLOCK_PLL))
#define CLOCK_ENABLE_HSI            1
#else
#define CLOCK_ENABLE_HSI            0
#endif

/* Check whether MSI must be enabled:
  - When MSI is used as SYSCLK
*/
#if IS_ACTIVE(CONFIG_USE_CLOCK_MSI)
#define CLOCK_ENABLE_MSI            1
#else
#define CLOCK_ENABLE_MSI            0
#endif

/**
 * @brief Configure the controllers clock system
 *
 * NOTE: currently there is no timeout for initialization of PLL and other locks
 *       -> when wrong values are chosen, the initialization could stall
 */
void stmclk_init_sysclk(void)
{
    /* disable any interrupts. Global interrupts could be enabled if this is
     * called from some kind of bootloader...  */
    unsigned is = irq_disable();
    /* Disable all interrupts */
    REG_CIR = 0x0;

    /* enable HSI clock for the duration of initialization */
    stmclk_enable_hsi();

    /* Reset the RCC clock configuration to the default reset state(for debug purpose) */
    /* Reset MSION, HSEON, CSSON and PLLON bits */
    RCC->CR &= ~(RCC_CR_MSION | RCC_CR_HSEON | RCC_CR_HSEBYP | RCC_CR_CSSON | RCC_CR_PLLON);

    /* use HSI as system clock while we do any further configuration and
    * configure the AHB and APB clock dividers as configured by the board */
    RCC->CFGR = (RCC_CFGR_SW_HSI | CLOCK_AHB_DIV | CLOCK_APB1_DIV | CLOCK_APB2_DIV);

#if defined(CPU_FAM_STM32L1)
    FLASH->ACR |= FLASH_ACR_ACC64;
#endif
    /* Enable Prefetch Buffer */
    FLASH->ACR |= FLASH_ACR_PRFTEN;
    /* Flash 1 wait state */
    FLASH->ACR |= CLOCK_FLASH_LATENCY;
    /* Select the Voltage Range 1 (1.8 V) */
    PWR->CR = PWR_CR_VOS_0;
    /* Wait Until the Voltage Regulator is ready */
    while((PWR->CSR & PWR_CSR_VOSF) != 0) {}

    /* Enable HSE if needed */
    if (IS_ACTIVE(CLOCK_ENABLE_HSE)) {
        RCC->CR |= (RCC_CR_HSEON);
        while (!(RCC->CR & RCC_CR_HSERDY)) {}
    }

    /* Enable MSI if needed */
    if (IS_ACTIVE(CLOCK_ENABLE_MSI)) {
        /* Configure MSI range and enable it */
        RCC->ICSCR |= CLOCK_MSIRANGE;
        RCC->CR |= (RCC_CR_MSION);
        while (!(RCC->CR & RCC_CR_MSIRDY)) {}
    }

    /* Enable PLL if needed */
    if (IS_ACTIVE(CLOCK_ENABLE_PLL)) {
        /* Configure PLL clock source and configure the different prescalers */
        RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLDIV | RCC_CFGR_PLLMUL);
        RCC->CFGR |= (CLOCK_PLL_SOURCE | CLOCK_PLL_DIV | CLOCK_PLL_MUL);
        /* Enable PLL */
        RCC->CR |= RCC_CR_PLLON;
        /* Wait till PLL is ready */
        while (!(RCC->CR & RCC_CR_PLLRDY)) {}
    }

    /* Disable HSI if it's unused */
    if (!IS_ACTIVE(CLOCK_ENABLE_HSI)) {
        RCC->CFGR &= ~(RCC_CFGR_SW);
    }

    /* Configure SYSCLK input source */
    if (IS_ACTIVE(CONFIG_USE_CLOCK_HSE)) {
        /* Select HSE as system clock and wait till it's used as system clock */
        RCC->CFGR |= RCC_CFGR_SW_HSE;
        while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSE) {}
    }
    else if (IS_ACTIVE(CONFIG_USE_CLOCK_MSI)) {
        /* Select MSI as system clock and wait till it's used as system clock */
        RCC->CFGR |= RCC_CFGR_SW_MSI;
        while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_MSI) {}
    }
    else if (IS_ACTIVE(CONFIG_USE_CLOCK_PLL)) {
        RCC->CFGR |= RCC_CFGR_SW_PLL;
        /* Select PLL as system clock and wait till it's used as system clock */
        while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {}
    }

    if (!IS_ACTIVE(CLOCK_ENABLE_HSI)) {
        /* Disable HSI only if not needed */
        stmclk_disable_hsi();
    }

    irq_restore(is);
}

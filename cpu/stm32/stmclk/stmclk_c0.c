/*
 * Copyright (C) 2024 BISSELL Homecare, Inc.
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
 * @brief       Implementation of STM32 clock configuration for the C0 family
 *
 * @author      Jason Parker <Jason.Parker@bissell.com>
 * @}
 */

#include "cpu.h"
#include "stmclk.h"
#include "periph_conf.h"
#include "periph/gpio.h"

#if defined(CPU_FAM_STM32C0)
#define RCC_CFGR_SW_HSI             (0)
#define RCC_CFGR_SW_HSE             (RCC_CFGR_SW_0)

#if CONFIG_CLOCK_HSISYS_DIV == 1
#define CLOCK_HSI_DIV               (0)
#elif CONFIG_CLOCK_HSISYS_DIV == 2
#define CLOCK_HSI_DIV               (RCC_CR_HSIDIV_0)
#elif CONFIG_CLOCK_HSISYS_DIV == 4
#define CLOCK_HSI_DIV               (RCC_CR_HSIDIV_1)
#elif CONFIG_CLOCK_HSISYS_DIV == 8
#define CLOCK_HSI_DIV               (RCC_CR_HSIDIV_1 | RCC_CR_HSIDIV_0)
#elif CONFIG_CLOCK_HSISYS_DIV == 16
#define CLOCK_HSI_DIV               (RCC_CR_HSIDIV_2)
#elif CONFIG_CLOCK_HSISYS_DIV == 32
#define CLOCK_HSI_DIV               (RCC_CR_HSIDIV_2 | RCC_CR_HSIDIV_0)
#elif CONFIG_CLOCK_HSISYS_DIV == 64
#define CLOCK_HSI_DIV               (RCC_CR_HSIDIV_2 | RCC_CR_HSIDIV_1)
#elif CONFIG_CLOCK_HSISYS_DIV == 128
#define CLOCK_HSI_DIV               (RCC_CR_HSIDIV_2 | RCC_CR_HSIDIV_1 | RCC_CR_HSIDIV_0)
#endif

#define CLOCK_AHB_DIV               (0)

#if CONFIG_CLOCK_APB1_DIV == 1
#define CLOCK_APB1_DIV              (0)
#elif CONFIG_CLOCK_APB1_DIV == 2
#define CLOCK_APB1_DIV              (RCC_CFGR_PPRE_2)
#elif CONFIG_CLOCK_APB1_DIV == 4
#define CLOCK_APB1_DIV              (RCC_CFGR_PPRE_2 | RCC_CFGR_PPRE_0)
#elif CONFIG_CLOCK_APB1_DIV == 8
#define CLOCK_APB1_DIV              (RCC_CFGR_PPRE_2 | RCC_CFGR_PPRE_1)
#elif CONFIG_CLOCK_APB1_DIV == 16
#define CLOCK_APB1_DIV              (RCC_CFGR_PPRE_2 | RCC_CFGR_PPRE_1 | RCC_CFGR_PPRE_0)
#endif
#endif /* CPU_FAM_STM32C0 */

/* Configure MCO */
#ifndef CONFIG_CLOCK_ENABLE_MCO
#define CONFIG_CLOCK_ENABLE_MCO     0   /* Don't enable MCO by default */
#endif

/* Configure the MCO clock source: options are HSE, HSI (default), LSE, LSI or SYSCLK*/
#define CONFIG_CLOCK_MCO_USE_PLLCLK 0   // PLL not supported in STM32C0

#ifndef CONFIG_CLOCK_MCO_USE_HSE
#define CONFIG_CLOCK_MCO_USE_HSE    0
#endif /* CONFIG_CLOCK_MCO_USE_HSE */

#ifndef CONFIG_CLOCK_MCO_USE_HSI
#define CONFIG_CLOCK_MCO_USE_HSI    1
#endif /* CONFIG_CLOCK_MCO_USE_HSI */

#ifndef CONFIG_CLOCK_MCO_USE_LSE
#define CONFIG_CLOCK_MCO_USE_LSE    0
#endif /* CONFIG_CLOCK_MCO_USE_LSE */

#ifndef CONFIG_CLOCK_MCO_USE_LSI
#define CONFIG_CLOCK_MCO_USE_LSI    0
#endif /* CONFIG_CLOCK_MCO_USE_LSI */

#ifndef CONFIG_CLOCK_MCO_USE_SYSCLK
#define CONFIG_CLOCK_MCO_USE_SYSCLK 0
#endif /* CONFIG_CLOCK_MCO_USE_SYSCLK */

#if IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSE) && \
    (IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSI) || IS_ACTIVE(CONFIG_CLOCK_MCO_USE_LSE) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO_USE_LSI) || IS_ACTIVE(CONFIG_CLOCK_MCO_USE_SYSCLK))
#error "Cannot use HSE as MCO clock source with other clocks"
#endif

#if IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSI) && \
    (IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSE) || IS_ACTIVE(CONFIG_CLOCK_MCO_USE_LSE) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO_USE_LSI) || IS_ACTIVE(CONFIG_CLOCK_MCO_USE_SYSCLK))
#error "Cannot use HSI as MCO clock source with other clocks"
#endif

#if IS_ACTIVE(CONFIG_CLOCK_MCO_USE_LSE) && \
    (IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSE) || IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSI) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO_USE_LSI) || IS_ACTIVE(CONFIG_CLOCK_MCO_USE_SYSCLK))
#error "Cannot use LSE as MCO clock source with other clocks"
#endif

#if IS_ACTIVE(CONFIG_CLOCK_MCO_USE_LSI) && \
    (IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSE) || IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSI) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO_USE_LSE) || IS_ACTIVE(CONFIG_CLOCK_MCO_USE_SYSCLK))
#error "Cannot use LSI as MCO clock source with other clocks"
#endif

#if IS_ACTIVE(CONFIG_CLOCK_MCO_USE_SYSCLK) && \
    (IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSE) || IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSI) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO_USE_LSE) || IS_ACTIVE(CONFIG_CLOCK_MCO_USE_LSI))
#error "Cannot use SYSCLK as MCO clock source with other clocks"
#endif

#if IS_ACTIVE(CONFIG_CLOCK_MCO_USE_SYSCLK)
#define CLOCK_MCO_SRC               (RCC_CFGR_MCOSEL_0)
#elif IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSI)
#define CLOCK_MCO_SRC               (RCC_CFGR_MCOSEL_1 | RCC_CFGR_MCOSEL_0)
#elif IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSE)
#define CLOCK_MCO_SRC               (RCC_CFGR_MCOSEL_2)
#elif IS_ACTIVE(CONFIG_CLOCK_MCO_USE_LSI)
#define CLOCK_MCO_SRC               (RCC_CFGR_MCOSEL_2 | RCC_CFGR_MCOSEL_1)
#elif IS_ACTIVE(CONFIG_CLOCK_MCO_USE_LSE)
#define CLOCK_MCO_SRC               (RCC_CFGR_MCOSEL_2 | RCC_CFGR_MCOSEL_1 | RCC_CFGR_MCOSEL_0)
#else
#error "Invalid MCO clock source selection"
#endif

/* Configure the MCO prescaler */
#ifndef CONFIG_CLOCK_MCO_PRE
#define CONFIG_CLOCK_MCO_PRE        (1)
#endif

/* Define MCO prescalers */
#if CONFIG_CLOCK_MCO_PRE == 1
#define CLOCK_MCO_PRE               (0)
#elif CONFIG_CLOCK_MCO_PRE == 2
#define CLOCK_MCO_PRE               (RCC_CFGR_MCOPRE_0)
#elif CONFIG_CLOCK_MCO_PRE == 4
#define CLOCK_MCO_PRE               (RCC_CFGR_MCOPRE_1)
#elif CONFIG_CLOCK_MCO_PRE == 8
#define CLOCK_MCO_PRE               (RCC_CFGR_MCOPRE_1 | RCC_CFGR_MCOPRE_0)
#elif CONFIG_CLOCK_MCO_PRE == 16
#define CLOCK_MCO_PRE               (RCC_CFGR_MCOPRE_2)
#elif CONFIG_CLOCK_MCO_PRE == 32
#define CLOCK_MCO_PRE               (RCC_CFGR_MCOPRE_2 | RCC_CFGR_MCOPRE_0)
#elif CONFIG_CLOCK_MCO_PRE == 64
#define CLOCK_MCO_PRE               (RCC_CFGR_MCOPRE_2 | RCC_CFGR_MCOPRE_1)
#elif CONFIG_CLOCK_MCO_PRE == 128
#define CLOCK_MCO_PRE               (RCC_CFGR_MCOPRE_2 | RCC_CFGR_MCOPRE_1 | RCC_CFGR_MCOPRE_0)
#else
#error "Invalid MCO prescaler"
#endif

/* PLL not supported in STM32C0 */
#define CLOCK_ENABLE_PLL            0

/* Check whether HSE is required:
  - When HSE is used as SYSCLK
  - When HSE is used as MCO clock source
*/
#if IS_ACTIVE(CONFIG_USE_CLOCK_HSE) || \
    (IS_ACTIVE(CONFIG_CLOCK_ENABLE_MCO) && IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSE))
#define CLOCK_ENABLE_HSE            1
#else
#define CLOCK_ENABLE_HSE            0
#endif

/* Check whether HSI is required:
  - When HSI is used as SYSCLK
  - When HSI is used as MCO clock source
*/
#if IS_ACTIVE(CONFIG_USE_CLOCK_HSI) || \
    (IS_ACTIVE(CONFIG_CLOCK_ENABLE_MCO) && IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSI))
#define CLOCK_ENABLE_HSI            1
#else
#define CLOCK_ENABLE_HSI            0
#endif

/* Check whether LSE must be enabled */
#if IS_ACTIVE(CONFIG_CLOCK_ENABLE_MCO) && IS_ACTIVE(CONFIG_CLOCK_MCO_USE_LSE)
#define CLOCK_ENABLE_LSE            1
#else
#define CLOCK_ENABLE_LSE            0
#endif

/* Check whether LSI must be enabled */
#if IS_ACTIVE(CONFIG_CLOCK_ENABLE_MCO) && IS_ACTIVE(CONFIG_CLOCK_MCO_USE_LSI)
#define CLOCK_ENABLE_LSI            1
#else
#define CLOCK_ENABLE_LSI            0
#endif

/** Determine the required flash wait states from the core clock frequency */
#if CLOCK_CORECLOCK >= 24000000
#define FLASH_WAITSTATES            (FLASH_ACR_LATENCY_0)   /* 1 wait states */
#else
#define FLASH_WAITSTATES            (0)                     /* 0 wait states */
#endif

void stmclk_init_sysclk(void)
{
    /* disable any interrupts. Global interrupts could be enabled if this is
     * called from some kind of bootloader...  */
    unsigned is = irq_disable();
    /* enable HSI clock for the duration of initialization */
    stmclk_enable_hsi();

    RCC->CIER = 0;

    /* use HSI as system clock while we do any further configuration and
     * configure the AHB and APB clock dividers as configured by the board */
    RCC->CFGR = (RCC_CFGR_SW_HSI | CLOCK_AHB_DIV | CLOCK_APB1_DIV);

    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI) {}

    /* we enable instruction cache, pre-fetch, and we set the required flash wait states */
    FLASH->ACR |= (FLASH_ACR_ICEN | FLASH_ACR_PRFTEN | FLASH_WAITSTATES);

    /* disable all active clocks except HSI -> resets the clk configuration */
    RCC->CR = RCC_CR_HSION;

    if (IS_ACTIVE(CONFIG_USE_CLOCK_HSI) && CONFIG_CLOCK_HSISYS_DIV != 1) {
        /* configure HSISYS divider */
        RCC->CR |= CLOCK_HSI_DIV;
        while (!(RCC->CR & RCC_CR_HSIRDY)) {}
    }

    /* Enable HSE if required */
    if (IS_ACTIVE(CLOCK_ENABLE_HSE)) {
        RCC->CR |= RCC_CR_HSEON;
        while (!(RCC->CR & RCC_CR_HSERDY)) {}
    }

    /* Configure SYSCLK */
    if (IS_ACTIVE(CONFIG_USE_CLOCK_HSE)) {
        RCC->CFGR = (RCC_CFGR_SW_HSE | CLOCK_AHB_DIV | CLOCK_APB1_DIV);

        RCC->CFGR |= RCC_CFGR_SW_HSE;
        while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSE) {}
    }

    if (!IS_ACTIVE(CLOCK_ENABLE_HSI)) {
        /* Disable HSI only if not used */
        stmclk_disable_hsi();
    }

    /* Enable the LSE if required for MCO
     * If available on the board, LSE might also be initialized for RTT/RTC
     * peripherals. For the monent, this initialization is done in the
     * corresponding peripheral drivers. */
    if (IS_ACTIVE(CLOCK_ENABLE_LSE)) {
        stmclk_dbp_unlock();
        RCC->CSR1 |= RCC_CSR1_LSEON;
        while (!(RCC->CSR1 & RCC_CSR1_LSERDY)) {}
        stmclk_dbp_lock();
    }

    /* Enable the LSI if required for MCO
     * If no LSE is available on the board, LSI might also be initialized for
     * RTT/RTC peripherals. For the monent, this initialization is done in the
     * corresponding peripheral drivers. */
    if (IS_ACTIVE(CLOCK_ENABLE_LSI)) {
        RCC->CSR2 |= RCC_CSR2_LSION;
        while (!(RCC->CSR2 & RCC_CSR2_LSIRDY)) {}
    }

    /* Configure MCO */
    if (IS_ACTIVE(CONFIG_CLOCK_ENABLE_MCO)) {
        /* As stated in the manual, it is highly recommended to change the MCO
           prescaler before enabling the MCO */
        RCC->CFGR |= CLOCK_MCO_PRE;
        RCC->CFGR |= CLOCK_MCO_SRC;

        /* Configure MCO pin (PA8 with AF0) */
        gpio_init(GPIO_PIN(PORT_A, 8), GPIO_OUT);
        gpio_init_af(GPIO_PIN(PORT_A, 8), GPIO_AF0);
    }

#if IS_USED(MODULE_PERIPH_HWRNG)
    /* HWRNG is clocked by HSI48 so enable this clock when the peripheral is used */
    RCC->CRRCR |= RCC_CRRCR_HSI48ON;
    while (!(RCC->CRRCR & RCC_CRRCR_HSI48RDY)) {}
#endif

    irq_restore(is);
}

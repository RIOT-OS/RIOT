/*
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2017 OTA keys S.A.
 * SPDX-FileCopyrightText: 2020 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_stm32
 * @{
 *
 * @file
 * @brief       Implementation of STM32 clock configuration for STM32 F0/F1/F3
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @}
 */

#include "cpu.h"
#include "stmclk.h"
#include "periph_conf.h"
#include "periph/gpio.h"

/* PLL configuration */
#if defined(CPU_FAM_STM32F1)
#if IS_ACTIVE(CONFIG_BOARD_HAS_HSE)
#define PLL_SRC                 (RCC_CFGR_PLLSRC) /* HSE */
#else
#define PLL_SRC                 (0) /* HSI / 2 */
#endif

/* RCC_CR_HSITRIM_4 is not defined on stm32f1 and corresponds to a value of 16.
   Use the same definition used for stm32f0/stm32f3 */
#define RCC_CR_HSITRIM_4        (0x10UL << RCC_CR_HSITRIM_Pos)
#define RCC_CFGR_PLLMUL         RCC_CFGR_PLLMULL
#define PLL_MUL                 ((CONFIG_CLOCK_PLL_MUL - 2) << RCC_CFGR_PLLMULL_Pos)
#else /* CPU_FAM_STM32F0 && CPU_FAM_STM32F3 */
#if CONFIG_BOARD_HAS_HSE
#define PLL_SRC                 (RCC_CFGR_PLLSRC_HSE_PREDIV | RCC_CFGR_PLLXTPRE_HSE_PREDIV_DIV1)
#else
#define PLL_SRC                 (RCC_CFGR_PLLSRC_HSI_DIV2)
#endif
#define PLL_MUL                 ((CONFIG_CLOCK_PLL_MUL - 2) << RCC_CFGR_PLLMUL_Pos)
#endif /* CPU_FAM_STM32F1 */

#define PLL_PREDIV              (CONFIG_CLOCK_PLL_PREDIV - 1)

#define CLOCK_AHB_DIV           (RCC_CFGR_HPRE_DIV1)

#if defined(CPU_FAM_STM32F0)
#if CONFIG_CLOCK_APB1_DIV == 1
#define CLOCK_APB1_DIV          (RCC_CFGR_PPRE_DIV1)
#elif CONFIG_CLOCK_APB1_DIV == 2
#define CLOCK_APB1_DIV          (RCC_CFGR_PPRE_DIV2)
#elif CONFIG_CLOCK_APB1_DIV == 4
#define CLOCK_APB1_DIV          (RCC_CFGR_PPRE_DIV4)
#elif CONFIG_CLOCK_APB1_DIV == 8
#define CLOCK_APB1_DIV          (RCC_CFGR_PPRE_DIV8)
#elif CONFIG_CLOCK_APB1_DIV == 16
#define CLOCK_APB1_DIV          (RCC_CFGR_PPRE_DIV16)
#else
#error "Invalid APB prescaler value (only 1, 2, 4, 8 and 16 allowed)"
#endif
#else
#if CONFIG_CLOCK_APB1_DIV == 1
#define CLOCK_APB1_DIV          (RCC_CFGR_PPRE1_DIV1)
#elif CONFIG_CLOCK_APB1_DIV == 2
#define CLOCK_APB1_DIV          (RCC_CFGR_PPRE1_DIV2)
#elif CONFIG_CLOCK_APB1_DIV == 4
#define CLOCK_APB1_DIV          (RCC_CFGR_PPRE1_DIV4)
#elif CONFIG_CLOCK_APB1_DIV == 8
#define CLOCK_APB1_DIV          (RCC_CFGR_PPRE1_DIV8)
#elif CONFIG_CLOCK_APB1_DIV == 16
#define CLOCK_APB1_DIV          (RCC_CFGR_PPRE1_DIV16)
#else
#error "Invalid APB1 prescaler value (only 1, 2, 4, 8 and 16 allowed)"
#endif

#if CONFIG_CLOCK_APB2_DIV == 1
#define CLOCK_APB2_DIV          (RCC_CFGR_PPRE2_DIV1)
#elif CONFIG_CLOCK_APB2_DIV == 2
#define CLOCK_APB2_DIV          (RCC_CFGR_PPRE2_DIV2)
#elif CONFIG_CLOCK_APB2_DIV == 4
#define CLOCK_APB2_DIV          (RCC_CFGR_PPRE2_DIV4)
#elif CONFIG_CLOCK_APB2_DIV == 8
#define CLOCK_APB2_DIV          (RCC_CFGR_PPRE2_DIV8)
#elif CONFIG_CLOCK_APB2_DIV == 16
#define CLOCK_APB2_DIV          (RCC_CFGR_PPRE2_DIV16)
#else
#error "Invalid APB2 prescaler value (only 1, 2, 4, 8 and 16 allowed)"
#endif
#endif

/* Configure MCO */
#ifndef CONFIG_CLOCK_ENABLE_MCO
#define CONFIG_CLOCK_ENABLE_MCO     0   /* Don't enable MCO by default */
#endif

/* Configure the MCO clock source: options are PLLCLK / 2 (default), HSE, HSI or SYSCLK*/
#ifndef CONFIG_CLOCK_MCO_USE_PLLCLK
#if IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSE) || IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSI) || \
    IS_ACTIVE(CONFIG_CLOCK_MCO_USE_SYSCLK)
#define CONFIG_CLOCK_MCO_USE_PLLCLK 0
#else
#define CONFIG_CLOCK_MCO_USE_PLLCLK 1   /* Use PLLCLK / 2 by default */
#endif
#endif /* CONFIG_CLOCK_MCO_USE_PLLCLK */

#ifndef CONFIG_CLOCK_MCO_USE_HSE
#define CONFIG_CLOCK_MCO_USE_HSE    0
#endif /* CONFIG_CLOCK_MCO_USE_HSE */

#ifndef CONFIG_CLOCK_MCO_USE_HSI
#define CONFIG_CLOCK_MCO_USE_HSI    0
#endif /* CONFIG_CLOCK_MCO_USE_HSI */

#ifndef CONFIG_CLOCK_MCO_USE_SYSCLK
#define CONFIG_CLOCK_MCO_USE_SYSCLK 0
#endif /* CONFIG_CLOCK_MCO_USE_SYSCLK */

#if IS_ACTIVE(CONFIG_CLOCK_MCO_USE_PLLCLK) && \
    (IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSE) || IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSI) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO_USE_SYSCLK))
#error "Cannot use PLLCLK as MCO clock source with other clocks"
#endif

#if IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSE) && \
    (IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSI) || IS_ACTIVE(CONFIG_CLOCK_MCO_USE_PLLCLK) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO_USE_SYSCLK))
#error "Cannot use HSE as MCO clock source with other clocks"
#endif

#if IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSI) && \
    (IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSE) || IS_ACTIVE(CONFIG_CLOCK_MCO_USE_PLLCLK) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO_USE_SYSCLK))
#error "Cannot use HSI as MCO clock source with other clocks"
#endif

#if IS_ACTIVE(CONFIG_CLOCK_MCO_USE_SYSCLK) && \
    (IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSE) || IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSI) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO_USE_PLLCLK))
#error "Cannot use SYSCLK as MCO clock source with other clocks"
#endif

/* Define MCO selection bitfields for F3, for compatibility reasons */
#if defined(CPU_FAM_STM32F3)
#define RCC_CFGR_MCOSEL_SYSCLK                  RCC_CFGR_MCO_SYSCLK
#define RCC_CFGR_MCOSEL_HSI                     RCC_CFGR_MCO_HSI
#define RCC_CFGR_MCOSEL_HSE                     RCC_CFGR_MCO_HSE
#define RCC_CFGR_MCOSEL_PLL_DIV2                RCC_CFGR_MCO_PLL
#endif

#if IS_ACTIVE(CONFIG_CLOCK_MCO_USE_SYSCLK)
#define CLOCK_MCO_SRC                           (RCC_CFGR_MCOSEL_SYSCLK)
#elif IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSI)
#define CLOCK_MCO_SRC                           (RCC_CFGR_MCOSEL_HSI)
#elif IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSE)
#define CLOCK_MCO_SRC                           (RCC_CFGR_MCOSEL_HSE)
#elif IS_ACTIVE(CONFIG_CLOCK_MCO_USE_PLLCLK)
#define CLOCK_MCO_SRC                           (RCC_CFGR_MCOSEL_PLL_DIV2)
#else
#error "Invalid MCO clock source selection"
#endif

/* Configure the MCO prescaler: valid values are 1, 2, 4, 8, 16, 32, 64, 128 */
#ifndef CONFIG_CLOCK_MCO_PRE
#define CONFIG_CLOCK_MCO_PRE                    (1)
#endif

/* MCO prescaler is only available on some STM32F0 and some STM32F3 lines */
#if defined(CPU_LINE_STM32F030x6) || defined(CPU_LINE_STM32F030xC) || \
    defined(CPU_LINE_STM32F031x6) || defined(CPU_LINE_STM32F038xx) || \
    defined(CPU_LINE_STM32F042x6) || defined(CPU_LINE_STM32F048xx) || \
    defined(CPU_LINE_STM32F070x6) || defined(CPU_LINE_STM32F070xB) || \
    defined(CPU_LINE_STM32F071xB) || defined(CPU_LINE_STM32F072xB) || \
    defined(CPU_LINE_STM32F078xx) || defined(CPU_LINE_STM32F091xC) || \
    defined(CPU_LINE_STM32F098xx) || \
    defined(CPU_LINE_STM32F301x8) || defined(CPU_LINE_STM32F302x8) || \
    defined(CPU_LINE_STM32F302xE) || defined(CPU_LINE_STM32F303x8) || \
    defined(CPU_LINE_STM32F303xE) || defined(CPU_LINE_STM32F318xx) || \
    defined(CPU_LINE_STM32F328xx) || defined(CPU_LINE_STM32F334x8) || \
    defined(CPU_LINE_STM32F398xx)
#if CONFIG_CLOCK_MCO_PRE == 1
#define CLOCK_MCO_PRE                           (RCC_CFGR_MCOPRE_DIV1)
#elif CONFIG_CLOCK_MCO_PRE == 2
#define CLOCK_MCO_PRE                           (RCC_CFGR_MCOPRE_DIV2)
#elif CONFIG_CLOCK_MCO_PRE == 4
#define CLOCK_MCO_PRE                           (RCC_CFGR_MCOPRE_DIV4)
#elif CONFIG_CLOCK_MCO_PRE == 8
#define CLOCK_MCO_PRE                           (RCC_CFGR_MCOPRE_DIV8)
#elif CONFIG_CLOCK_MCO_PRE == 16
#define CLOCK_MCO_PRE                           (RCC_CFGR_MCOPRE_DIV16)
#elif CONFIG_CLOCK_MCO_PRE == 32
#define CLOCK_MCO_PRE                           (RCC_CFGR_MCOPRE_DIV32)
#elif CONFIG_CLOCK_MCO_PRE == 64
#define CLOCK_MCO_PRE                           (RCC_CFGR_MCOPRE_DIV64)
#elif CONFIG_CLOCK_MCO_PRE == 128
#define CLOCK_MCO_PRE                           (RCC_CFGR_MCOPRE_DIV128)
#else
#error "Invalid MCO prescaler"
#endif
#else /* CPU_FAM_STM32F1 and other F0/F3 lines */
#if CONFIG_CLOCK_MCO_PRE != 1
#error "MCO prescaler is not configurable on this CPU"
#endif
#define CLOCK_MCO_PRE                           (0)
#endif

/* MCO prescaler is only available on STM32F0 and STM32F3 */
#if defined(CPU_FAM_STM32F1) && (CONFIG_CLOCK_MCO_PRE != 1)
#error "MCO prescaler is not available on STM32F1 (only value 1 is allowed)."
#endif

/* Make sure MCO output clock is less than 50MHz, can only be possible when
   using SYSCLK driven by PLL */
#if IS_ACTIVE(CONFIG_CLOCK_MCO_USE_SYSCLK) && IS_ACTIVE(CONFIG_USE_CLOCK_PLL) && \
    ((CLOCK_CORECLOCK / CONFIG_CLOCK_MCO_PRE) > MHZ(50))
#error "Cannot use a SYSCLK > 50MHz to output to MCO"
#endif

/* Check whether PLL is required */
/* Check whether PLL must be enabled:
  - When PLLCLK is used as SYSCLK
  - When MCO is enabled with PLLCLK as input clock
*/
#if IS_ACTIVE(CONFIG_USE_CLOCK_PLL) || \
    (IS_ACTIVE(CONFIG_CLOCK_ENABLE_MCO) && IS_ACTIVE(CONFIG_CLOCK_MCO_USE_PLLCLK))
#define CLOCK_ENABLE_PLL            1
#else
#define CLOCK_ENABLE_PLL            0
#endif

/* Check whether HSE is required:
  - When HSE is used as SYSCLK
  - When PLL is used as SYSCLK and the board provides HSE (since HSE will be
    used as PLL input clock)
  - When MCO is enabled with HSE as input clock
*/
#if IS_ACTIVE(CONFIG_USE_CLOCK_HSE) || \
    (IS_ACTIVE(CONFIG_BOARD_HAS_HSE) && IS_ACTIVE(CONFIG_USE_CLOCK_PLL)) || \
    (IS_ACTIVE(CONFIG_CLOCK_ENABLE_MCO) && IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSE))
#define CLOCK_ENABLE_HSE            1
#else
#define CLOCK_ENABLE_HSE            0
#endif

/* Check whether HSI is required:
  - When HSI is used as SYSCLK
  - When PLL is used as SYSCLK and the board doesn't provide HSE (since HSI will be
    used as PLL input clock)
  - When MCO is enabled with HSI as input clock
*/
#if IS_ACTIVE(CONFIG_USE_CLOCK_HSI) || \
    (!IS_ACTIVE(CONFIG_BOARD_HAS_HSE) && IS_ACTIVE(CONFIG_USE_CLOCK_PLL)) || \
    (IS_ACTIVE(CONFIG_CLOCK_ENABLE_MCO) && IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSI))
#define CLOCK_ENABLE_HSI            1
#else
#define CLOCK_ENABLE_HSI            0
#endif

/* Deduct the needed flash wait states from the core clock frequency */
#define FLASH_WAITSTATES        ((CLOCK_CORECLOCK - 1) / MHZ(24))
/* we enable I+D cashes, pre-fetch, and we set the actual number of
 * needed flash wait states */
#define FLASH_ACR_CONFIG        (FLASH_ACR_PRFTBE | FLASH_WAITSTATES)
/** @} */

void stmclk_init_sysclk(void)
{
    /* disable any interrupts. Global interrupts could be enabled if this is
     * called from some kind of bootloader...  */
    unsigned is = irq_disable();
    RCC->CIR = 0;

    /* enable HSI clock for the duration of initialization */
    stmclk_enable_hsi();

    /* use HSI as system clock while we do any further configuration and
     * configure the AHB and APB clock dividers as configure by the board */
#if defined(CPU_FAM_STM32F0)
    RCC->CFGR = (RCC_CFGR_SW_HSI | CLOCK_AHB_DIV | CLOCK_APB1_DIV);
#else /* CPU_FAM_STM32F1 && CPU_FAM_STM32F3 */
    RCC->CFGR = (RCC_CFGR_SW_HSI | CLOCK_AHB_DIV | CLOCK_APB1_DIV | CLOCK_APB2_DIV);
#endif /* CPU_FAM_STM32F0*/
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI) {}

    /* Flash config */
    FLASH->ACR = FLASH_ACR_CONFIG;

    /* disable all active clocks except HSI -> resets the clk configuration */
    RCC->CR = (RCC_CR_HSION | RCC_CR_HSITRIM_4);

    /* Enable and configure MCO if required */
    if (IS_ACTIVE(CONFIG_CLOCK_ENABLE_MCO)) {
        RCC->CFGR |= (CLOCK_MCO_SRC | CLOCK_MCO_PRE);

        /* Configure MCO pin (PA8/AF0) */
        gpio_init(GPIO_PIN(PORT_A, 8), GPIO_OUT);
#if defined(CPU_FAM_STM32F1)
        gpio_init_af(GPIO_PIN(PORT_A, 8), GPIO_AF_OUT_PP);
#else
        gpio_init_af(GPIO_PIN(PORT_A, 8), GPIO_AF0);
#endif
    }

    /* Enable HSE if it's used */
    if (IS_ACTIVE(CLOCK_ENABLE_HSE)) {
        RCC->CR |= (RCC_CR_HSEON);
        while (!(RCC->CR & RCC_CR_HSERDY)) {}
    }

    /* Enable PLL if it's used */
    if (IS_ACTIVE(CLOCK_ENABLE_PLL)) {
        RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMUL);
        /* set PLL configuration */
        RCC->CFGR |= (PLL_SRC | PLL_MUL);
        if (CONFIG_CLOCK_PLL_PREDIV == 2) {
            RCC->CFGR |= RCC_CFGR_PLLXTPRE; /* PREDIV == 2 */
        }
#if !defined(CPU_FAM_STM32F1)
        else if (CONFIG_CLOCK_PLL_PREDIV > 2) {
            RCC->CFGR2 = PLL_PREDIV;        /* PREDIV > 2 */
        }
#endif
        RCC->CR |= (RCC_CR_PLLON);
        while (!(RCC->CR & RCC_CR_PLLRDY)) {}
    }

    /* Configure SYSCLK */
    if (IS_ACTIVE(CONFIG_USE_CLOCK_HSE)) {
        RCC->CFGR |= RCC_CFGR_SW_HSE;
        while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSE) {}
    }
    else if (IS_ACTIVE(CONFIG_USE_CLOCK_PLL)) {
        RCC->CFGR |= RCC_CFGR_SW_PLL;
        while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {}
    }

    if (!IS_ACTIVE(CLOCK_ENABLE_HSI)) {
        /* Disable HSI only if not used */
        stmclk_disable_hsi();
    }

    irq_restore(is);
}

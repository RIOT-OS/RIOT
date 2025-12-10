/*
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2017 OTA keys S.A.
 * SPDX-FileCopyrightText: 2017 HAW-Hamburg
 * SPDX-FileCopyrightText: 2020 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_stm32
 * @{
 *
 * @file
 * @brief       Implementation of STM32 clock configuration for L5 family
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Nick van IJzendoorn <nijzendoorn@engineering-spirit.nl>
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @}
 */

#include "cpu.h"
#include "stmclk.h"
#include "periph_conf.h"

/* map CMSIS defines not present in wb and l5 families */
#define RCC_PLLCFGR_PLLSRC_HSE      (RCC_PLLCFGR_PLLSRC_1 | RCC_PLLCFGR_PLLSRC_0)
#define RCC_PLLCFGR_PLLSRC_HSI      (RCC_PLLCFGR_PLLSRC_1)
#define RCC_PLLCFGR_PLLSRC_MSI      (RCC_PLLCFGR_PLLSRC_0)
#define RCC_CFGR_SW_MSI             (0x00000000U)
#define RCC_CFGR_SW_HSI             (RCC_CFGR_SW_0)
#define RCC_CFGR_SW_HSE             (RCC_CFGR_SW_1)
#define RCC_CFGR_SW_PLL             (RCC_CFGR_SW_1 | RCC_CFGR_SW_0)
#define RCC_CFGR_SWS_MSI            (0x00000000U)
#define RCC_CFGR_SWS_HSI            (RCC_CFGR_SWS_0)
#define RCC_CFGR_SWS_HSE            (RCC_CFGR_SWS_1)
#define RCC_CFGR_SWS_PLL            (RCC_CFGR_SWS_1 | RCC_CFGR_SWS_0)

/* PLL configuration */
/* figure out which input to use */
#if IS_ACTIVE(CONFIG_CLOCK_PLL_SRC_MSI)
#define PLL_SRC                     RCC_PLLCFGR_PLLSRC_MSI
#elif IS_ACTIVE(CONFIG_CLOCK_PLL_SRC_HSE) && IS_ACTIVE(CONFIG_BOARD_HAS_HSE)
#define PLL_SRC                     RCC_PLLCFGR_PLLSRC_HSE
#elif IS_ACTIVE(CONFIG_CLOCK_PLL_SRC_HSI)
#define PLL_SRC                     RCC_PLLCFGR_PLLSRC_HSI
#else
#define PLL_SRC                     0
#endif

/* check configuration and get the corresponding bitfields */
#if (CONFIG_CLOCK_PLL_M < 1 || CONFIG_CLOCK_PLL_M > 16)
#error "PLL configuration: PLL M value is out of range"
#endif
#define PLL_M                       ((CONFIG_CLOCK_PLL_M - 1) << RCC_PLLCFGR_PLLM_Pos)

#if (CONFIG_CLOCK_PLL_N < 8 || CONFIG_CLOCK_PLL_N > 86)
#error "PLL configuration: PLL N value is out of range"
#endif
#define PLL_N                       (CONFIG_CLOCK_PLL_N << RCC_PLLCFGR_PLLN_Pos)

#if (CONFIG_CLOCK_PLL_R == 2)
#define PLL_R                       (0)
#elif (CONFIG_CLOCK_PLL_R == 4)
#define PLL_R                       (RCC_PLLCFGR_PLLR_0)
#elif (CONFIG_CLOCK_PLL_R == 6)
#define PLL_R                       (RCC_PLLCFGR_PLLR_1)
#elif (CONFIG_CLOCK_PLL_R == 8)
#define PLL_R                       (RCC_PLLCFGR_PLLR_1 | RCC_PLLCFGR_PLLR_0)
#else
#error "PLL configuration: PLL R value is invalid"
#endif

#if (CONFIG_CLOCK_PLL_Q == 2)
#define PLL_Q                       (0)
#elif (CONFIG_CLOCK_PLL_Q == 4)
#define PLL_Q                       (RCC_PLLCFGR_PLLQ_0)
#elif (CONFIG_CLOCK_PLL_Q == 6)
#define PLL_Q                       (RCC_PLLCFGR_PLLQ_1)
#elif (CONFIG_CLOCK_PLL_Q == 8)
#define PLL_Q                       (RCC_PLLCFGR_PLLQ_0 | RCC_PLLCFGR_PLLQ_1)
#else
#error "PLL configuration: PLL Q value is invalid"
#endif

/* Define MSI range bitfields */
#if CONFIG_CLOCK_MSI == KHZ(100)
#define CLOCK_MSIRANGE              (0)
#elif CONFIG_CLOCK_MSI == KHZ(200)
#define CLOCK_MSIRANGE              (RCC_CR_MSIRANGE_0)
#elif CONFIG_CLOCK_MSI == KHZ(400)
#define CLOCK_MSIRANGE              (RCC_CR_MSIRANGE_1)
#elif CONFIG_CLOCK_MSI == KHZ(800)
#define CLOCK_MSIRANGE              (RCC_CR_MSIRANGE_1 | RCC_CR_MSIRANGE_0)
#elif CONFIG_CLOCK_MSI == MHZ(1)
#define CLOCK_MSIRANGE              (RCC_CR_MSIRANGE_2)
#elif CONFIG_CLOCK_MSI == MHZ(2)
#define CLOCK_MSIRANGE              (RCC_CR_MSIRANGE_2 | RCC_CR_MSIRANGE_0)
#elif CONFIG_CLOCK_MSI == MHZ(4)
#define CLOCK_MSIRANGE              (RCC_CR_MSIRANGE_2 | RCC_CR_MSIRANGE_1)
#elif CONFIG_CLOCK_MSI == MHZ(8)
#define CLOCK_MSIRANGE              (RCC_CR_MSIRANGE_2 | RCC_CR_MSIRANGE_1 | RCC_CR_MSIRANGE_0)
#elif CONFIG_CLOCK_MSI == MHZ(16)
#define CLOCK_MSIRANGE              (RCC_CR_MSIRANGE_3)
#elif CONFIG_CLOCK_MSI == MHZ(24)
#define CLOCK_MSIRANGE              (RCC_CR_MSIRANGE_3 | RCC_CR_MSIRANGE_0)
#elif CONFIG_CLOCK_MSI == MHZ(32)
#define CLOCK_MSIRANGE              (RCC_CR_MSIRANGE_3 | RCC_CR_MSIRANGE_1)
#elif CONFIG_CLOCK_MSI == MHZ(48)
#define CLOCK_MSIRANGE              (RCC_CR_MSIRANGE_3 | RCC_CR_MSIRANGE_1 | RCC_CR_MSIRANGE_0)
#else
#error "Invalid MSI clock"
#endif

/* Configure 48MHz clock source */
#define CLOCK_PLLQ                  ((CLOCK_PLL_SRC / CONFIG_CLOCK_PLL_M) * CONFIG_CLOCK_PLL_N) / CONFIG_CLOCK_PLL_Q

#if CLOCK_PLLQ == MHZ(48)
#define CLOCK48MHZ_USE_PLLQ         1
#elif CONFIG_CLOCK_MSI == MHZ(48)
#define CLOCK48MHZ_USE_MSI          1
#else
#define CLOCK48MHZ_USE_PLLQ         0
#define CLOCK48MHZ_USE_MSI          0
#endif

#if IS_ACTIVE(CLOCK48MHZ_USE_PLLQ)
#define CLOCK48MHZ_SELECT           (RCC_CCIPR1_CLK48MSEL_1)
#elif IS_ACTIVE(CLOCK48MHZ_USE_MSI)
#define CLOCK48MHZ_SELECT           (RCC_CCIPR1_CLK48MSEL_1 | RCC_CCIPR1_CLK48MSEL_0)
#else
#define CLOCK48MHZ_SELECT           (0)
#endif

/* Configure the AHB and APB buses prescalers */
#define CLOCK_AHB_DIV               (0)

#if CONFIG_CLOCK_APB1_DIV == 1
#define CLOCK_APB1_DIV              (0)
#elif CONFIG_CLOCK_APB1_DIV == 2
#define CLOCK_APB1_DIV              (RCC_CFGR_PPRE1_2)
#elif CONFIG_CLOCK_APB1_DIV == 4
#define CLOCK_APB1_DIV              (RCC_CFGR_PPRE1_2 | RCC_CFGR_PPRE1_0)
#elif CONFIG_CLOCK_APB1_DIV == 8
#define CLOCK_APB1_DIV              (RCC_CFGR_PPRE1_2 | RCC_CFGR_PPRE1_1)
#elif CONFIG_CLOCK_APB1_DIV == 16
#define CLOCK_APB1_DIV              (RCC_CFGR_PPRE1_2 | RCC_CFGR_PPRE1_1 | RCC_CFGR_PPRE1_0)
#endif

#if CONFIG_CLOCK_APB2_DIV == 1
#define CLOCK_APB2_DIV              (0)
#elif CONFIG_CLOCK_APB2_DIV == 2
#define CLOCK_APB2_DIV              (RCC_CFGR_PPRE2_2)
#elif CONFIG_CLOCK_APB2_DIV == 4
#define CLOCK_APB2_DIV              (RCC_CFGR_PPRE2_2 | RCC_CFGR_PPRE2_0)
#elif CONFIG_CLOCK_APB2_DIV == 8
#define CLOCK_APB2_DIV              (RCC_CFGR_PPRE2_2 | RCC_CFGR_PPRE2_1)
#elif CONFIG_CLOCK_APB2_DIV == 16
#define CLOCK_APB2_DIV              (RCC_CFGR_PPRE2_2 | RCC_CFGR_PPRE2_1 | RCC_CFGR_PPRE2_0)
#endif

/* Only periph_hwrng requires 48MHz for the moment */
#if IS_USED(MODULE_PERIPH_HWRNG)
#if !IS_ACTIVE(CLOCK48MHZ_USE_PLLQ) && !IS_ACTIVE(CLOCK48MHZ_USE_MSI)
#error "No 48MHz clock source available, HWRNG cannot work"
#endif
#define CLOCK_ENABLE_48MHZ          1
#else
#define CLOCK_ENABLE_48MHZ          0
#endif

/* Check if PLL is required
  - When used as system clock
  - When PLLQ is used as 48MHz clock source
*/
#if IS_ACTIVE(CONFIG_USE_CLOCK_PLL) || \
    (IS_ACTIVE(CLOCK_ENABLE_48MHZ) && IS_ACTIVE(CLOCK48MHZ_USE_PLLQ))
#define CLOCK_ENABLE_PLL            1
#else
#define CLOCK_ENABLE_PLL            0
#endif

/* Check if HSE is required:
  - When used as system clock
  - When used as PLL input clock
*/
#if IS_ACTIVE(CONFIG_USE_CLOCK_HSE) || \
    (IS_ACTIVE(CLOCK_ENABLE_PLL) && IS_ACTIVE(CONFIG_CLOCK_PLL_SRC_HSE))
#define CLOCK_ENABLE_HSE            1
#else
#define CLOCK_ENABLE_HSE            0
#endif

/* HSE cannot be enabled if not provided by the board */
#if IS_ACTIVE(CLOCK_ENABLE_HSE) && !IS_ACTIVE(CONFIG_BOARD_HAS_HSE)
#error "HSE is required by the clock configuration but is not provided by the board."
#endif

/* Check if HSI is required:
  - When used as system clock
  - When used as PLL input clock
*/
#if IS_ACTIVE(CONFIG_USE_CLOCK_HSI) || \
    (IS_ACTIVE(CLOCK_ENABLE_PLL) && IS_ACTIVE(CONFIG_CLOCK_PLL_SRC_HSI))
#define CLOCK_ENABLE_HSI            1
#else
#define CLOCK_ENABLE_HSI            0
#endif

/* Check if MSI is required
  - When used as system clock
  - When used as PLL input clock
*/
#if IS_ACTIVE(CONFIG_USE_CLOCK_MSI) || \
    (IS_ACTIVE(CLOCK_ENABLE_PLL) && IS_ACTIVE(CONFIG_CLOCK_PLL_SRC_MSI)) || \
    (IS_ACTIVE(CLOCK_ENABLE_48MHZ) && IS_ACTIVE(CLOCK48MHZ_USE_MSI))
#define CLOCK_ENABLE_MSI            1
#else
#define CLOCK_ENABLE_MSI            0
#endif

/* Deduct the needed flash wait states from the core clock frequency */
#if CLOCK_AHB <= MHZ(20)
#define FLASH_WAITSTATES        FLASH_ACR_LATENCY_0WS
#elif CLOCK_AHB <= MHZ(40)
#define FLASH_WAITSTATES        FLASH_ACR_LATENCY_1WS
#elif CLOCK_AHB <= MHZ(60)
#define FLASH_WAITSTATES        FLASH_ACR_LATENCY_2WS
#elif CLOCK_AHB <= MHZ(80)
#define FLASH_WAITSTATES        FLASH_ACR_LATENCY_3WS
#elif CLOCK_AHB <= MHZ(100)
#define FLASH_WAITSTATES        FLASH_ACR_LATENCY_4WS
#elif CLOCK_AHB <= MHZ(110)
#define FLASH_WAITSTATES        FLASH_ACR_LATENCY_5WS
#endif

void stmclk_init_sysclk(void)
{
    /* disable any interrupts. Global interrupts could be enabled if this is
     * called from some kind of bootloader...  */
    unsigned is = irq_disable();
    RCC->CIER = 0;

    /* enable HSI clock for the duration of initialization */
    stmclk_enable_hsi();

    /* use HSI as system clock while we do any further configuration and
     * configure the AHB and APB clock dividers as configured by the board */
    RCC->CFGR = (RCC_CFGR_SW_HSI | CLOCK_AHB_DIV | CLOCK_APB1_DIV | CLOCK_APB2_DIV);
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI) {}

    /* configure flash wait states */
    FLASH->ACR = FLASH_WAITSTATES;

    /* disable all active clocks except HSI -> resets the clk configuration */
    RCC->CR = RCC_CR_HSION;

    /* Enable the HSE clock only when it's used */
    if (IS_ACTIVE(CLOCK_ENABLE_HSE)) {
        RCC->CR |= (RCC_CR_HSEON);
        while (!(RCC->CR & RCC_CR_HSERDY)) {}
    }

    /* Enable the MSI clock only when it's used */
    if (IS_ACTIVE(CLOCK_ENABLE_MSI)) {
        RCC->CR |= (RCC_CR_MSION | CLOCK_MSIRANGE | RCC_CR_MSIRGSEL);
        while (!(RCC->CR & RCC_CR_MSIRDY)) {}
    }

    /* Enable the PLL clock only when it's used */
    if (IS_ACTIVE(CLOCK_ENABLE_PLL)) {
        if (IS_ACTIVE(CONFIG_CLOCK_PLL_SRC_MSI) && IS_ACTIVE(CONFIG_BOARD_HAS_LSE)) {
            /* configure the low speed clock domain */
            stmclk_enable_lfclk();
            /* now we can enable the MSI PLL mode to enhance accuracy of the MSI */
            RCC->CR |= RCC_CR_MSIPLLEN;
            while (!(RCC->CR & RCC_CR_MSIRDY)) {}
        }

        /* configure and start the PLL */
        RCC->PLLCFGR = (PLL_SRC | PLL_M | PLL_N | PLL_R | PLL_Q);

        if (IS_ACTIVE(CONFIG_USE_CLOCK_PLL)) {
            /* Enable PLLCLK if PLL is used as system clock */
            RCC->PLLCFGR |= RCC_PLLCFGR_PLLREN;
        }

        if (IS_ACTIVE(CLOCK48MHZ_USE_PLLQ)) {
            /* Enable PLLQ if PLL is used as 48MHz source clock */
            RCC->PLLCFGR |= RCC_PLLCFGR_PLLQEN;
        }

        RCC->CR |= (RCC_CR_PLLON);
        while (!(RCC->CR & RCC_CR_PLLRDY)) {}
    }

    /* Configure SYSCLK */
    if (!IS_ACTIVE(CONFIG_USE_CLOCK_HSI)) {
        RCC->CFGR &= ~RCC_CFGR_SW;
    }

    if (IS_ACTIVE(CONFIG_USE_CLOCK_HSE)) {
        /* Select HSE as system clock */
        RCC->CFGR |= RCC_CFGR_SW_HSE;
        while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSE) {}
    }
    else if (IS_ACTIVE(CONFIG_USE_CLOCK_MSI)) {
        /* Select MSI as system clock */
        RCC->CFGR |= RCC_CFGR_SW_MSI;
        while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_MSI) {}
    }
    else if (IS_ACTIVE(CONFIG_USE_CLOCK_PLL)) {
        if (CLOCK_AHB > MHZ(80)) {
            /* Divide HCLK by 2 before enabling the PLL */
            RCC->CFGR |= RCC_CFGR_HPRE_3;
        }

        /* Select main PLL as system clock */
        RCC->CFGR |= RCC_CFGR_SW_PLL;
        while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {}

        if (CLOCK_AHB > MHZ(80)) {
            /* Wait 1us before switching back to full speed */
            /* Use volatile to prevent the compiler from optimizing the loop */
            volatile uint8_t count = CLOCK_CORECLOCK / MHZ(1);
            while (count--) {}
            RCC->CFGR &= ~RCC_CFGR_HPRE_3;
        }
    }

    if (!IS_ACTIVE(CLOCK_ENABLE_HSI)) {
        /* Disable HSI only if not used */
        stmclk_disable_hsi();
    }

    if (IS_ACTIVE(CLOCK_ENABLE_48MHZ)) {
        /* configure the clock used for the 48MHz clock tree (USB, RNG) */
        RCC->CCIPR1 = CLOCK48MHZ_SELECT;
    }

    irq_restore(is);
}

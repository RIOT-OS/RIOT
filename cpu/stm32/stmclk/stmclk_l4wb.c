/*
 * Copyright (C) 2017 Freie Universität Berlin
 *               2017 OTA keys S.A.
 *               2017 HAW-Hamburg
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
 * @brief       Implementation of STM32 clock configuration for L4 and WB families
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Nick van IJzendoorn <nijzendoorn@engineering-spirit.nl>
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#include "cpu.h"
#include "stmclk.h"
#include "periph_conf.h"

/* map CMSIS defines not present in stm32wb55xx.h */
#if defined(CPU_FAM_STM32WB)
#define RCC_PLLCFGR_PLLSRC_HSE      (RCC_PLLCFGR_PLLSRC_0 | RCC_PLLCFGR_PLLSRC_1)
#define RCC_PLLCFGR_PLLSRC_MSI      (RCC_PLLCFGR_PLLSRC_0)
#define RCC_CFGR_SW_MSI             (0x00000000U)
#define RCC_CFGR_SW_HSI             (RCC_CFGR_SW_0)
#define RCC_CFGR_SW_HSE             (RCC_CFGR_SW_1)
#define RCC_CFGR_SW_PLL             (RCC_CFGR_SW_1 + RCC_CFGR_SW_0)
#define RCC_CFGR_SWS_MSI            (0x00000000U)
#define RCC_CFGR_SWS_HSI            (RCC_CFGR_SWS_0)
#define RCC_CFGR_SWS_HSE            (RCC_CFGR_SWS_1)
#define RCC_CFGR_SWS_PLL            (RCC_CFGR_SWS_1 + RCC_CFGR_SWS_0)
#endif

/**
 * @name    PLL configuration
 * @{
 */
/* figure out which input to use */
#if IS_ACTIVE(CONFIG_CLOCK_PLL_SRC_MSI)
#define PLL_SRC                     RCC_PLLCFGR_PLLSRC_MSI
#elif IS_ACTIVE(CONFIG_CLOCK_PLL_SRC_HSE) && IS_ACTIVE(CONFIG_BOARD_HAS_HSE)
#define PLL_SRC                     RCC_PLLCFGR_PLLSRC_HSE
#else
#define PLL_SRC                     RCC_PLLCFGR_PLLSRC_HSI
#endif

/**check configuration and get the corresponding bitfields */
#if (CONFIG_CLOCK_PLL_M < 1 || CONFIG_CLOCK_PLL_M > 8)
#error "PLL configuration: PLL M value is out of range"
#endif
#define PLL_M                       ((CONFIG_CLOCK_PLL_M - 1) << RCC_PLLCFGR_PLLM_Pos)

#if (CONFIG_CLOCK_PLL_N < 8 || CONFIG_CLOCK_PLL_N > 86)
#error "PLL configuration: PLL N value is out of range"
#endif
#define PLL_N                       (CONFIG_CLOCK_PLL_N << RCC_PLLCFGR_PLLN_Pos)

#if defined(CPU_FAM_STM32WB)
#if (CONFIG_CLOCK_PLL_R < 1 || CONFIG_CLOCK_PLL_R > 8)
#error "PLL configuration: PLL R value is invalid"
#else
#define PLL_R                       ((CONFIG_CLOCK_PLL_R - 1)<< RCC_PLLCFGR_PLLR_Pos)
#endif
#else
#if (CONFIG_CLOCK_PLL_R == 2)
#define PLL_R                       (0)
#elif (CONFIG_CLOCK_PLL_R == 4)
#define PLL_R                       (RCC_PLLCFGR_PLLR_0)
#elif (CONFIG_CLOCK_PLL_R == 6)
#define PLL_R                       (RCC_PLLCFGR_PLLR_1)
#elif (CONFIG_CLOCK_PLL_R == 8)
#define PLL_R                       (RCC_PLLCFGR_PLLR_0 | RCC_PLLCFGR_PLLR_1)
#else
#error "PLL configuration: PLL R value is invalid"
#endif
#endif
/** @} */

#if CONFIG_CLOCK_MSI == KHZ(100)
#define CLOCK_MSIRANGE              (RCC_CR_MSIRANGE_0)
#elif CONFIG_CLOCK_MSI == KHZ(200)
#define CLOCK_MSIRANGE              (RCC_CR_MSIRANGE_1)
#elif CONFIG_CLOCK_MSI == KHZ(400)
#define CLOCK_MSIRANGE              (RCC_CR_MSIRANGE_2)
#elif CONFIG_CLOCK_MSI == KHZ(800)
#define CLOCK_MSIRANGE              (RCC_CR_MSIRANGE_3)
#elif CONFIG_CLOCK_MSI == MHZ(1)
#define CLOCK_MSIRANGE              (RCC_CR_MSIRANGE_4)
#elif CONFIG_CLOCK_MSI == MHZ(2)
#define CLOCK_MSIRANGE              (RCC_CR_MSIRANGE_5)
#elif CONFIG_CLOCK_MSI == MHZ(4)
#define CLOCK_MSIRANGE              (RCC_CR_MSIRANGE_6)
#elif CONFIG_CLOCK_MSI == MHZ(8)
#define CLOCK_MSIRANGE              (RCC_CR_MSIRANGE_7)
#elif CONFIG_CLOCK_MSI == MHZ(16)
#define CLOCK_MSIRANGE              (RCC_CR_MSIRANGE_8)
#elif CONFIG_CLOCK_MSI == MHZ(24)
#define CLOCK_MSIRANGE              (RCC_CR_MSIRANGE_9)
#elif CONFIG_CLOCK_MSI == MHZ(32)
#define CLOCK_MSIRANGE              (RCC_CR_MSIRANGE_10)
#elif CONFIG_CLOCK_MSI == MHZ(48)
#define CLOCK_MSIRANGE              (RCC_CR_MSIRANGE_11)
#else
#error "Invalid MSI clock"
#endif

#if defined(CPU_FAM_STM32WB)
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
#else /* CPU_FAM_STM32L4 */
#define CLOCK_AHB_DIV               (RCC_CFGR_HPRE_DIV1)

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
#endif /* CPU_FAM_STM32WB */

/**
 * @name    Deduct the needed flash wait states from the core clock frequency
 * @{
 */
#if defined(CPU_FAM_STM32WB)
#if (CLOCK_AHB <= 64000000)
#define FLASH_WAITSTATES        ((CLOCK_AHB - 1) / 18000000U)
#else
#define FLASH_WAITSTATES        FLASH_ACR_LATENCY_3WS
#endif
#else
#define FLASH_WAITSTATES        ((CLOCK_AHB - 1) / 16000000U)
#endif
/** @} */


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

#if defined(CPU_FAM_STM32WB)
    /* Use HSE/2 for radios systems */
    RCC->EXTCFGR = (RCC_EXTCFGR_RFCSS | CLOCK_EXTAHB_DIV);
#endif
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI) {}

    /* we enable I+D cashes, pre-fetch, and we set the actual number of
     * needed flash wait states */
    FLASH->ACR = (FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN |
                  FLASH_WAITSTATES);

    /* disable all active clocks except HSI -> resets the clk configuration
     * Note: on STM32L4x5 & STM32L4x6 this disables the following:
             PLLSAI1, PLLSAI2, Main PLL (via PLLON),
             Clock security system (via CSSON), MSI clock PLL (via MSIPLLEN),
             HSE crystal oscillator bypass (via HSEBYP), HSE,
             HSI16 automatic start from Stop (via HSIASFS),
             HSI16 always enable for peripheral kernels (via HSIKERON).

             Additionally it configures the MSI clock range (MSIRANGE) to
             ~100 kHz and the MSI clock to be based on MSISRANGE in RCC_CSR
             (instead of MSIRANGE in the RCC_CR) */
    RCC->CR = (RCC_CR_HSION);

    /* Enable the HSE clock only when it's provided by the board and required:
        - Use HSE as system clock
        - Use HSE as PLL input clock
    */
    if (IS_ACTIVE(CONFIG_BOARD_HAS_HSE) &&
        (IS_ACTIVE(CONFIG_USE_CLOCK_HSE) || IS_ACTIVE(CONFIG_CLOCK_PLL_SRC_HSE))) {
        RCC->CR |= (RCC_CR_HSEON);
        while (!(RCC->CR & RCC_CR_HSERDY)) {}
    }

    if (IS_ACTIVE(CONFIG_USE_CLOCK_HSE)) {
        /* Select HSE as system clock and configure the different prescalers */
        RCC->CFGR &= ~RCC_CFGR_SW;
        RCC->CFGR |= RCC_CFGR_SW_HSE;
    }
    else if (IS_ACTIVE(CONFIG_USE_CLOCK_MSI)) {
#if defined(CPU_FAM_STM32WB)
        RCC->CR |= (CLOCK_MSIRANGE | RCC_CR_MSION);
#else
        RCC->CR |= (CLOCK_MSIRANGE | RCC_CR_MSION | RCC_CR_MSIRGSEL);
#endif
        while (!(RCC->CR & RCC_CR_MSIRDY)) {}

        if (CONFIG_CLOCK_MSI == MHZ(48)) {
            /* select the MSI clock for the 48MHz clock tree (USB, RNG) */
            RCC->CCIPR = (RCC_CCIPR_CLK48SEL_0 | RCC_CCIPR_CLK48SEL_1);
        }
        /* Select MSI as system clock and configure the different prescalers */
        RCC->CFGR = (RCC_CFGR_SW_MSI | CLOCK_AHB_DIV | CLOCK_APB1_DIV | CLOCK_APB2_DIV);
    }
    else if (IS_ACTIVE(CONFIG_USE_CLOCK_PLL)) {
        if (IS_ACTIVE(CONFIG_CLOCK_PLL_SRC_MSI)) {
            /* reset clock to MSI with 48MHz, disables all other clocks */
#if defined(CPU_FAM_STM32WB)
            RCC->CR |= (CLOCK_MSIRANGE | RCC_CR_MSION);
#else
            RCC->CR |= (CLOCK_MSIRANGE | RCC_CR_MSION | RCC_CR_MSIRGSEL);
#endif
            while (!(RCC->CR & RCC_CR_MSIRDY)) {}

            if (IS_ACTIVE(CONFIG_BOARD_HAS_LSE)) {
                /* configure the low speed clock domain */
                stmclk_enable_lfclk();
                /* now we can enable the MSI PLL mode to enhance accuracy of the MSI */
                RCC->CR |= RCC_CR_MSIPLLEN;
                while (!(RCC->CR & RCC_CR_MSIRDY)) {}
            }

            if (CONFIG_CLOCK_MSI == MHZ(48)) {
                /* select the MSI clock for the 48MHz clock tree (USB, RNG) */
                RCC->CCIPR = (RCC_CCIPR_CLK48SEL_0 | RCC_CCIPR_CLK48SEL_1);
            }
        }

        /* now we can safely configure and start the PLL */
        RCC->PLLCFGR = (PLL_SRC | PLL_M | PLL_N | PLL_R | RCC_PLLCFGR_PLLREN);
        RCC->CR |= (RCC_CR_PLLON);
        while (!(RCC->CR & RCC_CR_PLLRDY)) {}

        /* now that the PLL is running, we use it as system clock */
        RCC->CFGR |= RCC_CFGR_SW_PLL;
        while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {}
    }

    if (!IS_ACTIVE(CONFIG_USE_CLOCK_HSI) ||
        (IS_ACTIVE(CONFIG_USE_CLOCK_PLL) && !IS_ACTIVE(CONFIG_CLOCK_PLL_SRC_HSI))) {
        /* Disable HSI only if not used */
        stmclk_disable_hsi();
    }

    if (IS_USED(MODULE_PERIPH_RTT)) {
        /* Ensure LPTIM1 clock source (LSI or LSE) is correctly reset when initializing
           the clock, this is particularly useful after waking up from deep sleep */
        if (IS_ACTIVE(CONFIG_BOARD_HAS_LSE)) {
            RCC->CCIPR |= RCC_CCIPR_LPTIM1SEL_0 | RCC_CCIPR_LPTIM1SEL_1;
        }
        else {
            RCC->CCIPR |= RCC_CCIPR_LPTIM1SEL_0;
        }
    }

    irq_restore(is);
}

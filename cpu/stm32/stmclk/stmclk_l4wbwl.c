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
#include "periph/gpio.h"

/* map CMSIS defines not present in stm32wb55xx.h */
#if defined(CPU_FAM_STM32WB) || defined(CPU_FAM_STM32WL)
#define RCC_PLLCFGR_PLLSRC_HSE      (RCC_PLLCFGR_PLLSRC_0 | RCC_PLLCFGR_PLLSRC_1)
#define RCC_PLLCFGR_PLLSRC_HSI      (RCC_PLLCFGR_PLLSRC_1)
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

#if defined(CPU_FAM_STM32WB) || defined(CPU_FAM_STM32WL)
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

#if defined(CPU_FAM_STM32WB) || defined(CPU_FAM_STM32WL)
#if (CONFIG_CLOCK_PLL_Q < 1 || CONFIG_CLOCK_PLL_Q > 8)
#error "PLL configuration: PLL Q value is invalid"
#else
#define PLL_Q                       ((CONFIG_CLOCK_PLL_Q - 1) << RCC_PLLCFGR_PLLQ_Pos)
#endif
#else
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
#endif
/** @} */

/* Define MSI range bit fields */
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

/* Configure MCO */
#ifndef CONFIG_CLOCK_ENABLE_MCO
#define CONFIG_CLOCK_ENABLE_MCO     0   /* Don't enable MCO by default */
#endif

/* Configure the MCO clock source: options are PLLCLK (default), HSE, HSI, LSE, LSI or SYSCLK*/
#ifndef CONFIG_CLOCK_MCO_USE_PLLCLK
#if IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSE) || IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSI) || \
    IS_ACTIVE(CONFIG_CLOCK_MCO_USE_LSE) || IS_ACTIVE(CONFIG_CLOCK_MCO_USE_LSI) || \
    IS_ACTIVE(CONFIG_CLOCK_MCO_USE_MSI) || IS_ACTIVE(CONFIG_CLOCK_MCO_USE_SYSCLK)
#define CONFIG_CLOCK_MCO_USE_PLLCLK 0
#else
#define CONFIG_CLOCK_MCO_USE_PLLCLK 1   /* Use PLLCLK by default */
#endif
#endif /* CONFIG_CLOCK_MCO_USE_PLLCLK */

#ifndef CONFIG_CLOCK_MCO_USE_HSE
#define CONFIG_CLOCK_MCO_USE_HSE    0
#endif /* CONFIG_CLOCK_MCO_USE_HSE */

#ifndef CONFIG_CLOCK_MCO_USE_HSI
#define CONFIG_CLOCK_MCO_USE_HSI    0
#endif /* CONFIG_CLOCK_MCO_USE_HSI */

#ifndef CONFIG_CLOCK_MCO_USE_LSE
#define CONFIG_CLOCK_MCO_USE_LSE    0
#endif /* CONFIG_CLOCK_MCO_USE_LSE */

#ifndef CONFIG_CLOCK_MCO_USE_LSI
#define CONFIG_CLOCK_MCO_USE_LSI    0
#endif /* CONFIG_CLOCK_MCO_USE_LSI */

#ifndef CONFIG_CLOCK_MCO_USE_MSI
#define CONFIG_CLOCK_MCO_USE_MSI    0
#endif /* CONFIG_CLOCK_MCO_USE_MSI */

#ifndef CONFIG_CLOCK_MCO_USE_SYSCLK
#define CONFIG_CLOCK_MCO_USE_SYSCLK 0
#endif /* CONFIG_CLOCK_MCO_USE_SYSCLK */

#if IS_ACTIVE(CONFIG_CLOCK_MCO_USE_PLLCLK) && \
    (IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSE) || IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSI) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO_USE_LSE) || IS_ACTIVE(CONFIG_CLOCK_MCO_USE_LSI) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO_USE_MSI) || IS_ACTIVE(CONFIG_CLOCK_MCO_USE_SYSCLK))
#error "Cannot use PLLCLK as MCO clock source with other clocks"
#endif

#if IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSE) && \
    (IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSI) || IS_ACTIVE(CONFIG_CLOCK_MCO_USE_LSE) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO_USE_LSI) || IS_ACTIVE(CONFIG_CLOCK_MCO_USE_MSI) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO_USE_PLLCLK) || IS_ACTIVE(CONFIG_CLOCK_MCO_USE_SYSCLK))
#error "Cannot use HSE as MCO clock source with other clocks"
#endif

#if IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSI) && \
    (IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSE) || IS_ACTIVE(CONFIG_CLOCK_MCO_USE_LSE) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO_USE_LSI) || IS_ACTIVE(CONFIG_CLOCK_MCO_USE_MSI) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO_USE_PLLCLK) || IS_ACTIVE(CONFIG_CLOCK_MCO_USE_SYSCLK))
#error "Cannot use HSI as MCO clock source with other clocks"
#endif

#if IS_ACTIVE(CONFIG_CLOCK_MCO_USE_LSE) && \
    (IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSE) || IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSI) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO_USE_LSI) || IS_ACTIVE(CONFIG_CLOCK_MCO_USE_MSI) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO_USE_PLLCLK) || IS_ACTIVE(CONFIG_CLOCK_MCO_USE_SYSCLK))
#error "Cannot use LSE as MCO clock source with other clocks"
#endif

#if IS_ACTIVE(CONFIG_CLOCK_MCO_USE_LSI) && \
    (IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSE) || IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSI) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO_USE_LSE) || IS_ACTIVE(CONFIG_CLOCK_MCO_USE_MSI) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO_USE_PLLCLK) || IS_ACTIVE(CONFIG_CLOCK_MCO_USE_SYSCLK))
#error "Cannot use LSI as MCO clock source with other clocks"
#endif

#if IS_ACTIVE(CONFIG_CLOCK_MCO_USE_MSI) && \
    (IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSE) || IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSI) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO_USE_LSE) || IS_ACTIVE(CONFIG_CLOCK_MCO_USE_LSI) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO_USE_PLL) || IS_ACTIVE(CONFIG_CLOCK_MCO_USE_SYSCLK))
#error "Cannot use MSI as MCO clock source with other clocks"
#endif

#if IS_ACTIVE(CONFIG_CLOCK_MCO_USE_SYSCLK) && \
    (IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSE) || IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSI) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO_USE_LSE) || IS_ACTIVE(CONFIG_CLOCK_MCO_USE_LSI) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO_USE_MSI) || IS_ACTIVE(CONFIG_CLOCK_MCO_USE_PLLCLK))
#error "Cannot use SYSCLK as MCO clock source with other clocks"
#endif

#if IS_ACTIVE(CONFIG_CLOCK_MCO_USE_SYSCLK)
#define CLOCK_MCO_SRC                           (RCC_CFGR_MCOSEL_0)
#elif IS_ACTIVE(CONFIG_CLOCK_MCO_USE_MSI)
#define CLOCK_MCO_SRC                           (RCC_CFGR_MCOSEL_1)
#elif IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSI)
#define CLOCK_MCO_SRC                           (RCC_CFGR_MCOSEL_1 | RCC_CFGR_MCOSEL_0)
#elif IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSE)
#define CLOCK_MCO_SRC                           (RCC_CFGR_MCOSEL_2)
#elif IS_ACTIVE(CONFIG_CLOCK_MCO_USE_PLLCLK)
#define CLOCK_MCO_SRC                           (RCC_CFGR_MCOSEL_2 | RCC_CFGR_MCOSEL_0)
#elif IS_ACTIVE(CONFIG_CLOCK_MCO_USE_LSI)
#define CLOCK_MCO_SRC                           (RCC_CFGR_MCOSEL_2 | RCC_CFGR_MCOSEL_1)
#elif IS_ACTIVE(CONFIG_CLOCK_MCO_USE_LSE)
#define CLOCK_MCO_SRC                           (RCC_CFGR_MCOSEL_2 | RCC_CFGR_MCOSEL_1 | RCC_CFGR_MCOSEL_0)
#else
#error "Invalid MCO clock source selection"
#endif

/* Configure the MCO prescaler: valid values are 1, 2, 4, 8, 16 */
#ifndef CONFIG_CLOCK_MCO_PRE
#define CONFIG_CLOCK_MCO_PRE                    (1)
#endif

#if defined(CPU_FAM_STM32WB) || defined(CPU_FAM_STM32WL)
/* Define bitfields for MCO prescaler for compatibility with L4*/
#define RCC_CFGR_MCOPRE_DIV1                    (0)
#define RCC_CFGR_MCOPRE_DIV2                    (RCC_CFGR_MCOPRE_0)
#define RCC_CFGR_MCOPRE_DIV4                    (RCC_CFGR_MCOPRE_1)
#define RCC_CFGR_MCOPRE_DIV8                    (RCC_CFGR_MCOPRE_1 | RCC_CFGR_MCOPRE_0)
#define RCC_CFGR_MCOPRE_DIV16                   (RCC_CFGR_MCOPRE_2)
#endif /* CPU_FAM_STM32WB */

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
#else
#error "Invalid MCO prescaler"
#endif

/* Configure main and peripheral bus clock prescalers */
#if defined(CPU_FAM_STM32WB) || defined(CPU_FAM_STM32WL)
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

#if defined(CPU_FAM_STM32WL)
#define CLOCK48MHZ_SELECT           (0)
#elif IS_ACTIVE(CLOCK48MHZ_USE_PLLQ)
#define CLOCK48MHZ_SELECT           (RCC_CCIPR_CLK48SEL_1)
#elif IS_ACTIVE(CLOCK48MHZ_USE_MSI)
#define CLOCK48MHZ_SELECT           (RCC_CCIPR_CLK48SEL_1 | RCC_CCIPR_CLK48SEL_0)
#else
#define CLOCK48MHZ_SELECT           (0)
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
  - When PLLCLK is used as MCO clock source
*/
#if IS_ACTIVE(CONFIG_USE_CLOCK_PLL) || \
    (IS_ACTIVE(CLOCK_ENABLE_48MHZ) && IS_ACTIVE(CLOCK48MHZ_USE_PLLQ)) || \
    (IS_ACTIVE(CONFIG_CLOCK_ENABLE_MCO) && IS_ACTIVE(CONFIG_CLOCK_MCO_USE_PLLCLK))
#define CLOCK_ENABLE_PLL            1
#else
#define CLOCK_ENABLE_PLL            0
#endif

/* Check if HSE is required:
  - When used as system clock
  - When used as PLL input clock
*/
#if IS_ACTIVE(CONFIG_USE_CLOCK_HSE) || \
    (IS_ACTIVE(CLOCK_ENABLE_PLL) && IS_ACTIVE(CONFIG_CLOCK_PLL_SRC_HSE)) || \
    (IS_ACTIVE(CONFIG_CLOCK_ENABLE_MCO) && IS_ACTIVE(CONFIG_CLOCK_MCO_USE_HSE))
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
    (IS_ACTIVE(CLOCK_ENABLE_PLL) && IS_ACTIVE(CONFIG_CLOCK_PLL_SRC_HSI)) || \
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

/* Check if MSI is required
  - When used as system clock
  - When used as PLL input clock
  - When used as 48MHz clock source
*/
#if IS_ACTIVE(CONFIG_USE_CLOCK_MSI) || \
    (IS_ACTIVE(CLOCK_ENABLE_PLL) && IS_ACTIVE(CONFIG_CLOCK_PLL_SRC_MSI)) || \
    (IS_ACTIVE(CONFIG_CLOCK_ENABLE_MCO) && IS_ACTIVE(CONFIG_CLOCK_MCO_USE_MSI)) || \
    (IS_ACTIVE(CLOCK_ENABLE_48MHZ) && IS_ACTIVE(CLOCK48MHZ_USE_MSI))
#define CLOCK_ENABLE_MSI            1
#else
#define CLOCK_ENABLE_MSI            0
#endif

/**
 * @name    Deduct the needed flash wait states from the core clock frequency
 * @{
 */
#if defined(CPU_FAM_STM32WB) || defined(CPU_FAM_STM32WL)
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
    if (IS_ACTIVE(CLOCK_ENABLE_HSE)) {
        RCC->CR |= (RCC_CR_HSEON);
        while (!(RCC->CR & RCC_CR_HSERDY)) {}
    }

    if (IS_ACTIVE(CLOCK_ENABLE_MSI)) {
#if defined(CPU_FAM_STM32WB)
        RCC->CR |= (CLOCK_MSIRANGE | RCC_CR_MSION);
#else
        RCC->CR |= (CLOCK_MSIRANGE | RCC_CR_MSION | RCC_CR_MSIRGSEL);
#endif
        while (!(RCC->CR & RCC_CR_MSIRDY)) {}
    }

    if (IS_ACTIVE(CLOCK_ENABLE_PLL)) {
        if (IS_ACTIVE(CONFIG_CLOCK_PLL_SRC_MSI) && IS_ACTIVE(CONFIG_BOARD_HAS_LSE)) {
            /* configure the low speed clock domain */
            stmclk_enable_lfclk();
            /* now we can enable the MSI PLL mode to enhance accuracy of the MSI */
            RCC->CR |= RCC_CR_MSIPLLEN;
            while (!(RCC->CR & RCC_CR_MSIRDY)) {}
        }

        /* now we can safely configure and start the PLL */
        RCC->PLLCFGR = (PLL_SRC | PLL_M | PLL_N | PLL_R | PLL_Q);
        if (IS_ACTIVE(CONFIG_USE_CLOCK_PLL) ||
            (IS_ACTIVE(CONFIG_CLOCK_ENABLE_MCO) && IS_ACTIVE(CONFIG_CLOCK_MCO_USE_PLLCLK))) {
            /* Enable PLLCLK if PLL is used as system clock or when PLLCLK is
               used MCO clock source */
            RCC->PLLCFGR |= RCC_PLLCFGR_PLLREN;
        }

        if (IS_ACTIVE(CLOCK48MHZ_USE_PLLQ)) {
            /* Enable PLLQ if PLL is used as 48MHz source clock */
            RCC->PLLCFGR |= RCC_PLLCFGR_PLLQEN;
        }

        RCC->CR |= (RCC_CR_PLLON);
        while (!(RCC->CR & RCC_CR_PLLRDY)) {}
    }

    /* Disable the current SYSCLK source (HSI), only if not used */
    if (!IS_ACTIVE(CLOCK_ENABLE_HSI)) {
        RCC->CFGR &= ~RCC_CFGR_SW;
    }

    /* Configure the system clock (SYSCLK) */
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
#ifdef CPU_FAM_STM32L4
        if (CLOCK_AHB > MHZ(80)) {
            /* Divide HCLK by 2 before enabling the PLL */
            RCC->CFGR |= RCC_CFGR_HPRE_DIV2;
        }
#endif

        /* Select PLL as system clock */
        RCC->CFGR |= RCC_CFGR_SW_PLL;
        while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {}

#ifdef CPU_FAM_STM32L4
        if (CLOCK_AHB > MHZ(80)) {
            /* Wait 1us before switching back to full speed */
            /* Use volatile to prevent the compiler from optimizing the loop */
            volatile uint8_t count = CLOCK_CORECLOCK / MHZ(1);
            while (count--) {}
            RCC->CFGR &= ~RCC_CFGR_HPRE_DIV2;
        }
#endif
    }

    if (IS_ACTIVE(CLOCK_ENABLE_48MHZ)) {
        /* configure the clock used for the 48MHz clock tree (USB, RNG) */
        RCC->CCIPR = CLOCK48MHZ_SELECT;
    }

    if (!IS_ACTIVE(CLOCK_ENABLE_HSI)) {
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

    /* Enable the LSE if required for MCO.
     * If available on the board, LSE might also be initialized for RTT/RTC
     * peripherals. For the monent, this initialization is done in the
     * corresponding peripheral drivers. */
    if (IS_ACTIVE(CLOCK_ENABLE_LSE)) {
        stmclk_dbp_unlock();
        RCC->BDCR |= RCC_BDCR_LSEON;
        while (!(RCC->BDCR & RCC_BDCR_LSERDY)) {}
        stmclk_dbp_lock();
    }

    /* Enable the LSI if required for MCO
     * If no LSE is available on the board, LSI might also be initialized for
     * RTT/RTC peripherals. For the monent, this initialization is done in the
     * corresponding peripheral drivers. */
    if (IS_ACTIVE(CLOCK_ENABLE_LSI)) {
#if defined(CPU_FAM_STM32WB)
        RCC->CSR |= RCC_CSR_LSI1ON;
        while (!(RCC->CSR & RCC_CSR_LSI1RDY)) {}
#else
        RCC->CSR |= RCC_CSR_LSION;
        while (!(RCC->CSR & RCC_CSR_LSIRDY)) {}
#endif
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

    irq_restore(is);
}

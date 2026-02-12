/*
 * SPDX-FileCopyrightText: 2024 Prime Controls, Inc.(R)
 * SPDX-FileCopyrightText: 2025 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_stm32
 * @{
 *
 * @file
 * @brief       Implementation of STM32 clock configuration for H7 family
 *
 * @author      Joshua DeWeese <jdeweese@primecontrols.com>
 * @author      Jay R Vaghela <jay.vaghela@tuhh.de>
 * @}
 */

#include "cpu.h"
#include "stmclk.h"
#include "periph_conf.h"
#include "periph/gpio.h"

/* PLL1 configuration */
#if IS_ACTIVE(CONFIG_USE_HSE_PLL)
#  define PLL1_SRC                     RCC_PLLCKSELR_PLLSRC_HSE
#elif IS_ACTIVE(CONFIG_USE_CSI_PLL)
#  define PLL1_SRC                     RCC_PLLCKSELR_PLLSRC_CSI
#elif IS_ACTIVE(CONFIG_USE_HSI_PLL)
#  define PLL1_SRC                     RCC_PLLCKSELR_PLLSRC_HSI
#else
#  define PLL1_SRC                     0
#endif

/* Compute the bitfields for the PLL1 configuration. */
#ifndef CONFIG_CLOCK_PLL1_M
#  define PLL1_M                       (0)
#else
#  define PLL1_M                       ((CONFIG_CLOCK_PLL1_M) << RCC_PLLCKSELR_DIVM1_Pos)
#endif

#ifndef CONFIG_CLOCK_PLL1_N
#  define PLL1_N                       (0)
#else
#  define PLL1_N                       ((CONFIG_CLOCK_PLL1_N - 1) << RCC_PLL1DIVR_N1_Pos)
#endif

#ifndef CONFIG_CLOCK_PLL1_P
#  define PLL1_P                       (0)
#else
#  define PLL1_P                       ((CONFIG_CLOCK_PLL1_P - 1) << RCC_PLL1DIVR_P1_Pos)
#endif

#ifndef CONFIG_CLOCK_PLL1_Q
#  define PLL1_Q                       (0)
#else
#  define PLL1_Q                       ((CONFIG_CLOCK_PLL1_Q - 1) << RCC_PLL1DIVR_Q1_Pos)
#endif

#ifndef CONFIG_CLOCK_PLL1_R
#  define PLL1_R                       (0)
#else
#  define PLL1_R                       ((CONFIG_CLOCK_PLL1_R - 1) << RCC_PLL1DIVR_R1_Pos)
#endif

/* Select 48MHz clock source between PLL1_Q or PLL3_Q. This depends on
 * the PLL parameters and if not possible on CPU lines which can provide 48MHz
 * from HSI48.
 * Determine if PLL is required, even if not used as SYSCLK
 * This is the case when USB/SDMMC is used in application and PLL1_Q/PLL3_Q is
 * configured to output 48MHz. */
#if (IS_USED(MODULE_PERIPH_USBDEV_CLK) || IS_USED(MODULE_PERIPH_SDMMC_CLK)) && \
    (CLOCK_PLL1_Q_OUT == 48)
#  define CLOCK_REQUIRE_PLL1_Q          1
#else
#  define CLOCK_REQUIRE_PLL1_Q          0
#endif

#if (defined(CPU_LINE_STM32H753XX)) && (IS_USED(MODULE_PERIPH_USBDEV_CLK)) && \
    (!IS_ACTIVE(CLOCK_REQUIRE_PLL1_Q) && (CLOCK_PLL3_Q_OUT == 48))
#  define CLOCK_REQUIRE_PLL3_Q          1
#else
#  define CLOCK_REQUIRE_PLL3_Q          0
#endif

#if (defined(CPU_LINE_STM32H753XX)) && (IS_USED(MODULE_PERIPH_SDMMC_CLK)) && \
    (!IS_ACTIVE(CLOCK_REQUIRE_PLL1_Q))
#  define CLOCK_REQUIRE_PLL2_R          1
#else
#  define CLOCK_REQUIRE_PLL2_R          0
#endif

/* PLL1_Q & PLL3_Q for USB_CLK MUX can only be used for STM32H753ZI.
 * HSI48 is only enabled if no suitbale 48MHz clock source can be generated
 * with PLL1_Q / PLL3_Q for USBDEV OR RNG. */
#if (defined(CPU_LINE_STM32H753XX)) && (IS_USED(MODULE_PERIPH_USBDEV_CLK)) && \
    (!IS_ACTIVE(CLOCK_REQUIRE_PLL1_Q)) && (!IS_ACTIVE(CLOCK_REQUIRE_PLL3_Q))
#  define CLOCK_REQUIRE_HSI48           1
/* Disable HSI48 if USBDEV or RNG PERIPH is not used. */
#elif (!IS_USED(MODULE_PERIPH_USBDEV_CLK))
#  define CLOCK_REQUIRE_HSI48           0
#endif

#if (IS_USED(MODULE_PERIPH_USBDEV_CLK)) && !(IS_ACTIVE(CLOCK_REQUIRE_PLL1_Q) || \
     IS_ACTIVE(CLOCK_REQUIRE_PLL3_Q) || IS_ACTIVE(CLOCK_REQUIRE_HSI48))
#  error No suitable 48MHz found, USB will not work
#endif

/* If no suitable clock for SDMMC is found */
#if (IS_USED(MODULE_SDMMC_CLK)) && !(IS_ACTIVE(CLOCK_REQUIRE_PLL1_Q) || \
     IS_ACTIVE(CLOCK_REQUIRE_PLL2_R))
#  error No suitable 48MHz found, SDMMC will not work
#endif

/* PLL3 Configuration */
#if defined(RCC_PLLCKSELR_DIVM3_Pos)
#  define PLL3_M                       ((CONFIG_CLOCK_PLL3_M) << RCC_PLLCKSELR_DIVM3_Pos)
#else
#  define PLL3_M                       (0)
#endif
#if defined(RCC_PLL3DIVR_N3_Pos)
#  define PLL3_N                       ((CONFIG_CLOCK_PLL3_N - 1) << RCC_PLL3DIVR_N3_Pos)
#else
#  define PLL3_N                       (0)
#endif
#if defined(RCC_PLL3DIVR_P3_Pos)
#  define PLL3_P                       ((CONFIG_CLOCK_PLL3_P - 1) << RCC_PLL3DIVR_P3_Pos)
#else
#  define PLL3_P                       (0)
#endif
#if defined(RCC_PLL3DIVR_Q3_Pos)
#  define PLL3_Q                       ((CONFIG_CLOCK_PLL3_Q - 1) << RCC_PLL3DIVR_Q3_Pos)
#else
#  define PLL3_Q                       (0)
#endif
#if defined(RCC_PLL3DIVR_R3_Pos)
#  define PLL3_R                       ((CONFIG_CLOCK_PLL3_R - 1) << RCC_PLL3DIVR_R3_Pos)
#else
#  define PLL3_R                       (0)
#endif

/* PLL2 Configuration */
#if defined(RCC_PLLCKSELR_DIVM2_Pos)
#  define PLL2_M                       ((CONFIG_CLOCK_PLL2_M) << RCC_PLLCKSELR_DIVM2_Pos)
#else
#  define PLL2_M                       (0)
#endif
#if defined(RCC_PLL2DIVR_N2_Pos)
#  define PLL2_N                       ((CONFIG_CLOCK_PLL2_N - 1) << RCC_PLL2DIVR_N2_Pos)
#else
#  define PLL2_N                       (0)
#endif
#if defined(RCC_PLL2DIVR_P2_Pos)
#  define PLL2_P                       ((CONFIG_CLOCK_PLL2_P - 1) << RCC_PLL2DIVR_P2_Pos)
#else
#  define PLL2_P                       (0)
#endif
#if defined(RCC_PLL2DIVR_Q2_Pos)
#  define PLL2_Q                       ((CONFIG_CLOCK_PLL2_Q - 1) << RCC_PLL2DIVR_Q2_Pos)
#else
#  define PLL2_Q                       (0)
#endif
#if defined(RCC_PLL2DIVR_R2_Pos)
#  define PLL2_R                       ((CONFIG_CLOCK_PLL2_R - 1) << RCC_PLL2DIVR_R2_Pos)
#else
#  define PLL2_R                       (0)
#endif

/* Configure HLCK and PCLK prescalers. */
#if CONFIG_CLOCK_AHB_DIV  == 1
#  define CLOCK_AHB_DIV               (RCC_D1CFGR_HPRE_DIV1)
#elif CONFIG_CLOCK_AHB_DIV == 2
#  define CLOCK_AHB_DIV               (RCC_D1CFGR_HPRE_DIV2) /* Default AHB prescaler */
#elif CONFIG_CLOCK_AHB_DIV == 4
#  define CLOCK_AHB_DIV               (RCC_D1CFGR_HPRE_DIV4)
#elif CONFIG_CLOCK_AHB_DIV == 8
#  define CLOCK_AHB_DIV               (RCC_D1CFGR_HPRE_DIV8)
#elif CONFIG_CLOCK_AHB_DIV == 16
#  define CLOCK_AHB_DIV               (RCC_D1CFGR_HPRE_DIV16)
#elif CONFIG_CLOCK_AHB_DIV == 64
#  define CLOCK_AHB_DIV               (RCC_D1CFGR_HPRE_DIV64)
#elif CONFIG_CLOCK_AHB_DIV == 128
#  define CLOCK_AHB_DIV               (RCC_D1CFGR_HPRE_DIV128)
#elif CONFIG_CLOCK_AHB_DIV == 256
#  define CLOCK_AHB_DIV               (RCC_D1CFGR_HPRE_DIV256)
#elif CONFIG_CLOCK_AHB_DIV == 512
#  define CLOCK_AHB_DIV               (RCC_D1CFGR_HPRE_DIV512)
#else
#  error "Invalid CORECLOCK prescaler value (only 1, 2, 4, 16, 64, 128 and 512 allowed)"
#endif

/* Configure Sysclk */
#if CONFIG_CLOCK_CORECLOCK_DIV  == 1
#  define CLOCK_CORECLOCK_DIV                 (RCC_D1CFGR_D1CPRE_DIV1)
#elif CONFIG_CLOCK_CORECLOCK_DIV == 2
#  define CLOCK_CORECLOCK_DIV                 (RCC_D1CFGR_D1CPRE_DIV2)
#elif CONFIG_CLOCK_CORECLOCK_DIV == 4
#  define CLOCK_CORECLOCK_DIV                 (RCC_D1CFGR_D1CPRE_DIV4)
#elif CONFIG_CLOCK_CORECLOCK_DIV == 8
#  define CLOCK_CORECLOCK_DIV                 (RCC_D1CFGR_D1CPRE_DIV8)
#elif CONFIG_CLOCK_CORECLOCK_DIV == 16
#  define CLOCK_CORECLOCK_DIV                 (RCC_D1CFGR_D1CPRE_DIV16)
#elif CONFIG_CLOCK_CORECLOCK_DIV == 64
#  define CLOCK_CORECLOCK_DIV                 (RCC_D1CFGR_D1CPRE_DIV64)
#elif CONFIG_CLOCK_CORECLOCK_DIV == 128
#  define CLOCK_CORECLOCK_DIV                 (RCC_D1CFGR_D1CPRE_DIV128)
#elif CONFIG_CLOCK_CORECLOCK_DIV == 256
#  define CLOCK_CORECLOCK_DIV                 (RCC_D1CFGR_D1CPRE_DIV256)
#elif CONFIG_CLOCK_CORECLOCK_DIV == 512
#  define CLOCK_CORECLOCK_DIV                 (RCC_D1CFGR_D1CPRE_DIV512)
#else
#  error "Invalid CORECLOCK prescaler value (only 1, 2, 4, 16, 64, 128 and 512 allowed)"
#endif

#if CONFIG_CLOCK_APB1_DIV == 1
#  define CLOCK_APB1_DIV              (RCC_D2CFGR_D2PPRE1_DIV1)
#elif CONFIG_CLOCK_APB1_DIV == 2
#  define CLOCK_APB1_DIV              (RCC_D2CFGR_D2PPRE1_DIV2)
#elif CONFIG_CLOCK_APB1_DIV == 4
#  define CLOCK_APB1_DIV              (RCC_D2CFGR_D2PPRE1_DIV4)
#elif CONFIG_CLOCK_APB1_DIV == 8
#  define CLOCK_APB1_DIV              (RCC_D2CFGR_D2PPRE1_DIV8)
#elif CONFIG_CLOCK_APB1_DIV == 16
#  define CLOCK_APB1_DIV              (RCC_D2CFGR_D2PPRE1_DIV16)
#else
#  error "Invalid APB1 prescaler value (only 1, 2, 4, 8 and 16 allowed)"
#endif

#if CONFIG_CLOCK_APB2_DIV == 1
#  define CLOCK_APB2_DIV              (RCC_D2CFGR_D2PPRE2_DIV1)
#elif CONFIG_CLOCK_APB2_DIV == 2
#  define CLOCK_APB2_DIV              (RCC_D2CFGR_D2PPRE2_DIV2)
#elif CONFIG_CLOCK_APB2_DIV == 4
#  define CLOCK_APB2_DIV              (RCC_D2CFGR_D2PPRE2_DIV4)
#elif CONFIG_CLOCK_APB2_DIV == 8
#  define CLOCK_APB2_DIV              (RCC_D2CFGR_D2PPRE2_DIV8)
#elif CONFIG_CLOCK_APB2_DIV == 16
#  define CLOCK_APB2_DIV              (RCC_D2CFGR_D2PPRE2_DIV16)
#else
#  error "Invalid APB2 prescaler value (only 1, 2, 4, 8 and 16 allowed)"
#endif

#if CONFIG_CLOCK_APB3_DIV == 1
#  define CLOCK_APB3_DIV              (RCC_D1CFGR_D1PPRE_DIV1)
#elif CONFIG_CLOCK_APB3_DIV == 2
#  define CLOCK_APB3_DIV              (RCC_D1CFGR_D1PPRE_DIV2)
#elif CONFIG_CLOCK_APB3_DIV == 4
#  define CLOCK_APB3_DIV              (RCC_D1CFGR_D1PPRE_DIV4)
#elif CONFIG_CLOCK_APB3_DIV == 8
#  define CLOCK_APB3_DIV              (RCC_D1CFGR_D1PPRE_DIV8)
#elif CONFIG_CLOCK_APB3_DIV == 16
#  define CLOCK_APB3_DIV              (RCC_D1CFGR_D1PPRE_DIV16)
#else
#  error "Invalid APB3 prescaler value (only 1, 2, 4, 8 and 16 allowed)"
#endif

#if CONFIG_CLOCK_APB4_DIV == 1
#  define CLOCK_APB4_DIV              (RCC_D3CFGR_D3PPRE_DIV1)
#elif CONFIG_CLOCK_APB4_DIV == 2
#  define CLOCK_APB4_DIV              (RCC_D3CFGR_D3PPRE_DIV2)
#elif CONFIG_CLOCK_APB4_DIV == 4
#  define CLOCK_APB4_DIV              (RCC_D3CFGR_D3PPRE_DIV4)
#elif CONFIG_CLOCK_APB4_DIV == 8
#  define CLOCK_APB4_DIV              (RCC_D3CFGR_D3PPRE_DIV8)
#elif CONFIG_CLOCK_APB4_DIV == 16
#  define CLOCK_APB4_DIV              (RCC_D3CFGR_D3PPRE_DIV16)
#else
#  error "Invalid APB4 prescaler value (only 1, 2, 4, 8 and 16 allowed)"
#endif

/* Deduct the needed flash wait states from the core clock frequency. */
#define FLASH_WAITSTATES            (CLOCK_CORECLOCK / 30000000U)
/* We enable I+D cashes, pre-fetch, and we set the actual number of
 * needed flash wait states. */
#if defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F4)
#  define FLASH_ACR_CONFIG  (FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN | FLASH_WAITSTATES)
#elif defined(CPU_FAM_STM32F7)
#  define FLASH_ACR_CONFIG            (FLASH_ACR_ARTEN | FLASH_ACR_PRFTEN | FLASH_WAITSTATES)
#elif defined(CPU_FAM_STM32H7)
#  define FLASH_ACR_CONFIG            (0)
#endif

/* Default is not configure MCO1 */
#ifndef CONFIG_CLOCK_ENABLE_MCO1
#  define CONFIG_CLOCK_ENABLE_MCO1    0
#endif

#if !defined(RCC_CFGR_MCO1) && IS_ACTIVE(CONFIG_CLOCK_ENABLE_MCO1)
#  error "stmclk: no MCO1 on this device"
#endif

/* Configure the MCO1 clock source: options are PLL (default), HSE, LSE, HSI48 or HSI. */
#ifndef CONFIG_CLOCK_MCO1_USE_PLL1_Q
#  if IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_HSE) || IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_HSI) || \
      IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_LSE) || IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_HSI48)
#    define CONFIG_CLOCK_MCO1_USE_PLL1_Q   0
#  else
#    define CONFIG_CLOCK_MCO1_USE_PLL1_Q   1     /* Use PLL1_Q output by default */
#  endif
#endif /* CONFIG_CLOCK_MCO1_USE_PLL1 */

#ifndef CONFIG_CLOCK_MCO1_USE_HSE
#  define CONFIG_CLOCK_MCO1_USE_HSE     0
#endif /* CONFIG_CLOCK_MCO1_USE_HSE */

#ifndef CONFIG_CLOCK_MCO1_USE_HSI
#  define CONFIG_CLOCK_MCO1_USE_HSI     0
#endif /* CONFIG_CLOCK_MCO1_USE_HSI */

#ifndef CONFIG_CLOCK_MCO1_USE_LSE
#  define CONFIG_CLOCK_MCO1_USE_LSE     0
#endif /* CONFIG_CLOCK_MCO1_USE_HSI */

#ifndef CONFIG_CLOCK_MCO1_USE_HSI48
#  define CONFIG_CLOCK_MCO1_USE_HSI48   0
#endif /* CONFIG_CLOCK_MCO1_USE_HSI */

#if IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_PLL1_Q) && \
    (IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_HSE) || IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_HSI) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_LSE) || IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_HSI48))
#  error "Cannot use PLL1_Q as MCO1 clock source with other clock"
#endif

#if IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_HSE) && \
    (IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_PLL1_Q) || IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_HSI) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_LSE) || IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_HSI48))
#  error "Cannot use HSE as MCO1 clock source with other clock"
#endif

#if IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_HSI) && \
    (IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_HSE) || IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_PLL1_Q) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_LSE) || IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_HSI48))
#  error "Cannot use HSI as MCO1 clock source with other clock"
#endif

#if IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_LSE) && \
    (IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_HSE) || IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_PLL1_Q) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_HSI) || IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_HSI48))
#  error "Cannot use LSE as MCO1 clock source with other clock"
#endif

#if IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_HSI48) && \
    (IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_HSE) || IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_PLL1_Q) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_HSI) || IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_LSE))
#  error "Cannot use HSI48 as MCO1 clock source with other clock"
#endif

#if IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_PLL1_Q)
#  define CLOCK_MCO1_SRC                          (RCC_CFGR_MCO1_1 | RCC_CFGR_MCO1_0)
#elif IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_HSE)
#  define CLOCK_MCO1_SRC                          (RCC_CFGR_MCO1_1)
#elif IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_HSI)
#  define CLOCK_MCO1_SRC                          (0)
#elif IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_LSE)
#  define CLOCK_MCO1_SRC                          (RCC_CFGR_MCO1_0)
#elif IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_HSI48)
#  define CLOCK_MCO1_SRC                          (RCC_CFGR_MCO1_2)
#else
#  error "Invalid MCO1 clock source selection"
#endif

/* Configure the MCO1 prescaler: options are 1 to 5. */
#ifndef CONFIG_CLOCK_MCO1_PRE
#  define CONFIG_CLOCK_MCO1_PRE           (1)
#endif

#if (CONFIG_CLOCK_MCO1_PRE >= 1) && (CONFIG_CLOCK_MCO1_PRE <= 16)
#  define CLOCK_MCO1_PRE  (((CONFIG_CLOCK_MCO1_PRE - 1) & 0xF) << RCC_CFGR_MCO1PRE_Pos)
#else
#  error "Invalid MCO1 prescaler"
#endif

/* Default is not configure MCO2. */
#ifndef CONFIG_CLOCK_ENABLE_MCO2
#  define CONFIG_CLOCK_ENABLE_MCO2        0
#endif

#if !defined(RCC_CFGR_MCO2) && IS_ACTIVE(CONFIG_CLOCK_ENABLE_MCO2)
#  error "stmclk: no MCO2 on this device"
#endif

/* Configure the MCO1 clock source: options are PLL1_P (default),PLL2_P, SYSCLK, LSI, CSI or HSE. */
#ifndef CONFIG_CLOCK_MCO2_USE_PLL1_P
#  if IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_HSE) || IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_CSI) || \
      IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_LSI) || IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_PLL2_P) || \
      IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_SYSCLK)
#    define CONFIG_CLOCK_MCO2_USE_PLL1_P   0
#  else
#    define CONFIG_CLOCK_MCO2_USE_PLL1_P   1     /* Use PLL by default */
#  endif
#endif /* CONFIG_CLOCK_MCO2_USE_PLL1 */

#ifndef CONFIG_CLOCK_MCO2_USE_PLL2_Q
#  if IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_HSE) || IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_CSI) || \
      IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_LSI) || IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_PLL1_P) || \
      IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_SYSCLK)
#    define CONFIG_CLOCK_MCO2_USE_PLL2     0
#  else
#    define CONFIG_CLOCK_MCO2_USE_PLL2_P   1     /* Use PLL by default */
#  endif
#endif /* CONFIG_CLOCK_MCO2_USE_PLL2 */

#ifndef CONFIG_CLOCK_MCO2_USE_HSE
#  define CONFIG_CLOCK_MCO2_USE_HSE      0
#endif /* CONFIG_CLOCK_MCO2_USE_HSE */

#ifndef CONFIG_CLOCK_MCO2_USE_LSI
#  define CONFIG_CLOCK_MCO2_USE_LSI      0
#endif /* CONFIG_CLOCK_MCO2_USE_LSI */

#ifndef CONFIG_CLOCK_MCO2_USE_CSI
#  define CONFIG_CLOCK_MCO2_USE_CSI      0
#endif /* CONFIG_CLOCK_MCO2_USE_CSI */

#ifndef CONFIG_CLOCK_MCO2_USE_SYSCLK
#  define CONFIG_CLOCK_MCO2_USE_SYSCLK   0
#endif /* CONFIG_CLOCK_MCO2_USE_SYSCLK */

#if IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_PLL1_P) && \
    (IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_HSE) || IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_CSI) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_LSI) || IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_PLL2_P) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_SYSCLK))
#  error "Cannot use PLL1_P as MCO2 clock source with other clock"
#endif

#if IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_HSE) && \
    (IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_PLL1_P) || IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_CSI) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_LSI) || IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_PLL2_P) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_SYSCLK))
#  error "Cannot use HSE as MCO2 clock source with other clock"
#endif

#if IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_CSI) && \
    (IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_HSE) || IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_PLL1_P) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_LSI) || IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_PLL2_P) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_SYSCLK))
#  error "Cannot use CSI as MCO2 clock source with other clock"
#endif

#if IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_LSI) && \
    (IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_HSE) || IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_PLL1_P) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_CSI) || IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_PLL2_P) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_SYSCLK))
#  error "Cannot use LSI as MCO2 clock source with other clock"
#endif

#if IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_PLL2_P) && \
    (IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_HSE) || IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_PLL1) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_HSI) || IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_LSI) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_SYSCLK))
#  error "Cannot use PLL2_P as MCO2 clock source with other clock"
#endif

#if IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_SYSCLK) && \
    (IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_HSE) || IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_PLL1_P) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_HSI) || IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_LSI) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_PLL2_P))
#  error "Cannot use SYSCLK as MCO2 clock source with other clock"
#endif

#if IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_SYSCLK)
#  define CLOCK_MCO2_SRC                          (0)
#elif IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_PLL2_P)
#  define CLOCK_MCO2_SRC                          (RCC_CFGR_MCO2_0)
#elif IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_HSE)
#  define CLOCK_MCO2_SRC                          (RCC_CFGR_MCO2_1)
#elif IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_PLL1_P)
#  define CLOCK_MCO2_SRC                          (RCC_CFGR_MCO2_1 | RCC_CFGR_MCO2_0)
#elif IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_CSI)
#  define CLOCK_MCO2_SRC                          (RCC_CFGR_MCO2_2)
#elif IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_LSI)
#  define CLOCK_MCO2_SRC                          (RCC_CFGR_MCO2_2 | RCC_CFGR_MCO2_0)
#else
#  error "Invalid MCO2 clock source selection"
#endif

/* Configure the MCO1 prescaler: options are 1 to 5. */
#ifndef CONFIG_CLOCK_MCO2_PRE
#  define CONFIG_CLOCK_MCO2_PRE           (1)
#endif

#if (CONFIG_CLOCK_MCO2_PRE >= 1) && (CONFIG_CLOCK_MCO2_PRE <= 16)
#  define CLOCK_MCO2_PRE  (((CONFIG_CLOCK_MCO2_PRE - 1) & 0xF) << RCC_CFGR_MCO2PRE_Pos)
#else
#  error "Invalid MCO2 prescaler"
#endif

/* Check whether PLL1 must be enabled:
 *  - When PLL1 is used as SYSCLK.
 *  - When PLL1 is required (PLL1_P/PLL1_Q/PLL1R).
 *  - When PLL1_P is used as input source for MCO1 or MCO2.
 */
#if IS_ACTIVE(CONFIG_USE_HSI_PLL) || IS_ACTIVE(CONFIG_USE_HSE_PLL) || \
    IS_ACTIVE(CONFIG_USE_CSI_PLL) || (IS_ACTIVE(CONFIG_CLOCK_ENABLE_MCO2) && \
    IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_PLL1_P))
#  define CLOCK_ENABLE_PLL1_P               1
#else
#  define CLOCK_ENABLE_PLL1_P               0
#endif

#if IS_ACTIVE(CONFIG_USE_HSI_PLL) || IS_ACTIVE(CONFIG_USE_HSE_PLL) || \
    IS_ACTIVE(CONFIG_USE_CSI_PLL) || IS_ACTIVE(CLOCK_REQUIRE_PLL1_Q) || \
    (IS_ACTIVE(CONFIG_CLOCK_ENABLE_MCO1) && IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_PLL1_Q))
#  define CLOCK_ENABLE_PLL1_Q               1
#else
#  define CLOCK_ENABLE_PLL1_Q               0
#endif

/* Check whether PLL2 must be enabled:
 *  - When PLL2 is required (PLL2_P/PLL2_Q/PLL2_R).
 *  - When PLL2 is used as input source for MCO1 or MCO2.
 */
#if IS_ACTIVE(CONFIG_USE_HSI_PLL) || IS_ACTIVE(CONFIG_USE_HSE_PLL) || \
    IS_ACTIVE(CONFIG_USE_CSI_PLL) || (IS_ACTIVE(CONFIG_CLOCK_ENABLE_MCO2) && \
    IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_PLL2_P))
#  define CLOCK_ENABLE_PLL2_P                1
#else
#  define CLOCK_ENABLE_PLL2_P                0
#endif

#if IS_ACTIVE(CLOCK_REQUIRE_PLL2_R)
#  define CLOCK_ENABLE_PLL2_R                1
#else
#  define CLOCK_ENABLE_PLL2_R                0
#endif

/* Check whether PLL3 must be enabled:
 *  - When PLL3 is required. */
#if  IS_ACTIVE(CLOCK_REQUIRE_PLL3_Q)
#  define CLOCK_ENABLE_PLL3_Q                1
#else
#  define CLOCK_ENABLE_PLL3_Q                0
#endif

/* Check whether HSE must be enabled:
 *  - When HSE is used as SYSCLK.
 *  - When PLL1 is used as SYSCLK and the board provides HSE (since HSE will be
 *    used as PLL1 input clock).
 *  - When HSE is used input source for MCO1 or MCO2.
 */
#if IS_ACTIVE(CONFIG_USE_HSE_PLL) || IS_ACTIVE(CONFIG_USE_HSE_DIRECT) ||\
    (IS_ACTIVE(CONFIG_CLOCK_ENABLE_MCO1) && IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_HSE)) ||\
    (IS_ACTIVE(CONFIG_CLOCK_ENABLE_MCO2) && IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_HSE))
#  define CLOCK_ENABLE_HSE                1
#else
#  define CLOCK_ENABLE_HSE                0
#endif

/* Check whether HSI must be enabled:
 *  - When HSI is used as SYSCLK.
 *  - When PLL1 is used as SYSCLK and the board doesn't provide HSE (since HSI will be
 *   used as PLL1 input clock).
 *  - When HSI is used input source for MCO1.
*/
#if IS_ACTIVE(CONFIG_USE_HSI_DIRECT) || IS_ACTIVE(CONFIG_USE_HSI_PLL) ||\
    (IS_ACTIVE(CONFIG_CLOCK_ENABLE_MCO1) && IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_HSI))
#  define CLOCK_ENABLE_HSI                1
#else
#  define CLOCK_ENABLE_HSI                0
#endif

#if IS_ACTIVE(CONFIG_USE_CSI_DIRECT) || IS_ACTIVE(CONFIG_USE_CSI_PLL) ||\
    (IS_ACTIVE(CONFIG_CLOCK_ENABLE_MCO2) && IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_CSI))
#  define CLOCK_ENABLE_CSI                1
#else
#  define CLOCK_ENABLE_CSI                0
#endif

#if IS_ACTIVE(CONFIG_USE_LSI) ||\
    (IS_ACTIVE(CONFIG_CLOCK_ENABLE_MCO2) && IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_LSI))
#  define CLOCK_ENABLE_LSI                1
#else
#  define CLOCK_ENABLE_LSI                0
#endif

#if IS_ACTIVE(CONFIG_USE_LSE) ||\
    (IS_ACTIVE(CONFIG_CLOCK_ENABLE_MCO1) && IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_LSE))
#  define CLOCK_ENABLE_LSE                1
#else
#  define CLOCK_ENABLE_LSE                0
#endif

#if (IS_ACTIVE(CONFIG_CLOCK_ENABLE_MCO1) && IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_HSI48)) ||\
    IS_ACTIVE(CLOCK_REQUIRE_HSI48)
#  define CLOCK_ENABLE_HSI48              1
#else
#  define CLOCK_ENABLE_HSI48              0
#endif

#if ! ( (defined(CONFIG_USE_HSI_PLL) && (CONFIG_USE_HSI_PLL)) || \
    (defined(CONFIG_USE_HSE_PLL) && (CONFIG_USE_HSE_PLL)) || \
    (defined(CONFIG_USE_CSI_PLL) && (CONFIG_USE_CSI_PLL)) || \
    (defined(CONFIG_USE_HSI_DIRECT) && (CONFIG_USE_HSI_DIRECT)) || \
    (defined(CONFIG_USE_HSE_DIRECT) && (CONFIG_USE_HSE_DIRECT)) || \
    (defined(CONFIG_USE_CSI_DIRECT) && (CONFIG_USE_CSI_DIRECT)) )
#  error "No valid SYSCLK source selected: enable one of CONFIG_USE_* in your board config"
#endif

void stmclk_init_sysclk(void)
{
    /* Disable any interrupts. Global interrupts could be enabled if this is
     * called from some kind of bootloader...  */
    unsigned is = irq_disable();
    RCC->CR = 0;

    /* Enable HSI clock for the duration of initialization. */
    stmclk_enable_hsi();

    /* Use HSI as system clock while we do any further configuration and
     * configure the AHB and APB clock dividers as configure by the board. */
    RCC->D1CFGR = CLOCK_CORECLOCK_DIV | CLOCK_AHB_DIV | CLOCK_APB3_DIV;
    RCC->D2CFGR = CLOCK_APB1_DIV | CLOCK_APB2_DIV;
    RCC->D3CFGR = CLOCK_APB4_DIV;

    RCC->CFGR = (RCC_CFGR_SW_HSI);
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI) {}

    /* Flash config */
    FLASH->ACR = FLASH_ACR_LATENCY_7WS;

    /* Enable internal LDO power supply. */
    PWR->CR3 |= PWR_CR3_LDOEN;
    while (!(PWR->CSR1 & PWR_CSR1_ACTVOSRDY)) {}

    /* Configuring VOS0 mode for Core clock frequency > 480MHz */
    if (CLOCK_CORECLOCK > MHZ(400))
    {
        /* Set voltage scaling to scale 1 mode. */
        PWR->D3CR = (PWR->D3CR & ~PWR_D3CR_VOS_Msk) | (PWR_D3CR_VOS_0 | PWR_D3CR_VOS_1);
        while (!(PWR->D3CR & PWR_D3CR_VOSRDY)) {}
        /* Enable SYSCFG clock. */
        RCC->APB4ENR |= RCC_APB4ENR_SYSCFGEN;
        /* Enable Over-Drive to enter VOS0 mode. */
        SYSCFG->PWRCR |= SYSCFG_PWRCR_ODEN;
        while (!(PWR->D3CR & PWR_D3CR_VOSRDY)) {}
    }
    /* Configuring VOS0 mode for up to 400MHz frequency. */
    else if (CLOCK_CORECLOCK > MHZ(300) && CLOCK_CORECLOCK <= MHZ(400))
    {
        /* Set voltage scaling to scale 1 mode. */
        PWR->D3CR = (PWR->D3CR & ~PWR_D3CR_VOS_Msk) | (PWR_D3CR_VOS_0 | PWR_D3CR_VOS_1);
        while (!(PWR->D3CR & PWR_D3CR_VOSRDY)) {}
    }

    /* Configuring VOS1 mode for up to 180MHz frequency. */
    else if (CLOCK_CORECLOCK > MHZ(200) && CLOCK_CORECLOCK <= MHZ(300))
    {
        /* Set voltage scaling to scale 2 mode. */
        PWR->D3CR = (PWR->D3CR & ~PWR_D3CR_VOS_Msk) | (PWR_D3CR_VOS_1);
        while (!(PWR->D3CR & PWR_D3CR_VOSRDY)) {}
    }
    else /* Configuring VOS2 mode for up to 200MHz frequency. */
    {
        /* Set voltage scaling to scale 3 mode. */
        PWR->D3CR = (PWR->D3CR & ~PWR_D3CR_VOS_Msk) | (PWR_D3CR_VOS_0);
        while (!(PWR->D3CR & PWR_D3CR_VOSRDY)) {}
    }

    /* Disable all active clocks except HSI -> resets the clk configuration. */
    RCC->CR = (RCC_CR_HSION | RCC_CR_HSIDIV_1);

    if (IS_ACTIVE(CONFIG_CLOCK_ENABLE_MCO1))
    {
        RCC->CFGR |= CLOCK_MCO1_SRC | CLOCK_MCO1_PRE;
        /* Configure GPIO pin (PA8/AF0) */
        gpio_init(GPIO_PIN(PORT_A, 8), GPIO_OUT);
        gpio_init_af(GPIO_PIN(PORT_A, 8), GPIO_AF0);
    }
    else if (IS_ACTIVE(CONFIG_CLOCK_ENABLE_MCO2))
    {
        RCC->CFGR |= CLOCK_MCO2_SRC | CLOCK_MCO2_PRE;

        /* Configure GPIO pin (PC9/AF0) */
        gpio_init(GPIO_PIN(PORT_C, 9), GPIO_OUT);
        gpio_init_af(GPIO_PIN(PORT_C, 9), GPIO_AF0);
    }

    if (IS_ACTIVE(CLOCK_ENABLE_HSE)) /* Enable HSE if required. */
    {
        RCC->CR |= (RCC_CR_HSEON);
        while (!(RCC->CR & RCC_CR_HSERDY)) {}
    }
    if (IS_ACTIVE(CLOCK_ENABLE_LSE)) /* Enable LSE if required. */
    {
        RCC->BDCR |= RCC_BDCR_LSEON;
        while (!(RCC->BDCR & RCC_BDCR_LSERDY)) {}
    }
    if (IS_ACTIVE(CLOCK_ENABLE_CSI)) /* Enable CSI if required. */
    {
        RCC->CR |= RCC_CR_CSION;
        while (!(RCC->CR & RCC_CR_CSIRDY)) {}
    }
    if (IS_ACTIVE(CLOCK_ENABLE_LSI)) /* Enable LSI if required. */
    {
        RCC->CSR |= RCC_CSR_LSION;
        while (!(RCC->CSR & RCC_CSR_LSIRDY)) {}
    }
    if (IS_ACTIVE(CLOCK_ENABLE_HSI48)) /* Enable HSI48 if required. */
    {
        RCC->CR |= RCC_CR_HSI48ON;
        while (!(RCC->CR & RCC_CR_HSI48RDY)) {}
    }

    /* Configure SYSCLK (preprocessor selection: only the chosen branch is
     * compiled). Prefer explicit DIRECT selections from the generated
     * configuration over defaulted PLL macros, so DIRECT options are tested
     * first. */
    if (IS_ACTIVE(CONFIG_USE_HSI_DIRECT) || IS_ACTIVE(CONFIG_USE_HSI_PLL))
    {
        /* Enable HSI as system clock. */
        RCC->CFGR |= (RCC_CFGR_SW_HSI);
        while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI) {}
    }
    else if (IS_ACTIVE(CONFIG_USE_HSE_DIRECT) || IS_ACTIVE(CONFIG_USE_HSE_PLL))
    {
        /* Enable HSE as system clock. */
        RCC->CFGR |= (RCC_CFGR_SW_HSE);
        while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSE) {}
    }
    else if (IS_ACTIVE(CONFIG_USE_CSI_DIRECT) || IS_ACTIVE(CONFIG_USE_CSI_PLL))
    {
        /* Enable CSI as system clock. */
        RCC->CFGR |= (RCC_CFGR_SW_CSI);
        while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_CSI) {}
    }
    else
    {
        /* No valid SYSCLK branch selected at preprocess time. This should be
         * caught by the compile-time guard above; however keep a runtime trap
         * in case this ever runs on a configuration that slipped through. */
        while (1) {}
    }

    /* Enable PLL if required */
    if (IS_ACTIVE(CONFIG_USE_HSI_PLL) || IS_ACTIVE(CONFIG_USE_HSE_PLL) ||\
        IS_ACTIVE(CONFIG_USE_CSI_PLL))
    {
        /* Now we can safely configure and start the PLL. */
        RCC->PLLCKSELR = (PLL1_SRC | PLL1_M | PLL2_M | PLL3_M);

        /* By default enabling all outputs, can be disabled later if not used. */
        RCC->PLLCFGR =
            ( RCC_PLLCFGR_PLL1RGE_3 | RCC_PLLCFGR_PLL2RGE_3 | RCC_PLLCFGR_PLL3RGE_3
            | RCC_PLLCFGR_DIVP1EN | RCC_PLLCFGR_DIVQ1EN | RCC_PLLCFGR_DIVR1EN
            | RCC_PLLCFGR_DIVP2EN | RCC_PLLCFGR_DIVQ2EN | RCC_PLLCFGR_DIVR2EN
            | RCC_PLLCFGR_DIVP3EN | RCC_PLLCFGR_DIVQ3EN| RCC_PLLCFGR_DIVR3EN);

        /* Set the dividers and multipliers for all PLLs. */
        RCC->PLL1DIVR = (PLL1_N | PLL1_P | PLL1_Q | PLL1_R);
        RCC->CR |= RCC_CR_PLL1ON;
        while (!(RCC->CR & RCC_CR_PLL1RDY)) {}
        RCC->PLL2DIVR = (PLL2_N | PLL2_P | PLL2_Q | PLL2_R);
        RCC->CR |= RCC_CR_PLL2ON;
        while (!(RCC->CR & RCC_CR_PLL2RDY)) {}
        RCC->PLL3DIVR = (PLL3_N | PLL3_P | PLL3_Q | PLL3_R);
        RCC->CR |= RCC_CR_PLL3ON;
        while (!(RCC->CR & RCC_CR_PLL3RDY)) {}
        RCC->CR |= (RCC_CR_PLLON);
        while (!(RCC->CR & RCC_CR_PLLRDY)) {}
        /* Set PLL1_P as system clock. */
        RCC->CFGR |= (RCC_CFGR_SW_PLL1);
        while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL1) {}
    }

    irq_restore(is);
}

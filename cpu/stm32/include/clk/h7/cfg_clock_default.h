/*
 * SPDX-FileCopyrightText: 2024 Prime Controls, Inc.(R)
 * SPDX-FileCopyrightText: 2025 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup         cpu_stm32
 * @{
 *
 * @file
 * @brief           Main header for STM32H7 clock configuration (STM32H753ZI)
 *
 * @note
 * This configuration supports:
 *  - HSE (8 MHz or 25 MHz) + PLL1 → 480 MHz
 *  - HSI (64 MHz) + PLL1 → 480 MHz
 *  - HSI direct (no PLL) with optional divider (/1, /2, /4, /8)
 *  - CSI (4 MHz) + PLL1 → 480 MHz
 *
 * @author      Joshua DeWeese <jdeweese@primecontrols.com>
 * @author      Jay R Vaghela <jay.vaghela@tuhh.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "cpu_conf.h"
#include "kernel_defines.h"
#include "modules.h"
#include "macros/units.h"
#include "lse.h"
#include "lsi.h"

/* ==========================
 *  DEFAULT CONFIGURATION - HSI PLL
 * ========================== */

 /**
 * @brief   Enable direct HSI clock usage (no PLL)
 * @note    Select system clock source:
 * Use HSI direct (no PLL) = 1
 */
#ifndef CONFIG_USE_HSI_DIRECT
#  define CONFIG_USE_HSI_DIRECT      0
#endif

/**
 * @brief   Default to HSI + PLL as clock
 */
#ifndef CONFIG_USE_HSI_PLL
#  if IS_ACTIVE(CONFIG_USE_HSI_DIRECT) || IS_ACTIVE(CONFIG_USE_HSE_DIRECT) || \
      IS_ACTIVE(CONFIG_USE_CSI_DIRECT) || IS_ACTIVE(CONFIG_USE_CSI_PLL) || \
      IS_ACTIVE(CONFIG_USE_HSE_PLL)
#    define CONFIG_USE_HSI_PLL       0
#  else
#    define CONFIG_USE_HSI_PLL       1
#endif
#endif

/**
 * @brief   HSI divider when using HSI direct (no PLL)
 * Valid values: 1, 2, 4, 8
 */
#ifndef CONFIG_CLOCK_HSI_DIV
#  define CONFIG_CLOCK_HSI_DIV       1
#endif

/**
 * @brief   Disable HSE + PLL usage by default
 */
#ifndef CONFIG_USE_HSE_PLL
#  define CONFIG_USE_HSE_PLL         0
#endif

/**
 * @brief   Disable HSE usage by default
 */
#ifndef CONFIG_USE_HSE_DIRECT
#  define CONFIG_USE_HSE_DIRECT      0
#endif

/**
 * @brief   Disable CSI usage by default
 */
#ifndef CONFIG_USE_CSI_PLL
#  define CONFIG_USE_CSI_PLL         0
#endif

/**
 * @brief   Disable CSI direct usage by default
 */
#ifndef CONFIG_USE_CSI_DIRECT
#  define CONFIG_USE_CSI_DIRECT      0
#endif

/* ==========================
 *  Base oscillator defaults
 * ========================== */
#if IS_ACTIVE(CONFIG_USE_HSI_DIRECT) || IS_ACTIVE(CONFIG_USE_HSI_PLL)
#  ifndef CONFIG_CLOCK_HSI
#    define CONFIG_CLOCK_HSI         64
#  endif
#endif

#if IS_ACTIVE(CONFIG_USE_HSE_PLL) || IS_ACTIVE(CONFIG_USE_HSE_DIRECT)
#  ifndef CONFIG_CLOCK_HSE
#    define CONFIG_CLOCK_HSE         8
#  endif
#endif

#if IS_ACTIVE(CONFIG_USE_CSI_DIRECT) || IS_ACTIVE(CONFIG_USE_CSI_PLL)
#  ifndef CONFIG_CLOCK_CSI
#    define CONFIG_CLOCK_CSI         4
#  endif
#endif

#ifndef CONFIG_CLOCK_LSE
#  define CONFIG_CLOCK_LSE           32768UL
#endif

#if IS_ACTIVE(CONFIG_USE_LSI)
#  ifndef CONFIG_CLOCK_LSI
#    define CONFIG_CLOCK_LSI         32000UL
#  endif
#endif

/**
 * @brief Core clock divider
 * @note SYSCLK will be divided by this value to produce CLOCK_CORECLOCK
 * @details Default: 1
 */
#ifndef CONFIG_CLOCK_CORECLOCK_DIV
#  define CONFIG_CLOCK_CORECLOCK_DIV     1
#endif

#if IS_ACTIVE(CONFIG_USE_HSI_PLL) || IS_ACTIVE(CONFIG_USE_HSE_PLL) || \
    IS_ACTIVE(CONFIG_USE_CSI_PLL)
/* --------------------------------------------------------------------------
 * MODE: PLL-based (default)
 * --------------------------------------------------------------------------
 * SYSCLK source = PLL1_P output
 * Target SYSCLK = 480 MHz
 * -------------------------------------------------------------------------- */

/* Default PLL1 configuration based on input clock. */
#  if (CONFIG_CLOCK_HSE == 8)
    /* HSE with PLL1 → 480 MHz (VCO=960 MHz) */
#    ifndef CONFIG_CLOCK_PLL1_M
#      define CONFIG_CLOCK_PLL1_M        4
#      define CONFIG_CLOCK_PLL1_N        480
#      define CONFIG_CLOCK_PLL1_P        2
#      define CONFIG_CLOCK_PLL1_Q        20
#      define CONFIG_CLOCK_PLL1_R        2
#    endif

#  elif (CONFIG_CLOCK_HSE == 25)
    /* HSE with PLL1 → 480 MHz (VCO=960 MHz) */
#    ifndef CONFIG_CLOCK_PLL1_M
#      define CONFIG_CLOCK_PLL1_M        5
#      define CONFIG_CLOCK_PLL1_N        192
#      define CONFIG_CLOCK_PLL1_P        2
#      define CONFIG_CLOCK_PLL1_Q        20
#      define CONFIG_CLOCK_PLL1_R        2
#    endif

#  elif (CONFIG_CLOCK_CSI == 4)
    /* CSI with PLL1 → 480 MHz (VCO=960 MHz) */
#    ifndef CONFIG_CLOCK_PLL1_M
#      define CONFIG_CLOCK_PLL1_M        1
#      define CONFIG_CLOCK_PLL1_N        240
#      define CONFIG_CLOCK_PLL1_P        2
#      define CONFIG_CLOCK_PLL1_Q        20
#      define CONFIG_CLOCK_PLL1_R        2
#    endif

#  elif (CONFIG_CLOCK_HSI == 64)
    /* HSI with PLL1 → 480 MHz (VCO=960 MHz) */
#    ifndef CONFIG_CLOCK_PLL1_M
#      define CONFIG_CLOCK_PLL1_M        8
#      define CONFIG_CLOCK_PLL1_N        120
#      define CONFIG_CLOCK_PLL1_P        2
#      define CONFIG_CLOCK_PLL1_Q        20
#      define CONFIG_CLOCK_PLL1_R        2
#   endif

#  else
#    error "Unsupported clock input for PLL configuration."
#  endif
#endif /* CONFIG_USE_HSI_DIRECT */

#include "clk/h7/lse.h"
#include "clk/h7/lsi.h"
#include "clk/h7/hse.h"
#include "clk/h7/hsi.h"
#include "clk/h7/csi.h"
#include "clk/h7/pll1.h"
#include "clk/h7/pll2.h"
#include "clk/h7/pll3.h"
#include "clk/h7/coreclock.h"

/**
 * @brief Default AHB clock divider
 * @note HCLK = SYSCLK / CONFIG_CLOCK_AHB_DIV, max 240 MHz
 */
#ifndef CONFIG_CLOCK_AHB_DIV
#  define CONFIG_CLOCK_AHB_DIV       2
#endif

#include "clk/h7/ahb.h"

/**
 * @brief   APB1 clock divider
 * @note    PCLK1 = HCLK / CONFIG_CLOCK_APB1_DIV (max 120 MHz)
 */
#ifndef CONFIG_CLOCK_APB1_DIV
#  define CONFIG_CLOCK_APB1_DIV      2
#endif

/**
 * @brief   APB2 clock divider
 * @note    PCLK2 = HCLK / CONFIG_CLOCK_APB2_DIV (max 120 MHz)
 */
#ifndef CONFIG_CLOCK_APB2_DIV
#  define CONFIG_CLOCK_APB2_DIV      2
#endif

/**
 * @brief   APB3 clock divider
 * @note    PCLK3 = HCLK / CONFIG_CLOCK_APB3_DIV (max 120 MHz)
 */
#ifndef CONFIG_CLOCK_APB3_DIV
#  define CONFIG_CLOCK_APB3_DIV      2
#endif

/**
 * @brief   APB4 clock divider
 * @note    PCLK4 = HCLK / CONFIG_CLOCK_APB4_DIV (max 120 MHz)
 */
#ifndef CONFIG_CLOCK_APB4_DIV
#  define CONFIG_CLOCK_APB4_DIV      2
#endif

#include "clk/h7/apb1.h"
#include "clk/h7/apb2.h"
#include "clk/h7/apb3.h"
#include "clk/h7/apb4.h"

#ifdef __cplusplus
}
#endif

/** @} */

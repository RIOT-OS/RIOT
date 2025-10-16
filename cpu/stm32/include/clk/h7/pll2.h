/*
 * SPDX-FileCopyrightText: 2025 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup         cpu_stm32
 * @{
 *
 * @file
 * @brief           PLL2 clock setup for STM32H7 family
 *
 * @author          Jay R Vaghela <jay.vaghela@tuhh.de>
 */

#include "cfg_clock_default.h"

#ifdef __cplusplus
extern "C" {
#endif

/* PLL2 configuration (DEFAULT): the following parameters configure a 48MHz USB clock
 * with HSE (8MHz/25MHz) or HSI (64MHz) or CSI (4MHz) as PLL input clock. */
#ifndef CONFIG_CLOCK_PLL2_M
#  if IS_ACTIVE(CONFIG_USE_CSI_PLL)
#    define CONFIG_CLOCK_PLL2_M         (1)
#  elif IS_ACTIVE(CONFIG_USE_HSE_PLL)
#    if CONFIG_CLOCK_HSE == MHZ(8)
#      define CONFIG_CLOCK_PLL2_M       (4)
#    elif CONFIG_CLOCK_HSE == MHZ(25)
#      define CONFIG_CLOCK_PLL2_M       (5)
#    endif
#  else /* HSI - 64MHz */
#    define CONFIG_CLOCK_PLL2_M         (8)
#  endif
#endif

#ifndef CONFIG_CLOCK_PLL2_N
#  if IS_ACTIVE(CONFIG_USE_CSI_PLL)
#    define CONFIG_CLOCK_PLL2_N         (240)
#  elif IS_ACTIVE(CONFIG_USE_HSE_PLL)
#    if CONFIG_CLOCK_HSE == MHZ(8)
#      define CONFIG_CLOCK_PLL2_N       (480)
#    elif CONFIG_CLOCK_HSE == MHZ(25)
#      define CONFIG_CLOCK_PLL2_N       (192)
#    endif
#  else /* HSI */
#    define CONFIG_CLOCK_PLL2_N         (120)
#  endif
#endif

#ifndef CONFIG_CLOCK_PLL2_P
#  if IS_ACTIVE(CONFIG_USE_CSI_PLL)
#    define CONFIG_CLOCK_PLL2_P         (2)
#  elif IS_ACTIVE(CONFIG_USE_HSE_PLL)
#    if CONFIG_CLOCK_HSE == MHZ(8)
#      define CONFIG_CLOCK_PLL2_P       (2)
#    elif CONFIG_CLOCK_HSE == MHZ(25)
#      define CONFIG_CLOCK_PLL2_P       (2)
#    endif
#  else /* HSI */
#    define CONFIG_CLOCK_PLL2_P         (2)
#  endif
#endif

#ifndef CONFIG_CLOCK_PLL2_Q
#  if IS_ACTIVE(CONFIG_USE_CSI_PLL)
#    define CONFIG_CLOCK_PLL2_Q         (20)
#  elif IS_ACTIVE(CONFIG_USE_HSE_PLL)
#    if CONFIG_CLOCK_HSE == MHZ(8)
#      define CONFIG_CLOCK_PLL2_Q       (20)
#    elif CONFIG_CLOCK_HSE == MHZ(25)
#      define CONFIG_CLOCK_PLL2_Q       (20)
#    endif
#  else /* HSI */
#    define CONFIG_CLOCK_PLL2_Q         (20)
#  endif
#endif

#ifndef CONFIG_CLOCK_PLL2_R
#  if IS_ACTIVE(CONFIG_USE_CSI_PLL)
#    define CONFIG_CLOCK_PLL2_R         (2)
#  elif IS_ACTIVE(CONFIG_USE_HSE_PLL)
#    if CONFIG_CLOCK_HSE == MHZ(8)
#      define CONFIG_CLOCK_PLL2_R       (2)
#    elif CONFIG_CLOCK_HSE == MHZ(25)
#      define CONFIG_CLOCK_PLL2_R       (2)
#    endif
#  else /* HSI */
#    define CONFIG_CLOCK_PLL2_R         (2)
#  endif
#endif

#if IS_ACTIVE(CONFIG_USE_HSI_PLL) || IS_ACTIVE(CONFIG_USE_HSE_PLL) || \
    IS_ACTIVE(CONFIG_USE_CSI_PLL)

/* Configure these values using KCONFIG */
#  define CLOCK_PLL2_M              CONFIG_CLOCK_PLL2_M
#  define CLOCK_PLL2_N              CONFIG_CLOCK_PLL2_N
#  define CLOCK_PLL2_P              CONFIG_CLOCK_PLL2_P
#  define CLOCK_PLL2_Q              CONFIG_CLOCK_PLL2_Q
#  define CLOCK_PLL2_R              CONFIG_CLOCK_PLL2_R

/* PLL2 input selection */
#  if IS_ACTIVE(CONFIG_USE_HSI_PLL)
#    define CLOCK_PLL2_INPUT         CLOCK_HSI
#  elif IS_ACTIVE(CONFIG_USE_HSE_PLL)
#    define CLOCK_PLL2_INPUT         CLOCK_HSE
#  elif IS_ACTIVE(CONFIG_USE_CSI_PLL)
#    define CLOCK_PLL2_INPUT         CLOCK_CSI
#  endif

/* PLL2 output frequencies */
#  define CLOCK_PLL2_VCO            ((CLOCK_PLL2_INPUT * CLOCK_PLL2_N) / (CLOCK_PLL2_M))
#  define CLOCK_PLL2_P_OUT          (CLOCK_PLL2_VCO / CLOCK_PLL2_P)
#  define CLOCK_PLL2_Q_OUT          (CLOCK_PLL2_VCO / CLOCK_PLL2_Q)
#  define CLOCK_PLL2_R_OUT          (CLOCK_PLL2_VCO / CLOCK_PLL2_R)

#endif /* PLL usage */

#ifdef __cplusplus
}
#endif

/** @} */

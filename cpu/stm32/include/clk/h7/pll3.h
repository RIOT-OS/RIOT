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
 * @brief           PLL3 clock setup for STM32H7 family
 *
 * @author          Jay R Vaghela <jay.vaghela@tuhh.de>
 */

#include "cfg_clock_default.h"

#ifdef __cplusplus
extern "C" {
#endif

/* PLL3 configuration (DEFAULT): the following parameters configure a 48MHz USB clock
 * with HSE (8MHz/25MHz) or HSI (64MHz) or CSI (4MHz) as PLL input clock. */
#ifndef CONFIG_CLOCK_PLL3_M
#  if IS_ACTIVE(CONFIG_USE_CSI_PLL)
#    define CONFIG_CLOCK_PLL3_M         (1)
#  elif IS_ACTIVE(CONFIG_USE_HSE_PLL)
#    if CONFIG_CLOCK_HSE == MHZ(8)
#      define CONFIG_CLOCK_PLL3_M       (4)
#    elif CONFIG_CLOCK_HSE == MHZ(25)
#      define CONFIG_CLOCK_PLL3_M       (5)
#    endif
#  else /* HSI - 64MHz */
#    define CONFIG_CLOCK_PLL3_M         (8)
#  endif
#endif

#ifndef CONFIG_CLOCK_PLL3_N
#  if IS_ACTIVE(CONFIG_USE_CSI_PLL)
#    define CONFIG_CLOCK_PLL3_N         (240)
#  elif IS_ACTIVE(CONFIG_USE_HSE_PLL)
#    if CONFIG_CLOCK_HSE == MHZ(8)
#      define CONFIG_CLOCK_PLL3_N       (480)
#    elif CONFIG_CLOCK_HSE == MHZ(25)
#      define CONFIG_CLOCK_PLL3_N       (192)
#    endif
#  else /* HSI */
#    define CONFIG_CLOCK_PLL3_N         (120)
#  endif
#endif

#ifndef CONFIG_CLOCK_PLL3_P
#  if IS_ACTIVE(CONFIG_USE_CSI_PLL)
#    define CONFIG_CLOCK_PLL3_P       (2)
#  elif IS_ACTIVE(CONFIG_USE_HSE_PLL)
#    if CONFIG_CLOCK_HSE == MHZ(8)
#      define CONFIG_CLOCK_PLL3_P       (2)
#    elif CONFIG_CLOCK_HSE == MHZ(25)
#      define CONFIG_CLOCK_PLL3_P       (2)
#    endif
#  else /* HSI */
#    define CONFIG_CLOCK_PLL3_P       (2)    /* 480 MHz with HSI */
#  endif
#endif

#ifndef CONFIG_CLOCK_PLL3_Q
#  if IS_ACTIVE(CONFIG_USE_CSI_PLL)
#    define CONFIG_CLOCK_PLL3_Q       (20)
#  elif IS_ACTIVE(CONFIG_USE_HSE_PLL)
#    if CONFIG_CLOCK_HSE == MHZ(8)
#      define CONFIG_CLOCK_PLL3_Q       (20)
#    elif CONFIG_CLOCK_HSE == MHZ(25)
#      define CONFIG_CLOCK_PLL3_Q       (20)
#    endif
#  else   /* HSI */
#    define CONFIG_CLOCK_PLL3_Q       (20)  /* Alternative 48MHz clock (USB/SDIO/SDMMC) */
#  endif
#endif

#ifndef CONFIG_CLOCK_PLL3_R
#  if IS_ACTIVE(CONFIG_USE_CSI_PLL)
#    define CONFIG_CLOCK_PLL3_R       (2)
#  elif IS_ACTIVE(CONFIG_USE_HSE_PLL)
#    if CONFIG_CLOCK_HSE == MHZ(8)
#      define CONFIG_CLOCK_PLL3_R       (2)
#    elif CONFIG_CLOCK_HSE == MHZ(25)
#      define CONFIG_CLOCK_PLL3_R       (2)
#    endif
#  else  /* HSI */
#    define CONFIG_CLOCK_PLL3_R       (2)     /* LTDC Clock, with HSI - 480MHz */
#  endif
#endif

/* PLL3 needed? */
#if IS_ACTIVE(CONFIG_USE_HSI_PLL) || IS_ACTIVE(CONFIG_CLOCK_HSE_PLL) || \
    IS_ACTIVE(CONFIG_USE_CSI_PLL)
/* Configure these values using KCONFIG */
#  define CLOCK_PLL3_M              CONFIG_CLOCK_PLL3_M
#  define CLOCK_PLL3_N              CONFIG_CLOCK_PLL3_N
#  define CLOCK_PLL3_P              CONFIG_CLOCK_PLL3_P
#  define CLOCK_PLL3_Q              CONFIG_CLOCK_PLL3_Q
#  define CLOCK_PLL3_R              CONFIG_CLOCK_PLL3_R

/* PLL3 input selection */
#  if IS_ACTIVE(CONFIG_USE_HSI_PLL)
#    define CLOCK_PLL3_INPUT         CLOCK_HSI
#  elif IS_ACTIVE(CONFIG_USE_HSE_PLL)
#    define CLOCK_PLL3_INPUT         CLOCK_HSE
#  elif IS_ACTIVE(CONFIG_USE_CSI_PLL)
#    define CLOCK_PLL3_INPUT         CLOCK_CSI
#  endif

/* PLL3 output frequencies */
#  define CLOCK_PLL3_VCO            ((CLOCK_PLL3_INPUT * CLOCK_PLL3_N) / (CLOCK_PLL3_M))
#  define CLOCK_PLL3_P_OUT          (CLOCK_PLL3_VCO / CLOCK_PLL3_P)
#  define CLOCK_PLL3_Q_OUT          (CLOCK_PLL3_VCO / CLOCK_PLL3_Q)
#  define CLOCK_PLL3_R_OUT          (CLOCK_PLL3_VCO / CLOCK_PLL3_R)
#endif /* PLL usage */

#ifdef __cplusplus
}
#endif

/** @} */

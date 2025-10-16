/*
 * SPDX-FileCopyrightText: 2024 Prime Controls, Inc.(R)
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup         cpu_stm32
 * @{
 *
 * @file
 * @brief           PLL1 clock setup for STM32H7 family
 *
 * @author          Joshua DeWeese <jdeweese@primecontrols.com>
 */

#include "cfg_clock_default.h"

#ifdef __cplusplus
extern "C" {
#endif

/* PLL1 needed? */
#if IS_ACTIVE(CONFIG_USE_HSI_PLL) || IS_ACTIVE(CONFIG_USE_HSE_PLL) || \
    IS_ACTIVE(CONFIG_USE_CSI_PLL)

/* PLL1 configuration */
#  define CLOCK_PLL1_M              CONFIG_CLOCK_PLL1_M
#  define CLOCK_PLL1_N              CONFIG_CLOCK_PLL1_N
#  define CLOCK_PLL1_P              CONFIG_CLOCK_PLL1_P
#  define CLOCK_PLL1_Q              CONFIG_CLOCK_PLL1_Q
#  define CLOCK_PLL1_R              CONFIG_CLOCK_PLL1_R

/* PLL1 input selection */
#  if IS_ACTIVE(CONFIG_USE_HSI_PLL)
#    define CLOCK_PLL1_INPUT         CLOCK_HSI
# elif IS_ACTIVE(CONFIG_USE_HSE_PLL)
#    define CLOCK_PLL1_INPUT         CLOCK_HSE
#  elif IS_ACTIVE(CONFIG_USE_CSI_PLL)
#    define CLOCK_PLL1_INPUT         CLOCK_CSI
#endif

/* PLL1 output frequencies */
#  define CLOCK_PLL1_VCO            ((CLOCK_PLL1_INPUT * CLOCK_PLL1_N) / (CLOCK_PLL1_M))
#  define CLOCK_PLL1_P_OUT          (CLOCK_PLL1_VCO / CLOCK_PLL1_P)
#  define CLOCK_PLL1_Q_OUT          (CLOCK_PLL1_VCO / CLOCK_PLL1_Q)
#  define CLOCK_PLL1_R_OUT          (CLOCK_PLL1_VCO / CLOCK_PLL1_R)

#endif

#ifdef __cplusplus
}
#endif

/** @} */

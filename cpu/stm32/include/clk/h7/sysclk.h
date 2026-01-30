/*
 * SPDX-FileCopyrightText: 2024 Prime Controls, Inc.(R)
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once
/**
 * @ingroup     cpu_stm32
 * @{
 *
 * @file
 * @brief       System clock configuration for STM32H7
 *
 * @author      Joshua DeWeese <jdeweese@primecontrols.com>
 */

#include "hse.h"
#include "hsi.h"
#include "csi.h"
#include "pll1.h"
#include "pll2.h"
#include "pll3.h"

#ifdef __cplusplus
extern "C" {
#endif

#if IS_ACTIVE(CONFIG_USE_HSI_DIRECT)
#  define CLOCK_SYSCLK            CLOCK_HSI
#elif IS_ACTIVE(CONFIG_USE_HSE_DIRECT)
#  define CLOCK_SYSCLK            CLOCK_HSE
#elif IS_ACTIVE(CONFIG_USE_CSI_DIRECT)
#  define CLOCK_SYSCLK            CLOCK_CSI
#elif IS_ACTIVE(CONFIG_USE_HSI_PLL) || IS_ACTIVE(CONFIG_USE_HSE_PLL) || \
      IS_ACTIVE(CONFIG_USE_CSI_PLL)
#  define CLOCK_SYSCLK            CLOCK_PLL1_P_OUT
#else
#  error "No valid clock source selected"
#endif

#ifdef __cplusplus
}
#endif

/** @} */

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
 * @brief           HSI clock setup for STM32H7 family
 *
 * @author          Joshua DeWeese <jdeweese@primecontrols.com>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "cpu_conf.h"
#include "kernel_defines.h"

#if IS_ACTIVE(CONFIG_USE_HSI_DIRECT) || IS_ACTIVE(CONFIG_USE_HSI_PLL)
#  ifndef CONFIG_CLOCK_HSI
#    error "CONFIG_CLOCK_HSI must be defined"
#  endif
#  ifndef CONFIG_CLOCK_HSI_DIV
#    define CONFIG_CLOCK_HSI_DIV 1
#  endif
#  define CLOCK_HSI               (CONFIG_CLOCK_HSI / CONFIG_CLOCK_HSI_DIV)
#endif

#ifdef __cplusplus
}
#endif

/** @} */

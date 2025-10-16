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
 * @brief           HSE clock setup for STM32H7 family
 *
 * @author          Joshua DeWeese <jdeweese@primecontrols.com>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "cpu_conf.h"
#include "kernel_defines.h"

#if IS_ACTIVE(CONFIG_USE_HSE_PLL) || IS_ACTIVE(CONFIG_USE_HSE_DIRECT)
#  ifndef CONFIG_CLOCK_HSE
#    error "CONFIG_CLOCK_HSE must be defined when using HSE PLL"
#  endif
#  define CLOCK_HSE               CONFIG_CLOCK_HSE
#endif

#ifdef __cplusplus
}
#endif

/** @} */

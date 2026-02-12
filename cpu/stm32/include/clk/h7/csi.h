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
 * @brief           CSI clock setup for STM32H7 family
 *
 * @author          Joshua DeWeese <jdeweese@primecontrols.com>
 */

#include "cpu_conf.h"
#include "kernel_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef RCC_CR_CSION
#  ifndef CONFIG_CLOCK_CSI
#    define CONFIG_CLOCK_CSI        4 /* Default CSI frequency is 4MHz */
#  endif
#  define CLOCK_CSI               CONFIG_CLOCK_CSI
#endif

#ifdef __cplusplus
}
#endif

/** @} */

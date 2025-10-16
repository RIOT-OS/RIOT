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
 * @brief       APB4 clock configuration for STM32H7
 *
 * @author      Joshua DeWeese <jdeweese@primecontrols.com>
 */

#include "ahb.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONFIG_CLOCK_APB4_DIV
#  define CONFIG_CLOCK_APB4_DIV 2
#endif

/**
 * @brief APB4 bus clock frequency
 */
#define CLOCK_APB4             (CLOCK_AHB / CONFIG_CLOCK_APB4_DIV)

#ifdef __cplusplus
}
#endif

/** @} */

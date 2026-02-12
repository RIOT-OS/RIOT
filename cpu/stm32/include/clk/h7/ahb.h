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
 * @brief       AHB clock configuration for STM32H7
 *
 * @author      Joshua DeWeese <jdeweese@primecontrols.com>
 */

#include "coreclock.h"
#include "sysclk.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief AHB bus clock frequency
 */
#define CLOCK_AHB   (CLOCK_CORECLOCK / CONFIG_CLOCK_AHB_DIV)

#ifdef __cplusplus
}
#endif

/** @} */

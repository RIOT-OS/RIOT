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
 * @brief           LSE clock setup for STM32H7 family
 *
 * @author          Jay R Vaghela <jay.vaghela@tuhh.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "cpu_conf.h"
#include "kernel_defines.h"

/**
 * @brief   External low-speed oscillator frequency
 *
 * Default value corresponds to a 32.768 kHz crystal.
 */
#ifndef CONFIG_CLOCK_LSE
#  define CONFIG_CLOCK_LSE        32768U /* Default LSE frequency is 32.768kHz */
#endif

/**
 * @brief   LSE clock frequency
 */
#if IS_ACTIVE(CONFIG_USE_LSE)
#  define CLOCK_LSE               CONFIG_CLOCK_LSE
#endif

#ifdef __cplusplus
}
#endif

/** @} */

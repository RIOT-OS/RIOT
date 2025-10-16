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
 * @brief           LSI clock setup for STM32H7 family
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
 * 32KHz internal RC Oscillator
 */
#ifndef CONFIG_CLOCK_LSI
#  define CONFIG_CLOCK_LSI        32000U /* Default LSI frequency is 32kHz */
#endif

/**
 * @brief   LSI clock frequency
 */
#if IS_ACTIVE(CONFIG_USE_LSI)
#  ifndef CLOCK_LSI
#    define CLOCK_LSI               CONFIG_CLOCK_LSI
#  endif
#endif

#ifdef __cplusplus
}
#endif

/** @} */

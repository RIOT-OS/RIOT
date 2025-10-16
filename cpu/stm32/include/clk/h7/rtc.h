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
 * @brief           RTC clock setup for STM32H7 family
 *
 * @author          Jay R Vaghela <jay.vaghela@tuhh.de>
 */

#include "lse.h"
#include "lsi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Select RTC clock source
 *
 * If enabled, the RTC is clocked from LSE.
 * Otherwise, LSI is used as RTC clock source.
 */
#ifndef CONFIG_RTC_USE_LSE
#  define CONFIG_RTC_USE_LSE 1
#endif

/**
 * @brief   RTC clock frequency
 */
#if CONFIG_RTC_USE_LSE
#  define CLOCK_RTC CLOCK_LSE
#else
#  define CLOCK_RTC CLOCK_LSI
#endif

#ifdef __cplusplus
}
#endif

/** @} */

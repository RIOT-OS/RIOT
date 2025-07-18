/*
 * SPDX-FileCopyrightText: 2016 Kees Bakker, SODAQ
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_common_sodaq
 * @{
 *
 * @file
 * @brief       Default RTC configuration for SODAQ boards
 *
 * @author      Kees Bakker <kees@sodaq.com>
 */

#include <stdint.h>

#include "cpu.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name RTC configuration
 * @{
 */
#define RTC_DEV             RTC->MODE2
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */

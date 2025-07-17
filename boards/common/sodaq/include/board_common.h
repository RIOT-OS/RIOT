/*
 * SPDX-FileCopyrightText: 2016 Kees Bakker, SODAQ
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    boards_common_sodaq SODAQ boards common
 * @ingroup     boards_common
 * @brief       Common support for all SODAQ boards
 * @{
 *
 * @file
 * @brief       Board common definitions for the SODAQ boards
 *
 * @author      Kees Bakker <kees@sodaq.com>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    xtimer configuration
 * @{
 */
#define XTIMER_DEV          TIMER_DEV(1)
#define XTIMER_CHAN         (0)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */

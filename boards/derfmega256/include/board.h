/*
 * SPDX-FileCopyrightText: 2019 Alexander Chudov <chudov@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_deRFmega256
 * @{
 *
 * @file
 * @brief       Board specific definitions for the deRFmega256 modules
 *
 * @author      Alexander Chudov <chudov@gmail.com>
 */

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name xtimer configuration values
 * @{
 */
#define XTIMER_WIDTH        (16)
#define XTIMER_HZ           (CLOCK_CORECLOCK / 64)
#define XTIMER_BACKOFF      (40)

/** @} */

#ifdef __cplusplus
}
#endif

/** @} */

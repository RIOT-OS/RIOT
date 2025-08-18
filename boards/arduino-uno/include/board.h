/*
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_arduino-uno
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Arduino Uno board
 *
 * @author      Martine Lenders <m.lenders@fu-berlin.de>
 */

/**
 * @name    ztimer configuration values
 *
 * @note    Overrides arduino-atmega defaults
 * @{
 */
#define CONFIG_ZTIMER_USEC_ADJUST_SET     (128)
#define CONFIG_ZTIMER_USEC_ADJUST_SLEEP   (116)
/** @} */

#include "board_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

/** @} */

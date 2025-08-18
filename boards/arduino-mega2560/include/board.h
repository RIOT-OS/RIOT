/*
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_arduino-mega2560
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Arduino Mega 2560 board
 *
 * @author      Martine Lenders <m.lenders@fu-berlin.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    ztimer configuration values
 * @{
 */
#define CONFIG_ZTIMER_USEC_ADJUST_SET     (132)
#define CONFIG_ZTIMER_USEC_ADJUST_SLEEP   (120)
/** @} */

#include "board_common.h"

#ifdef __cplusplus
}
#endif

/** @} */

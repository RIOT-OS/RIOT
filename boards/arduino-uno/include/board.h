/*
 * Copyright (C) 2017   Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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

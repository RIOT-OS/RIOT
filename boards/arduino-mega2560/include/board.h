/*
 * Copyright (C) 2017   Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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

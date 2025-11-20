/*
 * SPDX-FileCopyrightText: 2019 Robert Olsson <roolss@kth.se>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_avr-rss2
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the rss2 AtMega256rfr2 board
 * @author      Robert Olsson <roolss@kth.se>
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   Clock configuration
 * @{
 */
#define CLOCK_CORECLOCK     (16000000UL)
/** @} */

#ifdef __cplusplus
}
#endif

#include "periph_conf_atmega_common.h"

/** @} */

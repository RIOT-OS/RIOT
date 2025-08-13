/*
 * SPDX-FileCopyrightText: 2019 Otto-von-Guericke-Universität Magdeburg
 * SPDX-FileCopyrightText: 2021 Gerson Fernando Budke
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup boards_atmega328p_xplained_mini
 * @{
 *
 * @file
 * @brief   Peripheral MCU configuration for the ATmega328p xplained mini "board"
 *
 * @author  Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 * @author  Gerson Fernando Budke <nandojve@gmail.com>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Clock configuration
 * @{
 */
#ifndef CLOCK_CORECLOCK
/* Using 16MHz internal oscillator as default clock source */
#define CLOCK_CORECLOCK     (16000000UL)
#endif
/** @} */

#ifdef __cplusplus
}
#endif

#include "periph_conf_atmega_common.h"

/** @} */

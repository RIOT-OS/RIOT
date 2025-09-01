/*
 * SPDX-FileCopyrightText: 2019 Otto-von-Guericke-Universität Magdeburg
 * SPDX-FileCopyrightText: 2023 Hugues Larrive
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup boards_atmega8
 * @{
 *
 * @file
 * @brief   Peripheral MCU configuration for the ATmega8 standalone "board"
 *
 * @author  Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 * @author  Hugues Larrive <hugues.larrive@pm.me>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Clock configuration
 * @{
 */
#ifndef CLOCK_CORECLOCK
/* Using 8MHz internal oscillator as default clock source */
#define CLOCK_CORECLOCK     (8000000UL)
#endif
/** @} */

#ifdef __cplusplus
}
#endif

#include "periph_conf_atmega_common.h"

/** @} */

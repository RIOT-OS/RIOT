/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
 *               2021 Gerson Fernando Budke
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

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

#endif /* PERIPH_CONF_H */

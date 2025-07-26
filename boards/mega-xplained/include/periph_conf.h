/*
 * SPDX-FileCopyrightText: 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 * SPDX-FileCopyrightText: 2016 Laurent Navet <laurent.navet@gmail.com>
 * SPDX-FileCopyrightText: 2017 HAW Hamburg, Dimitri Nahm
 * SPDX-FileCopyrightText: 2018 Matthew Blue <matthew.blue.neuro@gmail.com>
 * SPDX-FileCopyrightText: 2019 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_mega-xplained
 * @{
 *
 * @file
 * @brief       Common configuration of MCU periphery for Mega Xplained
 *
 * @author      Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
 * @author      Laurent Navet <laurent.navet@gmail.com>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Dimitri Nahm <dimitri.nahm@haw-hamburg.de>
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Clock configuration
 *
 * Frequency of the internal 8MHz RC oscillator.
 *
 * @{
 */
#define CLOCK_CORECLOCK     (8000000UL)
/** @} */

#ifdef __cplusplus
}
#endif

#include "periph_conf_atmega_common.h"

/** @} */

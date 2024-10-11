/*
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *               2016 Laurent Navet <laurent.navet@gmail.com>
 *               2017 HAW Hamburg, Dimitri Nahm
 *               2018 Matthew Blue <matthew.blue.neuro@gmail.com>
 *               2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

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
#endif /* PERIPH_CONF_H */

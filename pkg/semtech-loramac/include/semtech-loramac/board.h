/*
 * Copyright (C) 2017 Fundacion Inria Chile
 *               2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         pkg_semtech-loramac
 * @brief           Internal required Semtech LoRaMAC definitions for radio
 * @{
 *
 * @file
 *
 * @author          José Ignacio Alamos <jialamos@uc.cl>
 * @author          Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author          Francisco Molina <francisco.molina@inria.cl>
 */

#ifndef SEMTECH_LORAMAC_BOARD_H
#define SEMTECH_LORAMAC_BOARD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>
#include "semtech-loramac/timer.h"

/**
 * @brief Radio wakeup time from SLEEP mode
 */
#define RADIO_OSC_STARTUP                           (1U) /* [ms] */

/**
 * @brief Radio PLL lock and Mode Ready delay which can vary with the temperature
 */
#define RADIO_SLEEP_TO_RX                           (2U) /* [ms] */

/**
 * @brief Radio complete Wake-up Time with margin for temperature compensation
 */
#define RADIO_WAKEUP_TIME ( RADIO_OSC_STARTUP + RADIO_SLEEP_TO_RX )

#ifdef __cplusplus
}
#endif

#endif /* SEMTECH_LORAMAC_BOARD_H */
/** @} */

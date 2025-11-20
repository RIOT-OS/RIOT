/*
 * Copyright (C) 2018 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_cc110x
 * @{
 *
 * @file
 * @brief       Functions related to the netdev interface of the CC110x driver
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#include "net/netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   RIOT's API to interact with the CC1100/CC1101 driver
 */
extern const netdev_driver_t cc110x_driver;

/**
 * @brief   Interrupt handler to call on both edges of the GDO0 and GDO2 pins
 *
 * @param   dev     The device descriptor of the transceiver
 *
 * This interrupt handler requests that the cc110x ISR is called in thread
 * context
 */
void cc110x_on_gdo(void *dev);

#ifdef __cplusplus
}
#endif

/** @} */

/*
 * Copyright (C) 2018 Hochschule RheinMain
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_ikea-tradfri
 * @{
 *
 * @file
 * @brief       Board specific definitions for the radio transceiver of the
 *              Ikea Tradfi module
 *
 * @author      Kai Beckmann <kai.beckmann@hs-rm.de>
 **/

#ifndef RAIL_RADIO_H
#define RAIL_RADIO_H

#include "cpu.h"

#include "periph_conf.h"
#include "periph/gpio.h"
#include "periph/spi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Select radio frequency
 *
 * ikea tradfri supports only 2.4 GHz
 */
#define RAIL_RADIO_BAND (2400)

/**
 * @brief Setting for the radio PA voltage in mV
 *
 * voltage of the radio PA in mV
 *
 * The setting depends on the module.
 *
 * the tradfi modules embedded in lamps are
 * connected to th external dc-dc, therefore 3.3V
 */
#define RAIL_RADIO_PA_VOLTAGE (3300)

/**
  * @brief Packet Trace Interface
  *
  * The Tradfri doesn't support PTI,
  * so don't enable it.
  */
#ifndef RAIL_PTI_ENABLED
#define RAIL_PTI_ENABLED      (0)
#endif

#ifdef __cplusplus
}
#endif

#endif /* RAIL_RADIO_H */
/** @} */

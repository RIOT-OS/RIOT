/*
 * Copyright (C) 2017-2018 Hochschule RheinMain
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_slwstk6000b Silicon Labs SLWSTK6000B starter kit
 * @ingroup     boards
 * @brief       Support for the Silicon Labs SLWRB4162a module
 * @{
 *
 * @file
 * @brief       Board specific definitions for the radio transceicer of the
 *              SLWRB4162a module
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
 * SLWRB4162a supports only 2.4 GHz
 */
#define RAIL_RADIO_BAND (2400)

/**
 * @brief Setting for the radio PA voltage in mV
 *
 * voltage of the radio PA in mV
 *
 * The setting depends on the module.
 *
 * The silabs dev modules are connected to
 * the internal dc/dc -> 1.8V
 */
#define RAIL_RADIO_PA_VOLTAGE (1800)


/**
  * @brief Packet Trace Interface
  *
  * To enable the PTI set vaule to 1.
  * In RAIL_PTI_CONFIG the correct pins
  * have to be configured
  */
#ifndef RAIL_PTI_ENABLED
#define RAIL_PTI_ENABLED      (0)
#endif

/**
 * c&p from gecko-sdk BSP for EFR32MG1_BRD4154A
 */
#if !defined(RAIL_PTI_CONFIG)
#define RAIL_PTI_CONFIG                                                    \
    {                                                                        \
        RAIL_PTI_MODE_UART, /* Only supported output mode for the WSTK*/   \
        1600000,            /* Choose 1.6 MHz for best the WSTK */         \
        6,                  /* WSTK uses location 6 for DOUT PB12 */       \
        gpioPortB,          /* FRC_DOUT#6 is PB12 */                       \
        12,                 /* FRC_DOUT#6 is PB12 */                       \
        6,                  /* UNUSED IN UART MODE */                      \
        gpioPortB,          /* UNUSED IN UART MODE */                      \
        11,                 /* UNUSED IN UART MODE */                      \
        6,                  /* WSTK uses location 6 for DFRAME */          \
        gpioPortB,          /* RC_DOUT#6 is PB13 */                        \
        13,                 /* RC_DOUT#6 is PB13 */                        \
    }
#endif


#ifdef __cplusplus
}
#endif

#endif /* RAIL_RADIO_H */
/** @} */

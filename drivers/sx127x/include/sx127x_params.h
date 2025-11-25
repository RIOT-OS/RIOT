/*
 * Copyright (C) 2017 Inria
 *               2017 Inria Chile
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_sx127x
 * @{
 * @file
 * @brief       Default configuration for SX127X driver
 *
 * @author      José Ignacio Alamos <jose.alamos@inria.cl>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef SX127X_PARAMS_H
#define SX127X_PARAMS_H

#include "board.h"
#include "sx127x.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the SX127X driver
 *          Pins are adapted to STM32 Nucleo-64 boards.
 * @{
 */
#ifndef SX127X_PARAM_SPI
#define SX127X_PARAM_SPI                    (SPI_DEV(0))
#endif

#ifndef SX127X_PARAM_SPI_NSS
#define SX127X_PARAM_SPI_NSS                GPIO_PIN(1, 6)       /* D10 */
#endif

#ifndef SX127X_PARAM_RESET
#define SX127X_PARAM_RESET                  GPIO_PIN(0, 0)       /* A0 */
#endif

#ifndef SX127X_PARAM_DIO0
#define SX127X_PARAM_DIO0                   GPIO_PIN(0, 10)      /* D2 */
#endif

#ifndef SX127X_PARAM_DIO1
#define SX127X_PARAM_DIO1                   GPIO_PIN(1, 3)       /* D3 */
#endif

#ifndef SX127X_PARAM_DIO2
#define SX127X_PARAM_DIO2                   GPIO_PIN(1, 5)       /* D4 */
#endif

#ifndef SX127X_PARAM_DIO3
#define SX127X_PARAM_DIO3                   GPIO_PIN(1, 4)       /* D5 */
#endif

#ifndef SX127X_PARAM_PASELECT
#define SX127X_PARAM_PASELECT               (SX127X_PA_RFO)
#endif

#ifndef SX127X_PARAM_TX_SWITCH
#define SX127X_PARAM_TX_SWITCH              GPIO_UNDEF
#endif

#ifndef SX127X_PARAM_RX_SWITCH
#define SX127X_PARAM_RX_SWITCH              GPIO_UNDEF
#endif

#ifndef SX127X_PARAMS
#if defined(SX127X_USE_TX_SWITCH) || defined(SX127X_USE_RX_SWITCH)
#define SX127X_PARAMS             { .spi            = SX127X_PARAM_SPI,          \
                                    .nss_pin        = SX127X_PARAM_SPI_NSS,      \
                                    .reset_pin      = SX127X_PARAM_RESET,        \
                                    .dio0_pin       = SX127X_PARAM_DIO0,         \
                                    .dio1_pin       = SX127X_PARAM_DIO1,         \
                                    .dio2_pin       = SX127X_PARAM_DIO2,         \
                                    .dio3_pin       = SX127X_PARAM_DIO3,         \
                                    .rx_switch_pin  = SX127X_PARAM_RX_SWITCH,    \
                                    .tx_switch_pin  = SX127X_PARAM_TX_SWITCH,    \
                                    .paselect       = SX127X_PARAM_PASELECT }
#else
#define SX127X_PARAMS             { .spi       = SX127X_PARAM_SPI,          \
                                    .nss_pin   = SX127X_PARAM_SPI_NSS,      \
                                    .reset_pin = SX127X_PARAM_RESET,        \
                                    .dio0_pin  = SX127X_PARAM_DIO0,         \
                                    .dio1_pin  = SX127X_PARAM_DIO1,         \
                                    .dio2_pin  = SX127X_PARAM_DIO2,         \
                                    .dio3_pin  = SX127X_PARAM_DIO3,         \
                                    .paselect  = SX127X_PARAM_PASELECT }
#endif
#endif
/**@}*/

/**
 * @brief   SX127X configuration
 */
static const sx127x_params_t sx127x_params[] =
{
    SX127X_PARAMS
};

#ifdef __cplusplus
}
#endif

#endif /* SX127X_PARAMS_H */
/** @} */

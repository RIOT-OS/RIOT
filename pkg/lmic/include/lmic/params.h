/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_lmic
 * @brief       LMIC default configuration definitions
 *
 *              If a SX1276 or SX1272 is configured for the board, then the
 *              board parameters or used. Otherwise the configuration for
 *              Nucleo and MBED LoRa modules is used.
 *
 * @{
 *
 * @file
 * @brief       LMIC default configuration definitions
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef LMIC_PARAMS_H
#define LMIC_PARAMS_H

#include "board.h"
#include "lmic.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef LMIC_PARAM_SPI
#ifdef SX127X_PARAM_SPI
#define LMIC_PARAM_SPI                           SX127X_PARAM_SPI
#else
#define LMIC_PARAM_SPI                           SPI_DEV(0)
#endif
#endif

#ifndef LMIC_PARAM_PIN_NSS
#ifdef SX127X_PARAM_SPI_NSS
#define LMIC_PARAM_PIN_NSS                       SX127X_PARAM_SPI_NSS
#else
#define LMIC_PARAM_PIN_NSS                       GPIO_PIN(1, 6)
#endif
#endif

#ifndef LMIC_PARAM_PIN_RXTX
#define LMIC_PARAM_PIN_RXTX                      GPIO_UNDEF
#endif

#ifndef LMIC_PARAM_PIN_RESET
#ifdef SX127X_PARAM_RESET
#define LMIC_PARAM_PIN_RESET                     SX127X_PARAM_RESET
#else
#define LMIC_PARAM_PIN_RESET                     GPIO_PIN(0, 0)
#endif
#endif

#ifndef LMIC_PARAM_PIN_DIO0
#ifdef SX127X_PARAM_DIO0
#define LMIC_PARAM_PIN_DIO0                      SX127X_PARAM_DIO0
#else
#define LMIC_PARAM_PIN_DIO0                      GPIO_PIN(0, 10)
#endif
#endif

#ifndef LMIC_PARAM_PIN_DIO1
#ifdef SX127X_PARAM_DIO1
#define LMIC_PARAM_PIN_DIO1                      SX127X_PARAM_DIO1
#else
#define LMIC_PARAM_PIN_DIO1                      GPIO_PIN(1, 3)
#endif
#endif

#ifndef LMIC_PARAM_PIN_DIO2
#ifdef SX127X_PARAM_DIO2
#define LMIC_PARAM_PIN_DIO2                      SX127X_PARAM_DIO2
#else
#define LMIC_PARAM_PIN_DIO2                      GPIO_PIN(1, 5)
#endif
#endif

#ifndef LMIC_PARAM_DEVEUI
#define LMIC_PARAM_DEVEUI                        { 0x05, 0x64, 0x00, 0xF0, \
                                                   0x7E, 0xD5, 0xB3, 0x70 }
#endif

#ifndef LMIC_PARAM_APPEUI
#define LMIC_PARAM_APPEUI                        { 0x61, 0x0A, 0x55, 0x9A, \
                                                   0xCB, 0x42, 0x5B, 0x00 }
#endif


#ifndef LMIC_PARAM_APPKEY
#define LMIC_PARAM_APPKEY                        { 0xC5, 0xA4, 0x5D, 0x22, \
                                                   0x89, 0x7D, 0xC0, 0x54, \
                                                   0xD2, 0x3A, 0x38, 0xD4, \
                                                   0xC4, 0x44, 0x43, 0xC9 }
#endif

#ifndef LMIC_PARAM_NWKSKEY
#define LMIC_PARAM_NWKSKEY                       { 0x4C, 0x78, 0x16, 0xAC, \
                                                   0xAA, 0x3C, 0x0B, 0xC6, \
                                                   0xD7, 0xB1, 0x7A, 0xAE, \
                                                   0xFB, 0x39, 0x92, 0x74 } 
#endif

#ifndef LMIC_PARAM_APPSKEY
#define LMIC_PARAM_APPSKEY                       { 0x62, 0x22, 0x05, 0xC8, \
                                                   0x49, 0xE8, 0xF3, 0x55, \
                                                   0x40, 0xCC, 0x6F, 0xC7, \
                                                   0x5A, 0x86, 0x51, 0xA6 }
#endif

#ifndef LMIC_PARAM_DEVADDR
#define LMIC_PARAM_DEVADDR                       0x26011D66
#endif

#ifdef __cplusplus
extern }
#endif

#endif /* LMIC_PARAMS_H */
/** @} */
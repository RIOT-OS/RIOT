/*
 * SPDX-FileCopyrightText: 2020 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_atwinc15x0
 * @{
 *
 * @file
 * @brief       Default configuration for the ATWINC15x0 WiFi netdev driver
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   SSID of the AP to be used.
 */
#ifndef WIFI_SSID
#define WIFI_SSID       "RIOT_AP"
#endif

/**
 * @brief   Passphrase used for the AP as clear text (max. 64 chars).
 */
#ifdef DOXYGEN
#define WIFI_PASS       "ThisistheRIOTporttoESP"
#endif

/**
 * @name    Set default configuration parameters
 *          Pins are adapted to Arduino Mega2560 boards.
 * @{
 */
#ifndef ATWINC15X0_PARAM_SPI
#define ATWINC15X0_PARAM_SPI            SPI_DEV(0)
#endif

#ifndef ATWINC15X0_PARAM_SPI_CLK
#define ATWINC15X0_PARAM_SPI_CLK        SPI_CLK_10MHZ
#endif

#ifndef ATWINC15X0_PARAM_SSN_PIN
#define ATWINC15X0_PARAM_SSN_PIN        GPIO_PIN(1, 4)       /* D10 (PB4) */
#endif

#ifndef ATWINC15X0_PARAM_RESET_PIN
#define ATWINC15X0_PARAM_RESET_PIN      GPIO_PIN(4, 3)       /* D5 (PE3) */
#endif

#ifndef ATWINC15X0_PARAM_IRQ_PIN
#define ATWINC15X0_PARAM_IRQ_PIN        GPIO_PIN(7, 4)       /* D7 (PH4) */
#endif

#ifndef ATWINC15X0_PARAM_CHIP_EN_PIN
#define ATWINC15X0_PARAM_CHIP_EN_PIN    GPIO_UNDEF
#endif

#ifndef ATWINC15X0_PARAM_WAKE_PIN
#define ATWINC15X0_PARAM_WAKE_PIN       GPIO_UNDEF
#endif

#ifndef ATWINC15X0_PARAMS
#define ATWINC15X0_PARAMS   {                                               \
                              .spi         = ATWINC15X0_PARAM_SPI,          \
                              .spi_clk     = ATWINC15X0_PARAM_SPI_CLK,      \
                              .ssn_pin     = ATWINC15X0_PARAM_SSN_PIN,      \
                              .reset_pin   = ATWINC15X0_PARAM_RESET_PIN,    \
                              .irq_pin     = ATWINC15X0_PARAM_IRQ_PIN,      \
                              .chip_en_pin = ATWINC15X0_PARAM_CHIP_EN_PIN,  \
                              .wake_pin    = ATWINC15X0_PARAM_WAKE_PIN,     \
                            }
#endif
/** @} */

/**
 * @brief   Allocate some memory to store the actual configuration
 */
static const atwinc15x0_params_t atwinc15x0_params[] =
{
    ATWINC15X0_PARAMS
};

#ifdef __cplusplus
}
#endif

/** @} */

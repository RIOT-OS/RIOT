/*
 * SPDX-FileCopyrightText: 2019 Juergen Fitschen <me@jue.yt>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_dose
 * @{
 * @file
 * @brief       Default configuration for the Differentially Operated Serial Ethernet driver
 *
 * @author      Juergen Fitschen <me@jue.yt>
 */

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the DOSE driver
 * @{
 */
#ifndef DOSE_PARAM_UART
#define DOSE_PARAM_UART         (UART_DEV(1))
#endif
#ifndef DOSE_PARAM_BAUDRATE
#define DOSE_PARAM_BAUDRATE     (115200)
#endif
#ifndef DOSE_PARAM_SENSE_PIN
#define DOSE_PARAM_SENSE_PIN    (GPIO_UNDEF)
#endif
#ifndef DOSE_PARAM_STANDBY_PIN
#define DOSE_PARAM_STANDBY_PIN  (GPIO_UNDEF)    /**< Standby/Silent mode */
#endif

#ifndef DOSE_PARAMS
#ifdef MODULE_PERIPH_UART_RXSTART_IRQ
#define DOSE_PARAMS             {                                        \
                                  .uart = DOSE_PARAM_UART,               \
                                  .baudrate = DOSE_PARAM_BAUDRATE,       \
                                  .standby_pin = DOSE_PARAM_STANDBY_PIN, \
                                }
#else /* MODULE_PERIPH_UART_RXSTART_IRQ */
#define DOSE_PARAMS             {                                        \
                                  .uart = DOSE_PARAM_UART,               \
                                  .baudrate = DOSE_PARAM_BAUDRATE,       \
                                  .standby_pin = DOSE_PARAM_STANDBY_PIN, \
                                  .sense_pin = DOSE_PARAM_SENSE_PIN,     \
                                }
#endif /* !MODULE_PERIPH_UART_RXSTART_IRQ */
#endif
/**@}*/

/**
 * @brief   DOSE configuration
 */
static const dose_params_t dose_params[] =
{
    DOSE_PARAMS
};

#ifdef __cplusplus
}
#endif

/** @} */

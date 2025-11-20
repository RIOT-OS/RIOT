/*
 * SPDX-FileCopyrightText: 2019 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_i-nucleo-lrwan1
 * @{
 *
 * @file
 * @brief       Board specific definitions for the ST I-NUCLEO-LRWAN1 board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include <stdint.h>

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    xtimer configuration
 * @{
 */
#define XTIMER_WIDTH        (16)
/** @} */

/**
 * @name    SX1272 configuration
 * @{
 **/
#define SX127X_PARAM_SPI_NSS                GPIO_PIN(PORT_A, 15)
#define SX127X_PARAM_RESET                  GPIO_PIN(PORT_A, 9)
#define SX127X_PARAM_DIO0                   GPIO_PIN(PORT_A, 2)
#define SX127X_PARAM_DIO1                   GPIO_PIN(PORT_A, 3)
#define SX127X_PARAM_DIO2                   GPIO_PIN(PORT_A, 5)
#define SX127X_PARAM_DIO3                   GPIO_PIN(PORT_A, 6)
#define SX127X_PARAM_PASELECT               (SX127X_PA_BOOST)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */

/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_lsn50
 * @{
 *
 * @file
 * @brief       Board specific definitions for the LSN50 board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef BOARD_H
#define BOARD_H

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
 * @name    sx1276 configuration
 * @{
 */
#define SX127X_PARAM_SPI                    SPI_DEV(0)
#define SX127X_PARAM_SPI_NSS                GPIO_PIN(PORT_A, 15)

#define SX127X_PARAM_RESET                  GPIO_PIN(PORT_B, 0)
#define SX127X_PARAM_DIO0                   GPIO_PIN(PORT_C, 13)
#define SX127X_PARAM_DIO1                   GPIO_PIN(PORT_B, 10)
#define SX127X_PARAM_DIO2                   GPIO_PIN(PORT_B, 11)
#define SX127X_PARAM_DIO3                   GPIO_PIN(PORT_B, 8)
#define SX127X_PARAM_DIO4                   GPIO_PIN(PORT_B, 9)
#define SX127X_PARAM_DIO5                   GPIO_PIN(PORT_B, 1)
#define SX127X_PARAM_PASELECT               (SX127X_PA_BOOST)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */

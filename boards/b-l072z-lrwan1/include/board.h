/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_b-l072z-lrwan1
 * @{
 *
 * @file
 * @brief       Board specific definitions for the ST B-L072Z-LRWAN1 board
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
#define SX127X_PARAM_SPI                    (SPI_DEV(1))
#define SX127X_PARAM_SPI_NSS                GPIO_PIN(PORT_A, 15)

#define SX127X_PARAM_RESET                  GPIO_PIN(PORT_C, 0)
#define SX127X_PARAM_DIO0                   GPIO_PIN(PORT_B, 4)
#define SX127X_PARAM_DIO1                   GPIO_PIN(PORT_B, 1)
#define SX127X_PARAM_DIO2                   GPIO_PIN(PORT_B, 0)
#define SX127X_PARAM_DIO3                   GPIO_PIN(PORT_C, 13)

#define RADIO_TCXO_VCC_PIN                  GPIO_PIN(PORT_A, 12)
/** @} */

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN_NUM        5
#define LED0_PORT           GPIO_PORT_A /**< GPIO port of LED 0 */
#define LED0_PORT_NUM       PORT_A

#define LED1_PIN_NUM        5
#define LED1_PORT           GPIO_PORT_B /**< GPIO port of LED 1 */
#define LED1_PORT_NUM       PORT_B

#define LED2_PIN_NUM        6
#define LED2_PORT           GPIO_PORT_B /**< GPIO port of LED 2 */
#define LED2_PORT_NUM       PORT_B

#define LED3_PIN_NUM        7
#define LED3_PORT           GPIO_PORT_B /**< GPIO port of LED 3 */
#define LED3_PORT_NUM       PORT_B
/** @} */

/**
 * @name    User button pin configuration
 * @{
 */
#define BTN0_PIN            GPIO_PIN(PORT_B, 2)     /**< User button pin */
#define BTN0_MODE           GPIO_IN_PU              /**< User button pin mode */
/** @} */

#ifdef __cplusplus
}
#endif

#include "stm32_leds.h"

/** @} */
#endif /* BOARD_H */

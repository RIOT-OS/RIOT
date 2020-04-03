/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_sodaq-explorer
 * @{
 *
 * @file
 * @brief       Board specific definitions for the SODAQ ExpLoRer board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "board_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(PA, 21)

#define LED0_PORT           PORT->Group[PA]
#define LED0_MASK           (1 << 21)

#define LED0_ON             (LED0_PORT.OUTSET.reg = LED0_MASK)
#define LED0_OFF            (LED0_PORT.OUTCLR.reg = LED0_MASK)
#define LED0_TOGGLE         (LED0_PORT.OUTTGL.reg = LED0_MASK)
/** @} */

/**
 * @name    User button
 */
#define BTN0_PIN            GPIO_PIN(PA, 14)
#define BTN0_MODE           GPIO_IN_PU
/** @} */

/**
 * @name    RN2483 configuration
 * @{
 */
#define RN2XX3_PARAM_UART         UART_DEV(2)
#define RN2XX3_PARAM_PIN_RESET    GPIO_PIN(PA, 7)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */

/*
 * Copyright (C) 2017 Kees Bakker, SODAQ
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_sodaq-one
 * @{
 *
 * @file
 * @brief       Board specific definitions for the SODAQ ONE board
 *
 * @author      Kees Bakker <kees@sodaq.com>
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
#define LED0_PIN            GPIO_PIN(PA, 15)

#define LED0_PORT           PORT->Group[PA]
#define LED0_MASK           (1 << 15)

#define LED0_OFF            (LED0_PORT.OUTSET.reg = LED0_MASK)
#define LED0_ON             (LED0_PORT.OUTCLR.reg = LED0_MASK)
#define LED0_TOGGLE         (LED0_PORT.OUTTGL.reg = LED0_MASK)

#define LED1_PIN            GPIO_PIN(PB, 10)

#define LED1_PORT           PORT->Group[PB]
#define LED1_MASK           (1 << 10)

#define LED1_OFF            (LED1_PORT.OUTSET.reg = LED1_MASK)
#define LED1_ON             (LED1_PORT.OUTCLR.reg = LED1_MASK)
#define LED1_TOGGLE         (LED1_PORT.OUTTGL.reg = LED1_MASK)

#define LED2_PIN            GPIO_PIN(PB, 11)

#define LED2_PORT           PORT->Group[PB]
#define LED2_MASK           (1 << 11)

#define LED2_OFF            (LED2_PORT.OUTSET.reg = LED2_MASK)
#define LED2_ON             (LED2_PORT.OUTCLR.reg = LED2_MASK)
#define LED2_TOGGLE         (LED2_PORT.OUTTGL.reg = LED2_MASK)
/** @} */

/**
 * @name    User button
 */
#define BTN0_PIN            GPIO_PIN(PA, 16)
#define BTN0_MODE           GPIO_IN
/** @} */

/**
 * @name    GPS Time Pulse
 */
#define GPS_TIMEPULSE_PIN   GPIO_PIN(PA, 14)
#define GPS_TIMEPULSE_MODE  GPIO_IN
/** @} */

/**
 * @name    GPS Enable
 * @{
 */
#define GPS_ENABLE_PIN      GPIO_PIN(PA, 18)

#define GPS_ENABLE_PORT     PORT->Group[PA]
#define GPS_ENABLE_MASK     (1 << 18)

#define GPS_ENABLE_ON       (GPS_ENABLE_PORT.OUTSET.reg = GPS_ENABLE_MASK)
#define GPS_ENABLE_OFF      (GPS_ENABLE_PORT.OUTCLR.reg = GPS_ENABLE_MASK)
/** @} */

/**
 * @name    LORA Reset
 * @{
 */
#define LORA_RESET_PIN      GPIO_PIN(PA, 4)

#define LORA_RESET_PORT     PORT->Group[PA]
#define LORA_RESET_MASK     (1 << 4)

#define LORA_RESET_OFF      (LORA_RESET_PORT.OUTSET.reg = LORA_RESET_MASK)
#define LORA_RESET_ON       (LORA_RESET_PORT.OUTCLR.reg = LORA_RESET_MASK)
#define LORA_RESET_TOGGLE   (LORA_RESET_PORT.OUTTGL.reg = LORA_RESET_MASK)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */

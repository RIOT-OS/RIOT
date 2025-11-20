/*
 * SPDX-FileCopyrightText: 2018 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_sodaq-sara-aff
 * @{
 *
 * @file
 * @brief       Board specific definitions for the SODAQ SARA AFF boards
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */

#include "cpu.h"
#include "board_common.h"
#include "periph/gpio.h"

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

#define LED0_OFF            (LED0_PORT.OUTCLR.reg = LED0_MASK)
#define LED0_ON             (LED0_PORT.OUTSET.reg = LED0_MASK)
#define LED0_TOGGLE         (LED0_PORT.OUTTGL.reg = LED0_MASK)

#define LED1_PIN            GPIO_PIN(PA, 12)

#define LED1_PORT           PORT->Group[PA]
#define LED1_MASK           (1 << 12)

#define LED1_OFF            (LED1_PORT.OUTSET.reg = LED1_MASK)
#define LED1_ON             (LED1_PORT.OUTCLR.reg = LED1_MASK)
#define LED1_TOGGLE         (LED1_PORT.OUTTGL.reg = LED1_MASK)

#define LED_RED_PIN         LED1_PIN
#define LED_RED_OFF         LED1_OFF
#define LED_RED_ON          LED1_ON
#define LED_RED_TOGGLE      LED1_TOGGLE

#define LED2_PIN            GPIO_PIN(PB, 15)

#define LED2_PORT           PORT->Group[PB]
#define LED2_MASK           (1 << 15)

#define LED2_OFF            (LED2_PORT.OUTSET.reg = LED2_MASK)
#define LED2_ON             (LED2_PORT.OUTCLR.reg = LED2_MASK)
#define LED2_TOGGLE         (LED2_PORT.OUTTGL.reg = LED2_MASK)

#define LED_GREEN_PIN         LED2_PIN
#define LED_GREEN_OFF         LED2_OFF
#define LED_GREEN_ON          LED2_ON
#define LED_GREEN_TOGGLE      LED2_TOGGLE

#define LED3_PIN            GPIO_PIN(PA, 13)

#define LED3_PORT           PORT->Group[PA]
#define LED3_MASK           (1 << 13)

#define LED3_OFF            (LED3_PORT.OUTSET.reg = LED3_MASK)
#define LED3_ON             (LED3_PORT.OUTCLR.reg = LED3_MASK)
#define LED3_TOGGLE         (LED3_PORT.OUTTGL.reg = LED3_MASK)

#define LED_BLUE_PIN         LED3_PIN
#define LED_BLUE_OFF         LED3_OFF
#define LED_BLUE_ON          LED3_ON
#define LED_BLUE_TOGGLE      LED3_TOGGLE
/** @} */

/**
 * @name    GPS Time Pulse
 * @{
 */
#define GPS_TIMEPULSE_PIN   GPIO_PIN(PA, 7)
#define GPS_TIMEPULSE_MODE  GPIO_IN
/** @} */

/**
 * @name    GPS Enable
 * @{
 */
#define GPS_ENABLE_PIN      GPIO_PIN(PA, 28)

#define GPS_ENABLE_PORT     PORT->Group[PA]
#define GPS_ENABLE_MASK     (1 << 28)

#define GPS_ENABLE_ON       (GPS_ENABLE_PORT.OUTSET.reg = GPS_ENABLE_MASK)
#define GPS_ENABLE_OFF      (GPS_ENABLE_PORT.OUTCLR.reg = GPS_ENABLE_MASK)
/** @} */

/**
 * @name    NB-IoT SARA module
 * @{
 */
#define NB_IOT_ENABLE_PIN      GPIO_PIN(PA, 27)

#define NB_IOT_ENABLE_PORT     PORT->Group[PA]
#define NB_IOT_ENABLE_MASK     (1 << 27)

#define NB_IOT_ENABLE          (NB_IOT_ENABLE_PORT.OUTSET.reg = NB_IOT_ENABLE_MASK)
#define NB_IOT_DISABLE         (NB_IOT_ENABLE_PORT.OUTCLR.reg = NB_IOT_ENABLE_MASK)

#define NB_IOT_RESET_PIN       GPIO_PIN(PB, 14)

#define NB_IOT_RESET_PORT      PORT->Group[PB]
#define NB_IOT_RESET_MASK      (1 << 14)

#define NB_IOT_RESET_ON        (NB_IOT_RESET_PORT.OUTSET.reg = NB_IOT_RESET_MASK)
#define NB_IOT_RESET_OFF       (NB_IOT_RESET_PORT.OUTCLR.reg = NB_IOT_RESET_MASK)

#define NB_IOT_RESET_PIN       GPIO_PIN(PB, 14)

#define NB_IOT_RESET_PORT      PORT->Group[PB]
#define NB_IOT_RESET_MASK      (1 << 14)

#define NB_IOT_RESET_ON        (NB_IOT_RESET_PORT.OUTSET.reg = NB_IOT_RESET_MASK)
#define NB_IOT_RESET_OFF       (NB_IOT_RESET_PORT.OUTCLR.reg = NB_IOT_RESET_MASK)

#define NB_IOT_TX_EN_PIN       GPIO_PIN(PB, 13)

#define NB_IOT_TX_EN_PORT      PORT->Group[PB]
#define NB_IOT_TX_EN_MASK      (1 << 13)

#define NB_IOT_TX_EN_ON        (NB_IOT_TX_EN_PORT.OUTSET.reg = NB_IOT_TX_EN_MASK)
#define NB_IOT_TX_EN_OFF       (NB_IOT_TX_EN_PORT.OUTCLR.reg = NB_IOT_TX_EN_MASK)

#define NB_IOT_TOGGLE_PIN       GPIO_PIN(PB, 17)

#define NB_IOT_TOGGLE_PORT      PORT->Group[PB]
#define NB_IOT_TOGGLE_MASK      (1 << 17)

#define NB_IOT_TOGGLE_ON        (NB_IOT_TOGGLE_PORT.OUTSET.reg = NB_IOT_TOGGLE_MASK)
#define NB_IOT_TOGGLE_OFF       (NB_IOT_TOGGLE_PORT.OUTCLR.reg = NB_IOT_TOGGLE_MASK)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */

/*
 * Copyright (C) 2019 Kees Bakker, SODAQ
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_sodaq-sara-sff
 * @{
 *
 * @file
 * @brief       Board specific definitions for the SODAQ SARA SFF board
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
#define LED0_PIN            GPIO_PIN(PA, 16)
#define LED0_PORT           PORT->Group[PA]
#define LED0_MASK           (1 << 16)

#define LED0_OFF            (LED0_PORT.OUTSET.reg = LED0_MASK)
#define LED0_ON             (LED0_PORT.OUTCLR.reg = LED0_MASK)
#define LED0_TOGGLE         (LED0_PORT.OUTTGL.reg = LED0_MASK)

#define LED_GREEN_PIN       LED0_PIN
#define LED_GREEN_OFF       LED0_OFF
#define LED_GREEN_ON        LED0_ON
#define LED_GREEN_TOGGLE    LED0_TOGGLE

#define LED1_PIN            GPIO_PIN(PA, 14)
#define LED1_PORT           PORT->Group[PA]
#define LED1_MASK           (1 << 14)

#define LED1_OFF            (LED1_PORT.OUTSET.reg = LED1_MASK)
#define LED1_ON             (LED1_PORT.OUTCLR.reg = LED1_MASK)
#define LED1_TOGGLE         (LED1_PORT.OUTTGL.reg = LED1_MASK)

#define LED_RED_PIN         LED1_PIN
#define LED_RED_OFF         LED1_OFF
#define LED_RED_ON          LED1_ON
#define LED_RED_TOGGLE      LED1_TOGGLE

#define LED2_PIN            GPIO_PIN(PA, 15)
#define LED2_PORT           PORT->Group[PA]
#define LED2_MASK           (1 << 15)

#define LED2_OFF            (LED2_PORT.OUTSET.reg = LED2_MASK)
#define LED2_ON             (LED2_PORT.OUTCLR.reg = LED2_MASK)
#define LED2_TOGGLE         (LED2_PORT.OUTTGL.reg = LED2_MASK)

#define LED_BLUE_PIN         LED2_PIN
#define LED_BLUE_OFF         LED2_OFF
#define LED_BLUE_ON          LED2_ON
#define LED_BLUE_TOGGLE      LED2_TOGGLE
/** @} */

/**
 * @name    User button
 * @{
 */
#define BTN0_PIN            GPIO_PIN(PB, 8)
#define BTN0_MODE           GPIO_IN
/** @} */

/**
 * @name    GPS Time Pulse
 * @{
 */
#define GPS_TIMEPULSE_PIN   GPIO_PIN(PA, 17)
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
 * @name    SARA (UBlox) Enable
 *
 * With this pin the power supply can be switched on or off.
 * @{
 */
#define SARA_ENABLE_PIN     GPIO_PIN(PA, 4)

#define SARA_ENABLE_PORT    PORT->Group[PA]
#define SARA_ENABLE_MASK    (1 << 4)

#define SARA_ENABLE_ON      (SARA_ENABLE_PORT.OUTSET.reg = SARA_ENABLE_MASK)
#define SARA_ENABLE_OFF     (SARA_ENABLE_PORT.OUTCLR.reg = SARA_ENABLE_MASK)
/** @} */

/**
 * @name    SARA (UBlox) TX Enable
 *
 * With this pin level shifters can be switched on or off.
 * @{
 */
#define SARA_TX_ENABLE_PIN  GPIO_PIN(PA, 27)

#define SARA_TX_ENABLE_PORT PORT->Group[PA]
#define SARA_TX_ENABLE_MASK (1 << 27)

#define SARA_TX_ENABLE_ON   (SARA_TX_ENABLE_PORT.OUTSET.reg = SARA_TX_ENABLE_MASK)
#define SARA_TX_ENABLE_OFF  (SARA_TX_ENABLE_PORT.OUTCLR.reg = SARA_TX_ENABLE_MASK)
/** @} */

/**
 * @name    SARA_R4XX PWR_ON
 *
 * This is the PWR_ON pin of the UBlox. It can be used to switch the unit on or off
 * by lowering this pin (at least for 0.15 seconds). This is needed to switch on from
 * power-off mode and from PSM deep sleep.
 * @{
 */
#define SARA_R4XX_PWR_ON_PIN    GPIO_PIN(PB, 10)

#define SARA_R4XX_PWR_ON_PORT   PORT->Group[PB]
#define SARA_R4XX_PWR_ON_MASK   (1 << 10)

#define SARA_R4XX_PWR_ON_ON     (SARA_R4XX_PWR_ON_PORT.OUTSET.reg = SARA_R4XX_PWR_ON_MASK)
#define SARA_R4XX_PWR_ON_OFF    (SARA_R4XX_PWR_ON_PORT.OUTCLR.reg = SARA_R4XX_PWR_ON_MASK)
/** @} */

#define SARA_STATUS_PIN     GPIO_PIN(PA, 28) /**< This is the V_INT of the board */

/**
 * @name    INT_MAG
 *
 * The INT_MAG pin of the accelerometer (LSM303AGRTR)
 * @{
 */
#define INT_MAG_PIN         GPIO_PIN(PA, 19)
#define INT_MAG_MODE        GPIO_IN
/** @} */

/**
 * @name    INT1_XL
 *
 * The INT1_XL pin of the accelerometer (LSM303AGRTR)
 * @{
 */
#define INT1_XL_PIN         GPIO_PIN(PA, 20)
#define INT1_XL_MODE        GPIO_IN
/** @} */

/**
 * @name    INT2_XL
 *
 * The INT2_XL pin of the accelerometer (LSM303AGRTR)
 * @{
 */
#define INT2_XL_PIN         GPIO_PIN(PA, 21)
#define INT2_XL_MODE        GPIO_IN
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */

/*
 * SPDX-FileCopyrightText: 2026 Baptiste Le Duc <baptiste.leduc@etik.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_seeedstudio-sensecap-t1000e
 * @{
 *
 * @file
 * @brief       Board specific definitions for the SeeedStudio SenseCAP T1000-E
 *
 * @author      Baptiste Le Duc <baptiste.leduc@etik.com>
 */

#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Default board power-rail configuration
 * @{
 */
#ifndef CONFIG_T1000E_ENABLE_3V3
#  define CONFIG_T1000E_ENABLE_3V3        1     /**< enable 3.3V rail at boot */
#endif
#ifndef CONFIG_T1000E_ENABLE_SENSOR
#  define CONFIG_T1000E_ENABLE_SENSOR     1     /**< enable sensor power rail at boot */
#endif
#ifndef CONFIG_T1000E_ENABLE_ACC
#  define CONFIG_T1000E_ENABLE_ACC        1     /**< enable accelerometer power rail at boot */
#endif
/** @} */

/**
 * @name    Button pin definitions
 * @{
 */
#define BTN0_PIN            GPIO_PIN(0, 6)
#define BTN0_MODE           GPIO_IN_PD
#define BTN0_INT_FLANK      GPIO_RISING
/** @} */

/**
 * @name    LED (on-board) configuration
 * @{
 */
#define LED0_PIN            GPIO_PIN(0, 24)
#define LED0_ON             gpio_set(LED0_PIN)
#define LED0_OFF            gpio_clear(LED0_PIN)
#define LED0_TOGGLE         gpio_toggle(LED0_PIN)
/** @} */

/**
 * @name    Buzzer configuration
 * @{
 */
#define BUZZER_EN_PIN       GPIO_PIN(1, 5)    /**< Buzzer power enable pin */
#define BUZZER_PIN          GPIO_PIN(0, 25)   /**< Buzzer PWM signal pin */
/** @} */

/**
 * @name    Power enable pins
 * @{
 */
#define T1000E_SENSOR_EN_PIN        GPIO_PIN(0, 4)  /**< Sensor power enable pin */
#define T1000E_3V3_EN_PIN           GPIO_PIN(1, 6)  /**< 3.3V rail enable pin */
#define T1000E_3V3_ACC_EN_PIN       GPIO_PIN(1, 7)  /**< 3.3V accelerometer rail enable pin */
/** @} */

/**
 * @name    LR1110 LoRa transceiver control pins
 * @{
 */
#define T1000E_LORA_NSS_PIN         GPIO_PIN(0, 12) /**< LR1110 SPI chip select pin */
#define T1000E_LORA_RESET_PIN       GPIO_PIN(1, 10) /**< LR1110 reset pin */
#define T1000E_LORA_IRQ_PIN         GPIO_PIN(1, 1)  /**< LR1110 interrupt request pin */
#define T1000E_LORA_BUSY_PIN        GPIO_PIN(0, 7)  /**< LR1110 busy pin */
/** @} */

/**
 * @name    GNSS (AG3335) control pins
 * @{
 */
#define T1000E_GPS_EN_PIN           GPIO_PIN(1, 11) /**< GNSS module power enable pin */
#define T1000E_GPS_RESET_PIN        GPIO_PIN(1, 15) /**< GNSS module reset pin */
#define T1000E_GPS_VRTC_EN_PIN      GPIO_PIN(0, 8)  /**< GNSS RTC backup voltage enable pin */
#define T1000E_GPS_SLEEP_INT_PIN    GPIO_PIN(1, 12) /**< GNSS sleep interrupt pin */
#define T1000E_GPS_RTC_INT_PIN      GPIO_PIN(0, 15) /**< GNSS RTC interrupt pin */
#define T1000E_GPS_RESETB_OUT_PIN   GPIO_PIN(1, 14) /**< GNSS reset-B output pin */
/** @} */

/**
 * @name    Battery and charging detection pins
 * @{
 */
#define T1000E_BAT_ADC_PIN          GPIO_PIN(0, 2)  /**< Battery voltage ADC input pin */
#define T1000E_CHRG_DETECT_PIN      GPIO_PIN(1, 3)  /**< Charger connected detection pin */
#define T1000E_EXT_PWR_DETECT_PIN   GPIO_PIN(0, 5)  /**< External power detection pin */
/** @} */

/**
 * @name    Accelerometer (QMA6100P) interrupt pin
 * @{
 */
#define T1000E_ACC_INT_PIN          GPIO_PIN(1, 2)  /**< Accelerometer interrupt pin */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */

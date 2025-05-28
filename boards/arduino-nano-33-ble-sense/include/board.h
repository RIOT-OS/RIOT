/*
 * Copyright (C) 2024 Méwen Berthelot
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_arduino-nano-33-ble-sense
 * @{
 *
 * @file
 * @brief       Board specific configuration for the Arduino Nano 33 BLE sense
 *
 * @author      Méwen Berthelot <berthelotmewen@gmail.com>
 */

#include "cpu.h"
#include "board_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LEDs pin configuration
 * @{
 */
#define LED0_PIN                GPIO_PIN(0, 13)
#define LED0_MASK               (1 << 13)
#define LED0_ON                 (NRF_P0->OUTCLR = LED0_MASK)
#define LED0_OFF                (NRF_P0->OUTSET = LED0_MASK)
#define LED0_TOGGLE             (NRF_P0->OUT   ^= LED0_MASK)

#define LED1_PIN                GPIO_PIN(0, 24)
#define LED1_MASK               (1 << 24)
#define LED1_ON                 (NRF_P0->OUTCLR = LED1_MASK)
#define LED1_OFF                (NRF_P0->OUTSET = LED1_MASK)
#define LED1_TOGGLE             (NRF_P0->OUT   ^= LED1_MASK)

#define LED2_PIN                GPIO_PIN(0, 16)
#define LED2_MASK               (1 << 16)
#define LED2_ON                 (NRF_P0->OUTCLR = LED2_MASK)
#define LED2_OFF                (NRF_P0->OUTSET = LED2_MASK)
#define LED2_TOGGLE             (NRF_P0->OUT   ^= LED2_MASK)

#define LED3_PIN                GPIO_PIN(0, 6)
#define LED3_MASK               (1 << 6)
#define LED3_ON                 (NRF_P0->OUTCLR = LED3_MASK)
#define LED3_OFF                (NRF_P0->OUTSET = LED3_MASK)
#define LED3_TOGGLE             (NRF_P0->OUT   ^= LED3_MASK)

#define LED4_PIN                GPIO_PIN(1, 9)
#define LED4_MASK               (1 << 9)
#define LED4_ON                 (NRF_P1->OUTCLR = LED4_MASK)
#define LED4_OFF                (NRF_P1->OUTSET = LED4_MASK)
#define LED4_TOGGLE             (NRF_P1->OUT   ^= LED4_MASK)
/** @} */

/**
 * @name    Internal powering pin definition
 * @{
 */
#define VDD                     GPIO_PIN(0, 22) /**< Internal VDD pin */
/** @} */

/**
 * @name    Internal I2C pins definition
 * @{
 */
#define SCL1                     GPIO_PIN(0, 15) /**< Internal SCL I2C pin */
#define SDA1                     GPIO_PIN(0, 14) /**< Internal SDA I2C pin */
/** @} */

/**
 * @name    HTS221 sensor configuration
 * @{
 */
#define HTS221_PARAM_I2C        I2C_DEV(1) /**< I2C device */
/** @} */

/**
 * @name    LPS22HB sensor configuration
 * @{
 */
#define LPSXXX_PARAM_ADDR       (0x5c) /**< I2C address */
#define LPSXXX_PARAM_I2C        I2C_DEV(1) /**< I2C device */
/** @} */

/**
 * @name    APDS9960 sensor configuration
 * @{
 */
#define APDS99XX_PARAM_DEV      I2C_DEV(1) /**< I2C device */
#define APDS99XX_PARAM_INT_PIN  GPIO_PIN(0, 19) /**< Interrupt pin */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */

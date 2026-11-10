/*
 * SPDX-FileCopyrightText: 2026 Luis A. Ruiz
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_stm32f769i-disco
 * @{
 *
 * @file
 * @brief       Mapping from MCU pins to Arduino pins
 *
 * @author      Luis A. Ruiz <luisan00@hotmail.com>
 */

#include "periph/adc.h"
#include "periph/dac.h"
#include "periph/gpio.h"
#include "periph/i2c.h"
#include "periph/pwm.h"
#include "periph/spi.h"
#include "periph/uart.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Arduino's UART devices
 * @{
 */
#define ARDUINO_UART_D0D1       UART_DEV(2)
/** @} */

/**
 * @name    Arduino's SPI buses
 * @{
 */
#define ARDUINO_SPI_D11D12D13   SPI_DEV(0)
/** @} */

/**
 * @name    Arduino's I2C buses
 * @{
 */
#define ARDUINO_I2C_UNO         I2C_DEV(0)
/** @} */

/**
 * @name    Mapping of MCU pins to Arduino pins
 * @{
 */
#define ARDUINO_PIN_0           GPIO_PIN(PORT_C, 7)
#define ARDUINO_PIN_1           GPIO_PIN(PORT_C, 6)
#define ARDUINO_PIN_2           GPIO_PIN(PORT_J, 1)
#define ARDUINO_PIN_3           GPIO_PIN(PORT_F, 6)
#define ARDUINO_PIN_4           GPIO_PIN(PORT_J, 0)
#define ARDUINO_PIN_5           GPIO_PIN(PORT_C, 8)
#define ARDUINO_PIN_6           GPIO_PIN(PORT_F, 7)
#define ARDUINO_PIN_7           GPIO_PIN(PORT_J, 3)

#define ARDUINO_PIN_8           GPIO_PIN(PORT_J, 4)
#define ARDUINO_PIN_9           GPIO_PIN(PORT_H, 6)
#define ARDUINO_PIN_10          GPIO_PIN(PORT_A, 11)
#define ARDUINO_PIN_11          GPIO_PIN(PORT_B, 15)
#define ARDUINO_PIN_12          GPIO_PIN(PORT_B, 14)
#define ARDUINO_PIN_13          GPIO_PIN(PORT_A, 12)    /* on-board LED LD3 */
#define ARDUINO_PIN_14          GPIO_PIN(PORT_B, 9)
#define ARDUINO_PIN_15          GPIO_PIN(PORT_B, 8)

/* analog pins as digital pins: */
#define ARDUINO_PIN_16          GPIO_PIN(PORT_A, 6)
#define ARDUINO_PIN_17          GPIO_PIN(PORT_A, 4)
#define ARDUINO_PIN_18          GPIO_PIN(PORT_C, 2)
#define ARDUINO_PIN_19          GPIO_PIN(PORT_F, 10)
#define ARDUINO_PIN_20          GPIO_PIN(PORT_F, 8)
#define ARDUINO_PIN_21          GPIO_PIN(PORT_F, 9)

#define ARDUINO_PIN_LAST        21
/** @} */

/**
 * @name    Aliases for analog pins
 * @{
 */
#define ARDUINO_PIN_A0          ARDUINO_PIN_16
#define ARDUINO_PIN_A1          ARDUINO_PIN_17
#define ARDUINO_PIN_A2          ARDUINO_PIN_18
#define ARDUINO_PIN_A3          ARDUINO_PIN_19
#define ARDUINO_PIN_A4          ARDUINO_PIN_20
#define ARDUINO_PIN_A5          ARDUINO_PIN_21
/** @} */

/**
 * @name    Mapping of Arduino analog pins to RIOT ADC lines
 * @{
 */
#define ARDUINO_A0              ADC_LINE(0)
#define ARDUINO_A1              ADC_LINE(1)
#define ARDUINO_A2              ADC_LINE(2)
#define ARDUINO_A3              ADC_LINE(3)
#define ARDUINO_A4              ADC_LINE(4)
#define ARDUINO_A5              ADC_LINE(5)

#define ARDUINO_ANALOG_PIN_LAST 5
/** @} */

/**
 * @name    Mapping of Arduino DAC pins to RIOT DAC lines
 *
 * The DAC output is on the A1 pin.
 * @{
 */
#define ARDUINO_DAC0            DAC_LINE(0)

#define ARDUINO_DAC_PIN_LAST    0
/** @} */

/**
 * @name    Mapping of Arduino pins to RIOT PWM devices
 * @{
 */
#define ARDUINO_PIN_3_PWM_DEV   PWM_DEV(0)
#define ARDUINO_PIN_3_PWM_CHAN  0

#define ARDUINO_PIN_5_PWM_DEV   PWM_DEV(1)
#define ARDUINO_PIN_5_PWM_CHAN  0

#define ARDUINO_PIN_6_PWM_DEV   PWM_DEV(2)
#define ARDUINO_PIN_6_PWM_CHAN  0

#define ARDUINO_PIN_9_PWM_DEV   PWM_DEV(3)
#define ARDUINO_PIN_9_PWM_CHAN  0

#define ARDUINO_PIN_10_PWM_DEV  PWM_DEV(4)
#define ARDUINO_PIN_10_PWM_CHAN 0

#define ARDUINO_PIN_11_PWM_DEV  PWM_DEV(3)
#define ARDUINO_PIN_11_PWM_CHAN 1
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */

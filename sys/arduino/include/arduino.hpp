/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_arduino_api Arduino API
 * @ingroup     sys_arduino
 * @brief       Implementation of the Arduino API in RIOT
 * @{
 *
 * @file
 * @brief       Main interface definition of the Arduino API
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef ARDUINO_HPP
#define ARDUINO_HPP

extern "C" {
#include <stdint.h>
#include "periph/gpio.h"
#include "arduino_board.h"
}

#include "serialport.hpp"

/**
 * @brief    Arduino boolean data type definion
 */
typedef bool boolean;

/**
 * @brief    Arduino byte data type definion
 */
typedef uint8_t byte;

/**
 * @brief   Possible pin configurations
 */
enum {
    INPUT,              /**< configure pin as input */
    OUTPUT,             /**< configure pin as output */
    INPUT_PULLUP        /**< configure pin as input with pull-up resistor */
};

/**
 * @brief   Possible pin states
 */
enum {
    LOW = 0,            /**< pin is cleared */
    HIGH = 1            /**< pin is set */
};

#ifndef ARDUINO_UART_DEV
/**
 * @brief UART device to use for Arduino serial
 */
#define ARDUINO_UART_DEV        UART_DEV(0)
#endif

/**
 * @brief   Primary serial port (mapped to ARDUINO_UART_DEV)
 */
static SerialPort Serial(ARDUINO_UART_DEV);

/**
 * @brief   Configure a pin as either input or output
 *
 * @param[in] pin       pin to configure
 * @param[in] mode      mode to set the pin to
 */
void pinMode(int pin, int mode);

/**
 * @brief   Set the value for the given pin
 *
 * @param[in] pin       pin to set
 * @param[in] state     HIGH or LOW
 */
void digitalWrite(int pin, int state);

/**
 * @brief   Read the current state of the given pin
 *
 * @param[in] pin       pin to read
 *
 * @return  state of the given pin, HIGH or LOW
 */
int digitalRead(int pin);

/**
 * @brief   Sleep for a given amount of time [milliseconds]
 *
 * @param[in] msec      number of milliseconds to sleep
 */
void delay(unsigned long msec);

/**
 * @brief   Sleep for a given amount of time [microseconds]
 *
 * @param[in] usec      number of microseconds to sleep
 */
void delayMicroseconds(unsigned long usec);

/**
 * @brief   Returns the number of microseconds since start
 *
 * @return value of microseconds since start
 */
unsigned long micros();

/**
 * @brief   Returns the number of milliseconds since start
 *
 * @return value of milliseconds since start
 */
unsigned long millis();

#if MODULE_PERIPH_ADC || DOXYGEN
/**
 * @brief   Read the current value of the given analog pin
 *
 * @param[in] pin       pin to read
 *
 * @return a value between 0 to 1023 that is proportionnal
 * to the voltage applied to the pin
 */
int analogRead(int pin);
#endif

#if MODULE_PERIPH_PWM || DOXYGEN
/**
 * @brief   PWM default frequency
 *
 * Can be overridden at board level in arduino_board.h.
 *
 * See table from https://www.arduino.cc/reference/en/language/functions/analog-io/analogwrite/
 * for reference values.
 */
#ifndef ARDUINO_PWM_FREQU
#define ARDUINO_PWM_FREQU           (1000U)
#endif

/**
 * @brief   PWM mode
 */
#define ARDUINO_PWM_MODE            PWM_LEFT

/**
 * @brief   PWM steps
 */
#define ARDUINO_PWM_STEPS           (256U)

/**
 * @brief   Write an analog value to a pin
 *
 * @param[in] pin       pin to write
 * @param[in] value     duty cycle value, between 0 and 255
 */
void analogWrite(int pin, int value);
#endif

#endif /* ARDUINO_HPP */
/** @} */

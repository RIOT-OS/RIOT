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

#ifndef ARDUINO_H
#define ARDUINO_H

extern "C" {
#include "periph/gpio.h"
#include "arduino_board.h"
}

#include "serialport.hpp"

/**
 * @brief   Possible pin configurations
 */
enum {
    INPUT,              /**< configure pin as input */
    OUTPUT,             /**< configure pin as output */
    INPUT_PULLUP,       /**< configure pin as input with pull-up resistor */
};

/**
 * @brief   Possible pin states
 */
enum {
    LOW = 0,            /**< pin is cleared */
    HIGH = 1,           /**< pin is set */
};

/**
 * @brief   Primary serial port (mapped to UART_DEV(0))
 */
static SerialPort Serial(UART_DEV(0));

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
void delay(int msec);

#endif /* ARDUINO_H */
/** @} */

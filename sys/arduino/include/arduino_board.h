/*
 * Copyright (C) 2016 Freie Universität Berlin
 *               2017 Thomas Perrot <thomas.perrot@tupi.fr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_arduino
 * @{
 *
 * @file
 * @brief       Configuration of the Arduino API for Arduino Atmega boards
 *
 * @author      Hauke Petersen  <hauke.petersen@fu-berlin.de>
 * @author      Laurent Navet   <laurent.navet@gmail.com>
 * @author      Thomas Perrot   <thomas.perrot@tupi.fr>
 */

#ifndef ARDUINO_BOARD_H
#define ARDUINO_BOARD_H

#include "arduino_board_analog.h"
#include "arduino_board_pins.h"
#include "arduino_board_pwm.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief F_CPU defines the CPU frequency in Hz.
 *
 * This is used in AVR's libc delay.h and setbaud.h
 *
 * In RIOT delay() has a different implementation using ztimer, and F_CPU is
 * already defined when using setbaud.h (see cpu/atmega_common/periph/uart.c)
 *
 * However Arduino libraries and sketches may expect F_CPU to be defined and
 * fail otherwise (for example the Arduino SDI-12 package expects this, for AVR
 * cpus). For this reason we define F_CPU here, if not already defined.
 */
#ifndef F_CPU
#define F_CPU CLOCK_CORECLOCK
#endif

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* ARDUINO_BOARD_H */

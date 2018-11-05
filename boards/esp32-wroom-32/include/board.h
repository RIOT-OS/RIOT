/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_esp32_wroom-32
 * @brief       Board specific definitions for generic ESP32-WROOM-32 boards
 * @{
 *
 * This configuration can be used for a large set of ESP32 boards that
 * use an ESP32-WROOM-32 module and simply break out all GPIOs to external
 * pads without having any special hardware or interfaces on-board.
 * Examples are Espressif's EPS32-DEVKIT or NodeMCU-ESP32S and a large
 * number of clones.
 *
 * For detailed information about the configuration of ESP32 boards, see
 * section \ref esp32_comm_periph "Common Peripherals".
 *
 * @note
 * Most definitions can be overridden by an \ref esp32_app_spec_conf
 * "application-specific board configuration".
 *
 * @file
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif

#ifdef __cplusplus
} /* end extern "C" */
#endif

/**
 * @name    Button pin definitions
 * @{
 */
/**
  * Generic ESP32 boards have a BOOT button, which can be used as normal button
  * during normal operation. Since the GPIO0 pin is pulled up, the button
  * signal is inverted, i.e., pressing the button will give a low signal.
  */
#define BUTTON0_PIN         GPIO0
/** @} */

/**
 * @name    LED (on-board) configuration
 *
 * Generic ESP32 boards usually do not have on-board LEDs.
 * @{
 */
/** @} */

/* include common board definitions as last step */
#include "board_common.h"

#endif /* BOARD_H */
/** @} */

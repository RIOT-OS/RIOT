/*
 * SPDX-FileCopyrightText: 2015 TriaGnoSys GmbH
 * SPDX-FileCopyrightText: 2017 Alexander Kurth, Sören Tempel, Tristan Bruns
 * SPDX-FileCopyrightText: 2018 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_blackpill_stm32f103c8
 * @brief       Support for the stm32f103c8 based Black pill.
 *
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the Black Pill board
 *
 * @author      Víctor Ariño <victor.arino@triagnosys.com>
 * @author      Sören Tempel <tempel@uni-bremen.de>
 * @author      Tristan Bruns <tbruns@uni-bremen.de>
 * @author      Alexander Kurth <kurth1@uni-bremen.de>
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   Macros for controlling the on-board LED.
 * @{
 */
#define LED0_PORT           GPIO_PORT_B /**< GPIO port of LED 0 */
#define LED0_PORT_NUM       PORT_B  /**< GPIO Port number the LED is connected to */
#define LED0_PIN_NUM        12      /**< Pin number the LED is connected to */
/** @} */

#ifdef __cplusplus
}
#endif

/* Beware: This include must come *after* LED0 parameters have been defined */
#include "board_common.h"

/** @} */

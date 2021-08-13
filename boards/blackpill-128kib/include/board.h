/*
 * Copyright (C) 2015 TriaGnoSys GmbH
 *               2017 Alexander Kurth, Sören Tempel, Tristan Bruns
 *               2018 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_blackpill_128kib
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

#ifndef BOARD_H
#define BOARD_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   Macros for controlling the on-board LED.
 * @{
 */
#define LED0_PORT           GPIOB   /**< GPIO-Port the LED is connected to */
#define LED0_PORTNUM        PORT_B  /**< GPIO Port number the LED is connected to */
#define LED0_PINNUM         (12)    /**< Pin number the LED is connected to */
/** @} */

#ifdef __cplusplus
}
#endif

/* Beware: This include must come *after* LED0 parameters have been defined */
#include "board_common.h"

#endif /* BOARD_H */
/** @} */

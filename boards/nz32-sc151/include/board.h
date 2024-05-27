/*
 * Copyright (C) 2016 Fundacion Inria Chile
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_nz32-sc151
 * @{
 *
 * @file
 * @brief       Board specific definitions for the nz32-sc151 board.
 *
 * @author      Francisco Molina <francisco.molina@inria.cl>
 */

#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    User LED pin definitions and handlers
 * @{
 */
#define LED0_PIN_NUM        2
#define LED0_PORT           GPIO_PORT_B /**< GPIO port of LED 0 */
#define LED0_PORT_NUM       PORT_B
 /** @} */

#ifdef __cplusplus
}
#endif

#include "stm32_leds.h"

#endif /* BOARD_H */
/** @} */

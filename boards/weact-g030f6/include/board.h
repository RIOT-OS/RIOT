/*
 * Copyright (C) 2024 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_weact-g030f6
 * @{
 *
 * @file
 * @brief       Board specific definitions for WeAct-G030F6
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph_conf.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LED0_PIN_NUM        4
#define LED0_PORT           GPIO_PORT_A /**< GPIO port of LED 0 */
#define LED0_PORT_NUM       PORT_A

#define BTN0_PIN            GPIO_PIN(PORT_A, 14)
#define BTN0_MODE           GPIO_IN

#ifdef __cplusplus
}
#endif

#include "stm32_leds.h"

#endif /* BOARD_H */
/** @} */

/*
 * Copyright (C) 2024 BISSELL Homecare, Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_stm32c0316-dk
 * @{
 *
 * @file
 * @brief       Board specific definitions for the STM32C0316-DK
 *
 * @author      Jason Parker <Jason.Parker@bissell.com>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph_conf.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LED0_PIN_NUM        5
#define LED0_PORT           GPIO_PORT_A /**< GPIO port of LED 0 */
#define LED0_PORT_NUM       PORT_A

#define BTN0_PIN            GPIO_PIN(PORT_A, 4)
#define BTN0_MODE           GPIO_IN

#ifdef __cplusplus
}
#endif

#include "stm32_leds.h"

#endif /* BOARD_H */
/** @} */

/*
 * Copyright (C) 2020 Benjamin Valentin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_bluepill-stm32f030c8
 *
 * This board can be bought very cheaply (< 2€) on sites like eBay or
 * AliExpress.
 *
 * @brief       Support for the STM32F030C8 based Bluepill
 * @{
 *
 * @file
 * @brief       Pin definitions and board configuration options
 *
 * @author      Benjamin Valentin <benpicco@googlemail.com>
 */

#ifndef BOARD_H
#define BOARD_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Xtimer configuration
 * @{
 */
#define XTIMER_WIDTH                (16)
/** @} */

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN_NUM        13
#define LED0_PORT           GPIO_PORT_C /**< GPIO port of LED 0 */
#define LED0_PORT_NUM       PORT_C
#define LED0_IS_INVERTED    1
/** @} */

#ifdef __cplusplus
}
#endif

#include "stm32_leds.h"

/** @} */
#endif /* BOARD_H */

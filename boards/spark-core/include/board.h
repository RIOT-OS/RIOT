/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_spark-core
 * @{
 *
 * @file
 * @brief       Board specific definitions for the spark-core board.
 *
 * @author      Christian Mehlis <mehlis@inf.fu-berlin.de>
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
 * @name Tell the xtimer that we use a 16-bit peripheral timer
 */
#define XTIMER_WIDTH        (16)

/**
 * @name    Macros for controlling the on-board LEDs
 * @{
 */
#define LED0_PIN_NUM        9
#define LED0_PORT           GPIO_PORT_A /**< GPIO port of LED 0 */
#define LED0_PORT_NUM       PORT_A

#define LED1_PIN_NUM        10
#define LED1_PORT           GPIO_PORT_A /**< GPIO port of LED 1 */
#define LED1_PORT_NUM       PORT_A

#define LED2_PIN_NUM        8
#define LED2_PORT           GPIO_PORT_A /**< GPIO port of LED 2 */
#define LED2_PORT_NUM       PORT_A

#define LED3_PIN_NUM        13
#define LED3_PORT           GPIO_PORT_A /**< GPIO port of LED 3 */
#define LED3_PORT_NUM       PORT_A
/** @} */

/**
 * @name User button configuration
 * @{
 */
#define BUTTON1             GPIO_PIN(PORT_B,2)
/** @} */

/**
 * @name CC3000 pin configuration
 * @{
 */
#define CC3000_SPI          SPI_DEV(0)
#define CC3000_CS           GPIO_PIN(PORT_B,12)
#define CC3000_EN           GPIO_PIN(PORT_B,8)
#define CC3000_INT          GPIO_PIN(PORT_B,11)
/** @} */

/**
 * @name EXTFLASH pin configuration
 * @{
 */
#define EXTFLASH_SPI        SPI_DEV(0)
#define EXTFLASH            GPIO_PIN(PORT_B,9)
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

#include "stm32_leds.h"

/** @} */
#endif /* BOARD_H */

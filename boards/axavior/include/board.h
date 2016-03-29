/*
 * Copyright (C) 2016 Engineering-Spirit
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    boards_axavior AxAvior Module
 * @ingroup     boards
 * @brief       Board specific files for the AxAvior Module.
 * @{
 *
 * @file
 * @brief       Board specific definitions for the AxAvior module.
 *
 * @author      Nick van IJzendoorn <nijzendoorn@engineering-spirit.nl>
 */

#ifndef BOARD_H_
#define BOARD_H_

#include <stdint.h>

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Tell the xtimer that we use a 16-bit peripheral timer
 */
#define XTIMER_MASK         (0xffff0000)

/**
 * @name Define the interface to the AT86RF231 radio
 *
 * {spi bus, spi speed, cs pin, int pin, reset pin, sleep pin}
 */
#define AT86RF2XX_PARAMS_BOARD      {.spi = SPI_0, \
                                     .spi_speed = SPI_SPEED_5MHZ, \
                                     .cs_pin = GPIO_PIN(PORT_C,8), \
                                     .int_pin = GPIO_PIN(PORT_C,7), \
                                     .sleep_pin = GPIO_PIN(PORT_C,11), \
                                     .reset_pin = GPIO_PIN(PORT_C,10)}

/**
 * @name Define the interface for the connected flash memory
 * @{
 */
#define EXTFLASH_SPI                SPI_0
#define EXTFLASH_CS                 GPIO_PIN(PORT_B,12)
#define EXTFLASH_ADDRESS_COUNT      (3)
#define EXTFLASH_CAPACITY           (500000)
/** @} */

/**
 * @name Define the interface for the joystick
 * @{
 */
#define JOYSTICK_SPI                SPI_1
#define JOYSTICK_CS                 GPIO_PIN(PORT_B, 0)
/** @} */

/**
 * @brief Define the DTR and DSR lines
 */
#define COMM_DSR                    GPIO_PIN(PORT_C, 4)
#define COMM_DTR                    GPIO_PIN(PORT_C, 5)

/**
 * @name LED pin definitions
 * @{
 */
#define LED_GREEN_PORT      (GPIOB)
#define LED_GREEN_PIN       (3)
#define LED_GREEN_GPIO      GPIO_PIN(PORT_B,3)
/** @} */

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LED_GREEN_ON        (LED_GREEN_PORT->BSRR = (16+(1<<LED_GREEN_PIN)))
#define LED_GREEN_OFF       (LED_GREEN_PORT->BSRR = (1<<LED_GREEN_PIN))
#define LED_GREEN_TOGGLE    (LED_GREEN_PORT->ODR ^= (1<<LED_GREEN_PIN))
/** @} */

/**
 * @name xtimer tuning values
 * @{
 */
#define XTIMER_OVERHEAD     6
#define XTIMER_SHOOT_EARLY  3
/** @} */

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H_ */
/** @} */

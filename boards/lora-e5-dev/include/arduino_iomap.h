/*
 * Copyright (C) 2022 J. David Ibáñez <jdavid.ibp@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_lora-e5-dev
 * @{
 *
 * @file
 * @brief       Mapping from MCU pins to Arduino pins
 *
 * You can use the defines in this file for simplified interaction with the
 * Arduino specific pin numbers.
 *
 * @author      J. David Ibáñez <jdavid.ibp@gmail.com>
 */

#ifndef ARDUINO_IOMAP_H
#define ARDUINO_IOMAP_H

#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Mapping of MCU pins to Arduino pins
 *
 * @warning Beware: Despite an Arduino pin mapping being available, Arduino shields
 *          are mechanically not compatible with the board. Check header file
 *          https://github.com/RIOT-OS/RIOT/blob/master/boards/lora-e5-dev/include/arduino_iomap.h#L42
 *          for the exact mapping.
 *
 * @{
 */

#define ARDUINO_PIN_3           GPIO_PIN(PORT_A, 13) /**< SWDIO of SWIM for program download */
#define ARDUINO_PIN_4           GPIO_PIN(PORT_A, 14) /**< SWCLK of SWIM for program download */
#define ARDUINO_PIN_5           GPIO_PIN(PORT_B, 15) /**< SCL of I2C2 from MCU */
#define ARDUINO_PIN_6           GPIO_PIN(PORT_A, 15) /**< SDA of I2C2 from MCU */
#define ARDUINO_PIN_7           GPIO_PIN(PORT_B, 4)  /**< MCU GPIO */
#define ARDUINO_PIN_8           GPIO_PIN(PORT_B, 3)  /**< MCU GPIO */
#define ARDUINO_PIN_9           GPIO_PIN(PORT_B, 7)  /**< UART1_RX from MCU */
#define ARDUINO_PIN_10          GPIO_PIN(PORT_B, 6)  /**< UART1_TX from MCU */
#define ARDUINO_PIN_11          GPIO_PIN(PORT_B, 5)  /**< MCU GPIO */
#define ARDUINO_PIN_12          GPIO_PIN(PORT_C, 1)  /**< MCU GPIO; LPUART1_TX from MCU */
#define ARDUINO_PIN_13          GPIO_PIN(PORT_C, 0)  /**< MCU GPIO; LPUART1_RX from MCU */
#define ARDUINO_PIN_18          GPIO_PIN(PORT_A, 3)  /**< MCU GPIO; USART2_RX from MCU */
#define ARDUINO_PIN_19          GPIO_PIN(PORT_A, 2)  /**< MCU GPIO; USART2_TX from MCU */
#define ARDUINO_PIN_20          GPIO_PIN(PORT_B, 10) /**< MCU GPIO */
#define ARDUINO_PIN_21          GPIO_PIN(PORT_A, 9)  /**< MCU GPIO */
#define ARDUINO_PIN_23          GPIO_PIN(PORT_A, 0)  /**< MCU GPIO */
#define ARDUINO_PIN_24          GPIO_PIN(PORT_B, 13) /**< SPI2_SCK from MCU; Boot pin(Active low) */
#define ARDUINO_PIN_25          GPIO_PIN(PORT_B, 9)  /**< SPI2_NSS from MCU */
#define ARDUINO_PIN_26          GPIO_PIN(PORT_B, 14) /**< SPI2_MISO from MCU */
#define ARDUINO_PIN_27          GPIO_PIN(PORT_A, 10) /**< SPI2_MOSI from MCU */
#define ARDUINO_PIN_28          GPIO_PIN(PORT_B, 0)  /**< Unavailable；Suspended treatment */

#define ARDUINO_PIN_LAST        28
/** @} */


#ifdef __cplusplus
}
#endif

#endif /* ARDUINO_IOMAP_H */
/** @} */

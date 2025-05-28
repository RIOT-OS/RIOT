/*
 * Copyright (C)  2024 Christian Amsüss <chrysn@fsfe.org>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_nrf52dk
 * @{
 *
 * @file
 * @brief       Mapping from MCU pins to Arduino pins
 *
 * @author      Christian Amsüss <chrysn@fsfe.org>
 *
 */

#include "periph/gpio.h"
#include "periph/adc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Mapping of MCU pins to Arduino pins
 *
 * For reference, see schematic file `nRF52 Development Kit - Hardware files
 * 3_0_0/PCA10040-nRF52832 Development Board 3_0_0/Schematic_Layout pdf
 * files/PCA10040_Schematic_And_PCB.pdf` in
 * `nrf52-development-kit---hardware-files-3_0_0.zip` from
 * <https://www.nordicsemi.com/Products/Development-hardware/nRF52-DK/Download?lang=en#infotabs>,
 * page 2 areas CD123
 *
 * @{
 */
#define ARDUINO_PIN_0           GPIO_PIN(0, 11)
#define ARDUINO_PIN_1           GPIO_PIN(0, 12)

#define ARDUINO_PIN_2           GPIO_PIN(0, 13)
#define ARDUINO_PIN_3           GPIO_PIN(0, 14)
#define ARDUINO_PIN_4           GPIO_PIN(0, 15)
#define ARDUINO_PIN_5           GPIO_PIN(0, 16)

/* Those interact with LEDs, SWO and IOEXP_IRQ */
#define ARDUINO_PIN_6           GPIO_PIN(0, 17)
#define ARDUINO_PIN_7           GPIO_PIN(0, 18)
#define ARDUINO_PIN_8           GPIO_PIN(0, 19)
#define ARDUINO_PIN_9           GPIO_PIN(0, 20)

#define ARDUINO_PIN_10          GPIO_PIN(0, 22)
#define ARDUINO_PIN_11          GPIO_PIN(0, 23)
#define ARDUINO_PIN_12          GPIO_PIN(0, 24)
#define ARDUINO_PIN_13          GPIO_PIN(0, 25)
#define ARDUINO_PIN_14          GPIO_PIN(0, 0)
#define ARDUINO_PIN_15          GPIO_PIN(0, 1)
/* Also RESET */
#define ARDUINO_PIN_16          GPIO_PIN(0, 21)

/* These are also UART ports */
#define ARDUINO_PIN_17          GPIO_PIN(0, 5)
#define ARDUINO_PIN_18          GPIO_PIN(0, 6)
#define ARDUINO_PIN_19          GPIO_PIN(0, 7)
#define ARDUINO_PIN_20          GPIO_PIN(0, 8)

#define ARDUINO_PIN_21          GPIO_PIN(0, 9)
#define ARDUINO_PIN_22          GPIO_PIN(0, 10)

#define ARDUINO_PIN_LAST        22
/** @} */

/**
 * @name    Aliases for analog pins
 * @{
 */
#define ARDUINO_PIN_A0          GPIO_PIN(0, 3)
#define ARDUINO_PIN_A1          GPIO_PIN(0, 4)
#define ARDUINO_PIN_A2          GPIO_PIN(0, 28)
#define ARDUINO_PIN_A3          GPIO_PIN(0, 29)
#define ARDUINO_PIN_A4          GPIO_PIN(0, 30)
#define ARDUINO_PIN_A5          GPIO_PIN(0, 31)
/** @} */

/**
 * @name    Arduino's I2C buses
 * @{
 */
/**
 * @brief   The only configured I2C
 */
#define ARDUINO_I2C_UNO         I2C_DEV(0)
/** @} */

/**
 * @name    Arduino's SPI buses
 * @{
 */
/**
 * @brief   D11..13 is 0.23..0.25, which is called SPI_DEV(0) here
 */
#define ARDUINO_SPI_D11D12D13   SPI_DEV(0)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */

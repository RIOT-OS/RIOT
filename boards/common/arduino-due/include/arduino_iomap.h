/*
 * SPDX-FileCopyrightText: 2015 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2017 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_common_arduino_due
 * @{
 *
 * @file
 * @brief       Mapping from MCU pins to Arduino pins
 *
 * You can use the defines in this file for simplified interaction with the
 * Arduino specific pin numbers.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include "periph/adc.h"
#include "periph/gpio.h"
#include "periph/i2c.h"
#include "periph/spi.h"
#include "periph/uart.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Arduino's default UART device
 * @{
 */
#define ARDUINO_UART_D0D1        UART_DEV(0)
/** @} */

/**
 * @name    Arduino's default SPI device
 * @{
 */
/**
 * @brief   SPI_DEV(0) is connected to the ISP header
 */
#define ARDUINO_SPI_ISP         SPI_DEV(0)
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
 * @name    Mapping of MCU pins to Arduino pins
 * @{
 */
#define ARDUINO_PIN_0           GPIO_PIN(PA, 8)
#define ARDUINO_PIN_1           GPIO_PIN(PA, 9)
#define ARDUINO_PIN_2           GPIO_PIN(PB, 25)
#define ARDUINO_PIN_3           GPIO_PIN(PC, 28)
#define ARDUINO_PIN_4           GPIO_PIN(PA, 29)
#define ARDUINO_PIN_5           GPIO_PIN(PC, 25)
#define ARDUINO_PIN_6           GPIO_PIN(PC, 24)
#define ARDUINO_PIN_7           GPIO_PIN(PC, 23)
#define ARDUINO_PIN_8           GPIO_PIN(PC, 22)
#define ARDUINO_PIN_9           GPIO_PIN(PC, 21)
#define ARDUINO_PIN_10          GPIO_PIN(PC, 29)
#define ARDUINO_PIN_11          GPIO_PIN(PD, 7)
#define ARDUINO_PIN_12          GPIO_PIN(PD, 8)
#define ARDUINO_PIN_13          GPIO_PIN(PB, 27)
#define ARDUINO_PIN_14          GPIO_PIN(PD, 4)
#define ARDUINO_PIN_15          GPIO_PIN(PD, 5)
#define ARDUINO_PIN_16          GPIO_PIN(PA, 13)
#define ARDUINO_PIN_17          GPIO_PIN(PA, 12)
#define ARDUINO_PIN_18          GPIO_PIN(PA, 11)
#define ARDUINO_PIN_19          GPIO_PIN(PA, 10)
#define ARDUINO_PIN_20          GPIO_PIN(PB, 12)
#define ARDUINO_PIN_21          GPIO_PIN(PB, 13)
#define ARDUINO_PIN_22          GPIO_PIN(PB, 26)
#define ARDUINO_PIN_23          GPIO_PIN(PA, 14)
#define ARDUINO_PIN_24          GPIO_PIN(PA, 15)
#define ARDUINO_PIN_25          GPIO_PIN(PD, 0)
#define ARDUINO_PIN_26          GPIO_PIN(PD, 1)
#define ARDUINO_PIN_27          GPIO_PIN(PD, 2)
#define ARDUINO_PIN_28          GPIO_PIN(PD, 3)
#define ARDUINO_PIN_29          GPIO_PIN(PD, 6)
#define ARDUINO_PIN_30          GPIO_PIN(PD, 9)
#define ARDUINO_PIN_31          GPIO_PIN(PA, 7)
#define ARDUINO_PIN_32          GPIO_PIN(PD, 10)
#define ARDUINO_PIN_33          GPIO_PIN(PC, 1)
#define ARDUINO_PIN_34          GPIO_PIN(PC, 2)
#define ARDUINO_PIN_35          GPIO_PIN(PC, 3)
#define ARDUINO_PIN_36          GPIO_PIN(PC, 4)
#define ARDUINO_PIN_37          GPIO_PIN(PC, 5)
#define ARDUINO_PIN_38          GPIO_PIN(PC, 6)
#define ARDUINO_PIN_39          GPIO_PIN(PC, 7)
#define ARDUINO_PIN_40          GPIO_PIN(PC, 8)
#define ARDUINO_PIN_41          GPIO_PIN(PC, 9)
#define ARDUINO_PIN_42          GPIO_PIN(PA, 19)
#define ARDUINO_PIN_43          GPIO_PIN(PA, 20)
#define ARDUINO_PIN_44          GPIO_PIN(PC, 19)
#define ARDUINO_PIN_45          GPIO_PIN(PC, 18)
#define ARDUINO_PIN_46          GPIO_PIN(PC, 17)
#define ARDUINO_PIN_47          GPIO_PIN(PC, 16)
#define ARDUINO_PIN_48          GPIO_PIN(PC, 15)
#define ARDUINO_PIN_49          GPIO_PIN(PC, 14)
#define ARDUINO_PIN_50          GPIO_PIN(PC, 13)
#define ARDUINO_PIN_51          GPIO_PIN(PC, 12)
#define ARDUINO_PIN_52          GPIO_PIN(PB, 21)
#define ARDUINO_PIN_53          GPIO_PIN(PB, 14)
#define ARDUINO_PIN_54          GPIO_PIN(PA, 16)
#define ARDUINO_PIN_55          GPIO_PIN(PA, 24)
#define ARDUINO_PIN_56          GPIO_PIN(PA, 23)
#define ARDUINO_PIN_57          GPIO_PIN(PA, 22)
#define ARDUINO_PIN_58          GPIO_PIN(PA, 6)
#define ARDUINO_PIN_59          GPIO_PIN(PA, 4)
#define ARDUINO_PIN_60          GPIO_PIN(PA, 3)
#define ARDUINO_PIN_61          GPIO_PIN(PA, 2)
#define ARDUINO_PIN_62          GPIO_PIN(PB, 17)
#define ARDUINO_PIN_63          GPIO_PIN(PB, 18)
#define ARDUINO_PIN_64          GPIO_PIN(PB, 19)
#define ARDUINO_PIN_65          GPIO_PIN(PB, 20)
#define ARDUINO_PIN_66          GPIO_PIN(PB, 15)
#define ARDUINO_PIN_67          GPIO_PIN(PB, 16)
#define ARDUINO_PIN_68          GPIO_PIN(PA, 1)
#define ARDUINO_PIN_69          GPIO_PIN(PA, 0)
#define ARDUINO_PIN_70          GPIO_PIN(PA, 17)
#define ARDUINO_PIN_71          GPIO_PIN(PA, 18)
#define ARDUINO_PIN_72          GPIO_PIN(PC, 30)
#define ARDUINO_PIN_73          GPIO_PIN(PA, 21)
#define ARDUINO_PIN_74          GPIO_PIN(PA, 25)
#define ARDUINO_PIN_75          GPIO_PIN(PA, 26)
#define ARDUINO_PIN_76          GPIO_PIN(PA, 27)
#define ARDUINO_PIN_77          GPIO_PIN(PA, 28)
#define ARDUINO_PIN_78          GPIO_PIN(PB, 23)

#define ARDUINO_PIN_LAST        78
/** @} */

/**
 * @name    Aliases for analog pins
 * @{
 */
#define ARDUINO_PIN_A0          ARDUINO_PIN_54
#define ARDUINO_PIN_A1          ARDUINO_PIN_55
#define ARDUINO_PIN_A2          ARDUINO_PIN_56
#define ARDUINO_PIN_A3          ARDUINO_PIN_57
#define ARDUINO_PIN_A4          ARDUINO_PIN_58
#define ARDUINO_PIN_A5          ARDUINO_PIN_59
#define ARDUINO_PIN_A6          ARDUINO_PIN_60
#define ARDUINO_PIN_A7          ARDUINO_PIN_61
#define ARDUINO_PIN_A8          ARDUINO_PIN_62
#define ARDUINO_PIN_A9          ARDUINO_PIN_63
#define ARDUINO_PIN_A10         ARDUINO_PIN_64
#define ARDUINO_PIN_A11         ARDUINO_PIN_65
#define ARDUINO_PIN_DAC0        ARDUINO_PIN_66
#define ARDUINO_PIN_DAC1        ARDUINO_PIN_67
/** @} */

/**
 * @name    Mapping of Arduino analog pins to RIOT ADC lines
 * @{
 */
#define ARDUINO_A0              ADC_LINE(7)
#define ARDUINO_A1              ADC_LINE(6)
#define ARDUINO_A2              ADC_LINE(5)
#define ARDUINO_A3              ADC_LINE(4)
#define ARDUINO_A4              ADC_LINE(3)
#define ARDUINO_A5              ADC_LINE(2)
#define ARDUINO_A6              ADC_LINE(1)
#define ARDUINO_A7              ADC_LINE(0)
#define ARDUINO_A8              ADC_LINE(10)
#define ARDUINO_A9              ADC_LINE(11)
#define ARDUINO_A10             ADC_LINE(12)
#define ARDUINO_A11             ADC_LINE(13)

#define ARDUINO_ANALOG_PIN_LAST 11
/** @} */

/**
 * @name    Mapping of Arduino DAC pins to RIOT DAC lines
 * @{
 */
#define ARDUINO_DAC0            DAC_LINE(0)
#define ARDUINO_DAC1            DAC_LINE(1)

#define ARDUINO_DAC_PIN_LAST    1                   /**< DAC1 is the last DAC pin */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */

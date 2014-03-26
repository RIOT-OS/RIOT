/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    board_stm32f3discovery STM32F3Discovery
 * @ingroup     boards
 * @brief       Board definitons for the STM32F3Discovery board.
 * @{
 *
 * @file        board.h
 * @brief       Board specific definitions for the STM32F3Discovery evaluation board.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __BOARD_H
#define __BOARD_H

/**
 * Define the nominal CPU core clock in this board
 */
#define F_CPU               (72000000U)

/**
 * @brief Configure the hardware timer
 */
#define ARCH_MAXTIMERS      (4U)
#define HW_TIMER            TIMER_0

/**
 * @brief Assign the UART that should be used as standard IO
 */
#define STD_IO              UART_0
#define STD_IO_BAUDRATE     (115200U)

/**
 * @brief Assign the board's LEDs
 */
#define LED_3               GPIO_8
#define LED_4               GPIO_9
#define LED_5               GPIO_10
#define LED_6               GPIO_11
#define LED_7               GPIO_12
#define LED_8               GPIO_13
#define LED_9               GPIO_14
#define LED_10              GPIO_15
// for compatability to other boards
#define RED_LED             LED_3
#define GREEN_LED           LED_10

/**
 * @brief Assign the board's user button
 */
#define USER_BUTTON         GPIO_7

/**
 * @brief Assign on board L3GD20 Gyroscope
 */
#define L3GD20_SPI          SPI_0
#define L3GD20_INT1         GPIO_4
#define L3GD20_INT2         GPIO_5
#define L3GD20_CS           GPIO_3

/**
 * @brief Assign on board LSM303DLHC accelero- and magnetometer
 */
#define LSM303DLHC_I2C      I2C_0
#define LSM303DLHC_DRDY     GPIO_0
#define LSM303DLHC_INT1     GPIO_1
#define LSM303DLHC_INT2     GPIO_2

/**
 * @brief Initialize a minimal board configuration by initializing the LEDs and UART_0.
 */
void board_init(void);


#endif /** __BOARD_H */
/** @} */

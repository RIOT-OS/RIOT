/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    boards_fox fox
 * @ingroup     boards
 * @brief       Board specific files for the fox board.
 * @{
 *
 * @file
 * @brief       Board specific definitions for the fox board.
 *
 * @author      Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
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
 * Define the nominal CPU core clock in this board
 */
#define F_CPU               CLOCK_CORECLOCK

/**
 * @name Define the UART to be used as stdio and its baudrate
 * @{
 */
#define STDIO               UART_0
#define STDIO_BAUDRATE      (115200)
#define STDIO_RX_BUFSIZE    (64U)
/** @} */

/**
 * Assign the hardware timer
 */
#define HW_TIMER            TIMER_0

/**
 * @name Define the interface to the AT86RF231 radio
 * @{
 */
#define AT86RF231_SPI       SPI_0
#define AT86RF231_CS        GPIO_11
#define AT86RF231_INT       GPIO_12
#define AT86RF231_RESET     GPIO_13
#define AT86RF231_SLEEP     GPIO_14
/** @} */

/**
 * @name Define the interface to the LPS331AP pressure sensor
 * @{
 */
#define LPS331AP_I2C        I2C_0
#define LPS331AP_ADDR       0x5c
/** @} */

/**
 * @name Define the interface for the L3G4200D gyroscope
 * @{
 */
#define L3G4200D_I2C        I2C_0
#define L3G4200D_ADDR       0x68
#define L3G4200D_DRDY       GPIO_2
#define L3G4200D_INT        GPIO_1
/** @} */

/**
 * @name Define the interface to the LSM303DLHC accelerometer and magnetometer
 * @{
 */
#define LSM303DLHC_I2C      I2C_0
#define LSM303DLHC_ACC_ADDR (25)
#define LSM303DLHC_MAG_ADDR (30)
#define LSM303DLHC_INT1     GPIO_4
#define LSM303DLHC_INT2     GPIO_3
#define LSM303DLHC_DRDY     GPIO_9
/** @} */

/**
 * @name LED pin definitions
 * @{
 */
#define LED_RED_PORT        (GPIOB)
#define LED_RED_PIN         (10)
#define LED_GREEN_PORT      (GPIOB)
#define LED_GREEN_PIN       (12)
/** @} */

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LED_RED_ON          (LED_RED_PORT->ODR &= ~(1<<LED_RED_PIN))
#define LED_RED_OFF         (LED_RED_PORT->ODR |= (1<<LED_RED_PIN))
#define LED_RED_TOGGLE      (LED_RED_PORT->ODR ^= (1<<LED_RED_PIN))

#define LED_GREEN_ON        (LED_GREEN_PORT->ODR &= ~(1<<LED_GREEN_PIN))
#define LED_GREEN_OFF       (LED_GREEN_PORT->ODR |= (1<<LED_GREEN_PIN))
#define LED_GREEN_TOGGLE    (LED_GREEN_PORT->ODR ^= (1<<LED_GREEN_PIN))

#define LED_ORANGE_ON
#define LED_ORANGE_OFF
#define LED_ORANGE_TOGGLE
/** @} */

/**
 * Define the type for the radio packet length for the transceiver
 */
typedef uint8_t radio_packet_length_t;

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H_ */
/** @} */

/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
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
                                     .cs_pin = GPIO_PIN(PORT_A, 1), \
                                     .int_pin = GPIO_PIN(PORT_C, 2), \
                                     .sleep_pin = GPIO_PIN(PORT_A, 0), \
                                     .reset_pin = GPIO_PIN(PORT_C, 1)}

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
#define L3G4200D_DRDY       GPIO_PIN(PORT_B,8)
#define L3G4200D_INT        GPIO_PIN(PORT_B,11)
/** @} */

/**
 * @name Define the interface to the LSM303DLHC accelerometer and magnetometer
 * @{
 */
#define LSM303DLHC_I2C      I2C_0
#define LSM303DLHC_ACC_ADDR (25)
#define LSM303DLHC_MAG_ADDR (30)
#define LSM303DLHC_INT1     GPIO_PIN(PORT_B,9)
#define LSM303DLHC_INT2     GPIO_PIN(PORT_B,5)
#define LSM303DLHC_DRDY     GPIO_PIN(PORT_A,9)
/** @} */

/**
 * @brief   LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(PORT_B, 10)
#define LED1_PIN            GPIO_PIN(PORT_B, 12)

#define LED0_MASK           (1 << 10)
#define LED1_MASK           (1 << 12)

#define LED0_ON             (GPIOB->ODR &= ~LED0_MASK)
#define LED0_OFF            (GPIOB->ODR |=  LED0_MASK)
#define LED0_TOGGLE         (GPIOB->ODR ^=  LED0_MASK)

#define LED1_ON             (GPIOB->ODR &= ~LED1_MASK)
#define LED1_OFF            (GPIOB->ODR |=  LED1_MASK)
#define LED1_TOGGLE         (GPIOB->ODR ^=  LED1_MASK)
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

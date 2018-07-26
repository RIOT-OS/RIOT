/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_fox
 * @{
 *
 * @file
 * @brief       Board specific definitions for the fox board
 *
 * @author      Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
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
 * @name    Xtimer configuration
 *
 * Tell the xtimer that we use a 16-bit peripheral timer
 * @{
 */
#define XTIMER_WIDTH        (16)
/** @} */

/**
 * @name    Define the interface to the AT86RF231 radio
 *
 * {spi bus, spi speed, cs pin, int pin, reset pin, sleep pin}
 * @{
 */
#define AT86RF2XX_PARAM_CS         GPIO_PIN(PORT_A, 1)
#define AT86RF2XX_PARAM_INT        GPIO_PIN(PORT_C, 2)
#define AT86RF2XX_PARAM_SLEEP      GPIO_PIN(PORT_A, 0)
#define AT86RF2XX_PARAM_RESET      GPIO_PIN(PORT_C, 1)
/** @} */

/**
 * @name    Define the interface to the LPS331AP pressure sensor
 * @{
 */
#define LPS331AP_PARAM_ADDR        (0x5C)
/** @} */

/**
 * @name    Define the interface for the L3G4200D gyroscope
 * @{
 */
#define L3G4200D_PARAM_INT2        GPIO_PIN(PORT_B, 8)
#define L3G4200D_PARAM_INT1        GPIO_PIN(PORT_B, 11)
/** @} */

/**
 * @name    Define the interface to the LSM303DLHC accelerometer and magnetometer
 * @{
 */
#define LSM303DLHC_PARAM_ACC_ADDR  (0x25)
#define LSM303DLHC_PARAM_MAG_ADDR  (0x30)
#define LSM303DLHC_PARAM_ACC_PIN   GPIO_PIN(PORT_B, 9)
#define LSM303DLHC_PARAM_MAG_PIN   GPIO_PIN(PORT_A, 9)
/** @} */

/**
 * @name    LED pin definitions and handlers
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
 * @brief   Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */

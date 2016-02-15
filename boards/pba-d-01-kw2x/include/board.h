/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2015 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    board_phywave_eval phyWAVE-KW22 Board
 * @ingroup     boards
 * @brief       Board specific implementations for the phyWAVE evaluation board
 * @{
 *
 * @file
 * @brief       Board specific definitions for the phyWAVE evaluation board
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 */

#ifndef BOARD_H_
#define BOARD_H_

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name LED pin definitions
 * @{
 */
#define LED_R_PORT_CLKEN()    (SIM->SCGC5 |= (SIM_SCGC5_PORTD_MASK)) /**< Clock Enable for PORTD*/
#define LED_G_PORT_CLKEN()    (SIM->SCGC5 |= (SIM_SCGC5_PORTD_MASK)) /**< Clock Enable for PORTD*/
#define LED_B_PORT_CLKEN()    (SIM->SCGC5 |= (SIM_SCGC5_PORTA_MASK)) /**< Clock Enable for PORTA*/
#define LED_R_PORT            PORTD /**< PORT for Red LED*/
#define LED_R_GPIO            GPIOD /**< GPIO-Device for Red LED*/
#define LED_G_PORT            PORTD /**< PORT for Green LED*/
#define LED_G_GPIO            GPIOD /**< GPIO-Device for Green LED*/
#define LED_B_PORT            PORTA /**< PORT for Blue LED*/
#define LED_B_GPIO            GPIOA /**< GPIO-Device for Blue LED*/
#define LED_R_PIN             6     /**< Red LED connected to PINx*/
#define LED_G_PIN             4     /**< Green LED connected to PINx*/
#define LED_B_PIN             4     /**< Blue LED connected to PINx*/
/** @} */

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LED_B_ON            (LED_B_GPIO->PCOR = (1 << LED_B_PIN))
#define LED_B_OFF           (LED_B_GPIO->PSOR = (1 << LED_B_PIN))
#define LED_B_TOGGLE        (LED_B_GPIO->PTOR = (1 << LED_B_PIN))
#define LED_G_ON            (LED_G_GPIO->PCOR = (1 << LED_G_PIN))
#define LED_G_OFF           (LED_G_GPIO->PSOR = (1 << LED_G_PIN))
#define LED_G_TOGGLE        (LED_G_GPIO->PTOR = (1 << LED_G_PIN))
#define LED_R_ON            (LED_R_GPIO->PCOR = (1 << LED_R_PIN))
#define LED_R_OFF           (LED_R_GPIO->PSOR = (1 << LED_R_PIN))
#define LED_R_TOGGLE        (LED_R_GPIO->PTOR = (1 << LED_R_PIN))

/* for compatability to other boards */
#define LED_GREEN_ON        LED_G_ON
#define LED_GREEN_OFF       LED_G_OFF
#define LED_GREEN_TOGGLE    LED_G_TOGGLE
#define LED_RED_ON          LED_R_ON
#define LED_RED_OFF         LED_R_OFF
#define LED_RED_TOGGLE      LED_R_TOGGLE
/** @} */

/**
@name KW2XRF configuration
@{
*/
#define KW2XRF_SPI        (SPI_1)
#define KW2XRF_CS         (GPIO_PIN(KW2XDRF_PORT, KW2XDRF_PCS0_PIN))
#define KW2XRF_INT        (GPIO_PIN(KW2XDRF_PORT, KW2XDRF_IRQ_PIN))
#define KW2XRF_SPI_SPEED  (SPI_SPEED_10MHZ)
#define KW2XRF_SHARED_SPI 0
/** @}*/

/**
 * @name Define the interface for the HDC1000 humidity sensor
 * @{
 */
#define HDC1000_I2C         (I2C_0)
#define HDC1000_ADDR        (0x43)
/** @} */

/**
 * @name Define the interface for the MAG3110 magnetometer sensor
 * @{
 */
#define MAG3110_I2C         (I2C_0)
#define MAG3110_ADDR        (0x0E)
/** @} */

/**
 * @name Define the interface for the MMA8652 tri-axis accelerometer sensor
 * @{
 */
#define MMA8652_I2C         (I2C_0)
#define MMA8652_ADDR        (0x1D)
/** @} */

/**
 * @name Define the interface for the MPL3115A2 pressure sensor
 * @{
 */
#define MPL3115A2_I2C       (I2C_0)
#define MPL3115A2_ADDR      (0x60)
/** @} */

/**
 * @name Define the interface for the TCS3772 light sensor
 * @{
 */
#define TCS37727_I2C        (I2C_0)
#define TCS37727_ADDR       (0x29)
/** @} */

/**
 * @name Define the interface for the TMP006 IR-Termopile sensor
 * @{
 */
#define TMP006_I2C          (I2C_0)
#define TMP006_ADDR         (0x41)
/** @} */

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H */
/** @} */

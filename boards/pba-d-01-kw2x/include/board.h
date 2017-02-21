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

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(PORT_D, 6)
#define LED1_PIN            GPIO_PIN(PORT_D, 4)
#define LED2_PIN            GPIO_PIN(PORT_A, 4)

#define LED0_MASK           (1 << 6)
#define LED1_MASK           (1 << 4)
#define LED2_MASK           (1 << 4)

#define LED0_ON            (GPIOD->PCOR = LED0_MASK)
#define LED0_OFF           (GPIOD->PSOR = LED0_MASK)
#define LED0_TOGGLE        (GPIOD->PTOR = LED0_MASK)

#define LED1_ON            (GPIOD->PCOR = LED1_MASK)
#define LED1_OFF           (GPIOD->PSOR = LED1_MASK)
#define LED1_TOGGLE        (GPIOD->PTOR = LED1_MASK)

#define LED2_ON            (GPIOA->PCOR = LED2_MASK)
#define LED2_OFF           (GPIOA->PSOR = LED2_MASK)
#define LED2_TOGGLE        (GPIOA->PTOR = LED2_MASK)
/** @} */

/**
 * @name Macro for button S1/S2.
 * @{
 */
#define BUTTON_PORT         PORTD
#define BUTTON_PIN          1
#define BUTTON_GPIO         GPIO_PIN(PORT_D, BUTTON_PIN)
/** @} */

/**
 * @name KW2XRF configuration
 *
 * {spi bus, cs pin, int pin, spi speed,}
 */
#define KW2XRF_PARAMS_BOARD  {.spi = SPI_DEV(1), \
                              .spi_clk = SPI_CLK_10MHZ, \
                              .cs_pin = GPIO_PIN(KW2XDRF_PORT, KW2XDRF_PCS0_PIN), \
                              .int_pin = GPIO_PIN(KW2XDRF_PORT, KW2XDRF_IRQ_PIN) }
#define KW2XRF_SHARED_SPI   0
/** @}*/

/**
 * @name Define the interface for the HDC1000 humidity sensor
 * @{
 */
#define HDC1000_I2C         (I2C_DEV(0))
#define HDC1000_ADDR        (0x43)
/** @} */

/**
 * @name Define the interface for the MAG3110 magnetometer sensor
 * @{
 */
#define MAG3110_I2C         (I2C_DEV(0))
#define MAG3110_ADDR        (0x0E)
/** @} */

/**
 * @name Define the interface for the MMA8652 tri-axis accelerometer sensor
 * @{
 */
#define MMA8652_I2C         (I2C_DEV(0))
#define MMA8652_ADDR        (0x1D)
/** @} */

/**
 * @name Define the interface for the MPL3115A2 pressure sensor
 * @{
 */
#define MPL3115A2_I2C       (I2C_DEV(0))
#define MPL3115A2_ADDR      (0x60)
/** @} */

/**
 * @name Define the interface for the TCS3772 light sensor
 * @{
 */
#define TCS37727_PARAMS     { .i2c   = I2C_DEV(0), \
                              .addr  = 0x29, \
                              .atime = TCS37727_PARAM_ATIME }
/** @} */

/**
 * @name Define the interface for the TMP006 IR-Termopile sensor
 * @{
 */
#define TMP006_I2C          (I2C_DEV(0))
#define TMP006_ADDR         (0x41)
/** @} */

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */

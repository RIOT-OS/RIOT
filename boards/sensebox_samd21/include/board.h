/*
 * Copyright (C) 2018 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_sensebox_samd21
 * @brief       Support for the SenseBox MCU with SAMD21 board.
 *
 * @{
 *
 * @file
 * @brief       Board specific configuration for the SenseBox MCU with SAMD21
 * board.
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph_conf.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(PA, 27)

#define LED_PORT            PORT->Group[PA]
#define LED0_MASK           (1 << 27)

#define LED0_ON             (LED_PORT.OUTSET.reg = LED0_MASK)
#define LED0_OFF            (LED_PORT.OUTCLR.reg = LED0_MASK)
#define LED0_TOGGLE         (LED_PORT.OUTTGL.reg = LED0_MASK)

#define LED1_PIN            GPIO_PIN(PA, 28)
#define LED1_MASK           (1 << 28)

#define LED1_ON             (LED_PORT.OUTSET.reg = LED1_MASK)
#define LED1_OFF            (LED_PORT.OUTCLR.reg = LED1_MASK)
#define LED1_TOGGLE         (LED_PORT.OUTTGL.reg = LED1_MASK)
/** @} */

/**
 * @name    SW0 (Button) pin definitions
 * @{
 */
#define BTN0_PORT           PORT->Group[PA]
#define BTN0_PIN            GPIO_PIN(PA, 20)
#define BTN0_MODE           GPIO_IN_PU
/** @} */

/**
 * @name    XBEE1 bus
 * @brief   XBEE1 is enabled by default. The enable pin is
 * 'XBEE1_EN_PIN'. To disable this bus call 'XBEE1_DISABLE', to re-enable it
 * call 'XBEE1_ENABLE'.
 * @{
 */
#define XBEE1_EN_PORT           PORT->Group[PB]
#define XBEE1_EN_MASK           (1 << 3)
#define XBEE1_EN_MODE           GPIO_OUT
#define XBEE1_EN_PIN            GPIO_PIN(PB, 3)

#define XBEE1_ENABLE            (XBEE1_EN_PORT.OUTCLR.reg = XBEE1_EN_MASK)
#define XBEE1_DISABLE           (XBEE1_EN_PORT.OUTSET.reg = XBEE1_EN_MASK)

#define XBEE1_CS_PIN            GPIO_PIN(PA, 18)

#define XBEE1_INT_PIN           GPIO_PIN(PA, 21)
/** @} */

/**
 * @name    XBEE2 bus
 * @brief   XBEE2 is enabled by default. The enable pin is
 * 'XBEE2_EN_PIN'. To disable this bus call 'XBEE2_DISABLE', to re-enable it
 * call 'XBEE2_ENABLE'.
 * @{
 */
#define XBEE2_EN_PORT           PORT->Group[PB]
#define XBEE2_EN_MASK           (1 << 10)
#define XBEE2_EN_PIN            GPIO_PIN(PB, 10)
#define XBEE2_EN_MODE           GPIO_OUT

#define XBEE2_ENABLE            (XBEE2_EN_PORT.OUTCLR.reg = XBEE2_EN_MASK)
#define XBEE2_DISABLE           (XBEE2_EN_PORT.OUTSET.reg = XBEE2_EN_MASK)

#define XBEE2_CS_PIN            GPIO_PIN(PA, 14)

#define XBEE2_INT_PIN           GPIO_PIN(PA, 15)
/** @} */

/**
 * @name    I2C bus
 * @brief   I2C is enabled by default. The enable pin is
 * 'I2C_EN_PIN'. To disable this bus call 'I2C_DISABLE', to re-enable it call
 * 'I2C_ENABLE'.
 * @{
 */
#define I2C_EN_PORT           PORT->Group[PB]
#define I2C_EN_MASK           (1 << 11)
#define I2C_EN_PIN            GPIO_PIN(PB, 11)
#define I2C_EN_MODE           GPIO_OUT

#define I2C_ENABLE            (I2C_EN_PORT.OUTSET.reg = I2C_EN_MASK)
#define I2C_DISABLE           (I2C_EN_PORT.OUTCLR.reg = I2C_EN_MASK)
/** @} */

/**
 * @name        SX127X
 *
 * SX127X configuration (on XBEE1 port). This particular board has
 * merged DIO0 and DIO1 interupt pins into one (defined as DIOMULTI).
 * @{
 */
#define SX127X_PARAM_SPI                    (SPI_DEV(0))

#define SX127X_PARAM_SPI_NSS                XBEE1_CS_PIN        /* D23 */

#define SX127X_PARAM_RESET                  GPIO_UNDEF

#define SX127X_PARAM_DIOMULTI               XBEE1_INT_PIN       /* D24 */

#define SX127X_PARAM_PASELECT               (SX127X_PA_RFO)

#define SX127X_PARAMS                     { .spi       = SX127X_PARAM_SPI,     \
                                            .nss_pin   = SX127X_PARAM_SPI_NSS, \
                                            .reset_pin = SX127X_PARAM_RESET,   \
                                            .dio0_pin  = SX127X_PARAM_DIOMULTI,\
                                            .dio1_pin  = SX127X_PARAM_DIO1,    \
                                            .dio2_pin  = SX127X_PARAM_DIO2,    \
                                            .dio3_pin  = SX127X_PARAM_DIO3,    \
                                            .paselect  = SX127X_PARAM_PASELECT }
/** @} */

/**
 * @brief   Delay to wait for SPI reset
 */
#define WAIT_FOR_SPI_RESET (3000000UL)

/**
 * @brief    HDC1000 Temperature & Humidity sensor
 */
#define HDC1000_PARAM_ADDR           (0x40)

/**
 * @brief    TSL2561 Visible light sensor
 */
#define TSL2561_PARAM_ADDR           TSL2561_ADDR_LOW

/**
 * @brief   Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */

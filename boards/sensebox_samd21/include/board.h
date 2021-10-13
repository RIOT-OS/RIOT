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
#include "mtd_sdcard.h"

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
 * merged DIO0 and DIO1 interrupt pins into one. The driver will always
 * check the interrupt type in the ISR handler, so it's enough to set
 * the DIO0 pin in order to handle both DIO0 and DIO1.
 * @{
 */
#define SX127X_PARAM_SPI                    (SPI_DEV(0))

#define SX127X_PARAM_SPI_NSS                XBEE1_CS_PIN        /* D23 */

#define SX127X_PARAM_RESET                  GPIO_UNDEF

#define SX127X_PARAM_DIO0                   XBEE1_INT_PIN       /* D24 */

#define SX127X_PARAM_DIO1                   GPIO_UNDEF

#define SX127X_PARAM_DIO2                   GPIO_UNDEF

#define SX127X_PARAM_DIO3                   GPIO_UNDEF

#define SX127X_PARAM_PASELECT               (SX127X_PA_BOOST)
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
 * @brief    BMP280 Pressure and temperature sensor
 */
#define BMX280_PARAM_I2C_ADDR        (0x76)

#if defined(MODULE_MTD_SDCARD) || defined(DOXYGEN)
/**
 * @brief MTD device 0 (SD Card) definition. mtd0 is defined in board.c
 * @{
 */
extern mtd_dev_t *mtd0;
#define MTD_0 mtd0
/** @} */

/**
 * @brief Attributes for the mtd_sdcard driver
 * @{
 */
#ifndef MTD_SD_CARD_PAGE_SIZE
#define MTD_SD_CARD_PAGE_SIZE           (512)
#endif

#ifndef MTD_SD_CARD_PAGES_PER_SECTOR
#define MTD_SD_CARD_PAGES_PER_SECTOR    (128)
#endif

#ifndef MTD_SD_CARD_SECTOR_COUNT
#define MTD_SD_CARD_SECTOR_COUNT        (3921920UL)
#endif
/** @} */
#endif /* MODULE_MTD_SDCARD || DOXYGEN */

/**
 * @brief   Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */

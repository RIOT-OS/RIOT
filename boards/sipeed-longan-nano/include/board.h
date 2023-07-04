/*
 * Copyright (C) 2023 Gunar Schorcht <gunar@schorcht.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_sipeed_longan_nano
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Sipeed Longan Nano board
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef BOARD_H
#define BOARD_H

#include "board_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Button pin definitions
 * @{
 */
#define BTN0_PIN            GPIO_PIN(PORT_A, 8)
#define BTN0_MODE           GPIO_IN
#define BTN0_INT_FLANK      GPIO_RISING
/** @} */

/**
 * @name    LED (on-board) configuration
 * @{
 */
#define LED0_PIN            GPIO_PIN(PORT_C, 13)
#define LED0_MASK           (1 << 13)
#define LED0_ON             (GPIOC->BC = LED0_MASK)
#define LED0_OFF            (GPIOC->BOP = LED0_MASK)
#define LED0_TOGGLE         (GPIOC->OCTL ^= LED0_MASK)

#define LED1_PIN            GPIO_PIN(PORT_A, 1)
#define LED1_MASK           (1 << 1)
#define LED1_ON             (GPIOA->BC = LED1_MASK)
#define LED1_OFF            (GPIOA->BOP = LED1_MASK)
#define LED1_TOGGLE         (GPIOA->OCTL ^= LED1_MASK)

#define LED2_PIN            GPIO_PIN(PORT_A, 2)
#define LED2_MASK           (1 << 2)
#define LED2_ON             (GPIOA->BC = LED2_MASK)
#define LED2_OFF            (GPIOA->BOP = LED2_MASK)
#define LED2_TOGGLE         (GPIOA->OCTL ^= LED2_MASK)

#define LED_RED_PIN         LED0_PIN    /**< LED0 is red */
#define LED_GREEN_PIN       LED1_PIN    /**< LED1 is green */
#define LED_BLUE_PIN        LED2_PIN    /**< LED2 is blue */
/** @} */

#if defined(MODULE_SDCARD_SPI)
#define SDCARD_SPI_PARAM_SPI         SPI_DEV(0)
#define SDCARD_SPI_PARAM_CS          GPIO_PIN(PORT_B, 12)
#define SDCARD_SPI_PARAM_CLK         GPIO_PIN(PORT_B, 13)
#define SDCARD_SPI_PARAM_MISO        GPIO_PIN(PORT_B, 14)
#define SDCARD_SPI_PARAM_MOSI        GPIO_PIN(PORT_B, 15)
#endif

#if defined(MODULE_ST7735) && defined(CONFIG_SIPEED_LONGAN_NANO_WITH_TFT)
#define ST7735_PARAM_SPI          SPI_DEV(1)            /**< SPI device */
#define ST7735_PARAM_SPI_CLK      SPI_CLK_5MHZ          /**< SPI clock frequency */
#define ST7735_PARAM_SPI_MODE     SPI_MODE_0            /**< SPI mode */
#define ST7735_PARAM_CS           GPIO_PIN(PORT_B, 2)   /**< Chip Select pin */
#define ST7735_PARAM_DCX          GPIO_PIN(PORT_B, 0)   /**< DCX pin */
#define ST7735_PARAM_RST          GPIO_PIN(PORT_B, 1)   /**< Reset pin */
#define ST7735_PARAM_RGB          0                     /**< RGB mode enable */
#define ST7735_PARAM_INVERTED     1                     /**< Inverted mode enable */
#define ST7735_PARAM_NUM_LINES    160U                  /**< Number of lines */
#define ST7735_PARAM_RGB_CHANNELS 80U                   /**< Number of columns */
#define ST7735_PARAM_ROTATION     ST7735_ROTATION_HORZ  /**< Rotation mode */
#define ST7735_PARAM_OFFSET_Y     25                    /**< Vertical offset */
#endif

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */

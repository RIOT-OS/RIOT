/*
 * SPDX-FileCopyrightText: 2019 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_stm32f723e-disco
 * @{
 *
 * @file
 * @brief       Board specific definitions for the STM32F723E-DISCO board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LCD Backlight control defines
 * @{
 */
#define BACKLIGHT_PIN       GPIO_PIN(PORT_H, 11)    /**< Backlight pin (HIGH active LD_BL) */
#define BACKLIGHT_MASK      (1 << 11)               /**< Backlight pin mask */

/** Set the backlight pin */
#define BACKLIGHT_ON        (GPIOH->BSRR = BACKLIGHT_MASK)
/** Clear the backlight pin */
#define BACKLIGHT_OFF       (GPIOH->BSRR = (BACKLIGHT_MASK << 16))
/** Toggle the backlight pin */
#define BACKLIGHT_TOGGLE    (GPIOH->ODR  ^= BACKLIGHT_MASK)
/** @} */

/**
 * @name    LCD display definitions
 *
 * The STM32F723G-DISCO board has a 240 x 240 pixel TFT color LCD display with
 * ST7789H2 driver IC using the MCU 8080 16-bit parallel interface (default with
 * SB15=ON and SB16=OFF). SB15 and SB16 control the ST7789H2 `IM0` signal.
 * With SB15=OFF and SB16=ON, the MCU 8080 8-bit parallel interface is enabled.
 *
 * For the `LCD_*` pins, please refer to the
 * [schematic](https://www.st.com/resource/en/schematic_pack/mb1260-f723e-d03_schematic.pdf).
 *
 * @{
 */
#define LCD_SCREEN_WIDTH    240                     /**< LCD width */
#define LCD_SCREEN_HEIGHT   240                     /**< LCD height */

#define LCD_D0              GPIO_PIN(PORT_D, 14)    /**< LCD_D0 pin */
#define LCD_D1              GPIO_PIN(PORT_D, 15)    /**< LCD_D1 pin */
#define LCD_D2              GPIO_PIN(PORT_D, 0)     /**< LCD_D2 pin */
#define LCD_D3              GPIO_PIN(PORT_D, 1)     /**< LCD_D3 pin */
#define LCD_D4              GPIO_PIN(PORT_E, 7)     /**< LCD_D4 pin */
#define LCD_D5              GPIO_PIN(PORT_E, 8)     /**< LCD_D5 pin */
#define LCD_D6              GPIO_PIN(PORT_E, 9)     /**< LCD_D6 pin */
#define LCD_D7              GPIO_PIN(PORT_E, 10)    /**< LCD_D7 pin */
#define LCD_D8              GPIO_PIN(PORT_E, 11)    /**< LCD_D8 pin */
#define LCD_D9              GPIO_PIN(PORT_E, 12)    /**< LCD_D9 pin */
#define LCD_D10             GPIO_PIN(PORT_E, 13)    /**< LCD_D10 pin */
#define LCD_D11             GPIO_PIN(PORT_E, 14)    /**< LCD_D11 pin */
#define LCD_D12             GPIO_PIN(PORT_E, 15)    /**< LCD_D12 pin */
#define LCD_D13             GPIO_PIN(PORT_D, 8)     /**< LCD_D13 pin */
#define LCD_D14             GPIO_PIN(PORT_D, 9)     /**< LCD_D14 pin */
#define LCD_D15             GPIO_PIN(PORT_D, 10)    /**< LCD_D15 pin */
#define LCD_TE              GPIO_PIN(PORT_C, 8)     /**< LCD_TE pin */
#define LCD_WE              GPIO_PIN(PORT_D, 5)     /**< LCD_WE pin */
#define LCD_OE              GPIO_PIN(PORT_D, 4)     /**< LCD_OE pin */
#define LCD_RS              GPIO_PIN(PORT_F, 0)     /**< LCD_RS pin */
#define LCD_NE              GPIO_PIN(PORT_G, 9)     /**< LCD_NE pin */
#define LCD_RST             GPIO_PIN(PORT_H, 7)     /**< LCD_RST pin */
#define LCD_BACKLIGHT       BACKLIGHT_PIN           /**< LCD_BL pin */

#define ST77XX_PARAM_CNTRL         ST77XX_CNTRL_ST7789  /**< ST77xx controller variant */
#define ST77XX_PARAM_SPI           SPI_UNDEF        /**< parallel interface is used */
#define ST77XX_PARAM_D0            LCD_D0           /**< ST77xx D0 signal */
#define ST77XX_PARAM_D1            LCD_D1           /**< ST77xx D1 signal */
#define ST77XX_PARAM_D2            LCD_D2           /**< ST77xx D2 signal */
#define ST77XX_PARAM_D3            LCD_D3           /**< ST77xx D3 signal */
#define ST77XX_PARAM_D4            LCD_D4           /**< ST77xx D4 signal */
#define ST77XX_PARAM_D5            LCD_D5           /**< ST77xx D5 signal */
#define ST77XX_PARAM_D6            LCD_D6           /**< ST77xx D6 signal */
#define ST77XX_PARAM_D7            LCD_D7           /**< ST77xx D7 signal */
#define ST77XX_PARAM_D8            LCD_D8           /**< ST77xx D8 signal */
#define ST77XX_PARAM_D9            LCD_D9           /**< ST77xx D9 signal */
#define ST77XX_PARAM_D10           LCD_D10          /**< ST77xx D10 signal */
#define ST77XX_PARAM_D11           LCD_D11          /**< ST77xx D11 signal */
#define ST77XX_PARAM_D12           LCD_D12          /**< ST77xx D12 signal */
#define ST77XX_PARAM_D13           LCD_D13          /**< ST77xx D13 signal */
#define ST77XX_PARAM_D14           LCD_D14          /**< ST77xx D14 signal */
#define ST77XX_PARAM_D15           LCD_D15          /**< ST77xx D15 signal */
#define ST77XX_PARAM_WRX           LCD_WE           /**< ST77xx WR signal */
#define ST77XX_PARAM_RDX           LCD_OE           /**< ST77xx RD signal */
#define ST77XX_PARAM_DCX           LCD_RS           /**< ST77xx RS signal */
#define ST77XX_PARAM_RST           LCD_RST          /**< ST77xx RST signal */
#define ST77XX_PARAM_CS            LCD_NE           /**< ST77xx CS signal */
#define ST77XX_PARAM_RGB           1                /**< ST77xx RGB mode */
#define ST77XX_PARAM_INVERTED      1                /**< ST77xx inverted colors */
#define ST77XX_PARAM_ROTATION      ST77XX_ROTATION_90  /**< ST77xx rotation */
#define ST77XX_PARAM_OFFSET_X      80                  /**< ST77xx offset because of rotation */
#define ST77XX_PARAM_NUM_LINES     LCD_SCREEN_HEIGHT   /**< ST77xx number of lines */
#define ST77XX_PARAM_RGB_CHANNELS  LCD_SCREEN_WIDTH    /**< ST77xx number of channels */
/** @} */

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LED0_PIN_NUM        5
#define LED0_PORT           GPIO_PORT_A /**< GPIO port of LED 0 */
#define LED0_PORT_NUM       PORT_A

#define LED1_PIN_NUM        7
#define LED1_PORT           GPIO_PORT_A /**< GPIO port of LED 1 */
#define LED1_PORT_NUM       PORT_A

#define LED2_PIN_NUM        1
#define LED2_PORT           GPIO_PORT_B /**< GPIO port of LED 2 */
#define LED2_PORT_NUM       PORT_B
/** @} */

/**
 * @name User button
 * @{
 */
#define BTN0_PIN            GPIO_PIN(PORT_A, 0)
#define BTN0_MODE           GPIO_IN
/** @} */

/**
 * @name FT5X06 touch panel configuration
 * @{
 */
#define FT5X06_PARAM_I2C_DEV            I2C_DEV(1)              /**< I2C device */
#define FT5X06_PARAM_INT_PIN            GPIO_PIN(PORT_I, 9)     /**< Interrupt pin */
#define FT5X06_PARAM_XMAX               (240)                   /**< Max width */
#define FT5X06_PARAM_YMAX               (240)                   /**< Max height */
#define FT5X06_PARAM_XYCONV             FT5X06_NO_CONV          /**< No coordinate conversion */
#define FT5X06_PARAM_TYPE               FT5X06_TYPE_FT6X06      /**< Device type */
/** @} */

#ifdef __cplusplus
}
#endif

#include "stm32_leds.h"

/** @} */

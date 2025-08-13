/*
 * SPDX-FileCopyrightText: 2018 Inria
 * SPDX-FileCopyrightText: 2023 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_stm32l496g-disco
 * @{
 *
 * @file
 * @brief       Board specific definitions for the STM32L496G-DISCO board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#include <stdint.h>

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LCD Backlight control defines
 * @{
 */
#define BACKLIGHT_PIN       GPIO_PIN(PORT_I, 0)     /**< Backlight pin (HIGH active LD_BL) */
#define BACKLIGHT_MASK      (1 << 0)                /**< Backlight pin mask */

/** Set the backlight pin */
#define BACKLIGHT_ON        (GPIOI->BSRR = BACKLIGHT_MASK)
/** Clear the backlight pin */
#define BACKLIGHT_OFF       (GPIOI->BSRR = (BACKLIGHT_MASK << 16))
/** Toggle the backlight pin */
#define BACKLIGHT_TOGGLE    (GPIOI->ODR  ^= BACKLIGHT_MASK)
/** @} */

/**
 * @name    LCD display definitions
 *
 * The STM32L496-DISCO board has a 240 x 240 pixel TFT color LCD display with
 * ST7789 driver IC using MCU 8080 16-bit parallel interface (default with
 * SB14=ON and SB15=OFF). SB14 and SB15 control the ST7789 `IM0` signal.
 * With SB14=OFF and SB15=ON, the MCU 8080 8-bit parallel interface is enabled.
 *
 * For the `LCD_*` pins the identifiers are used as given in the [schematic]
 * (https://www.st.com/resource/en/schematic_pack/mb1261-l496g-b06-schematic.pdf).
 *
 * The LCD display has to be switched on explicitly by activating VDD_LCD using
 * the LOW active `LD_PWR_ON` signal.
 *
 * @note The `LCD_RESET` signal is provided by the MFX GPIO2 pin. Since the MFX
 * is not yet supported, the `LCD_RESET` signal cannot be used by the display
 * driver. However, since the `LCD_RESET` signal is pulled up and the MFX GPIO2
 * pin is configured as a floating input by default, the driver will work
 * without MFX support and the `LCD_RESET` signal.
 *
 * @{
 */
#define LCD_DISP_PIN        GPIO_PIN(PORT_H, 0)     /**< LCD power pin (LOW active LD_PWR_ON) */
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
#define LCD_TE              GPIO_PIN(PORT_H, 7)     /**< LCD_TE pin */
#define LCD_WE              GPIO_PIN(PORT_D, 5)     /**< LCD_WE pin */
#define LCD_OE              GPIO_PIN(PORT_D, 4)     /**< LCD_OE pin */
#define LCD_RS              GPIO_PIN(PORT_D, 13)    /**< LCD_RS pin */
#define LCD_NE              GPIO_PIN(PORT_D, 7)     /**< LCD_NE pin */
#define LCD_RST             GPIO_UNDEF              /**< LCD_RST pin (MFX_GPIO2) */
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
#define ST77XX_PARAM_ROTATION      ST77XX_ROTATION_0   /**< ST77xx rotation */
#define ST77XX_PARAM_NUM_LINES     LCD_SCREEN_HEIGHT   /**< ST77xx number of lines */
#define ST77XX_PARAM_RGB_CHANNELS  LCD_SCREEN_WIDTH    /**< ST77xx number of channels */
/** @} */

/**
 * @name Touch panel configuration
 *
 * The board uses a FT6236 touch panel driver IC.
 *
 * @{
 */
#define FT5X06_PARAM_I2C_DEV    I2C_DEV(1)           /**< I2C device */
#define FT5X06_PARAM_INT_PIN    GPIO_PIN(PORT_G, 14) /**< Interrupt pin */
#define FT5X06_PARAM_XMAX       LCD_SCREEN_WIDTH     /**< Max width */
#define FT5X06_PARAM_YMAX       LCD_SCREEN_HEIGHT    /**< Max height */
#define FT5X06_PARAM_TYPE       FT5X06_TYPE_FT6X36   /**< Device type */
#define FT5X06_PARAM_XYCONV     FT5X06_SWAP_XY | FT5X06_MIRROR_Y /**< Swap XY, then mirror Y */
/** @} */

/**
 * @name    LED pin definitions and handlers
 *
 * The board has 3 user LEDs:
 * - LD1 orange, active HIGH (not directly usable, connected to MFX GPIO4
 * - LD2 green, active HIGH (connected to PB13)
 * - LD3 green, active LOW (connected to PA5)
 * @{
 */
#define LED0_PIN_NUM        13          /**< green LED (LD2) pin */
#define LED0_PORT           GPIO_PORT_B /**< GPIO port of green LED (LD2) */
#define LED0_PORT_NUM       PORT_B      /**< green LED (LD2) port */

#define LED1_PIN_NUM        5           /**< green LED (LD3) pin */
#define LED1_PORT           GPIO_PORT_A /**< GPIO port of green LED (LD3) */
#define LED1_PORT_NUM       PORT_A      /**< green LED (LD3) port */
#define LED1_IS_INVERTED    1           /**< green LED (LD3) is LOW active */
/** @} */

/**
 * @name   Joystick buttons
 *
 * The board has a joystick that is exposed as user buttons.
 * @{
 */
#define BTN0_PIN            GPIO_PIN(PORT_C, 13)    /**< Center button pin  */
#define BTN0_MODE           GPIO_IN_PD              /**< Center button mode */

#define BTN1_PIN            GPIO_PIN(PORT_I, 9)     /**< Left button pin    */
#define BTN1_MODE           GPIO_IN_PD              /**< Left button mode   */

#define BTN2_PIN            GPIO_PIN(PORT_I, 10)    /**< Down button pin    */
#define BTN2_MODE           GPIO_IN_PD              /**< Down button mode   */

#define BTN3_PIN            GPIO_PIN(PORT_F, 11)    /**< Right button pin   */
#define BTN3_MODE           GPIO_IN_PD              /**< Right button mode  */

#define BTN4_PIN            GPIO_PIN(PORT_I, 8)     /**< Up button pin      */
#define BTN4_MODE           GPIO_IN_PD              /**< Up button mode     */
/** @} */

#ifdef __cplusplus
}
#endif

#include "stm32_leds.h"

/** @} */

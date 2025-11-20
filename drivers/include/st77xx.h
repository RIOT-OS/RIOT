/*
 * Copyright (C) 2018 Koen Zandberg
 *               2021 Francisco Molina
 *               2023 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    drivers_st77xx ST77xx display driver
 * @ingroup     drivers_display
 *
 * @brief       Driver for displays with a ST77xx display controller
 *
 * The ST77xx display controller driver is a driver for small RGB displays
 * using a ST77xx display controller. The driver supports ST7735, ST7789 and
 * ST7796. The support of the respective controller is enabled by the
 * corresponding module `st7735`, `st7789` or `st7796`. The ST77xx controller
 * used by a display must be specified in the board definition in parameter
 * @ref lcd_params_t::cntrl or as macro @ref ST77XX_PARAM_CNTRL if the
 * default parameter set @ref ST77XX_PARAMS is used.
 *
 * The driver communicates with the device either via an
 *
 * - SPI serial interface (if module `lcd_spi` enabled) or an
 * - MCU 8080 8-/16-bit parallel interface (if module `lcd_parallel` or
 *   module `lcd_parallel_16` is enabled).
 *
 * Usually the device driver is used either for a single display with SPI serial
 * interface or for a display with parallel MCU 8080 8-/16-bit parallel
 * interface. However, the device driver can also be used simultaneously for
 * multiple displays with different interfaces if several of the `lcd_spi`,
 * `lcd_parallel` and `lcd_parallel_16bit` modules are enabled at the same time.
 * In this case, please refer to the notes in @ref lcd_params_t.
 *
 * The device requires colors to be send in big endian RGB-565 format. The
 * @ref CONFIG_LCD_LE_MODE compile time option can switch this, but only use this
 * when strictly necessary. This option will slow down the driver as it
 * certainly can't use DMA anymore, every short has to be converted before
 * transfer.
 *
 * @{
 *
 * @file
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#include "lcd.h"

#ifdef MODULE_DISP_DEV
#include "disp_dev.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ST7735 Customized Configuration Enable
 *
 * Define CONFIG_ST7735_CUSTOM_CONFIG=1 to use customized voltage
 * configurations. Otherwise ST7735 is using reset defaults.
 */
#ifndef CONFIG_ST7735_CUSTOM_CONFIG
#define CONFIG_ST7735_CUSTOM_CONFIG     0
#endif

/**
 * @brief ST7735 AVDD voltage (in millivolts)
 *
 * A default voltage of 4.9V is used for AVDD.
 * Valid values must be in the range of 4500 (4.5V) to 5100 (5.1V)
 * in steps of 100.
 */
#ifndef CONFIG_ST7735_AVDD
#define CONFIG_ST7735_AVDD             4900
#endif

/**
 * @brief ST7735 GVDD voltage (in millivolts)
 *
 * A default voltage of 4.6V is used for GVDD (gamma reference positive voltage).
 * Valid values must be in the range of 3150 (3.15V) to 4700 (4.7V)
 * in steps of 50.
 */
#ifndef CONFIG_ST7735_GVDD
#define CONFIG_ST7735_GVDD             4600
#endif

/**
 * @brief ST7735 GVL voltage (in millivolts)
 *
 * A default voltage of -4.6V is used for GVCL (gamma reference negative voltage).
 * Valid values must be in the range of -4700 (-4.7V) to -3150 (-3.15V)
 * in steps of 50.
 */
#ifndef CONFIG_ST7735_GVCL
#define CONFIG_ST7735_GVCL             -4600
#endif

/**
 * @brief ST7735 VCOM voltage (in millivolts)
 *
 * A default voltage of -0.425V is used for VCOM. VCOM needs to be adjusted
 * to match the capacitance and performance specifications of the TFT panel
 * to maximize contrast and minimize flickering.
 * Valid values must be in the range of -2000 (-2.0V) to -425 (-0.425V)
 * in steps of 25.
 */
#ifndef CONFIG_ST7735_VCOM
#define CONFIG_ST7735_VCOM              -775
#endif

/**
 * @brief ST7735 VGH voltage (in millivolts)
 *
 * A default voltage of 14.7V is used for VGH, the high voltage for gate drivers.
 * Valid values must be in the range of 10000 (10V) to 15000 (15V) and
 * in the range of (2 * AVDD + 2.1V) and (3 * AVDD + 2.4 V).
 */
#ifndef CONFIG_ST7735_VGH
#define CONFIG_ST7735_VGH               14700
#endif

/**
 * @brief ST7735 VGL voltage (in millivolts)
 *
 * A default voltage of -10V is used for VGL, the low voltage for gate drivers.
 * Valid values must be in the range of -13000 (-13V) to -7500 (-7.5V)
 * in steps of 2500.
 */
#ifndef CONFIG_ST7735_VGL
#define CONFIG_ST7735_VGL               -10000
#endif

/**
 * @brief ST7789 Customized Configuration Enable
 *
 * Define CONFIG_ST7789_CUSTOM_CONFIG=1 to use customized voltage
 * configurations. Otherwise ST7735 is using reset defaults.
 */
#ifndef CONFIG_ST7789_CUSTOM_CONFIG
#define CONFIG_ST7789_CUSTOM_CONFIG     0
#endif

/**
 * @brief ST7789 AVDD voltage (in millivolts)
 *
 * A default voltage of 6.8V is used for AVDD.
 * Valid values must be in the range of 6400 (6.4V) to 6800 (6.8V)
 * in steps of 100.
 */
#ifndef CONFIG_ST7789_AVDD
#define CONFIG_ST7789_AVDD              6800
#endif

/**
 * @brief ST7789 AVCL voltage (in millivolts)
 *
 * A default voltage of -4.8V is used for AVCL.
 * Valid values must be in the range of -5000 (-5.0V) to -4400 (-4.4V)
 * in steps of 100.
 */
#ifndef CONFIG_ST7789_AVCL
#define CONFIG_ST7789_AVCL              -4800
#endif

/**
 * @brief ST7789 VCOM voltage (in millivolts)
 *
 * A default voltage of 0.9V is used for VCOM. VCOM needs to be adjusted
 * to match the capacitance and performance specifications of the TFT panel
 * to maximize contrast and minimize flickering. VCOM is used to derive
 * the GVDD (gamma reference positive voltage) and
 * the GVCL (gamma reference negative voltage) as follows:
 *
 *     GDDV = +VRH + VCOM + VCOM_OFFSET + (0.5 * VDV)
 *     GVCL = -VRH + VCOM + VCOM_OFFSET - (0.5 * VDV)
 *
 * Valid values must be in the range of 100 (0.1V) to 1675 (1.675V)
 * in steps of 25.
 */
#ifndef CONFIG_ST7789_VCOM
#define CONFIG_ST7789_VCOM              900
#endif

/**
 * @brief ST7789 VCOM voltage offset (in millivolts)
 *
 * A default voltage of 0V is used for VCOM voltage offset (VCOM_OFFSET).
 * VCOM_OFFSET is used to derive
 * the GVDD (gamma reference positive voltage) and
 * the GVCL (gamma reference negative voltage) as follows:
 *
 *     GDDV = +VRH + VCOM + VCOM_OFFSET + (0.5 * VDV)
 *     GVCL = -VRH + VCOM + VCOM_OFFSET - (0.5 * VDV)
 *
 * Valid values must be in the range of -800 (-0.8V) to 775 (0.775V)
 * in steps of 25.
 */
#ifndef CONFIG_ST7789_VCOM_OFFSET
#define CONFIG_ST7789_VCOM_OFFSET       0
#endif

/**
 * @brief ST7789 VDV voltage (in millivolts)
 *
 * A default voltage of 0V is used for VDV. VDV is used to derive
 * the GVDD (gamma reference positive voltage) and
 * the GVCL (gamma reference negative voltage) as follows:
 *
 *     GDDV = +VRH + VCOM + VCOM_OFFSET + (0.5 * VDV)
 *     GVCL = -VRH + VCOM + VCOM_OFFSET - (0.5 * VDV)
 *
 * Valid values must be in the range of -800 (-0.8V) to 775 (0.775V)
 * in steps of 25.
 */
#ifndef CONFIG_ST7789_VDV
#define CONFIG_ST7789_VDV               0
#endif

/**
 * @brief ST7789 VRH voltage (in millivolts)
 *
 * A default voltage of 4.1V is used for VRH. VRH is used to derive
 * the GVDD (gamma reference positive voltage) and
 * the GVCL (gamma reference negative voltage) as follows:
 *
 *     GDDV = +VRH + VCOM + VCOM_OFFSET + (0.5 * VDV)
 *     GVCL = -VRH + VCOM + VCOM_OFFSET - (0.5 * VDV)
 *
 * Valid values must be in the range of 3350 (3.35V) to 5500 (5.5V)
 * in steps of 50.
 */
#ifndef CONFIG_ST7789_VRH
#define CONFIG_ST7789_VRH               4100
#endif

/**
 * @brief ST7796 Customized Configuration Enable
 *
 * Define CONFIG_ST7796_CUSTOM_CONFIG=1 to use customized voltage
 * configurations. Otherwise ST7735 is using reset defaults.
 */
#ifndef CONFIG_ST7796_CUSTOM_CONFIG
#define CONFIG_ST7796_CUSTOM_CONFIG     0
#endif

/**
 * @brief ST7796 AVDD voltage (in millivolts)
 *
 * A default voltage of 6.6V is used for AVDD.
 * Valid values must be in the range of 6200 (6.2V) to 6800 (6.8V)
 * in steps of 100.
 */
#ifndef CONFIG_ST7796_AVDD
#define CONFIG_ST7796_AVDD              6600
#endif

/**
 * @brief ST7796 AVCL voltage (in millivolts)
 *
 * A default voltage of -4.4V is used for AVCL.
 * Valid values must be in the range of -5000 (-5.0V) to -4400 (-4.4V)
 * in steps of 100.
 */
#ifndef CONFIG_ST7796_AVCL
#define CONFIG_ST7796_AVCL              -4400
#endif

/**
 * @brief ST7796 VCOM voltage (in millivolts)
 *
 * A default voltage of 1.0V is used for VCOM. VCOM needs to be adjusted
 * to match the capacitance and performance specifications of the TFT panel
 * to maximize contrast and minimize flickering. VCOM is used to derive
 * the GVDD (gamma reference positive voltage) and
 * the GVCL (gamma reference negative voltage) as follows:
 *
 *     GDDV = +VRH + VCOM + VCOM_OFFSET
 *     GVCL = -VRH + VCOM + VCOM_OFFSET
 *
 * Valid values must be in the range of 100 (0.1V) to 1875 (1.875V)
 * in steps of 25.
 */
#ifndef CONFIG_ST7796_VCOM
#define CONFIG_ST7796_VCOM              1000
#endif

/**
 * @brief ST7796 VCOM voltage offset (in millivolts)
 *
 * A default voltage of 0V is used for VCOM voltage offset (VCOM_OFFSET).
 * VCOM_OFFSET is used to derive
 * the GVDD (gamma reference positive voltage) and
 * the GVCL (gamma reference negative voltage) as follows:
 *
 *     GDDV = +VRH + VCOM + VCOM_OFFSET
 *     GVCL = -VRH + VCOM + VCOM_OFFSET
 *
 * Valid values must be in the range of -800 (-0.8V) to 775 (0.775V)
 * in steps of 25.
 */
#ifndef CONFIG_ST7796_VCOM_OFFSET
#define CONFIG_ST7796_VCOM_OFFSET       0
#endif

/**
 * @brief ST7796 VRH voltage (in millivolts)
 *
 * A default voltage of 4.1V is used for VRH. VRH is used to derive
 * the GVDD (gamma reference positive voltage) and
 * the GVCL (gamma reference negative voltage) as follows:
 *
 *     GDDV = +VRH + VCOM + VCOM_OFFSET
 *     GVCL = -VRH + VCOM + VCOM_OFFSET
 *
 * Valid values must be in the range of 3350 (3.35V) to 5500 (5.5V)
 * in steps of 50.
 */
#ifndef CONFIG_ST7796_VRH
#define CONFIG_ST7796_VRH               4100
#endif

/**
 * @name    ST77xx display rotation modes
 *
 * @note Using a rotation mode may require the definition of
 *       @ref ST77XX_PARAM_OFFSET_X or @ref ST77XX_PARAM_OFFSET_Y.
 *
 * Usually the dimension of an LCD is defined by W x H (width x height) in
 * pixels, where width is the smaller dimension than height, e.g. 240 x 320
 * (W x H). The height is then used as parameter @ref ST77XX_PARAM_NUM_LINES
 * and the width as parameter @ref ST77XX_PARAM_RGB_CHANNELS. So vertical
 * orientation means no rotation. Vertical orientation is given when the
 * connector is shown at the bottom of the display.
 *
 * For example, the ST7789 controller supports 320 gate outputs and 240 source
 * outputs (RGB channels). A display with a size of 240 x 320 (W x H) pixels
 * can be used by setting `ST77XX_PARAM_NUM_LINES=320` and
 * `ST77XX_PARAM_RGB_CHANNELS=240`.
 *
 * However, if the ST7789 is used with a display of 240 x 240 (W x H) pixels
 * and a rotation is used, an offset must be defined for X or Y because the
 * origin of the image will change. For example, in the case of
 * @ref ST77XX_ROTATION_90 an offset for X would have to be defined by
 * `ST77XX_PARAM_OFFSET_X=80` and in the case of @ref ST77XX_ROTATION_180
 * an offset for Y would have to be defined by `ST77XX_PARAM_OFFSET_X=80`.
 *
 * Using the correct offset on rotation can be even more difficult if the
 * display size in both dimensions is smaller than the size supported by
 * the controller. In this case, the origin can deviate from 0, 0 even
 * without rotation.
 *
 * @{
 */
#define ST77XX_ROTATION_VERT            0                   /**< Vertical mode */
#define ST77XX_ROTATION_VERT_FLIP       LCD_MADCTL_MX | \
                                        LCD_MADCTL_MY       /**< Flipped vertical */
#define ST77XX_ROTATION_HORZ            LCD_MADCTL_MV | \
                                        LCD_MADCTL_MX       /**< Horizontal mode */
#define ST77XX_ROTATION_HORZ_FLIP       LCD_MADCTL_MV | \
                                        LCD_MADCTL_MY       /**< Horizontal flipped */

#define ST77XX_ROTATION_0               0                   /**< 0 deg counterclockwise */
#define ST77XX_ROTATION_90              LCD_MADCTL_MV | \
                                        LCD_MADCTL_MY       /**< 90 deg counterclockwise */
#define ST77XX_ROTATION_180             LCD_MADCTL_MX | \
                                        LCD_MADCTL_MY       /**< 180 deg counterclockwise */
#define ST77XX_ROTATION_270             LCD_MADCTL_MV | \
                                        LCD_MADCTL_MX       /**< 270 deg counterclockwise */
/** @} */

/**
 * @brief    ST77xx display controller variants
 */
enum {
    ST77XX_CNTRL_ST7735,
    ST77XX_CNTRL_ST7789,
    ST77XX_CNTRL_ST7796,
};

/**
 * @brief   Device descriptor for a ST77xx display
 *
 */
typedef struct {
    lcd_t dev;                    /**< Pointer to the common lcd device */
} st77xx_t;

/**
 * @brief   LCD device operations table
 */
extern const lcd_driver_t lcd_st77xx_driver;

#ifdef __cplusplus
}
#endif
/** @} */

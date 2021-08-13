/*
 * Copyright (C) 2018 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_ili9341 ILI9341 display driver
 * @ingroup     drivers_display
 *
 * @brief       Driver for the ILI9341 display
 *
 * @{
 *
 * @file
 * @brief       Driver for ili941 display
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * The ILI9341 is a generic display driver for small RGB displays. The driver
 * implemented here operates over SPI to communicate with the device.
 *
 * The device requires colors to be send in big endian RGB-565 format. The
 * @ref CONFIG_ILI9341_LE_MODE compile time option can switch this, but only use this
 * when strictly necessary. This option will slow down the driver as it
 * certainly can't use DMA anymore, every short has to be converted before
 * transfer.
 */

#ifndef ILI9341_H
#define ILI9341_H

#include "periph/spi.h"
#include "periph/gpio.h"

#ifdef MODULE_DISP_DEV
#include "disp_dev.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup drivers_ili9341_config     ILI9341 display driver compile configuration
 * @ingroup config_drivers_display
 * @{
 */
/**
 * @brief ILI9341 gvdd level.
 *
 * Default GVDD voltage of 4.8V. GVDD is reference level for the VCOM level and
 * the grayscale voltage level. GVDD should be ≦ (AVDD - 0.5) V .
 */
#ifndef CONFIG_ILI9341_GVDD
#define CONFIG_ILI9341_GVDD             4800
#endif

/**
 * @brief ILI9341 VCOMH voltage level.
 *
 * Default VCOMH voltage of 4.25V. VCOMH represents the high level of VCOM AC
 * voltage. VCOM levels needs to be adjusted to match the capacitance and
 * performance specifications of the TFT panel to maximize contrast and minimize
 * flickering.
 */
#ifndef CONFIG_ILI9341_VCOMH
#define CONFIG_ILI9341_VCOMH            4250
#endif

/**
 * @brief ILI9341 VCOML voltage level.
 *
 * Default VCOML voltage of -2V. VCOML represents the low level of VCOM AC
 * voltage. VCOM levels needs to be adjusted to match the capacitance and
 * performance specifications of the TFT panel to maximize contrast and minimize
 * flickering
 */
#ifndef CONFIG_ILI9341_VCOML
#define CONFIG_ILI9341_VCOML            -2000
#endif

/**
 * @brief Convert little endian colors to big endian.
 *
 * Compile time switch to change the driver to convert little endian
 * colors to big endian.
 */
#ifdef DOXYGEN
#define CONFIG_ILI9341_LE_MODE
#endif
/** @} */

/**
 * @brief   Device initialization parameters
 */
typedef struct {
    spi_t spi;          /**< SPI device that the display is connected to */
    spi_clk_t spi_clk;  /**< SPI clock speed to use */
    spi_mode_t spi_mode;/**< SPI mode */
    gpio_t cs_pin;      /**< pin connected to the CHIP SELECT line */
    gpio_t dcx_pin;     /**< pin connected to the DC line */
    gpio_t rst_pin;     /**< pin connected to the reset line */
    bool rgb;           /**< True when display is connected in RGB mode
                          *  False when display is connected in BGR mode */
    bool inverted;      /**< Display works in inverted color mode */
    uint16_t lines;     /**< Number of lines, from 16 to 320 in 8 line steps */
} ili9341_params_t;

/**
 * @brief   Device descriptor for a ili9341
 */
typedef struct {
#ifdef MODULE_DISP_DEV
    disp_dev_t *dev;                    /**< Pointer to the generic display device */
#endif
    const ili9341_params_t *params;     /**< Device initialization parameters */
} ili9341_t;

/**
 * @brief   Setup an ili9341 display device
 *
 * @param[out]  dev     device descriptor
 * @param[in]   params  parameters for device initialization
 */
int ili9341_init(ili9341_t *dev, const ili9341_params_t *params);

/**
 * @brief   Fill a rectangular area with a single pixel color
 *
 * the rectangular area is defined as x1 being the first column of pixels and
 * x2 being the last column of pixels to fill. similar to that, y1 is the first
 * row to fill and y2 is the last row to fill.
 *
 * @param[in]   dev     device descriptor
 * @param[in]   x1      x coordinate of the first corner
 * @param[in]   x2      x coordinate of the opposite corner
 * @param[in]   y1      y coordinate of the first corner
 * @param[in]   y2      y coordinate of the opposite corner
 * @param[in]   color   single color to fill the area with
 */
void ili9341_fill(const ili9341_t *dev, uint16_t x1, uint16_t x2,
                  uint16_t y1, uint16_t y2, uint16_t color);

/**
 * @brief   Fill a rectangular area with an array of pixels
 *
 * the rectangular area is defined as x1 being the first column of pixels and
 * x2 being the last column of pixels to fill. similar to that, y1 is the first
 * row to fill and y2 is the last row to fill.
 *
 * @note @p color must have a length equal to `(x2 - x1 + 1) * (y2 - y1 + 1)`
 *
 * @param[in]   dev     device descriptor
 * @param[in]   x1      x coordinate of the first corner
 * @param[in]   x2      x coordinate of the opposite corner
 * @param[in]   y1      y coordinate of the first corner
 * @param[in]   y2      y coordinate of the opposite corner
 * @param[in]   color   array of colors to fill the area with
 */
void ili9341_pixmap(const ili9341_t *dev, uint16_t x1, uint16_t x2, uint16_t y1,
                 uint16_t y2, const uint16_t *color);

/**
 * @brief   Raw write command
 *
 * @param[in]   dev     device descriptor
 * @param[in]   cmd     command code
 * @param[in]   data    command data to the device
 * @param[in]   len     length of the command data
 */
void ili9341_write_cmd(const ili9341_t *dev, uint8_t cmd, const uint8_t *data,
                       size_t len);

/**
 * @brief   Raw read command
 *
 * @pre         len > 0
 *
 * @param[in]   dev     device descriptor
 * @param[in]   cmd     command
 * @param[out]  data    data from the device
 * @param[in]   len     length of the returned data
 */
void ili9341_read_cmd(const ili9341_t *dev, uint8_t cmd, uint8_t *data, size_t len);

/**
 * @brief   Invert the display colors
 *
 * @param[in]   dev     device descriptor
 */
void ili9341_invert_on(const ili9341_t *dev);

/**
 * @brief   Disable color inversion
 *
 * @param[in]   dev     device descriptor
 */
void ili9341_invert_off(const ili9341_t *dev);

#ifdef __cplusplus
}
#endif
#endif /* ILI9341_H */
/** @} */

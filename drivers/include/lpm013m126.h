/*
 * Copyright (C) 2018 Koen Zandberg
 * Copyright (C) 2022 Nicole Faerber <nicole.faerber@digitaluhr-manufaktur.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_lpm013m126 LPM013M126 display driver
 * @ingroup     drivers_display
 *
 * @brief       Driver for the LPM013M126 display
 *
 * @{
 *
 * @file
 * @brief       Driver for lpm013m126 display
 *
 * @author      Nicole Faerber <nicole.faerber@digitaluhr-manufaktur.de>
 *
 * The LPM013M126 is a generic display driver for small RGB displays. The driver
 * implemented here operates over SPI to communicate with the device.
 *
 */

#ifndef LPM013M126_H
#define LPM013M126_H

#include "periph/spi.h"
#include "periph/gpio.h"

#ifdef MODULE_DISP_DEV
#include "disp_dev.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif



/**
 * @brief   Device initialization parameters
 */
typedef struct {
    spi_t spi;                      /**< SPI device that the display is connected to */
    spi_clk_t spi_clk;              /**< SPI clock speed to use */
    spi_mode_t spi_mode;            /**< SPI mode */
    gpio_t cs_pin;                  /**< pin connected to the CHIP SELECT line */
    gpio_t extcom_pin;                 /**< pin connected to the DC line */
    gpio_t disp_pin;                 /**< pin connected to the reset line */
    bool inverted;                  /**< Display works in inverted color mode */
    uint16_t lines;                 /**< Number of lines, from 16 to 320 in 8 line steps */
} lpm013m126_params_t;

/**
 * @brief   Device descriptor for a ili9341
 */
typedef struct {
#ifdef MODULE_DISP_DEV
    disp_dev_t *dev;                    /**< Pointer to the generic display device */
#endif
    const lpm013m126_params_t *params;     /**< Device initialization parameters */
} lpm013m126_t;

/**
 * @brief   Setup an lpm013m126 display device
 *
 * @param[out]  dev     device descriptor
 * @param[in]   params  parameters for device initialization
 */
int lpm013m126_init(lpm013m126_t *dev, const lpm013m126_params_t *params);

int lpm013m126_write_lines(const lpm013m126_t *dev, uint8_t *lines, uint8_t start, uint16_t cnt);

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
void lpm013m126_pixmap(const lpm013m126_t *dev, uint16_t x1, uint16_t x2, uint16_t y1,
                 uint16_t y2, const uint16_t *color);

/**
 * @brief   Invert the display colors
 *
 * @param[in]   dev     device descriptor
 */
void lpm013m126_invert(const lpm013m126_t *dev);

/**
 * @brief   Disable color inversion
 *
 * @param[in]   dev     device descriptor
 */
void lpm013m126_noupdate(const lpm013m126_t *dev);

#ifdef __cplusplus
}
#endif
#endif /* LPM013M126_H */
/** @} */

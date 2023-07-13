/*
 * Copyright (C) 2018 Koen Zandberg
 *               2021 Francisco Molina
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_lcd LCD display driver
 * @ingroup     drivers_display
 *
 * @brief       Driver for the LCD display
 *
 * @{
 *
 * @file
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * The LCD is a generic display driver for small RGB displays. The driver
 * implemented here operates over SPI to communicate with the device.
 *
 * The device requires colors to be send in big endian RGB-565 format. The
 * @ref CONFIG_LCD_LE_MODE compile time option can switch this, but only use this
 * when strictly necessary. This option will slow down the driver as it
 * certainly can't use DMA anymore, every short has to be converted before
 * transfer.
 */

#ifndef LCD_H
#define LCD_H

#include "board.h"
#include "periph/spi.h"
#include "periph/gpio.h"

#ifdef MODULE_DISP_DEV
#include "disp_dev.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Convert little endian colors to big endian.
 *
 * Compile time switch to change the driver to convert little endian
 * colors to big endian.
 */
#ifdef DOXYGEN
#define CONFIG_LCD_LE_MODE
#endif

/**
 * @name Memory access control bits
 * @{
 */
#define LCD_MADCTL_MY           0x80    /**< Row address order */
#define LCD_MADCTL_MX           0x40    /**< Column access order */
#define LCD_MADCTL_MV           0x20    /**< Row column exchange */
#define LCD_MADCTL_ML           0x10    /**< Vertical refresh order */
#define LCD_MADCTL_BGR          0x08    /**< Color selector switch control */
#define LCD_MADCTL_MH           0x04    /**< Horizontal refresh direction */
/** @} */

/**
 * @brief   Device initialization parameters
 */
typedef  struct {
    spi_t spi;                  /**< SPI device that the display is connected to */
    spi_clk_t spi_clk;          /**< SPI clock speed to use */
    spi_mode_t spi_mode;        /**< SPI mode */
    gpio_t cs_pin;              /**< pin connected to the CHIP SELECT line */
    gpio_t dcx_pin;             /**< pin connected to the DC line */
    gpio_t rst_pin;             /**< pin connected to the reset line */
    bool rgb;                   /**< True when display is connected in RGB mode
                                 *  False when display is connected in BGR mode */
    bool inverted;              /**< Display works in inverted color mode */
    uint16_t lines;             /**< Number of lines, from 16 to 320 in 8 line steps */
    uint16_t rgb_channels;      /**< Display rgb channels */
    uint8_t rotation;           /**< Display rotation mode */
    uint8_t offset_x;           /**< LCD offset to apply on x axis. */
    uint8_t offset_y;           /**< LCD offset to apply on y axis. */
#if MODULE_LCD_MULTI_CNTRL || DOXYGEN
    uint8_t cntrl;              /**< controller variant used, if the controller-
                                     specific driver supports multiple
                                     controller variants */
#endif
} lcd_params_t;

/**
 * @brief   LCD driver interface
 *
 * This define the functions to access a LCD.
 */
typedef struct lcd_driver lcd_driver_t;

/**
 * @brief   Device descriptor for a lcd
 */
typedef struct {
#ifdef MODULE_DISP_DEV
    disp_dev_t *dev;                /**< Pointer to the generic display device */
#endif
    const lcd_driver_t *driver;     /**< LCD driver */
    const lcd_params_t *params;     /**< Device initialization parameters */
} lcd_t;

/**
 * @brief   LCD driver interface
 *
 * This define the functions to access a LCD.
 *
 */
struct lcd_driver {
    /**
     * @brief   Initialize LCD controller
     *
     * @param[in] dev  Pointer to the selected driver
     *
     * @returns 0 on success
     * @returns < 0 value in error
     */
    int (*init)(lcd_t *dev, const lcd_params_t *params);

    /**
     * @brief   Set area LCD work area
     *
     * This function pointer can be NULL if the controller specific driver
     * does not require anything special. In this case the default
     * implementation is used which sets the column addresses and the row
     * addresses of the area including the coordinates of the opposite corner.
     *
     * @param[in] dev  Pointer to the selected driver
     * @param[in] x1   x coordinate of the first corner
     * @param[in] x2   x coordinate of the opposite corner
     * @param[in] y1   y coordinate of the first corner
     * @param[in] y2   y coordinate of the opposite corner
     *
     */
    void (*set_area)(const lcd_t *dev, uint16_t x1, uint16_t x2, uint16_t y1,
                     uint16_t y2);
};

/**
 * @brief   Low Level to acquire the device
 *
 * @param[out]  dev     device descriptor
 */
void lcd_ll_acquire(const lcd_t *dev);

/**
 * @brief   Low Level function to release the device
 *
 * @param[out]  dev     device descriptor
 */
void lcd_ll_release(const lcd_t *dev);

/**
 * @brief   Low level function to write a command
 *
 * @pre The device must have already been acquired with @ref lcd_ll_acquire
 *      before this function can be called.
 *
 * @param[in]   dev     device descriptor
 * @param[in]   cmd     command code
 * @param[in]   data    command data to the device
 * @param[in]   len     length of the command data
 */
void lcd_ll_write_cmd(const lcd_t *dev, uint8_t cmd, const uint8_t *data,
                      size_t len);

/**
 * @brief   Low level function for read command command
 *
 * @note Very often the SPI MISO signal of the serial interface or the RDX
 *       signal of the MCU 8080 parallel interface are not connected to the
 *       display. In this case the read command does not provide valid data.
 *
 * @pre The device must have already been acquired with @ref lcd_ll_acquire
 *      before this function can be called.
 * @pre len > 0
 *
 * @param[in]   dev     device descriptor
 * @param[in]   cmd     command
 * @param[out]  data    data from the device
 * @param[in]   len     length of the returned data
 */
void lcd_ll_read_cmd(const lcd_t *dev, uint8_t cmd, uint8_t *data, size_t len);

/**
 * @brief   Setup an lcd display device
 *
 * @param[out]  dev     device descriptor
 * @param[in]   params  parameters for device initialization
 */
int lcd_init(lcd_t *dev, const lcd_params_t *params);

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
void lcd_fill(const lcd_t *dev, uint16_t x1, uint16_t x2,
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
void lcd_pixmap(const lcd_t *dev, uint16_t x1, uint16_t x2, uint16_t y1,
                uint16_t y2, const uint16_t *color);

/**
 * @brief   Raw write command
 *
 * @param[in]   dev     device descriptor
 * @param[in]   cmd     command code
 * @param[in]   data    command data to the device
 * @param[in]   len     length of the command data
 */
void lcd_write_cmd(const lcd_t *dev, uint8_t cmd, const uint8_t *data,
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
void lcd_read_cmd(const lcd_t *dev, uint8_t cmd, uint8_t *data, size_t len);

/**
 * @brief   Invert the display colors
 *
 * @param[in]   dev     device descriptor
 */
void lcd_invert_on(const lcd_t *dev);

/**
 * @brief   Disable color inversion
 *
 * @param[in]   dev     device descriptor
 */
void lcd_invert_off(const lcd_t *dev);

#ifdef __cplusplus
}
#endif
#endif /* LCD_H */
/** @} */

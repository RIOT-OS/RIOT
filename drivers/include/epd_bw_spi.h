/*
 * Copyright (C) 2019 Silke Hofstra
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef EPD_BW_SPI_H
#define EPD_BW_SPI_H

/**
 * @defgroup    drivers_epd_bw_spi Generic black/white e-paper/e-ink SPI display driver.
 * @ingroup     drivers_display
 * @brief       Device driver for black/white e-ink/e-paper SPI displays.
 *
 * This driver provides functionality for working with black/white e-ink (e-paper) SPI displays.
 * Various display controllers are currently supported out of the box, see @ref epd_bw_spi_params.h.
 * Please open an issue or pull request with your controller details (size, Vcom, LUTs)
 * if your display controller is not included yet.
 *
 * Use of this driver requires knowing the parameters of your display.
 * See epd_bw_spi_params_t and @ref epd_bw_spi_params.h for more details on the parameters.
 * Note that while the reset and busy pins are optional, using them is highly recommended.
 *
 * This driver has @ref drivers_disp_dev support.
 *
 * @{
 * @file
 * @brief       Generic black/white e-paper/e-ink display SPI driver.
 *
 * @author      Silke Hofstra <silke@slxh.eu>
 */

#include "periph/spi.h"
#include "periph/gpio.h"

#ifdef MODULE_DISP_DEV
#include "disp_dev.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define EPD_BW_SPI_COLOR_WHITE (0xFF)   /**< White (8x1 pixels) */
#define EPD_BW_SPI_COLOR_BLACK (0x00)   /**< Black (8x1 pixels) */

/**
 * @brief   Data entry mode settings.
 *
 * This setting affect the automatic increment/decrement of the address counters.
 */
typedef enum {
    EPD_BW_SPI_Y_DEC_X_DEC  = 0x0, /**< Y decrement, X decrement */
    EPD_BW_SPI_Y_DEC_X_INC  = 0x1, /**< Y decrement, X increment */
    EPD_BW_SPI_Y_INC_X_DEC  = 0x2, /**< Y increment, X decrement */
    EPD_BW_SPI_Y_INC_X_INC  = 0x3, /**< Y increment, X increment */
} epd_bw_spi_entry_mode_t;

/**
 * @brief   Additional status codes for black/white SPI e-paper displays.
 */
enum {
    EPD_BW_SPI_DC_FAIL      = -5,
    EPD_BW_SPI_RST_FAIL     = -6,
    EPD_BW_SPI_BUSY_FAIL    = -7,
};

/**
 * @brief   Display controller parameters.
 */
typedef struct {
    uint8_t vcom;               /**< VCOM voltage level */
    const uint16_t size_x;      /**< supported number of horizontal pixels */
    const uint16_t size_y;      /**< supported number of vertical pixels */
    const uint8_t lut_size;     /**< size of the waveform lookup table */
    const uint8_t *lut_full;    /**< lookup table for a full display refresh */
    const uint8_t *lut_part;    /**< lookup table for a partial display refresh */
} epd_bw_spi_controller_t;

/**
 * @brief   SPI display device initialisation parameters.
 */
typedef struct {
    spi_t spi;                              /**< SPI device that the display is connected to */
    spi_clk_t spi_clk;                      /**< SPI clock speed to use */
    gpio_t cs_pin;                          /**< pin connected to the CHIP SELECT line */
    gpio_t dc_pin;                          /**< pin connected to the DC line */
    gpio_t rst_pin;                         /**< pin connected to the reset line (optional) */
    gpio_t busy_pin;                        /**< pin connected to the busy line (optional) */
    bool busy_value;                        /**< expected value for the busy pin
                                                 when the display is busy */
    bool dummy;                             /**< if device requires a dummy cycle before read */
    epd_bw_spi_controller_t controller;     /**< display controller of the e-Paper display */
    epd_bw_spi_entry_mode_t entry_mode;     /**< data entry mode */
    uint16_t size_x;                        /**< number of horizontal pixels in the display */
    uint16_t size_y;                        /**< number of vertical pixels in the display */
    uint16_t partial_refresh_max;           /**< maximum number of partial refreshes to perform
                                                 before triggering a full refresh */
} epd_bw_spi_params_t;

/**
 * @brief   Device initialisation parameters.
 */
typedef struct {
    #ifdef MODULE_DISP_DEV
    disp_dev_t *dev;                    /**< pointer to the generic display device */
    #endif
    epd_bw_spi_params_t params;         /**< SPI display parameters */
    uint16_t partial_refresh_count;     /**< number of partial refreshes since
                                             the last full refresh */
} epd_bw_spi_t;

/**
 * @brief   Initialise the display.
 *
 * @param[out] dev          Display to initialise.
 * @param[in]  params       SPI Display parameters to use for initialisation.
 */
int epd_bw_spi_init(epd_bw_spi_t *dev, const epd_bw_spi_params_t *params);

/**
 * @brief   Activate the display.
 *
 * @param[in] dev   Device descriptor.
 */
void epd_bw_spi_activate(epd_bw_spi_t *dev);

/**
 * @brief   Deactivate the display.
 *
 * @param[in] dev   Device descriptor.
 */
void epd_bw_spi_deactivate(epd_bw_spi_t *dev);

/**
 * @brief   Initialise the display for a full refresh.
 *
 * @param[in] dev   Device descriptor.
 */
void epd_bw_spi_init_full(epd_bw_spi_t *dev);

/**
 * @brief   Update the display with a full refresh.
 *
 * @param[in] dev   Device descriptor.
 */
void epd_bw_spi_update_full(epd_bw_spi_t *dev);

/**
 * @brief   Initialise the display for a partial refresh.
 *
 * @param[in] dev   Device descriptor.
 */
void epd_bw_spi_init_part(epd_bw_spi_t *dev);

/**
 * @brief   Update the display with a partial refresh.
 *
 * @param[in] dev   Device descriptor.
 */
void epd_bw_spi_update_part(epd_bw_spi_t *dev);

/**
 * @brief   Initialise the display for an automatic partial/full refresh.
 *
 * @param[in] dev   Device descriptor.
 */
void epd_bw_spi_init_auto(epd_bw_spi_t *dev);

/**
 * @brief   Update the display with an automatic partial/full refresh.
 *
 * @param[in] dev   Device descriptor.
 */
void epd_bw_spi_update_auto(epd_bw_spi_t *dev);

/**
 * @brief   Clear the entire display.
 *
 * @param[in] dev   Device descriptor.
 */
void epd_bw_spi_clear(epd_bw_spi_t *dev);

/**
 * @brief   Fill an area with a single color.
 *
 * @param[in] dev   Device descriptor
 * @param[in] x1    X coordinate of the first corner (multiple of 8).
 * @param[in] x2    X coordinate of the opposite corner (multiple of 8).
 * @param[in] y1    Y coordinate of the first corner.
 * @param[in] y2    Y coordinate of the opposite corner.
 * @param[in] color Color to use (`EPD_BW_SPI_COLOR_BLACK` or `EPD_BW_SPI_COLOR_WHITE`)
 */
void epd_bw_spi_fill(epd_bw_spi_t *dev, uint8_t x1, uint8_t x2, uint16_t y1,
                     uint16_t y2,
                     uint8_t color);

/**
 * @brief   Fill an area with an array of pixels.
 *
 * Note that the length of the array should be the same as the number of pixels
 * in the given area.
 *
 * @param[in] dev   Device descriptor.
 * @param[in] x1    X coordinate of the first corner (multiple of 8).
 * @param[in] x2    X coordinate of the opposite corner (multiple of 8).
 * @param[in] y1    Y coordinate of the first corner.
 * @param[in] y2    Y coordinate of the opposite corner.
 * @param[in] px    Array of pixels to use.
 */
void epd_bw_spi_fill_pixels(epd_bw_spi_t *dev, uint8_t x1, uint8_t x2,
                            uint16_t y1, uint16_t y2,
                            uint8_t *px);

/**
 * @brief   Set the area in which can be drawn.
 *
 * @param[in] dev   Device descriptor.
 * @param[in] x1    X coordinate of the first corner (multiple of 8).
 * @param[in] x2    X coordinate of the opposite corner (multiple of 8).
 * @param[in] y1    Y coordinate of the first corner.
 * @param[in] y2    Y coordinate of the opposite corner.
 */
void epd_bw_spi_set_area(epd_bw_spi_t *dev, uint8_t x1, uint8_t x2, uint16_t y1,
                         uint16_t y2);

/**
 * @brief   Write to the RAM of the epd_bw_spi controller.
 *
 * Together with `epd_bw_spi_set_area()`, this allows one to draw a pregenerated
 * image on the screen.
 *
 * @param[in] dev   Device descriptor.
 * @param[in] buf   Buffer to write to the display.
 * @param[in] len   Size of the buffer to write to the display.
 */
void epd_bw_spi_write_buffer(epd_bw_spi_t *dev, const uint8_t *buf, size_t len);

/**
 * @brief   Set the display to deep sleep mode.
 *
 * After the display has gone to sleep, a wake can be triggered with the reset pin.
 * Do not use this if no reset pin has been defined.
 *
 * @param[in] dev   Device descriptor.
 */
void epd_bw_spi_sleep(epd_bw_spi_t *dev);

/**
 * @brief   Wake the device.
 *
 * This doesn't do anything without using the reset pin.
 *
 * @param[in] dev   Device descriptor.
 */
void epd_bw_spi_wake(epd_bw_spi_t *dev);

/**
 * @brief   Perform a soft reset of the device.
 *
 * This resets all commands and parameters to their default values,
 * except for sleep mode and the RAM.
 *
 * @param[in] dev   Device descriptor.
 */
void epd_bw_spi_swreset(epd_bw_spi_t *dev);

#ifdef __cplusplus
}
#endif
/** @} */
#endif /* EPD_BW_SPI_H */

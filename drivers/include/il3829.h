/*
 * Copyright (C) 2018 Silke Hofstra
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_il3829  IL3829 E-ink/E-paper controller
 * @ingroup     drivers_actuators
 * @brief       Device driver for the il3829 E-ink/E-paper display controller.
 *
 * The IL3829 is a display controller for black and white E-ink/E-paper displays.
 * It is commonly found in 1.54 inch displays.
 *
 * More information:
 * - [IL3829 datasheet (Good Display)](http://www.e-paper-display.com/download_detail/downloadsId=534.html)
 * - [1.54inch e-Paper module (Waveshare Wiki)](https://www.waveshare.com/wiki/1.54inch_e-Paper_Module)
 *
 * ## Module usage
 *
 * Add the following to your makefile.
 * Note that not all display boards have the *reset* and *busy* pins exposed.
 *
 * ```make
 * USEMODULE += il3829
 *
 * # Set the following to the correct pins
 * CFLAGS += -DIL3829_PARAM_CS=GPIO_PIN\(PORT_A,1\)
 * CFLAGS += -DIL3829_PARAM_DC=GPIO_PIN\(PORT_A,2\)
 * CFLAGS += -DIL3829_PARAM_RST=GPIO_PIN\(PORT_A,3\) # if RST is exposed
 * CFLAGS += -DIL3829_PARAM_BUSY=GPIO_PIN\(PORT_A,4\) # if BUSY is exposed
 * ```
 *
 * This allows initialisation using `il3829_params[0]` or `IL3829_PARAMS`.
 *
 * The driver includes the RIOT logo in various sizes in `il3829_pictures.h`.
 *
 * ## Driver usage
 *
 * A write cycle to the display looks like the following.
 * Substitute `full` by `part` for a partial refresh.
 *
 * ```c
 * il3829_init_full(dev);
 * il3829_activate(dev);
 * il3829_write_buffer(dev, riot_icon_200, sizeof riot_icon_200);
 * il3829_update_full(dev);
 * il3829_deactivate(dev);
 * ```
 *
 * The IL3829 has two memory buffers for storing the images.
 * After an update (either partial or full), the controller switches to the
 * other buffer.
 *
 * @{
 * @file
 * @brief       Device driver for the il3829 display controller
 *
 * @author      Silke Hofstra <silke@slxh.eu>
 */

#ifndef IL3829_H
#define IL3829_H

#include "periph/spi.h"
#include "periph/gpio.h"
#include "spi_display.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef IL3829_VCOM
/**
 * @brief IL3829 VCOM voltage level
 *
 * Default VCOM voltage of ???
 */
#define IL3829_VCOM 0x9B /* VCOM 7C (TODO ??? a8/9b?) */
#endif

#define IL3829_COLOR_WHITE (0xFF) /**< White (8x1 pixels) */
#define IL3829_COLOR_BLACK (0x00) /**< Black (8x1 pixels) */

/**
 * @brief   Data entry mode settings.
 *
 * This setting affect the automatic increment/decrement of the address counters.
 */
typedef enum {
    IL3829_Y_DEC_X_DEC = 0b00, /**< Y decrement, X decrement */
    IL3829_Y_DEC_X_INC = 0b01, /**< Y decrement, X increment */
    IL3829_Y_INC_X_DEC = 0b10, /**< Y increment, X decrement */
    IL3829_Y_INC_X_INC = 0b11, /**< Y increment, X increment */
} il3829_entry_mode_t;

/**
 * @brief   Device initialisation parameters.
 */
typedef struct {
    spi_display_params_t params;    /**< SPI display parameters */
    uint8_t size_x;                 /**< number of horizontal pixels in the display */
    uint16_t size_y;                /**< number of vertical pixels in the display */
    il3829_entry_mode_t entry_mode; /**< data entry mode */
} il3829_t;

/**
 * @brief   Initialise the display.
 *
 * @param[out] dev          Display to initialise.
 * @param[in]  params       SPI Display parameters to use for initialisation.
 * @param[in]  size_x       Number of horizontal pixels in the display.
 * @param[in]  size_y       Number of vertical pixels in the display.
 * @param[in]  entry_mode   Data entry mode.
 */
int il3829_init(il3829_t *dev, const spi_display_params_t *params,
                uint8_t size_x, uint16_t size_y, il3829_entry_mode_t entry_mode);

/**
 * @brief   Activate the display.
 *
 * @param[in] dev   Device descriptor.
 */
void il3829_activate(il3829_t *dev);

/**
 * @brief   Deactivate the display.
 *
 * @param[in] dev   Device descriptor.
 */
void il3829_deactivate(il3829_t *dev);

/**
 * @brief   Initialise the display for a full refresh.
 *
 * @param[in] dev   Device descriptor.
 */
void il3829_init_full(il3829_t *dev);

/**
 * @brief   Update the display with a full refresh.
 *
 * @param[in] dev   Device descriptor.
 */
void il3829_update_full(il3829_t *dev);

/**
 * @brief   Initialise the display for a partial refresh.
 *
 * @param[in] dev   Device descriptor.
 */
void il3829_init_part(il3829_t *dev);

/**
 * @brief   Update the display with a partial refresh.
 *
 * @param[in] dev   Device descriptor.
 */
void il3829_update_part(il3829_t *dev);

/**
 * @brief   Clear the entire display.
 *
 * @param[in] dev   Device descriptor.
 */
void il3829_clear(il3829_t *dev);

/**
 * @brief   Fill an area with a single color.
 *
 * @param[in] dev   Device descriptor
 * @param[in] x1    X coordinate of the first corner (multiple of 8).
 * @param[in] x2    X coordinate of the opposite corner (multiple of 8).
 * @param[in] y1    Y coordinate of the first corner.
 * @param[in] y2    Y coordinate of the opposite corner.
 * @param[in] color Color to use (`IL3829_COLOR_BLACK` or `IL3829_COLOR_WHITE`)
 */
void il3829_fill(il3829_t *dev, uint8_t x1, uint8_t x2, uint16_t y1, uint16_t y2,
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
void il3829_fill_pixels(il3829_t *dev, uint8_t x1, uint8_t x2, uint16_t y1, uint16_t y2,
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
void il3829_set_area(il3829_t *dev, uint8_t x1, uint8_t x2, uint16_t y1, uint16_t y2);

/**
 * @brief   Write to the RAM of the IL3829 controller.
 *
 * Together with `il3829_set_area()`, this allows one to draw a pregenerated
 * image on the screen.
 *
 * @param[in] dev   Device descriptor.
 * @param[in] buf   Buffer to write to the display.
 * @param[in] len   Size of the buffer to write to the display.
 */
void il3829_write_buffer(il3829_t *dev, const uint8_t *buf, size_t len);

/**
 * @brief   Set the display to deep sleep mode.
 *
 * After the display has gone to sleep, a wake can be triggered with the reset pin.
 *
 * @param[in] dev   Device descriptor.
 */
void il3829_sleep(il3829_t *dev);

/**
 * @brief   Wake the device.
 *
 * This doesn't do anything without using the reset pin.
 *
 * @param[in] dev   Device descriptor.
 */
void il3829_wake(il3829_t *dev);

/**
 * @brief   Perform a soft reset of the device.
 *
 * This resets all commands and parameters to their default values,
 * except for sleep mode and the RAM.
 *
 * @param[in] dev   Device descriptor.
 */
void il3829_swreset(il3829_t *dev);

#ifdef __cplusplus
}
#endif
#endif /* IL3829_H */
/** @} */

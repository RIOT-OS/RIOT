/*
 * Copyright (C) 2023 Silke Hofstra
 * Copyright (C) 2023 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_uc8151d uc8151d
 * @ingroup     drivers_display
 * @brief       UC8151D e-paper display driver
 *
 * This driver provides functionality for working with UC8151D-based e-paper displays. The provided
 * api mimics that of the @ref drivers_epd_bw_spi module and parameters are mostly the same to make
 * porting from one to the other easy.
 *
 * So far only the monochrome mode is supported, tri-color displays are not yet supported by this
 * driver.
 *
 * Tested with the [GDEW026T0D](https://www.good-display.com/product/233.html) E-ink display.
 *
 * ## Usage
 *
 * Initialization is done via the usual @ref uc8151d_init function and has to be called only once.
 *
 * To save power, the display must be put into a deep sleep mode after every sequence via
 * @ref uc8151d_sleep. The @ref uc8151d_wake function wakes the display up via the reset pin.
 *
 * ### Updating the display content
 *
 * Controlling the content of the display always follows roughly the same pattern:
 *
 *  1. Wake the display from deep sleep via a reset
 *  2. Initialize the display in one of the modes (full/partial)
 *  3. Write new content to the display
 *  4. Call one of the update functions to refresh the display content (full/partial)
 *  5. Put the device to sleep
 *
 * Step 3 can be repeated multiple times when multiple regions of the display needs updating.
 *
 * #### Content Initialization
 *
 * After waking the display from sleep, one of the init functions has to be called to start a new
 * display content update. The difference between the functions is in which refresh mode they use to
 * update the display content
 *
 * - @ref uc8151d_init_full: This configures the display to perform a full refresh of the content,
 *   usually blinking a few times back and forth between black and white. This can take a few
 *   seconds to complete but gives the best results in clarity.
 * - @ref uc8151d_init_partial: This configures the display to perform a fast refresh of the new
 *   content. It attempts to change the pixels to the required color in a non-intrusive way to
 *   onlookers. The downside is that the display usually slowly loses contrast after a number of
 *   these updates.
 *
 * These functions are implemented by providing different LUTs to the display that describe the
 * waveform used to updated the pixels.
 *
 * #### Content refresh
 *
 * After all new content has been written to the display, one of the refresh functions has to be
 * called to let the display perform the update to the display.
 *
 * - @ref uc8151d_refresh_full: This updates the full display with the mode chosen during
 *   initialization. Multiple areas can be updated at once. Existing content can be affected, either
 *   have their contrast reinforced again (or accidentally disappear).
 * - @ref uc8151d_refresh_part: This updates only the affected region with the mode chosen during
 *   init. Only one area (via @ref uc8151d_set_area) can be updated per refresh this way. Other
 *   content on the display is not affected but might slowly lose contrast after multiple refreshes
 *   of this kind
 *
 * #### Example
 *
 * To wake the display up and turn the whole display black:
 * ```
 *  uc8151d_wake(dev);
 *  uc8151d_init_full(dev);
 *  uc8151d_fill(dev, 0, dev->params.size_x, 0, dev->params.size_y, UC8151D_COLOR_BLACK);
 *  uc8151d_update_full(dev);
 *  uc8151d_sleep(dev);
 * ```
 *
 * To update a specific part (x and y) with an image of the RIOT logo:
 * ```
 *  uc8151d_wake(dev);
 *  uc8151d_init_part(dev);
 *  uc8151d_set_area(dev, x, x + riot_32_width, y, y + riot_32_height);
 *  uc8151d_write_buffer(dev, riot_logo_32, sizeof(riot_logo_32));
 *  uc8151d_update_part(dev);
 *  uc8151d_sleep(dev);
 *  ```
 *
 *  See the test application for other usage patterns
 *
 * @{
 *
 * @file
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef UC8151D_H
#define UC8151D_H

#include <stdint.h>
#include "mutex.h"
#include "periph/gpio.h"
#include "periph/spi.h"

#include "uc8151d_constants.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Default frame rate used for refreshes
 */
#ifndef CONFIG_UC8151D_FRAME_RATE_DEFAULT
#define CONFIG_UC8151D_FRAME_RATE_DEFAULT UC8151D_FRAME_RATE_DEFAULT
#endif

/**
 * @brief Slow refresh rate for thorough refreshes
 */
#ifndef CONFIG_UC8151D_FRAME_RATE_SLOW
#define CONFIG_UC8151D_FRAME_RATE_SLOW  UC8151D_FREQUENCY_17HZ
#endif

/**
 * @brief Fast frame rate for fast refreshes
 */
#ifndef CONFIG_UC8151D_FRAME_RATE_FAST
#define CONFIG_UC8151D_FRAME_RATE_FAST  UC8151D_FREQUENCY_171HZ
#endif

/**
 * @brief Default frame rate on device reset
 */
#define UC8151D_FRAME_RATE_DEFAULT UC8151D_FREQUENCY_50HZ

/**
 * @brief   Additional status codes for UC8151D.
 *
 * Extra negative to prevent collision with the error codes from @ref drivers_periph_spi
 */
enum {
    UC8151D_DC_FAIL     = -1000,    /**< Initialization of D/C pin failed */
    UC8151D_RST_FAIL    = -1001,    /**< Initialization of reset pin failed */
    UC8151D_BUSY_FAIL   = -1002,    /**< Initialization of busy pin failed */
};

/**
 * @brief   Device initialization parameters
 */
typedef struct {
    spi_t spi;                              /**< SPI device that the display is connected to */
    spi_clk_t spi_clk;                      /**< SPI clock speed to use */
    gpio_t cs_pin;                          /**< pin connected to the CHIP SELECT line */
    gpio_t dc_pin;                          /**< pin connected to the DC line */
    gpio_t rst_pin;                         /**< pin connected to the reset line */
    gpio_t busy_pin;                        /**< pin connected to the busy line */

    uint8_t size_x;                         /**< number of horizontal pixels in the display */
    uint16_t size_y;                        /**< number of vertical pixels in the display */
} uc8151d_params_t;

/**
 * @brief   Device descriptor for the driver
 */
typedef struct {
    uc8151d_params_t params;            /**< Initialization parameters */
    mutex_t gpio_wait;                  /**< Mutex used to block the busy pin wait */
    bool invert;                        /**< Invert display colors */
    uc8151d_frequency_t frame_rate;     /**< Frame rate used during updates */
} uc8151d_t;

#define UC8151D_COLOR_WHITE (0xFF)      /**< White (8x1 pixels) */
#define UC8151D_COLOR_BLACK (0x00)      /**< Black (8x1 pixels) */

/**
 * @brief   Initialize the given device
 *
 * @param[inout] dev        Device descriptor of the driver
 * @param[in]    params     Initialization parameters
 *
 * @return                  0 on success
 */
int uc8151d_init(uc8151d_t *dev, const uc8151d_params_t *params);

/**
 * @brief   Initialise the display for a full refresh.
 *
 * @param[in] dev   Device descriptor.
 */
void uc8151d_init_full(uc8151d_t *dev);

/**
 * @brief   Initialise the display for a fast partial update
 *
 * @param[in] dev   Device descriptor.
 */
void uc8151d_init_part(uc8151d_t *dev);

/**
 * @brief   Set Monochrome display colors to inverted
 *
 * @note    The actual invert setting of the display hardware is updated during the refresh, this
 * function must be used before the display refresh functions are used to get updated on the
 * hardware.
 *
 * @param[in] dev    Device descriptor.
 * @param[in] invert True to invert, false to revert
 */
static inline void uc8151d_set_invert(uc8151d_t *dev, bool invert)
{
    dev->invert = invert;
}

/**
 * @brief   Set the refresh rate of the display updates
 *
 * @note    The actual frame rate of the display hardware is updated during the initialization, this
 * function must be used before the display init functions are used to get updated on the hardware.
 *
 * @param[in] dev           Device descriptor.
 * @param[in] frame_rate    Frame rate used for updates
 */
static inline void uc8151d_set_frame_rate(uc8151d_t *dev, uc8151d_frequency_t frame_rate)
{
    dev->frame_rate = frame_rate;
}

/**
 * @brief   Wake the device via reset from deep sleep.
 *
 * @param[in] dev   Device descriptor.
 */
void uc8151d_wake(uc8151d_t *dev);

/**
 * @brief   Write to the RAM of the UC8151D controller.
 *
 * This writes to the full display area of the controllers. Together with @ref uc8151d_set_area,
 * this allows one to draw a pre-generated image on a limited part of the screen.
 *
 * @param[in] dev   Device descriptor.
 * @param[in] data  Buffer to write to the display.
 * @param[in] len   Size of the buffer in bytes to write to the display.
 */
void uc8151d_write_buffer(uc8151d_t *dev, const uint8_t *data, size_t len);

/**
 * @brief   Set the area in which can be drawn.
 *
 * @param[in] dev   Device descriptor.
 * @param[in] x1    X coordinate of the first corner (multiple of 8).
 * @param[in] x2    X coordinate of the opposite corner (multiple of 8).
 * @param[in] y1    Y coordinate of the first corner.
 * @param[in] y2    Y coordinate of the opposite corner.
 */
void uc8151d_set_area(uc8151d_t *dev, uint8_t x1, uint8_t x2, uint16_t y1,
                      uint16_t y2);

/**
 * @brief   Update the full display area with the loaded refresh type
 *
 * @param[in] dev   Device descriptor.
 */
void uc8151d_update_full(uc8151d_t *dev);

/**
 * @brief   Update the modified area of the display with the loaded refresh type
 *
 * @param[in] dev   Device descriptor.
 */
void uc8151d_update_part(uc8151d_t *dev);

/**
 * @brief   Dummy function to increase compatibility with the @ref drivers_epd_bw_spi driver
 *
 * @param[in] dev   Device descriptor.
 */
static inline void uc8151d_deactivate(uc8151d_t *dev)
{
    (void)dev;
}

/**
 * @brief   Dummy function to increase compatibility with the @ref drivers_epd_bw_spi driver
 *
 * @param[in] dev   Device descriptor.
 */
static inline void uc8151d_activate(uc8151d_t *dev)
{
    (void)dev;
}

/**
 * @brief   Set the display to deep sleep mode.
 *
 * After the display has gone to sleep, a wake can be triggered with the reset pin.
 *
 * @param[in] dev   Device descriptor.
 */
void uc8151d_sleep(uc8151d_t *dev);

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
void uc8151d_fill(uc8151d_t *dev, uint8_t x1, uint8_t x2, uint16_t y1,
                  uint16_t y2, uint8_t color);

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
void uc8151d_fill_pixels(uc8151d_t *dev, uint8_t x1, uint8_t x2,
                         uint16_t y1, uint16_t y2,
                         uint8_t *px);
#ifdef __cplusplus
}
#endif

#endif /* UC8151D_H */
/** @} */

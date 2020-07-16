/*
 * Copyright (C) 2020 Inria
 *               2020 Philipp-Alexander Blum
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_disp_dev Display device generic API
 * @ingroup     drivers_display
 * @brief       Define the generic API of a display device
 * @experimental This API is experimental and in an early state - expect
 *               changes!
 * @{
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Philipp-Alexander Blum <philipp-blum@jakiku.de>
 */

#ifndef DISP_DEV_H
#define DISP_DEV_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "board.h"

#ifndef BACKLIGHT_ON
#define BACKLIGHT_ON
#endif

#ifndef BACKLIGHT_OFF
#define BACKLIGHT_OFF
#endif

/**
 * @brief   Forward declaration for display device struct
 */
typedef struct disp_dev disp_dev_t;

/**
 * @brief Type to store the update coordinates in
 */
typedef struct {
    uint16_t x1; /**< Left coordinate */
    uint16_t x2; /**< Right coordinate */
    uint16_t y1; /**< Top coordinate */
    uint16_t y2; /**< Bottom coordinate */
} disp_dev_coordinates_t;

/**
 * @brief Type for color depth.
 */
typedef enum {
    MONOCHROME = 0,
    2_BIT_COLOR,
    GRAYSCALE,
    8_BIT_COLOR,
    16_BIT_HIGH_COLOR,
    24_BIT_TRUE_COLOR
} disp_dev_color_depth_t;

/**
 * @brief Type to hold a pixel for monochrome data
 */
typedef struct {
    bool color;
} disp_dev_pix_mono_t;

/**
 * @brief Type to hold a pixel for 2bit color data
 */
typedef struct {
    bool black;
    bool color;
} disp_dev_pix_2bit_color_t;

/**
 * @brief Type to hold a pixel for greyscale data
 */
typedef struct {
    uint8_t contrast;
} disp_dev_pix_greyscale_t;

/**
 * @brief Type to hold a pixel for 8bit color data
 */
typedef struct {
    uint8_t rgb;
} disp_dev_pix_8bit_color_t;

/**
 * @brief Type to hold a pixel for 16bit high color data
 */
typedef struct {
    uint16_t rgb;
} disp_dev_pix_16bit_hc_t;

/**
 * @brief Type to hold a pixel for monochrome color data
 */
typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} disp_dev_pix_24bit_tc_t;

/**
 * @brief   Generic type for a display driver
 */
typedef struct {
    /**
     * @brief   Map an area to display on the device
     *
     * @param[in] dev           Pointer to the display device
     * @param[in] coordinates   Coordinate
     * @param[in] color         Array of color to map to the display
     * @param[in] color_depth   Color depth of color
     */
    void (*map)(disp_dev_t *dev, disp_dev_coordinates_t *coordinates,
                const void *color, disp_dev_color_depth_t color_depth);

    /**
     * @brief   Get the height of the display device
     *
     * @param[in] dev       Pointer to the display device
     *
     * @return              Height in pixels
     */
    uint16_t (*height)(const disp_dev_t *dev);

    /**
     * @brief   Get the width of the display device
     *
     * @param[in] dev       Pointer to the display device
     *
     * @return              Width in pixels
     */
    uint16_t (*width)(const disp_dev_t *dev);

    /**
     * @brief   Get the color depth of the display device
     *
     * @return              The color depth
     */
    disp_dev_color_depth_t (*color_depth)(const disp_dev_t *dev);

    /**
     * @brief   Invert the display device colors
     *
     * @param[in] dev       Network device descriptor
     * @param[in] invert    Invert mode (true if invert, false otherwise)
     */
    void (*set_invert)(const disp_dev_t *dev, bool invert);
} disp_dev_driver_t;

/**
 * @brief   Generic type for a display device
 */
struct disp_dev {
    const disp_dev_driver_t *driver;    /**< Pointer to driver of the display device */
};

/**
 * @brief   Map an area to display on the device
 *
 * @param[in] dev            Pointer to the display device
 * @param[in] coordinates    Coordinate
 * @param[in] color          Array of color to map to the display
 * @param[in] color_depth    Color depth of color
 */
void disp_dev_map(disp_dev_t *dev, disp_dev_coordinates_t *coordinates,
                  const void *color, disp_dev_color_depth_t color_depth);

/**
 * @brief   Get the height of the display device
 *
 * @param[in] dev       Pointer to the display device
 *
 * @return              Height in pixels
 */
uint16_t disp_dev_height(const disp_dev_t *dev);

/**
 * @brief   Get the width of the display device
 *
 * @param[in] dev       Pointer to the display device
 *
 * @return              Width in pixels
 */
uint16_t disp_dev_width(const disp_dev_t *dev);

/**
 * @brief   Get the color depth of the display device
 *
 * @return              The color depth
 */
disp_dev_color_depth_t disp_dev_color_depth(const disp_dev_t *dev);

/**
 * @brief   Invert the display device colors
 *
 * @param[in] dev       Pointer to the display device
 * @param[in] invert    Invert mode (true if invert, false otherwise)
 */
void disp_dev_set_invert(const disp_dev_t *dev, bool invert);

/**
 * @brief   Enable the backlight pin
 */
static inline void disp_dev_backlight_on(void)
{
    BACKLIGHT_ON;
}

/**
 * @brief   Disable the backlight pin
 */
static inline void disp_dev_backlight_off(void)
{
    BACKLIGHT_OFF;
}

#ifdef __cplusplus
}
#endif

#endif /* DISP_DEV_H */
/** @} */

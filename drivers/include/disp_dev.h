/*
 * SPDX-FileCopyrightText: 2020 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_disp_dev Display device generic API
 * @ingroup     drivers_display
 * @brief       Define the generic API of a display device
 * @experimental This API is experimental and in an early state - expect
 *               changes!
 * @{
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

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
 * @brief   Display area coordinates
 */
typedef struct {
    uint16_t x1;                    /**< Horizontal start position (included) */
    uint16_t x2;                    /**< Horizontal end position (included) */
    uint16_t y1;                    /**< Vertical start position (included) */
    uint16_t y2;                    /**< Vertical end position (included) */
} disp_dev_area_t;

/**
 * @brief   Generic type for a display driver
 */
typedef struct {
    /**
     * @brief   Map an area to display on the device
     *
     * @param[in] dev   Pointer to the display device
     * @param[in] area  Coordinates of display area
     * @param[in] color Array of color to map to the display
     */
    void (*map)(const disp_dev_t *dev,
                const disp_dev_area_t *area,
                const uint16_t *color);

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
    uint8_t (*color_depth)(const disp_dev_t *dev);

    /**
     * @brief   Invert the display device colors
     *
     * @param[in] dev       Pointer to the display device
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
 * @brief   Disp dev registry entry
 */
typedef struct disp_dev_reg {
    struct disp_dev_reg *next;      /**< pointer to the next display device in the list */
    disp_dev_t *dev;                /**< pointer to the device descriptor */
    uint8_t screen_id;              /**< id of the screen this display is attached to */
} disp_dev_reg_t;

/**
 * @brief   Export the display device registry as global variable
 */
extern disp_dev_reg_t *disp_dev_reg;

/**
 * @brief   Add pointer to a display device item to the list of display items
 *
 * @param[in] dev   Pointer to the display device
 *
 * @return          0 on success
 * @return         -ENODEV on invalid entry
*/
int disp_dev_reg_add(disp_dev_reg_t *dev);

/**
 * @brief   Find the display device that is attached to a given screen
 *
 * @param[in] screen_id Identifier (number) of the screen
 *
 * @return              pointer to the display device in the registry
 * @return              NULL if there's no match
*/
disp_dev_reg_t *disp_dev_reg_find_screen(uint8_t screen_id);

/**
 * @brief   Map an area to display on the device
 *
 * @param[in] dev   Pointer to the display device
 * @param[in] area  Coordinates of display area
 * @param[in] color Array of color to map to the display
 */
void disp_dev_map(const disp_dev_t *dev,
                  const disp_dev_area_t *area,
                  const uint16_t *color);

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
uint8_t disp_dev_color_depth(const disp_dev_t *dev);

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

/** @} */

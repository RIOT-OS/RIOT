/*
 * SPDX-FileCopyrightText: 2020 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_touch_dev Touch device generic API
 * @ingroup     drivers_misc
 * @brief       Define the generic API of a touch device
 * @experimental This API is experimental and in an early state - expect
 *               changes!
 * @{
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * @brief   Invalid touch value
 */
#define TOUCH_DEV_VALUE_INVALID         ((touch_t){ UINT16_MAX, UINT16_MAX })

/**
 * @brief   Forward declaration for touch device struct
 */
typedef struct touch_dev touch_dev_t;

/**
 * @brief   Touch coordinates
 */
typedef struct {
    uint16_t x;     /**< X coordinate */
    uint16_t y;     /**< Y coordinate */
} touch_t;

/**
 * @brief   Signature of touch event callback triggered from interrupt
 *
 * @param[in] arg       optional context for the callback
 */
typedef void (*touch_event_cb_t)(void *arg);

/**
 * @brief   Generic type for a touch driver
 */
typedef struct {

    /**
     * @brief   Get the height of the touch device
     *
     * @param[in] dev       Pointer to the touch device
     *
     * @return              Height in points
     */
    uint16_t (*height)(const touch_dev_t *dev);

    /**
     * @brief   Get the width of the touch device
     *
     * @param[in] dev       Pointer to the touch device
     *
     * @return              Width in points
     */
    uint16_t (*width)(const touch_dev_t *dev);

    /**
     * @brief   Get the maximum number of touches the touch device supports
     *
     * This function pointer can be NULL. In this case, the maximum number of
     * touches is assumed to be 1.
     *
     * @param[in] dev       Pointer to the touch device
     *
     * @return              number of touches
     */
    uint8_t (*max_numof)(const touch_dev_t *dev);

    /**
     * @brief   Get the current touches on the touch device
     *
     * If @p touches is NULL, this function only returns the number of touches.
     *
     * @param[in] dev       Pointer to the touch device
     * @param[out] touches  The array of touches
     * @param[in] len       The touches array len
     * @return              number of touch positions, 0 means no touch
     */
    uint8_t (*touches)(const touch_dev_t *dev, touch_t *touches, size_t len);

    /**
     * @brief   Set and configure the touch event callback
     *
     * @param[in] dev       Pointer to the touch device
     * @param[in] cb        The callback function
     * @param[in] arg       Context argument
     */
    void (*set_event_callback)(const touch_dev_t *dev, touch_event_cb_t cb, void *arg);
} touch_dev_driver_t;

/**
 * @brief   Generic type for a touch device
 */
struct touch_dev {
    const touch_dev_driver_t *driver;    /**< Pointer to driver of the touch device */
};

/**
 * @brief   Touch dev registry entry
 */
typedef struct touch_dev_reg {
    struct touch_dev_reg *next;     /**< pointer to the next touch device in the list */
    touch_dev_t *dev;               /**< pointer to the device descriptor */
    uint8_t screen_id;              /**< id of the screen this touch device is attached to */
} touch_dev_reg_t;

/**
 * @brief   Export the touch device registry as global variable
 */
extern touch_dev_reg_t *touch_dev_reg;

/**
 * @brief   Add pointer to a touch device item to the list of touch items
 *
 * @param[in] dev   Pointer to the touch device
 *
 * @return          0 on success
 * @return         -ENODEV on invalid entry
*/
int touch_dev_reg_add(touch_dev_reg_t *dev);

/**
 * @brief   Find the touch device that is attached to a given screen
 *
 * @param[in] screen_id Identifier (number) of the screen
 *
 * @return              pointer to the touch device in the registry
 * @return              NULL if there's no match
*/
touch_dev_reg_t *touch_dev_reg_find_screen(uint8_t screen_id);

/**
 * @brief   Get the height of the touch device
 *
 * @param[in] dev       Pointer to the touch device
 *
 * @return              Height in points
 */
uint16_t touch_dev_height(const touch_dev_t *dev);

/**
 * @brief   Get the width of the touch device
 *
 * @param[in] dev       Pointer to the touch device
 *
 * @return              Width in points
 */
uint16_t touch_dev_width(const touch_dev_t *dev);

/**
 * @brief   Get the maximum number of touches the touch device supports
 *
 * @param[in] dev       Pointer to the touch device
 *
 * @return              number of touches
 */
static inline uint8_t touch_dev_max_numof(const touch_dev_t *dev)
{
    assert(dev);
    return (dev->driver->max_numof) ? dev->driver->max_numof(dev) : 1;
}

/**
 * @brief   Get the current touches on the touch device
 *
 * If @p touches is NULL, this function only returns the number of touches.
 *
 * Touch position coordinates use the top left corner as origin.
 *
 * @param[in] dev       Pointer to the touch device
 * @param[out] touches  The array of touches
 * @param[in] len       The touches array len
 * @return              number of touch positions, 0 means no touch
 */
uint8_t touch_dev_touches(const touch_dev_t *dev, touch_t *touches, size_t len);

/**
 * @brief   Set and configure the touch event callback
 *
 * @param[in] dev       Pointer to the touch device
 * @param[in] cb        The callback function
 * @param[in] arg       Context argument
 */
void touch_dev_set_touch_event_callback(const touch_dev_t *dev, touch_event_cb_t cb, void *arg);

#ifdef __cplusplus
}
#endif

/** @} */

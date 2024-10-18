/*
 * Copyright (C) 2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_my9221 MY9221 LED controller
 * @ingroup     drivers_actuators
 * @brief       Driver for the MY-Semi MY9221 LED controller
 *
 * @{
 * @file
 * @brief       Interface for the MY9221 LED controller driver
 *
 * @author      Sebastian Meiling <s@mlng.net>
 */

#ifndef MY9221_H
#define MY9221_H

#include <stdint.h>

#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Maximum number of distinct LEDs the controller can operate
 */
#define MY9221_LED_MAX          (12U)

/**
 * @brief   Max brightness value to turn LED full on
 */
#define MY9221_LED_ON           (0xFF)

/**
 * @brief   Min brightness value to turn LED off
 */
#define MY9221_LED_OFF          (0x00)

/**
 * @name    Direction the controller accesses LEDs
 * @{
 */
enum {
    MY9221_DIR_FWD,                 /**< forward */
    MY9221_DIR_REV,                 /**< backward */
};
/** @} */

/**
 * @name    Driver specific return codes
 * @{
 */
enum {
    MY9221_OK,                      /**< success */
    MY9221_ERR,                     /**< failure */
};
/** @} */

/**
 * @brief   Parameters needed for device initialization
 */
typedef struct {
    uint8_t leds;                   /**< number of LEDs */
    uint8_t dir;                    /**< led direction */
    gpio_t clk;                     /**< clock gpio pin */
    gpio_t dat;                     /**< data gpio pin */
} my9221_params_t;

/**
 * @brief   Device descriptor for MY9221 LED controller
 */
typedef struct {
    my9221_params_t params;         /**< config parameters */
    uint8_t state[MY9221_LED_MAX];  /**< state of individual leds */
} my9221_t;

/**
 * @brief   Initialize the given driver
 *
 * @param[out]  dev         device descriptor of MY9221 LED controller
 * @param[in]   params      configuration parameters
 *
 * @return                  0 on success, otherwise error
 */
int my9221_init(my9221_t *dev, const my9221_params_t *params);

/**
 * @brief   Set device state
 *
 * @note    If @p state is NULL or @p len is 0, current device state is set
 *          otherwise, current state is overwritten by @p state.
 *
 * @param[in]   dev         device descriptor of MY9221 LED controller
 * @param[in]   state       new device state array
 * @param[in]   len         length of state array
 */
void my9221_set_state(my9221_t *dev, const uint8_t *state, uint8_t  len);

/**
 * @brief   Set brightness of distinct LED
 *
 * @param[in]   dev         device descriptor of MY9221 LED controller
 * @param[in]   led         led number, start with 0
 * @param[in]   alpha       brightness level for led
 */
void my9221_set_led(my9221_t *dev, const uint8_t led, const uint8_t alpha);

/**
 * @brief   Toggle a distinct LED
 *
 * @param[in]   dev         device descriptor of MY9221 LED controller
 * @param[in]   led         led number, start with 0
 */
void my9221_toggle_led(my9221_t *dev, const uint8_t led);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* MY9221_H */

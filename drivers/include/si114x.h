/*
 * Copyright (C) 2016-2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_si114x Si1145/6/7 UV/Ambient light/Proximity sensors
 * @ingroup     drivers_sensors
 * @brief       Device driver for the Si1145/6/7 sensors family
 * @{
 *
 * @file
 * @brief       Device driver interface for the Si114x sensors family
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *              Bas Stottelaar <basstottelaar@gmail.com>
 */

#ifndef SI114X_H
#define SI114X_H

#include "saul.h"
#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialization return codes
 */
typedef enum {
    SI114X_OK,
    SI114X_ERR_I2C,
    SI114X_ERR_NODEV
} si114x_ret_code_t;

/**
 * @brief Active LED current parameters
 */
typedef enum {
    SI114X_PS_LED1 = 1,                /**< 5.6mA */
    SI114X_PS_LED2,                    /**< 11.2mA */
    SI114X_PS_LED3,                    /**< 22.4mA */
    SI114X_PS_LED4,                    /**< 45mA */
    SI114X_PS_LED5,                    /**< 67mA */
    SI114X_PS_LED6,                    /**< 90mA */
    SI114X_PS_LED7,                    /**< 112mA */
    SI114X_PS_LED8,                    /**< 135mA */
    SI114X_PS_LED9,                    /**< 157mA */
    SI114X_PS_LED10,                   /**< 180mA */
    SI114X_PS_LED11,                   /**< 202mA */
    SI114X_PS_LED12,                   /**< 224mA */
    SI114X_PS_LED13,                   /**< 269mA */
    SI114X_PS_LED14,                   /**< 314mA */
    SI114X_PS_LED15                    /**< 359mA */
} si114x_led_current_t;

/**
 * @brief Device initialization parameters
 */
typedef struct {
    i2c_t i2c_dev;                     /**< I2C device which is used */
    si114x_led_current_t led_current;  /**< Proximity LED current */
} si114x_params_t;


/**
 * @brief Device descriptor for the Si114x sensor
 */
typedef struct {
    si114x_params_t params;           /**< Si114x initialization parameters */
} si114x_t;

/**
 * @brief Initialize the given Si114x device
 *
 * @param[out] dev          Initialized device descriptor of Si114x device
 * @param[in]  params       Initialization parameters
 *
 * @return                  SI114X_OK: all good
 * @return                  -SI114X_ERR_I2C: I2C bus cannot be initialized
 * @return                  -SI114X_ERR_NODEV: not a valid Si114x device
 */
int8_t si114x_init(si114x_t *dev, const si114x_params_t * params);

/**
 * @brief Read UV index from the given Si114x device
 *
 * @param[in] dev           Device descriptor of Si114x device to read from
 *
 * @return                  UV index
 */
uint16_t si114x_read_uv(si114x_t *dev);

/**
 * @brief Read IR light value from the given Si114x device, returned in lx
 *
 * @param[in]  dev          Device descriptor of Si114x device to read from
 *
 * @return                  IR light in lx
 */
uint16_t si114x_read_ir(si114x_t *dev);

/**
 * @brief Read visible light value from the given Si114x device, returned in lx
 *
 * @param[in]  dev          Device descriptor of Si114x device to read from
 *
 * @return                  Visible light in lx
 */
uint16_t si114x_read_visible(si114x_t *dev);

/**
 * @brief Read distance measure from the given Si114x device, returned in ADC
 *        counts.
 *
 * @param[in]  dev          Device descriptor of Si114x device to read from
 *
 * @return                  Distance in ADC counts
 */
uint16_t si114x_read_distance(si114x_t *dev);

/**
 * @brief Read the response register.
 *
 * @param[in]  dev          Device descriptor of Si114x device to read from
 *
 * @return                  Device response register contents
 */
uint8_t si114x_read_response(si114x_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* SI114X_H */
/** @} */

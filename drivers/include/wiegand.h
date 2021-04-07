/*
 * Copyright (C) 2020 Hackerspace San Salvador
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_wiegand Wiegand
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 *
 * @brief       GPIO based Wiegand driver
 *
 * This driver provides @ref drivers_saul capabilities.
 * @{
 *
 * @file
 * @brief       Driver for the Wiegand Protocol
 *
 * @author      Mario Gomez <mario.gomez@teubi.co>
 */

#ifndef WIEGAND_H
#define WIEGAND_H

#include <stdint.h>
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Wait time used by the wiegand protocol (25 ms)
 */
#ifndef WIEGAND_WAIT
#define WIEGAND_WAIT      25000
#endif

/**
 * @brief   Parameters needed for device initialization
 */
typedef struct {
    gpio_t d0;              /**< Wiegand D0 data line */
    gpio_t d1;              /**< Wiegand D1 data line */
    gpio_flank_t flank;     /**< flank detection */
} wiegand_params_t;

/**
 * @brief   Device descriptor for a wiegand device
 */
typedef struct {
    uint32_t code;                          /**< Wiegand code */
    int16_t type;                           /**< Wiegand type */
    bool ready;                             /**< Ready to convert */
    int16_t last_bit_count;                 /**< Latest bit-count */
    volatile uint32_t wg_card_temp_high;    /**< Card MSB */
    volatile uint32_t wg_card_temp;         /**< Card LSV  */
    volatile uint32_t wg_last_wiegand;      /**< Last card lecture */
    volatile int16_t wg_bit_count;          /**< Number of bits read */
    const wiegand_params_t *conn;           /**< Connection parameters */
} wiegand_t;

/**
 * @brief   Initialize a Wiegand device
 *
 * @param[out] dev          device descriptor
 * @param[in] params        configuration parameters
 *
 * @return                   0 on success
 * @return                  -1 on error
 */
int16_t wiegand_init(wiegand_t *dev, const wiegand_params_t *params);

/**
 * @brief   Checks and stores new Wiegand data
 *
 * @param[out] dev          device descriptor of sensor
 *
 * @return                  true if code was read
 * @return                  false if no data
 */
bool wiegand_available(wiegand_t *dev);

/**
 * @brief   Reads the code read from the Wiegand device
 *
 * @param[in]  dev          device descriptor of sensor
 *
 * @return                  Code read from device
 */
uint32_t wiegand_get_code(wiegand_t *dev);

/**
 * @brief   Reads the Wiegand device type
 *
 * @param[out] dev         device descriptor of sensor
 *
 * @return                 Wiegand type
 */
int16_t wiegand_get_type(wiegand_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* WIEGAND_H */
/** @} */

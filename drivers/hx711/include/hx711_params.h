/*
 * Copyright (C) 2019 Philipp-Alexander Blum
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_hx711
 *
 * @{
 * @file
 * @brief       Default configuration for HX711 devices
 *
 * @author      Philipp-Alexander Blum <philipp-blum@jakiku.de>
 *
 *
 */

#ifndef HX711_PARAMS_H
#define HX711_PARAMS_H

#include "board.h"
#include "hx711.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* @name   Set default configuration parameters for the HX711 driver
* @{
*/
#ifndef HX711_PARAM_SCK
#define HX711_PARAM_SCK     GPIO_PIN(0, 0)
#endif
#ifndef HX711_PARAM_DOUT
#define HX711_PARAM_DOUT    GPIO_PIN(0, 0)
#endif
/**
 * @note Read the HX711 datasheet for more information
 * CHANNEL_A_128 => 1
 * CHANNEL_B_32 => 2
 * CHANNEL_A_64 => 3
 */
#ifndef HX711_PARAM_GAIN_PULSES
#define HX711_PARAM_GAIN_PULSES    1
#endif
/**
 * @brief Offset, where to start 0 at. Formular: (RAW_VALUE - HX711_PARAM_OFFSET) / HX711_PARAM_SCALE
*/
#ifndef HX711_PARAM_OFFSET
#define HX711_PARAM_OFFSET  0
#endif
#ifndef HX711_PARAM_SCALE
#define HX711_PARAM_SCALE   1
#endif
/**
 * @brief Default for hx711_read_average. Also used in hx711_get_units.
 * Reads the value HX711_PARAM_AVG_TIMES times and returns SUM / HX711_PARAM_AVG_TIMES.
 */
#ifndef HX711_PARAM_AVG_TIMES
#define HX711_PARAM_AVG_TIMES   20
#endif
#ifndef HX711_PARAM_SLEEP_TIME
#define HX711_PARAM_SLEEP_TIME  1000
#endif
#ifndef HX711_SAUL_INFO
#define HX711_SAUL_INFO          { .name = "hx711" }
#endif


/**
 * @brief Meta information to keep in the SAUL registry
 */
static const saul_reg_info_t hx711_saul_info = HX711_SAUL_INFO;
/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* HX711_PARAMS_H */

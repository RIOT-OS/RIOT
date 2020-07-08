/*
 * Copyright (C) 2018 - 2020 Philipp-Alexander Blum <philipp-blum@jakiku.de>
 *               2019 Kaspar Schleiser <kaspar@schleiser.de>
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
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
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
#define HX711_PARAM_SCK         GPIO_PIN(0, 0)
#endif
#ifndef HX711_PARAM_DOUT
#define HX711_PARAM_DOUT        GPIO_PIN(0, 0)
#endif
/**@}*/

/**
 * @brief Default gain value. Read the HX711 datasheet for more information
 */
#ifndef HX711_PARAM_GAIN
#define HX711_PARAM_GAIN        (CHANNEL_A_128)
#endif

/**
 * @brief How much to divide the result.
 * Formular: (SUM(RAW_VALUES) / TIMES - OFFSET) / DIVIDER
*/
#ifndef HX711_PARAM_DIVIDER
#define HX711_PARAM_DIVIDER       362
#endif
/**@}*/

/**
 * @brief How many times SAUL has to read the ADC
 *        in order to create an average value.
 *        Formular: (SUM(RAW_VALUES) / TIMES - OFFSET) / DIVIDER
 */
#ifndef HX711_PARAM_AVG_TIMES
#define HX711_PARAM_AVG_TIMES 4
#endif

/**
 * @brief The SAUL info the hx711 device
 */
#ifndef HX711_SAUL_INFO
#define HX711_SAUL_INFO          { .name = "hx711" }
#endif

/**
 * @brief The params for the HX711
 */
#ifndef HX711_PARAMS
#define HX711_PARAMS             { .sck = HX711_PARAM_SCK, \
                                   .dout = HX711_PARAM_DOUT, \
                                   .gain = HX711_PARAM_GAIN, \
                                   .read_times = HX711_PARAM_AVG_TIMES, \
                                   .divider = HX711_PARAM_DIVIDER }
#endif

/**
 * @brief   Configure HX711 devices
 */
static const hx711_params_t hx711_params[] =
{
        HX711_PARAMS
};

/**
 * @brief   Allocate and configure entries to the SAUL registry
 */
static const saul_reg_info_t hx711_saul_info[] =
{
        HX711_SAUL_INFO
};

#ifdef __cplusplus
}
#endif

#endif /* HX711_PARAMS_H */

/*
 * Copyright (C) 2018 - 2020 Philipp-Alexander Blum <philipp-blum@jakiku.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_hx711 HX711 weigh scale ADC
 * @ingroup     drivers_sensors
 * @brief       Driver for the HX711 weigh scale ADC
 *
 * @{
 * @file
 * @brief       HX711 driver
 *
 * @author      Philipp-Alexander Blum <philipp-blum@jakiku.de>
 */

#ifndef HX711_H
#define HX711_H

#include "periph/gpio.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @note Selectable gain and channel:
 *       Read the HX711 datasheet for more information
 */
typedef enum {
    CHANNEL_A_128 = 1,
    CHANNEL_B_32 = 2,
    CHANNEL_A_64 = 3
} hx711_gain_t;

/**
 * @brief   Configuration parameters of the HX711 device
 * @{
 */
typedef struct {
    gpio_t sck;         /**< SCK GPIO pin */
    gpio_t dout;        /**< DT or DOUT GPIO pin */
    hx711_gain_t gain;  /**< Gain and channel selection.
                         * Read HX711 documentation for more information. */
    uint8_t read_times; /**< How many times SAUL reads the value
                         *   and creates an average from the sum. */
    uint16_t divider;   /**< The devider in the formular:
                         *   ((SUM(RAW_VALUES) / TIMES) - OFFSET) / DIVIDER */
} hx711_params_t;

/**
 * @brief   Device descriptor for the HX711
 */
typedef struct {
    hx711_params_t params; /**< params for the hx711 */
    int32_t offset;        /**< Offset which will be set while taring */
} hx711_t;

/** @} */

/**
 * @brief Initializes the hx711 device, without taring it.
 * @param dev The device to initialize
 * @param params The parameters for the hx711 device
 */
void hx711_init(hx711_t *dev, const hx711_params_t *params);

/**
 * @brief Gets a single 24-Bit value from the HX711.
 * @return The 24-bit value with gain.
 */
int32_t hx711_read(hx711_t *dev);

/**
 * @brief Read a raw value for a configurable times
 * @param dev The hx711 device
 * @param times The amount of repeated measurements. Calculates the average.
 * @return the average value.
 */
int32_t hx711_read_average(hx711_t *dev, uint8_t times);

/**
 * @brief Read a value a configurable times and return the average value.
 * @param dev The hx711 device
 * @param times The amount of repeated measurements.
 * @return returns (SUM(RAW_VALUES) / TIMES) - OFFSET
 */
int32_t hx711_get_value(hx711_t *dev, uint8_t times);

/**
 * @brief Read the average of a configurable times
 *        of a cleared and divided value
 * @param dev The hx711 device
 * @param times The amount of repeated measurements.
 * @return returns ((SUM(RAW_VALUES) / TIMES) - OFFSET) / DIVIDER
 */
int32_t hx711_get_units(hx711_t *dev, int8_t times);

/**
 * @brief Get the offset and set it to the device. Tare the scale.
 * @param dev The hx711 device
 * @param times The amount of repeated measurements, before setting.
 */
void hx711_tare(hx711_t *dev, uint8_t times);

/**
 * @brief Power a given hx711 down
 * @param dev The hx711 device
 */
void hx711_power_down(hx711_t *dev);

/**
 * @brief Power a given hx711 up
 * @param dev The hx711 device
 * @return returns ((SUM(RAW_VALUES) / TIMES) - OFFSET) / DIVIDER
 */
void hx711_power_up(hx711_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* HX711_H */
/** @} */
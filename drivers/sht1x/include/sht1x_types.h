/*
 * Copyright 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_sht1x
 *
 * @{
 * @file
 * @brief       Data types of SHT10/SHT11/SHT15 Device Driver
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef SHT1X_TYPES_H
#define SHT1X_TYPES_H

#include <stdint.h>
#include <periph/gpio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Possible configuration (=status byte) values of the SHT10/11/15
 *
 * These values can be or'ed together to get the configuration.
 */
typedef enum {
    /** Use 8/12 bit resolution instead of 12/14 bit for temp/hum */
    SHT1X_CONF_LOW_RESOLUTION   = 0x01,
    /** Don't upload calibration data to register to safe 10 millisec */
    SHT1X_CONF_SKIP_CALIBRATION = 0x02,
    /** Waste 8mA at 5V to increase the sensor temperature up to 10°C */
    SHT1X_CONF_ENABLE_HEATER    = 0x04,
    /** Skip the CRC check (and reading the CRC byte) to safe time */
    SHT1X_CONF_SKIP_CRC         = 0x08,
} sht1x_conf_t;

/**
 * @brief Possible values for Vdd (measured temperature depends on it)
 */
typedef enum {
    SHT1X_VDD_5_0V = 0,
    SHT1X_VDD_4_0V = 1,
    SHT1X_VDD_3_5V = 2,
    SHT1X_VDD_3_0V = 3,
    SHT1X_VDD_2_5V = 4,
} sht1x_vdd_t;

/**
 * @brief SHT10/11/15 temperature humidity sensor
 */
typedef struct {
    gpio_t  clk;      /**< GPIO connected to the clock pin of the SHT1X */
    gpio_t  data;     /**< GPIO connected to the data pin of the SHT1X */
    int16_t temp_off; /**< Offset to add to the measured temperature */
    int16_t hum_off;  /**< Offset to add to the measured humidity */
    uint8_t conf;     /**< Status byte (containing configuration) of the SHT1X */
    uint8_t vdd;      /**< Supply voltage of the SHT1X (as sht1x_vdd_t) */
} sht1x_dev_t;

/**
 * @brief Parameters required to set up the SHT10/11/15 device driver
 */
typedef struct {
    gpio_t      clk;  /**< GPIO connected to the clock pin of the SHT1X */
    gpio_t      data; /**< GPIO connected to the data pin of the SHT1X */
    sht1x_vdd_t vdd;  /**< The supply voltage of the SHT1X */
} sht1x_params_t;

#ifdef __cplusplus
}
#endif

#endif /* SHT1X_TYPES_H */
/** @} */

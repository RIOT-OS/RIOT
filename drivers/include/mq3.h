/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    drivers_mq3 MQ-3 Alcohol Tester
 * @ingroup     drivers_sensors
 * @brief       Device driver for the MQ-3 alcohol sensor
 * @{
 *
 * @file
 * @brief       Device driver interface for the MQ-3 alcohol sensor
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef MQ3_H
#define MQ3_H

#include "periph/adc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   maximum unprocessed value fetched form the sensor
 */
#define MQ3_MAX_RAW_VALUE           (1023U)

/**
 * @brief   device descriptor for a MQ-3 sensor
 */
typedef struct {
    adc_t adc_line;         /**< the used ADC line */
} mq3_t;

/**
 * @brief   Initialize a MQ-3 alcohol sensor
 *
 * The MQ-3 sensor is interfaced by a single ADC pin, specified by `adc` and `channel`.
 *
 * @note The sensor needs about a minute to heat up before meaningful measurements
 *       can be made.
 *
 * @param[out] dev      device descriptor of an MQ-3 sensor
 * @param[in] adc_line  the ADC device the sensor is connected to
 *
 * @retval  0       success
 * @retval  -1      failure
 */
int mq3_init(mq3_t *dev, adc_t adc_line);

/**
 * @brief   Read the RAW sensor value, can be between 0 and MQ3_MAX_RAW_VALUE
 *
 * @param[in] dev       device descriptor of the MQ-3 sensor to read from
 *
 * @return              the raw sensor value, between 0 and MQ3_MAX_RAW_VALUE
 */
int16_t mq3_read_raw(const mq3_t *dev);

/**
 * @brief   Read the scaled sensor value of PPM of alcohol
 *
 * @param[in] dev       device descriptor of the MQ-3 sensor to read from
 *
 * @return              the scaled sensor value in PPM of alcohol
 */
int16_t mq3_read(const mq3_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* MQ3_H */
/** @} */

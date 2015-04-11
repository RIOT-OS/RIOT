/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    driver_mq3 MQ-3 Alcohol Tester
 * @ingroup     drivers
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

#define MQ3_MAX_RAW_VALUE           (1023U)

/**
 * @brief device descriptor for a MQ-3 sensor
 */
typedef struct {
    adc_t adc_dev;          /**< the used ADC device */
    int adc_chan;           /**< used channel of the ADC */
} mq3_t;

/**
 * @brief Initialize a MQ-3 alcohol sensor
 *
 * The MQ-3 sensor is interfaced by a single ADC pin, specified by `adc` and `channel`.
 *
 * @note The sensor needs about a minute to heat up before meaningful measurements
 *       can be made.
 *
 * @param[out] dev      device descriptor of an MQ-3 sensor
 * @param[in] adc       the ADC device the sensor is connected to
 * @param[in] channel   the channel of the ADC device used
 *
 * @return              0 on success
 * @return              -1 on error
 */
int mq3_init(mq3_t *dev, adc_t adc, int channel);

/**
 * @brief Read the RAW sensor value, can be between 0 and MQ3_MAX_RAW_VALUE
 *
 * @param[in] dev       device descriptor of the MQ-3 sensor to read from
 *
 * @return              the raw sensor value, between 0 and MQ3_MAX_RAW_VALUE
 */
int mq3_read_raw(mq3_t *dev);

/**
 * @brief Read the scaled sensor value of PPM of alcohol
 *
 * @param[in] dev       device descriptor of the MQ-3 sensor to read from
 *
 * @return              the scaled sensor value in PPM of alcohol
 */
int mq3_read(mq3_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* MQ3_H */
/** @} */

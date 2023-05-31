/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_mq3
 * @{
 *
 * @file
 * @brief       Device driver implementation for the MQ-3 alcohol sensor
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "mq3.h"

#define PRECISION           ADC_RES_10BIT
#define MIN                 (100)               /* TODO: calibrate to useful value */
#define FACTOR              (233)               /* TODO: calibrate to useful value */
#define DIVISOR             (100)

int mq3_init(mq3_t *dev, adc_t adc_line)
{
    dev->adc_line = adc_line;
    return adc_init(dev->adc_line);
}

int16_t mq3_read_raw(const mq3_t *dev)
{
    return adc_sample(dev->adc_line, PRECISION);
}

int16_t mq3_read(const mq3_t *dev)
{
    int32_t res = mq3_read_raw(dev);
    res = (res > MIN) ? res - MIN : 0;
    return res * FACTOR / DIVISOR;
}

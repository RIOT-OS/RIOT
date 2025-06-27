/*
 * SPDX-FileCopyrightText: 2014 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
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
#include "macros/math.h"

#define PRECISION           ADC_RES_10BIT
/* TODO: calibrate to useful value */
#define MIN                 (100U)
/* TODO: calibrate to useful value */
#define SHIFT               (12U)
#define FACTOR              DIV_ROUND(233UL << SHIFT, 100)

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
    uint32_t res = mq3_read_raw(dev);
    res = (res > MIN) ? res - MIN : 0;
    /* same as `(int16_t)(res * 2.33)` */
    return (res * FACTOR) >> SHIFT;
}

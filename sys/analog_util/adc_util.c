/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_analog_util
 * @{
 *
 * @file
 * @brief       ADC utility function implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "analog_util.h"

/* keep a max value to ADC resolution mapping for quick access in the ROM */
static const int val_max[] = {
    [ADC_RES_6BIT] =    0x003f,
    [ADC_RES_8BIT] =    0x00ff,
    [ADC_RES_10BIT] =   0x03ff,
    [ADC_RES_12BIT] =   0x0fff,
    [ADC_RES_14BIT] =   0x3fff,
    [ADC_RES_16BIT] =   0xffff
};

int adc_util_map(int sample, adc_res_t res, int min, int max)
{
    return ((((max - min) * sample) / val_max[res]) + min);
}

float adc_util_mapf(int sample, adc_res_t res, float min, float max)
{
    return ((((max - min) * sample) / val_max[res]) + min);
}

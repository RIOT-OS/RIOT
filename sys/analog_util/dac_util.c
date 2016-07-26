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
 * @brief       DAC utility function implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <limits.h>

#include "analog_util.h"

uint16_t dac_util_map(int value, int min, int max)
{
    return (uint16_t)((value - min) * UINT16_MAX) / (max - min);
}

uint16_t dac_util_mapf(float value, float min, float max)
{
    return (uint16_t)(((value - min) * UINT16_MAX) / (max - min));
}

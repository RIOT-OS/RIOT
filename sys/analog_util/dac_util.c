/*
 * SPDX-FileCopyrightText: 2015 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
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
    return (uint16_t)(((value - min) * UINT16_MAX) / (max - min));
}

uint16_t dac_util_mapf(float value, float min, float max)
{
    return (uint16_t)(((value - min) * UINT16_MAX) / (max - min));
}

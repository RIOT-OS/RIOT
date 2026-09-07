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

#include <errno.h>
#include <limits.h>
#include <inttypes.h>
#include <stdio.h>
#include <math.h>

#ifdef M_PI
#  define PI M_PI
#elif defined M_PIf
#  define PI M_PIf
#else
#  define PI 3.14159265358979323846
#endif

#include "imath.h"
#include "analog_util.h"
#include "macros/math.h"
#include "modules.h"
#include "periph/dac.h"

uint16_t dac_util_map(int value, int min, int max)
{
    return (uint16_t)(((value - min) * UINT16_MAX) / (max - min));
}

uint16_t dac_util_mapf(float value, float min, float max)
{
    return (uint16_t)(((value - min) * UINT16_MAX) / (max - min));
}

int dac_util_sine(uint16_t center_mv, uint16_t amplitude_mv, uint32_t freq_hz,
                  uint16_t dac_ref_mv, uint16_t *buf, uint32_t *buf_max)
{
    if (freq_hz == 0) {
        return -EINVAL;
    }
    if (center_mv < amplitude_mv) {
        return -EINVAL;
    }
    if (((uint32_t)center_mv + amplitude_mv) > (uint32_t)dac_ref_mv) {
        return -EINVAL;
    }
    uint32_t dac_freq = dac_get_freq();
    uint32_t samples_per_period = DIV_ROUND(dac_freq, freq_hz);
    if (samples_per_period > *buf_max) {
        printf("DAC: buffer too small for requested frequency %lu Hz, need %lu samples, have %lu\n",
               freq_hz, samples_per_period, *buf_max);
        return -ENOMEM;
    }

    uint16_t dac_center = dac_util_map(center_mv, 0, dac_ref_mv);
    uint16_t dac_max_val = dac_util_map(center_mv + amplitude_mv, 0, dac_ref_mv);
    uint16_t dac_amplitude = dac_max_val - dac_center;

#if IS_USED(MODULE_CORTEXM_FPU)
    float radix_step = (2.0f * (float)PI) / (float)samples_per_period;
    for (uint32_t i = 0; i < samples_per_period; ++i) {
        float val = (float)dac_center + ((float)dac_amplitude * sinf((float)i * radix_step));
        buf[i] = (uint16_t)(val + 0.5f);
    }
#else
    for (uint32_t i = 0; i < samples_per_period; i++) {
        uint32_t angle = (i * SINI_PERIOD) / samples_per_period;
        int32_t s = fast_sini(angle);
        int32_t sample = (int32_t)dac_center + (((int32_t)dac_amplitude * s) / SINI_MAX);
        buf[i] = (uint16_t)sample;
    }
#endif

    *buf_max = samples_per_period;
    return 0;
}

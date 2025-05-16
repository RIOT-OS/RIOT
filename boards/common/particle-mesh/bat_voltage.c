/*
 * Copyright (C)  2024 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_particle-mesh
 * @ingroup     saul_bat_voltage
 * @{
 * @file
 * @brief       Implementation of battery voltage convert function
 *
 * @author      Martine S. Lenders <martine.lenders@tu-dresden.de>
 *
 * @}
 */

#ifdef MODULE_SAUL_BAT_VOLTAGE
#include "saul/bat_voltage.h"

int16_t saul_bat_voltage_convert(int32_t adc_sample)
{
    /* See https://community.particle.io/t/can-argon-or-xenon-read-the-battery-state/45554/6
     * and https://docs.particle.io/assets/images/xenon/schematic-main.png */
    return (int16_t)((adc_sample * 33L * 1403L) / 10000L);
}

#endif /* MODULE_SAUL_BAT_VOLTAGE */

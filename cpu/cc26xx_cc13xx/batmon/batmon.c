/*
 * Copyright (C) 2021 Jean Pierre Dudey
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     cpu_cc26xx_cc13xx
 *
 * @{
 *
 * @file
 * @brief       Battery monitor API
 *
 * @author      Jean Pierre Dudey <jeandudey@hotmail.com>
 *
 * @}
 */

#include "cpu.h"
#include "saul.h"

/** Bits used to enable measurements/calculations */
#define BATMON_ENABLE   (AON_BATMON_CTL_CALC_EN | AON_BATMON_CTL_MEAS_EN)

void cc26xx_cc13xx_batmon_init(void)
{
    /* Disable events */
#ifdef CPU_VARIANT_X2
    AON_BATMON->EVENTMASK &= ~AON_BATMON_EVENTMASK_TEMP_BELOW_LL_MASK;
    AON_BATMON->EVENTMASK &= ~AON_BATMON_EVENTMASK_TEMP_OVER_UL_MASK;
#endif /* CPU_VARIANT_X2 */

    /* Enable calculations and measurements */
    AON_BATMON->CTL |= BATMON_ENABLE;
}

static int _read_voltage(const void *dev, phydat_t *res)
{
    (void)dev;

    uint16_t value = AON_BATMON->BAT >> AON_BATMON_BAT_FRAC_s;

    /* voltage is provided in a 3.8 fixed-point value, convert
     * it directly to mV */
    res->val[0] = (value * 125) >> 5;
    res->unit = UNIT_V;
    res->scale = -3;

    return 1;
}

static int _read_temperature(const void *dev, phydat_t *res)
{
    (void)dev;

    /* Shift left then right to sign extend the AON_BATMON.TEMP field */
    int32_t signed_temp = (int32_t)AON_BATMON->TEMP;
    signed_temp <<= 32 - AON_BATMON_TEMP_INT_w - AON_BATMON_TEMP_INT_s;
    signed_temp >>= 32 - AON_BATMON_TEMP_INT_w - AON_BATMON_TEMP_INT_s;

    /* Casting voltage_slope to int8_t prior to assignment in order to make
     * sure sign extension works properly */
    int32_t voltage_slope = (int8_t)*((reg8_t *)&FCFG->MISC_TRIM);
    int32_t temp_correction = (int32_t)voltage_slope
                            * ((int32_t)AON_BATMON->BAT - 0x300) >> 4;

    res->val[0] = ((signed_temp - temp_correction) + 0x80) >> 8;

    res->unit = UNIT_TEMP_C;
    res->scale = 0;

    return 1;
}

saul_driver_t cc26xx_cc13xx_voltage_saul_driver = {
    .read = _read_voltage,
    .write = NULL,
    .type = SAUL_SENSE_VOLTAGE,
};

saul_driver_t cc26xx_cc13xx_temperature_saul_driver = {
    .read = _read_temperature,
    .write = NULL,
    .type = SAUL_SENSE_TEMP,
};

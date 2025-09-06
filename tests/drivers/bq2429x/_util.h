/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     tests
 * @brief       Util functions for test application for the BQ24298x device
 *              driver
 * @author      Jean Pierre Dudey <jeandudey@hotmail.com>
 * @file
 */

#include <assert.h>
#include "bq2429x.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline const char *_util_vlim_to_str(bq2429x_input_voltage_limit_t vlim)
{
    switch (vlim) {
        case BQ2429X_VLIM_3880:
            return "3880 mV";
        case BQ2429X_VLIM_3960:
            return "3960 mV";
        case BQ2429X_VLIM_4040:
            return "4040 mV";
        case BQ2429X_VLIM_4120:
            return "4120 mV";
        case BQ2429X_VLIM_4200:
            return "4200 mV";
        case BQ2429X_VLIM_4280:
            return "4280 mV";
        case BQ2429X_VLIM_4360:
            return "4360 mV";
        case BQ2429X_VLIM_4440:
            return "4440 mV";
        case BQ2429X_VLIM_4520:
            return "4520 mV";
        case BQ2429X_VLIM_4600:
            return "4600 mV";
        case BQ2429X_VLIM_4680:
            return "4680 mV";
        case BQ2429X_VLIM_4760:
            return "4760 mV";
        case BQ2429X_VLIM_4840:
            return "4840 mV";
        case BQ2429X_VLIM_4920:
            return "4920 mV";
        case BQ2429X_VLIM_5000:
            return "5000 mV";
        case BQ2429X_VLIM_5080:
            return "5080 mV";
        default:
            assert(0);
            break;
    }

    return "";
}

static inline const char *_util_ilim_to_str(bq2429x_input_current_limit_t vlim)
{
    switch (vlim) {
        case BQ2429X_ILIM_100:
            return "100 mA";
        case BQ2429X_ILIM_150:
            return "150 mA";
        case BQ2429X_ILIM_500:
            return "500 mA";
        case BQ2429X_ILIM_900:
            return "900 mA";
        case BQ2429X_ILIM_1000:
            return "1000 mA";
        case BQ2429X_ILIM_1500:
            return "1500 mA";
        case BQ2429X_ILIM_2000:
            return "2000 mA";
        case BQ2429X_ILIM_3000:
            return "3000 mA";
        default:
            assert(0);
            break;
    }

    return "";
}

static inline const char *_util_ichg_to_str(bq2429x_charge_current_t vlim)
{
    switch (vlim) {
        case BQ2429X_ICHG_512:
            return "512 mA";
        case BQ2429X_ICHG_1024:
            return "1024 mA";
        case BQ2429X_ICHG_2048:
            return "2048 mA";
        case BQ2429X_ICHG_3008:
            return "3008 mA";
        case BQ2429X_ICHG_4032:
            return "4032 mA";
        case BQ2429X_ICHG_4544:
            return "4544 mA";
        default:
            assert(0);
            break;
    }

    return "";
}

static inline const char *_util_vreg_to_str(bq2429x_charge_voltage_limit_t vreg)
{
    switch (vreg) {
        case BQ2429X_VREG_3504:
            return "3504 mV";
        case BQ2429X_VREG_3600:
            return "3600 mV";
        case BQ2429X_VREG_3808:
            return "3808 mV";
        case BQ2429X_VREG_3904:
            return "3904 mV";
        case BQ2429X_VREG_4000:
            return "4000 mV";
        case BQ2429X_VREG_4112:
            return "4112 mV";
        case BQ2429X_VREG_4208:
            return "4208 mV";
        case BQ2429X_VREG_4304:
            return "4304 mV";
        case BQ2429X_VREG_4352:
            return "4352 mV";
        case BQ2429X_VREG_4400:
            return "4400 mV";
        default:
            assert(0);
            break;
    }

    return "";
}

static inline const char *_util_vbus_stat_to_str(bq2429x_vbus_stat_t stat)
{
    switch (stat) {
        case BQ2429X_VBUS_NONE:
            return "None";
        case BQ2429X_VBUS_USB_CHARGER:
            return "USB";
        case BQ2429X_VBUS_AP_CHARGER:
            return "Adapter Port";
        case BQ2429X_VBUS_OTG:
            return "OTG";
        default:
            assert(0);
            break;
    }

    return "";
}

static inline const char *_util_chrg_stat_to_str(bq2429x_chrg_stat_t stat)
{
    switch (stat) {
        case BQ2429X_CHARGE_NONE:
            return "None";
        case BQ2429X_CHARGE_PRE:
            return "Pre-charge";
        case BQ2429X_CHARGE_FAST:
            return "Fast Charging";
        case BQ2429X_CHARGE_DONE:
            return "Charge Done";
        default:
            assert(0);
            break;
    }

    return "";
}

static inline const char *_util_chrg_fault_to_str(bq2429x_chrg_fault_t fault)
{
    switch (fault) {
        case BQ2429x_CHRG_FAULT_NORMAL:
            return "Normal";
        case BQ2429x_CHRG_FAULT_INPUT:
            return "Input Fault (OVP or bad source)";
        case BQ2429x_CHRG_FAULT_THERMAL_SHUTDOWN:
            return "Thermal Shutdown";
        case BQ2429x_CHRG_FAULT_CHARGE_TIMER_EXPIRATION:
            return "Charge Timer Expiration";
        default:
            assert(0);
            break;
    }

    return "";
}

#ifdef __cplusplus
}
#endif

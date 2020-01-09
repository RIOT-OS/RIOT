/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the INA3221 sensor driver
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 *
 * @}
 */

#include <string.h>
#include "periph/gpio.h"
#include "xtimer.h"
#include "fmt.h"
#include "ina3221_internal.h"
#include "ina3221_params.h"
#include "ina3221.h"

/* Issue critical alert if shunt voltage is above thist value */
#define CRIT_ALERT_LIM_UV      (2500)
/* Issue warning alert if shunt voltage is above this value */
#define WARN_ALERT_LIM_UV      (1600)
/* Issue power valid alert if bus voltage is below this value */
#define PV_LOWER_LIM_MV        (4500)
/* Issue power valid alert if bus voltage is above this value */
#define PV_UPPER_LIM_MV        (5500)
/* Issue critical alert if the sum of all shunt voltages is above this value */
#define SUM_SHUNT_ALERT_LIM_UV (8000)

#define COL   ("              ")
#define THEAD (    \
        "channel       | shunt [uV]   | bus [mV]     | current [uA] | power [uW]   ")
#define HLINE (    \
        "--------------+--------------+--------------+--------------+--------------")

#ifdef MODULE_INA3221_ALERTS
static void warning_alert(void *arg)
{
    ina3221_t *dev = (ina3221_t *)arg;

    (void)dev;
    puts("WARNING");
}

static void critical_alert(void *arg)
{
    ina3221_t *dev = (ina3221_t *)arg;

    (void)dev;
    puts("CRITICAL");
}

static void timing_control_alert(void *arg)
{
    ina3221_t *dev = (ina3221_t *)arg;

    (void)dev;
    puts("TIMING CONTROL");
}

static void power_valid_alert(void *arg)
{
    ina3221_t *dev = (ina3221_t *)arg;

    (void)dev;
    puts("POWER VALID");
}
#endif

int main(void)
{
    xtimer_init();

    int status;
    ina3221_t dev;

    for (int32_t i = INA3221_MIN_SHUNT_UV; i <= INA3221_MAX_SHUNT_UV;
         i += INA3221_SHUNT_VOLTAGE_PRECISION_UV) {
        int16_t reg = shunt_voltage_uv_to_reg_val(i);
        int32_t sv = reg_val_to_shunt_voltage_uv(reg);
        int16_t _reg = shunt_voltage_uv_to_reg_val(sv);
        if (reg != _reg || i != sv) {
            printf(
                "[FAILURE] shunt V conversiom: shunt V: %" PRId32 ", REG: %" PRId16 ", shunt V: %" PRId32 ", REG: %" PRId16 "\n", i, reg, sv,
                _reg);
            return 1;
        }
    }
    for (int32_t i = INA3221_MIN_BUS_MV; i <= INA3221_MAX_BUS_MV;
         i += INA3221_BUS_VOLTAGE_PRECISION_MV) {
        int16_t reg = bus_voltage_mv_to_reg_val(i);
        int32_t bv = reg_val_to_bus_voltage_mv(reg);
        int16_t _reg = bus_voltage_mv_to_reg_val(bv);
        if (reg != _reg || i != bv) {
            printf(
                "[FAILURE] bus V conversion: bus V: %" PRId32 ", REG: %" PRId16 ", bus V: %" PRId32 ", REG: %" PRId16 "\n", i, reg, bv,
                _reg);
            return 1;
        }
    }
    for (int32_t i = INA3221_MIN_SHUNT_SUM_UV; i <= INA3221_MAX_SHUNT_SUM_UV;
         i += INA3221_SHUNT_VOLTAGE_PRECISION_UV) {
        int16_t reg = sum_shunt_voltage_uv_to_reg_val(i);
        int32_t sv = sum_reg_val_to_shunt_voltage_uv(reg);
        int16_t _reg = sum_shunt_voltage_uv_to_reg_val(sv);
        if (reg != _reg || i != sv) {
            printf(
                "[FAILURE] shunt sum V conversion: sum V: %" PRId32 ", REG: %" PRId16 ", sum V: %" PRId32 ", REG: %" PRId16 "\n", i, reg, sv,
                _reg);
            return 1;
        }
    }

    status = ina3221_init(&dev, &ina3221_params[0]);
    if (status != INA3221_OK) {
        printf("[FAILURE] ina3221_init: %d\n", status);
        return 1;
    }
    else {
        puts("[SUCCESS] ina3221_init");
    }

#ifdef MODULE_INA3221_ALERTS
    status = ina3221_enable_warning_alert(&dev, warning_alert, &dev);
    if (status == -ENOTSUP) {
        puts("[WARNING] INA3221_ALERT_WRN not supported");
    }
    else if (status != INA3221_OK) {
        printf("[FAILURE] ina3221_enable_alert INA3221_ALERT_WRN: %d\n",
               status);
        return 1;
    }

    status = ina3221_enable_critical_alert(&dev, critical_alert, &dev);
    if (status == -ENOTSUP) {
        puts("[WARNING] INA3221_ALERT_CRT not supported");
    }
    else if (status != INA3221_OK) {
        printf("[FAILURE] ina3221_enable_alert INA3221_ALERT_CRT: %d\n",
               status);
        return 1;
    }

    status = ina3221_enable_timing_control_alert(&dev, timing_control_alert,
                                                 &dev);
    if (status == -ENOTSUP) {
        puts("[WARNING] INA3221_ALERT_TC not supported");
    }
    else if (status != INA3221_OK) {
        printf("[FAILURE] ina3221_enable_alert INA3221_ALERT_TC: %d\n", status);
        return 1;
    }

    status = ina3221_enable_power_valid_alert(&dev, power_valid_alert, &dev);
    if (status == -ENOTSUP) {
        puts("[WARNING] INA3221_ALERT_PV not supported");
    }
    else if (status != INA3221_OK) {
        printf("[FAILURE] ina3221_enable_alert INA3221_ALERT_PV: %d\n", status);
        return 1;
    }
    puts("[SUCCESS] ina3221_enable_alert");
#endif

    int32_t shunt_uv[INA3221_NUM_CH] = { 0 };
    int16_t bus_mv[INA3221_NUM_CH] = { 0 };
    int32_t sum_shunt_uv = 0;
    int32_t current_ua[INA3221_NUM_CH] = { 0 };
    int32_t power_uw[INA3221_NUM_CH] = { 0 };
    uint16_t flags = 0;

    status = ina3221_read_shunt_uv(&dev,
                                   INA3221_CH1 | INA3221_CH2 | INA3221_CH3,
                                   shunt_uv, &flags);
    if (status != INA3221_NUM_CH) {
        printf("[FAILURE] ina3221_read_shunt_uv: %d\n", status);
        return 1;
    }
    else {
        puts("[SUCCESS] ina3221_read_shunt_uv");
    }

    status = ina3221_calculate_current_ua(&dev,
                                          INA3221_CH1 | INA3221_CH2 | INA3221_CH3, shunt_uv,
                                          current_ua);
    if (status != INA3221_NUM_CH) {
        printf("[FAILURE] ina3221_calculate_current_ua: %d\n", status);
        return 1;
    }
    else {
        puts("[SUCCESS] ina3221_calculate_current_ua");
    }

    status = ina3221_read_bus_mv(&dev, INA3221_CH1 | INA3221_CH2 | INA3221_CH3,
                                 bus_mv, &flags);
    if (status != INA3221_NUM_CH) {
        printf("[FAILURE] ina3221_read_bus_mv: %d\n", status);
        return 1;
    }
    else {
        puts("[SUCCESS] ina3221_read_bus_mv");
    }

    status = ina3221_calculate_power_uw(bus_mv, current_ua, status, power_uw);
    if (status != INA3221_NUM_CH) {
        printf("[FAILURE] ina3221_calculate_power_uw: %d\n", status);
        return 1;
    }
    else {
        puts("[SUCCESS] ina3221_calculate_power_uw");
    }

    status = ina3221_set_enable_sum_channel(&dev, INA3221_CH_ENABLE,
                                            INA3221_CH_ENABLE,
                                            INA3221_CH_ENABLE);
    if (status < 0) {
        printf("[FAILURE] ina3221_set_enable_sum_channel: %d\n", status);
        return 1;
    }
    else {
        printf("[SUCCESS] ina3221_set_enable_sum_channel");
    }

    status = ina3221_read_shunt_sum_uv(&dev, &sum_shunt_uv, &flags);
    if (status != INA3221_OK) {
        printf("[FAILURE] ina3221_read_shunt_sum_uv: %d\n", status);
        return 1;
    }
    else {
        puts("[SUCCESS] ina3221_read_shunt_sum_uv");
    }

    status = INA3221_TRIGGER_SHUNT_AND_BUS(&dev);
    if (status < 0) {
        printf("[FAILURE] INA3221_TRIGGER_SHUNT_AND_BUS: %d\n", status);
        return 1;
    }
    else {
        puts("[SUCCESS] INA3221_TRIGGER_SHUNT_AND_BUS");
    }
    flags = 0;
    while (!(flags & INA3221_FLAG_CONV_READY)) {
        status = ina3221_read_flags(&dev, &flags);
        if (status < 0) {
            printf("[FAILURE] ina3221_read_flags: %d\n", status);
            return 1;
        }
    }
    status = ina3221_read_shunt_uv(&dev,
                                   INA3221_CH1 | INA3221_CH2 | INA3221_CH3,
                                   shunt_uv, &flags);
    if (status != INA3221_NUM_CH) {
        printf("[FAILURE] ina3221_read_shunt_uv (triggered): %d\n", status);
        return 1;
    }
    else {
        puts("[SUCCESS] ina3221_read_shunt_uv (triggered)");
    }

    status = ina3221_read_bus_mv(&dev, INA3221_CH1 | INA3221_CH2 | INA3221_CH3,
                                 bus_mv, &flags);
    if (status != INA3221_NUM_CH) {
        printf("[FAILURE] ina3221_read_bus_mv (triggered): %d\n", status);
        return 1;
    }
    else {
        puts("[SUCCESS] ina3221_read_bus_mv (triggered)");
    }

    int32_t crit_alert_lim = CRIT_ALERT_LIM_UV;
    status = ina3221_set_crit_alert_limit(&dev,
                                          INA3221_CH1 | INA3221_CH2 | INA3221_CH3,
                                          crit_alert_lim);
    if (status != INA3221_NUM_CH) {
        printf("[FAILURE] ina3221_set_crit_alert_limit: %d\n", status);
        return 1;
    }
    else {
        puts("[SUCCESS] ina3221_set_crit_alert_limit");
    }

    int32_t warn_alert_lim = WARN_ALERT_LIM_UV;
    status = ina3221_set_warn_alert_limit(&dev,
                                          INA3221_CH1 | INA3221_CH2 | INA3221_CH3,
                                          warn_alert_lim);
    if (status != INA3221_NUM_CH) {
        printf("[FAILURE] ina3221_set_warn_alert_limit: %d\n", status);
        return 1;
    }
    else {
        puts("[SUCCESS] ina3221_set_warn_alert_limit");
    }

    int32_t pv_lower_lim = PV_LOWER_LIM_MV;
    status = ina3221_set_power_valid_lower_limit(&dev, pv_lower_lim);
    if (status != INA3221_OK) {
        printf("[FAILURE] ina3221_set_power_valid_lower_limit: %d\n", status);
        return 1;
    }
    else {
        puts("[SUCCESS] ina3221_set_power_valid_lower_limit");
    }

    int32_t pv_upper_lim = PV_UPPER_LIM_MV;
    status = ina3221_set_power_valid_upper_limit(&dev, pv_upper_lim);
    if (status != INA3221_OK) {
        printf("[FAILURE] ina3221_set_power_valid_upper_limit: %d\n", status);
        return 1;
    }
    else {
        puts("[SUCCESS] ina3221_set_power_valid_upper_limit");
    }

    int32_t sum_shunt_alert_lim = SUM_SHUNT_ALERT_LIM_UV;
    status =
        ina3221_set_shunt_voltage_sum_alert_limit(&dev, sum_shunt_alert_lim);
    if (status != INA3221_OK) {
        printf("[FAILURE] ina3221_set_shunt_voltage_sum_alert_limit: %d\n",
               status);
        return 1;
    }
    else {
        puts("[SUCCESS] ina3221_set_shunt_voltage_sum_alert_limit");
    }

    status = ina3221_set_mode(&dev, INA3221_MODE_CONTINUOUS_SHUNT_BUS);
    if (status != INA3221_OK) {
        printf("[FAILURE] ina3221_set_mode: %d\n", status);
        return 1;
    }
    char line_buffer[strlen(THEAD) + 1];
    char col_buffer[strlen(COL) + 1];
    while (1) {
        status = ina3221_read_flags(&dev, &flags);
        if (status != INA3221_OK) {
            printf("[FAILURE] ina3221_read_flags: %d\n", status);
            return 1;
        }
        if (!(flags & INA3221_FLAG_CONV_READY)) {
            xtimer_sleep(2);
            continue;
        }
        status = ina3221_read_shunt_uv(&dev,
                                       INA3221_CH1 | INA3221_CH2 | INA3221_CH3,
                                       shunt_uv, NULL);
        if (status != INA3221_NUM_CH) {
            printf("[FAILURE] ina3221_read_shunt_uv: %d\n", status);
            return 1;
        }
        status = ina3221_read_bus_mv(&dev,
                                     INA3221_CH1 | INA3221_CH2 | INA3221_CH3,
                                     bus_mv, NULL);
        if (status != INA3221_NUM_CH) {
            printf("[FAILURE] ina3221_read_bus_mv: %d\n", status);
            return 1;
        }
        status = ina3221_calculate_current_ua(&dev,
                                              INA3221_CH1 | INA3221_CH2 | INA3221_CH3, shunt_uv,
                                              current_ua);
        if (status != INA3221_NUM_CH) {
            printf("[FAILURE] ina3221_calculate_current_ua: %d\n", status);
            return 1;
        }
        status = ina3221_calculate_power_uw(bus_mv, current_ua, INA3221_NUM_CH,
                                            power_uw);
        if (status != INA3221_NUM_CH) {
            printf("[FAILURE] ina3221_calculate_power_uw: %d\n", status);
            return 1;
        }
        puts(THEAD);
        puts(HLINE);
        for (int8_t i = 0; i < INA3221_NUM_CH; i++) {
            char *p = line_buffer;
            strcpy(col_buffer, COL);
            fmt_char(col_buffer, '0' + i);
            p += sprintf(p, "%s|", col_buffer);
            strcpy(col_buffer, COL);
            fmt_s32_dec(col_buffer, shunt_uv[i]);
            p += sprintf(p, "%s|", col_buffer);
            strcpy(col_buffer, COL);
            fmt_s16_dec(col_buffer, bus_mv[i]);
            p += sprintf(p, "%s|", col_buffer);
            strcpy(col_buffer, COL);
            fmt_s32_dec(col_buffer, current_ua[i]);
            p += sprintf(p, "%s|", col_buffer);
            strcpy(col_buffer, COL);
            fmt_s32_dec(col_buffer, power_uw[i]);
            p += sprintf(p, "%s", col_buffer);
            printf("%s\n", line_buffer);
        }
        puts("");
        snprintf(line_buffer, sizeof(line_buffer),
                 "FLAGS: CF1:%d CF2:%d CF3:%d SF:%d WF1:%d WF2:%d WF3:%d PVF:%d TCF:%d CVRF:%d",
                 (flags & INA3221_FLAG_CRIT_ALERT_CH1)  ? 1 : 0,
                 (flags & INA3221_FLAG_CRIT_ALERT_CH2)  ? 1 : 0,
                 (flags & INA3221_FLAG_CRIT_ALERT_CH3)  ? 1 : 0,
                 (flags & INA3221_FLAG_SHUNT_SUM_ALERT) ? 1 : 0,
                 (flags & INA3221_FLAG_WARN_ALERT_CH1)  ? 1 : 0,
                 (flags & INA3221_FLAG_WARN_ALERT_CH2)  ? 1 : 0,
                 (flags & INA3221_FLAG_WARN_ALERT_CH3)  ? 1 : 0,
                 (flags & INA3221_FLAG_POWER_VALID)     ? 1 : 0,
                 (flags & INA3221_FLAG_TIMING_CONTROL)  ? 1 : 0,
                 (flags & INA3221_FLAG_CONV_READY)      ? 1 : 0);
        printf("%s\n", line_buffer);
        puts(HLINE);
    }
    return 0;
}

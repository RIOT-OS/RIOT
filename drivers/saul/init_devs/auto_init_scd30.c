/*
 * SPDX-FileCopyrightText: 2020 Puhang Ding
 * SPDX-FileCopyrightText: 2020 Nishchay Agrawal
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of SCD30 driver.
 *
 * @author      Puhang Ding      <czarsir@gmail.com>
 * @author      Nishchay Agrawal <f2016088@pilani.bits-pilani.ac.in>
 *
 * @}
 */

#include "saul_reg.h"
#include "log.h"

#include "scd30.h"
#include "scd30_params.h"
#include "scd30_internal.h"

/**
 * @brief   Allocation of memory for device descriptors
 */
static scd30_t scd30_devs[SCD30_NUMOF];

/**
 * @brief   Reference the driver structs.
 * @{
 */
extern const saul_driver_t scd30_co2_saul_driver;
extern const saul_driver_t scd30_temp_saul_driver;
extern const saul_driver_t scd30_hum_saul_driver;
/** @} */

/**
 * @brief   Memory for the SAUL registry entries
 */
#define SENSORS_NUMOF 3
static saul_reg_t saul_entries[SCD30_NUMOF * SENSORS_NUMOF];

void auto_init_scd30(void)
{
    size_t sensor_no = 0;

    for (size_t i = 0; i < SCD30_NUMOF; i++) {

        int res = scd30_init(&scd30_devs[i], &scd30_params[i]);
        if (res < 0) {
            LOG_ERROR("[auto_init_saul] error initializing SCD30 #%u\n", i);
            continue;
        }

        /*** CO2 concentration ***/
        saul_entries[sensor_no].dev = &scd30_devs[i];
        saul_entries[sensor_no].name = scd30_saul_info[i].name;
        saul_entries[sensor_no].driver = &scd30_co2_saul_driver;
        saul_reg_add(&saul_entries[sensor_no]);
        sensor_no++;

        /*** Temperature ***/
        saul_entries[sensor_no].dev = &scd30_devs[i];
        saul_entries[sensor_no].name = scd30_saul_info[i].name;
        saul_entries[sensor_no].driver = &scd30_temp_saul_driver;
        saul_reg_add(&saul_entries[sensor_no]);
        sensor_no++;

        /*** Relative Humidity ***/
        saul_entries[sensor_no].dev = &scd30_devs[i];
        saul_entries[sensor_no].name = scd30_saul_info[i].name;
        saul_entries[sensor_no].driver = &scd30_hum_saul_driver;
        saul_reg_add(&saul_entries[sensor_no]);
        sensor_no++;

        scd30_set_param(&scd30_devs[i], SCD30_START, SCD30_DEF_PRESSURE);
    }
}

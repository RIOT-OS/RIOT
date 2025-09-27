/*
 * SPDX-FileCopyrightText: 2019 Dirk Ehmen
 * SPDX-FileCopyrightText: 2020 Nishchay Agrawal
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of SDP3X driver.
 *
 * @author      Dirk Ehmen       <ehmen@ibr.cs.tu-bs.de>
 * @author      Nishchay Agrawal <f2016088@pilani.bits-pilani.ac.in>
 *
 * @}
 */

#include <stdio.h>
#include "log.h"
#include "saul_reg.h"

#include "sdp3x_params.h"
#include "sdp3x.h"

/**
 * @brief   Allocation of memory for device descriptors
 */
static sdp3x_t sdp3x_devs[SDP3X_NUMOF];

/**
 * @brief   Reference the driver structs.
 * @{
 */
extern const saul_driver_t sdp3x_temperature_saul_driver;
extern const saul_driver_t sdp3x_differential_pressure_saul_driver;
/** @} */

/**
 * @brief   Memory for the SAUL registry entries
 */
#define SENSORS_NUMOF 2
static saul_reg_t saul_entries[SDP3X_NUMOF * SENSORS_NUMOF];

void auto_init_sdp3x(void)
{
    size_t se_ix = 0;

    for (size_t i = 0; i < SDP3X_NUMOF; i++) {
        int res = sdp3x_init(&sdp3x_devs[i], &sdp3x_params[i]);
        if (res < 0) {
            LOG_ERROR("[auto_init_saul] error initializing SDP3X #%u\n", i);
            continue;
        }

        /* temperature */
        saul_entries[se_ix].dev = &sdp3x_devs[i];
        saul_entries[se_ix].name = sdp3x_saul_info[i].name;
        saul_entries[se_ix].driver = &sdp3x_temperature_saul_driver;
        saul_reg_add(&saul_entries[se_ix]);
        se_ix++;

        /* differential pressure */
        saul_entries[se_ix].dev = &sdp3x_devs[i];
        saul_entries[se_ix].name = sdp3x_saul_info[i].name;
        saul_entries[se_ix].driver = &sdp3x_differential_pressure_saul_driver;
        saul_reg_add(&saul_entries[se_ix]);
        se_ix++;
    }
}

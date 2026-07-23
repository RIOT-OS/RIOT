/*
 * SPDX-FileCopyrightText: 2026 RIOT Contributors
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include <assert.h>
#include <stdio.h>

#include "saul.h"
#include "saul_reg.h"
#include "sht4x.h"
#include "sht4x_params.h"

#define SHT4X_NUM      ARRAY_SIZE(sht4x_params)
extern sht4x_dev_t sht4x_devs[SHT4X_NUM];

static saul_reg_t saul_entries[SHT4X_NUM * 2];

extern const saul_driver_t sht4x_saul_driver_temperature;
extern const saul_driver_t sht4x_saul_driver_humidity;

void auto_init_sht4x(void)
{
    assert(SHT4X_INFO_NUM == SHT4X_NUM);

    for (unsigned i = 0; i < SHT4X_NUM; i++) {
        if (sht4x_init(&sht4x_devs[i], &sht4x_params[i]) != SHT4X_OK) {
            LOG_ERROR("[auto_init_saul] error initializing sht4x #%u\n", i);
            continue;
        }

        saul_entries[(i * 2)].dev = &sht4x_devs[i];
        saul_entries[(i * 2)].name = sht4x_saul_info[i].name;
        saul_entries[(i * 2)].driver = &sht4x_saul_driver_temperature;

        saul_entries[(i * 2) + 1].dev = &sht4x_devs[i];
        saul_entries[(i * 2) + 1].name = sht4x_saul_info[i].name;
        saul_entries[(i * 2) + 1].driver = &sht4x_saul_driver_humidity;
    }

    saul_register(saul_entries, ARRAY_SIZE(saul_entries));
}

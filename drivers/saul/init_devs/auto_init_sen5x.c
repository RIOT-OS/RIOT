/*
 * SPDX-FileCopyrightText: 2023 TU Braunschweig Institut für Betriebssysteme und Rechnerverbund
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of sen5x device driver.
 *
 * @author      Daniel Prigoshij <prigoshi@ibr.cs.tu-bs.de>
 *
 * @}
 */

#include "assert.h"
#include "log.h"
#include "saul_reg.h"
#include "sen5x.h"
#include "sen5x_params.h"

/**
 * @brief   Allocation of memory for device descriptors
 */
static sen5x_t sen5x_devs[SEN5X_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[SEN5X_NUM * 12];

/**
 * @brief   Define the number of saul info
 */
#define SEN5X_INFO_NUM    ARRAY_SIZE(sen5x_saul_info)

/**
 * @name    Reference the driver structs.
 * @{
 */
extern const saul_driver_t sen5x_mass_concentration_pm1p0_driver;
extern const saul_driver_t sen5x_mass_concentration_pm2p5_driver;
extern const saul_driver_t sen5x_mass_concentration_pm4p0_driver;
extern const saul_driver_t sen5x_mass_concentration_pm10p0_driver;
extern const saul_driver_t sen5x_number_concentration_pm0p5_driver;
extern const saul_driver_t sen5x_number_concentration_pm1p0_driver;
extern const saul_driver_t sen5x_number_concentration_pm2p5_driver;
extern const saul_driver_t sen5x_number_concentration_pm4p0_driver;
extern const saul_driver_t sen5x_number_concentration_pm10p0_driver;
extern const saul_driver_t sen5x_typical_particle_size_driver;
extern const saul_driver_t sen5x_ambient_humidity_driver;
extern const saul_driver_t sen5x_ambient_temperature_driver;
/** @} */

void auto_init_sen5x(void)
{
    assert(SEN5X_INFO_NUM == SEN5X_NUM);

    for (unsigned i = 0; i < SEN5X_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing SEN5X #%u\n", i);

        if (sen5x_init(&sen5x_devs[i], &sen5x_params[i]) != 0) {
            LOG_ERROR("[auto_init_saul] error initializing SEN5X #%u\n", i);
            continue;
        }

        sen5x_wake(&sen5x_devs[i]);

        /* Mass Concentration pm1p0 */
        saul_entries[(i * 12)].dev = &(sen5x_devs[i]);
        saul_entries[(i * 12)].name = sen5x_saul_info[i].name;
        saul_entries[(i * 12)].driver = &sen5x_mass_concentration_pm1p0_driver;

        /* Mass Concentration pm2p5 */
        saul_entries[(i * 12) + 1].dev = &(sen5x_devs[i]);
        saul_entries[(i * 12) + 1].name = sen5x_saul_info[i].name;
        saul_entries[(i * 12) + 1].driver = &sen5x_mass_concentration_pm2p5_driver;

        /* Mass Concentration pm4p0 */
        saul_entries[(i * 12) + 2].dev = &(sen5x_devs[i]);
        saul_entries[(i * 12) + 2].name = sen5x_saul_info[i].name;
        saul_entries[(i * 12) + 2].driver = &sen5x_mass_concentration_pm4p0_driver;

        /* Mass Concentration pm10p0 */
        saul_entries[(i * 12) + 3].dev = &(sen5x_devs[i]);
        saul_entries[(i * 12) + 3].name = sen5x_saul_info[i].name;
        saul_entries[(i * 12) + 3].driver = &sen5x_mass_concentration_pm10p0_driver;

        /* Number Concentration pm0p5 */
        saul_entries[(i * 12) + 4].dev = &(sen5x_devs[i]);
        saul_entries[(i * 12) + 4].name = sen5x_saul_info[i].name;
        saul_entries[(i * 12) + 4].driver = &sen5x_number_concentration_pm0p5_driver;

        /* Number Concentration pm1p0 */
        saul_entries[(i * 12) + 5].dev = &(sen5x_devs[i]);
        saul_entries[(i * 12) + 5].name = sen5x_saul_info[i].name;
        saul_entries[(i * 12) + 5].driver = &sen5x_number_concentration_pm1p0_driver;

        /* Number Concentration pm2p5 */
        saul_entries[(i * 12) + 6].dev = &(sen5x_devs[i]);
        saul_entries[(i * 12) + 6].name = sen5x_saul_info[i].name;
        saul_entries[(i * 12) + 6].driver = &sen5x_number_concentration_pm2p5_driver;

        /* Number Concentration pm4p0 */
        saul_entries[(i * 12) + 7].dev = &(sen5x_devs[i]);
        saul_entries[(i * 12) + 7].name = sen5x_saul_info[i].name;
        saul_entries[(i * 12) + 7].driver = &sen5x_number_concentration_pm4p0_driver;

        /* Number Concentration pm10p0 */
        saul_entries[(i * 12) + 8].dev = &(sen5x_devs[i]);
        saul_entries[(i * 12) + 8].name = sen5x_saul_info[i].name;
        saul_entries[(i * 12) + 8].driver = &sen5x_number_concentration_pm10p0_driver;

        /* Typical particle size */
        saul_entries[(i * 12) + 9].dev = &(sen5x_devs[i]);
        saul_entries[(i * 12) + 9].name = sen5x_saul_info[i].name;
        saul_entries[(i * 12) + 9].driver = &sen5x_typical_particle_size_driver;

        /* Ambient humidity */
        saul_entries[(i * 12) + 10].dev = &(sen5x_devs[i]);
        saul_entries[(i * 12) + 10].name = sen5x_saul_info[i].name;
        saul_entries[(i * 12) + 10].driver = &sen5x_ambient_humidity_driver;

        /* Ambient temperature */
        saul_entries[(i * 12) + 11].dev = &(sen5x_devs[i]);
        saul_entries[(i * 12) + 11].name = sen5x_saul_info[i].name;
        saul_entries[(i * 12) + 11].driver = &sen5x_ambient_temperature_driver;

        /* Add register entries to saul */
        for (unsigned int j = 0; j < 12; j++) {
            saul_reg_add(&(saul_entries[(i * 4) + j]));
        }
    }
}

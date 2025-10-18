/*
 * SPDX-FileCopyrightText: 2025 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_hall_effect
 * @{
 *
 * @file
 * @brief       SAUL adaption for generic hall effect sensor
 *              to measure the number as well as rate of rotation.
 *
 * @author      Leonard Herbst <leonard.herbst@tu-dresden.de>
 * @}
 */

#include "log.h"
#include "saul_reg.h"
#include "hall_effect_params.h"

/**
 * @brief Define the number of configured hall effect sensors
 */
#define HALL_EFFECT_NUM    ARRAY_SIZE(hall_effect_params)

/**
 * @brief Memory for the device descriptors
 */
static hall_effect_t hall_effect_devs[HALL_EFFECT_NUM];

/**
 * @brief Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[HALL_EFFECT_NUM * 2];

extern saul_driver_t hall_effect_rpm_saul_driver;
extern saul_driver_t hall_pulse_count_saul_driver;

void auto_init_hall_effect(void)
{
    for (unsigned i = 0; i < HALL_EFFECT_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing hall_effect #%u\n", i);

        int res = hall_effect_init(&hall_effect_devs[i], &hall_effect_params[i]);
        if (res != 0) {
            LOG_ERROR("[auto_init_saul] error initializing hall_effect #%u\n", i);
            continue;
        }

        /* RPM */
        saul_entries[(i * 2)].dev = &(hall_effect_devs[i]);
        saul_entries[(i * 2)].name = hall_effect_saul_info[i][0].name;
        saul_entries[(i * 2)].driver = &hall_effect_rpm_saul_driver;

        /* Pulse Count */
        saul_entries[(i * 2) + 1].dev = &(hall_effect_devs[i]);
        saul_entries[(i * 2) + 1].name = hall_effect_saul_info[i][1].name;
        saul_entries[(i * 2) + 1].driver = &hall_pulse_count_saul_driver;

        saul_reg_add(&(saul_entries[(i * 2)]));
        saul_reg_add(&(saul_entries[(i * 2) + 1]));
    }
}

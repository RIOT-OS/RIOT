/*
 * SPDX-FileCopyrightText: 2025 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_inc_encoder
 * @{
 *
 * @file
 * @brief       SAUL adaption for generic incremental rotary encoder
 *              to measure the number as well as rate of rotation.
 *
 * @author      Leonard Herbst <leonard.herbst@tu-dresden.de>
 * @}
 */

#include "log.h"
#include "saul_reg.h"
#include "inc_encoder_params.h"

/**
 * @brief Define the number of configured incremental rotary encoders
 */
#define INC_ENCODER_NUM    ARRAY_SIZE(inc_encoder_params)

/**
 * @brief Memory for the device descriptors
 */
static inc_encoder_t inc_encoder_devs[INC_ENCODER_NUM];

/**
 * @brief Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[INC_ENCODER_NUM * 2];

extern saul_driver_t inc_encoder_rpm_saul_driver;
extern saul_driver_t inc_encoder_rev_count_saul_driver;

void auto_init_inc_encoder(void)
{
    for (unsigned i = 0; i < INC_ENCODER_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing inc_encoder #%u\n", i);

        int res = inc_encoder_init(&inc_encoder_devs[i], &inc_encoder_params[i]);
        if (res != 0) {
            LOG_ERROR("[auto_init_saul] error initializing inc_encoder #%u\n", i);
            continue;
        }

        /* RPM */
        saul_entries[(i * 2)].dev = &(inc_encoder_devs[i]);
        saul_entries[(i * 2)].name = inc_encoder_saul_info[i][0].name;
        saul_entries[(i * 2)].driver = &inc_encoder_rpm_saul_driver;

        /* Rev Count */
        saul_entries[(i * 2) + 1].dev = &(inc_encoder_devs[i]);
        saul_entries[(i * 2) + 1].name = inc_encoder_saul_info[i][1].name;
        saul_entries[(i * 2) + 1].driver = &inc_encoder_rev_count_saul_driver;

        saul_reg_add(&(saul_entries[(i * 2)]));
        saul_reg_add(&(saul_entries[(i * 2) + 1]));
    }
}

/*
 * SPDX-FileCopyrightText: 2017 UC Berkeley
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/*
 * @ingroup     sys_auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization for PULSE_COUNTER devices
 *
 * @author      Hyung-Sin Kim <hs.kim@cs.berkeley.edu>
 *
 * @}
 */

#include "assert.h"
#include "log.h"
#include "saul_reg.h"
#include "pulse_counter_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define PULSE_COUNTER_NUM    ARRAY_SIZE(pulse_counter_params)

/**
 * @brief   Allocate memory for the device descriptors
 */
static pulse_counter_t pulse_counter_devs[PULSE_COUNTER_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[PULSE_COUNTER_NUM];

/**
 * @brief   Define the number of configured sensors
 */
#define PULSE_COUNTER_INFO_NUM    ARRAY_SIZE(pulse_counter_saul_info)

/**
 * @brief   Reference the driver struct
 */
extern saul_driver_t pulse_counter_saul_driver;

void auto_init_pulse_counter(void)
{
    assert(PULSE_COUNTER_NUM == PULSE_COUNTER_INFO_NUM);
    for (unsigned i = 0; i < PULSE_COUNTER_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing pulse_counter #%u\n", i);

        int res = pulse_counter_init(&pulse_counter_devs[i], &pulse_counter_params[i]);
        if (res != 0) {
            LOG_ERROR("[auto_init_saul] error initializing pulse_counter #%u\n", i);
        }
        else {
            saul_entries[i].dev = &(pulse_counter_devs[i]);
            saul_entries[i].name = pulse_counter_saul_info[i].name;
            saul_entries[i].driver = &pulse_counter_saul_driver;
            saul_reg_add(&(saul_entries[i]));
        }
    }
}

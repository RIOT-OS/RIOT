/*
 * Copyright (C) 2017 UC Berkeley
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
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

#ifdef MODULE_PULSE_COUNTER

#include "log.h"
#include "saul_reg.h"
#include "pulse_counter_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define PULSE_COUNTER_NUM    (sizeof(pulse_counter_params) / sizeof(pulse_counter_params[0]))

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
#define PULSE_COUNTER_INFO_NUM    (sizeof(pulse_counter_saul_info) / sizeof(pulse_counter_saul_info[0]))

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

#else
typedef int dont_be_pedantic;
#endif /* MODULE_PULSE_COUNTER */

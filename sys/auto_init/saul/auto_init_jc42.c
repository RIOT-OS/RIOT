/*
 * Copyright (C) 2017 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of jc42 compatible driver.
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#ifdef MODULE_JC42

#include "assert.h"
#include "log.h"
#include "saul_reg.h"
#include "jc42.h"
#include "jc42_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define JC42_NUM      (sizeof(jc42_params) / sizeof(jc42_params[0]))

/**
 * @brief   Allocation of memory for device descriptors
 */
static jc42_t jc42_devs[JC42_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[JC42_NUM];

/**
 * @brief   Define the number of saul info
 */
#define JC42_INFO_NUM    (sizeof(jc42_saul_info) / sizeof(jc42_saul_info[0]))

/**
 * @brief   Reference the driver structs.
 */
extern const saul_driver_t jc42_temperature_saul_driver;

void auto_init_jc42(void)
{
    assert(JC42_NUM == JC42_INFO_NUM);

    for (unsigned i = 0; i < JC42_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing jc42 #%u\n", i);

        if (jc42_init(&jc42_devs[i], &jc42_params[i]) < 0) {
            LOG_ERROR("[auto_init_saul] error initializing jc42 #%u\n", i);
            continue;
        }

        /* temperature */
        saul_entries[i].dev = &(jc42_devs[i]);
        saul_entries[i].name = jc42_saul_info[i].name;
        saul_entries[i].driver = &jc42_temperature_saul_driver;

        /* register to saul */
        saul_reg_add(&(saul_entries[i]));
    }
}
#else
typedef int dont_be_pedantic;
#endif /* MODULE_JC42 */

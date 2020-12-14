/*
 * Copyright (C) 2020 iosabi
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     sys_auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of SI1133 driver.
 *
 * @author      iosabi <iosabi@protonmail.com>
 *
 * @}
 */

#ifdef MODULE_SI1133

#include "assert.h"
#include "log.h"
#include "saul_reg.h"
#include "si1133.h"
#include "si1133_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define SI1133_NUMOF    ARRAY_SIZE(si1133_params)

/**
 * @brief   Allocation of memory for device descriptors
 */
static si1133_t si1133_devs[SI1133_NUMOF];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[SI1133_NUMOF * 4];

/**
 * @brief   Define the number of saul info
 */
#define SI1133_INFO_NUMOF    ARRAY_SIZE(si1133_saul_reg_info)

/**
 * @name    Reference the driver structs
 * @{
 */
extern const saul_driver_t si1133_uv_saul_driver;
extern const saul_driver_t si1133_ir_saul_driver;
extern const saul_driver_t si1133_visible_saul_driver;
/** @} */

void auto_init_si1133(void)
{
    assert(SI1133_INFO_NUMOF == SI1133_NUMOF);
    unsigned entry = 0;
    for (unsigned i = 0; i < SI1133_NUMOF; i++) {
        LOG_DEBUG("[auto_init_saul] initializing SI1133 #%u\n", i);

        si1133_ret_code_t ret = si1133_init(&si1133_devs[i], &si1133_params[i]);
        if (ret != SI1133_OK) {
            LOG_ERROR("[auto_init_saul] error initializing SI1133 #%u: %d\n",
                      i, (int)ret);
            continue;
        }

        /* UV index */
        saul_entries[entry].dev = &si1133_devs[i];
        saul_entries[entry].name = si1133_saul_reg_info[i].name;
        saul_entries[entry].driver = &si1133_uv_saul_driver;
        saul_reg_add(&saul_entries[entry++]);

        /* Infra red */
        saul_entries[entry].dev = &si1133_devs[i];
        saul_entries[entry].name = si1133_saul_reg_info[i].name;
        saul_entries[entry].driver = &si1133_ir_saul_driver;
        saul_reg_add(&saul_entries[entry++]);

        /* Visible */
        saul_entries[entry].dev = &si1133_devs[i];
        saul_entries[entry].name = si1133_saul_reg_info[i].name;
        saul_entries[entry].driver = &si1133_visible_saul_driver;
        saul_reg_add(&saul_entries[entry]);
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_SI1133 */

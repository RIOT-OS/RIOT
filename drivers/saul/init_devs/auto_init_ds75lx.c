/*
 * Copyright (C) 2019 Inria
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
 * @brief       Auto initialization of DS75LX driver.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include "assert.h"
#include "log.h"
#include "saul_reg.h"
#include "ds75lx.h"
#include "ds75lx_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define DS75LX_NUM      ARRAY_SIZE(ds75lx_params)

/**
 * @brief   Allocation of memory for device descriptors
 */
static ds75lx_t ds75lx_devs[DS75LX_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[DS75LX_NUM];

/**
 * @brief   Define the number of saul info
 */
#define DS75LX_INFO_NUM ARRAY_SIZE(ds75lx_saul_info)

/**
 * @name    Reference the driver structs.
 * @{
 */
extern const saul_driver_t ds75lx_temperature_saul_driver;
/** @} */

void auto_init_ds75lx(void)
{
    assert(DS75LX_INFO_NUM == DS75LX_NUM);

    for (unsigned i = 0; i < DS75LX_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing ds75lx #%u\n", i);

        if (ds75lx_init(&ds75lx_devs[i],
                        &ds75lx_params[i]) != DS75LX_OK) {
            LOG_ERROR("[auto_init_saul] error initializing ds75lx #%u\n", i);
            continue;
        }

        saul_entries[i].dev = &(ds75lx_devs[i]);
        saul_entries[i].name = ds75lx_saul_info[i].name;
        saul_entries[i].driver = &ds75lx_temperature_saul_driver;

        /* register to saul */
        saul_reg_add(&(saul_entries[i]));
    }
}

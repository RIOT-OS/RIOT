/*
 * Copyright (C) 2019 University of Applied Sciences Emden / Leer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     sys_auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of Atlas Scientific pH OEM sensor
 *
 * @author      igor.knippenberg@gmail.com
 *
 * @}
 */

#include "assert.h"
#include "log.h"

#include "saul_reg.h"
#include "ph_oem.h"
#include "ph_oem_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define PH_OEM_NUM   (sizeof(ph_oem_params) / sizeof(ph_oem_params[0]))

/**
 * @brief   Allocate memory for the device descriptors
 */
static ph_oem_t ph_oem_devs[PH_OEM_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[PH_OEM_NUM];

/**
 * @brief   Define the number of saul info
 */
#define PH_OEM_INFO_NUM (sizeof(ph_oem_saul_info) / sizeof(ph_oem_saul_info[0]))

/**
 * @brief   Reference the driver struct
 */
extern saul_driver_t ph_oem_saul_driver;

void auto_init_ph_oem(void)
{
    assert(PH_OEM_INFO_NUM == PH_OEM_NUM);

    for (unsigned i = 0; i < PH_OEM_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing ph_oem #%d\n", i);
        if (ph_oem_init(&ph_oem_devs[i], &ph_oem_params[i]) < 0) {
            LOG_ERROR("[auto_init_saul] error initializing ph_oem #%d\n", i);
            continue;
        }

        saul_entries[i].dev = &(ph_oem_devs[i]);
        saul_entries[i].name = ph_oem_saul_info[i].name;
        saul_entries[i].driver = &ph_oem_saul_driver;
        saul_reg_add(&(saul_entries[i]));
    }
}

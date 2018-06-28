/*
 * Copyright (C) 2016 Eistec AB
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
 * @brief       Auto initialization of LIS3DH accelerometers
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#ifdef MODULE_LIS3DH

#include "log.h"
#include "saul_reg.h"
#include "lis3dh.h"
#include "lis3dh_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define LIS3DH_NUM    (sizeof(lis3dh_params) / sizeof(lis3dh_params[0]))

/**
 * @brief   Allocate memory for the device descriptors
 */
static lis3dh_t lis3dh_devs[LIS3DH_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[LIS3DH_NUM];

/**
 * @brief   Define the number of saul info
 */
#define LIS3DH_INFO_NUM    (sizeof(lis3dh_saul_info) / sizeof(lis3dh_saul_info[0]))

/**
 * @brief   Reference the driver struct
 */
extern saul_driver_t lis3dh_saul_driver;


void auto_init_lis3dh(void)
{
    assert(LIS3DH_NUM == LIS3DH_INFO_NUM);

    for (unsigned int i = 0; i < LIS3DH_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing lis3dh #%u\n", i);

        if (lis3dh_init(&lis3dh_devs[i], &lis3dh_params[i]) < 0) {
            LOG_ERROR("[auto_init_saul] error initializing lis3dh #%u\n", i);
            continue;
        }

        if (lis3dh_set_odr(&lis3dh_devs[i], lis3dh_params[i].odr) < 0) {
            LOG_ERROR("[auto_init_saul] error setting ODR for lis3dh #%u\n", i);
            continue;
        }

        saul_entries[i].dev = &(lis3dh_devs[i]);
        saul_entries[i].name = lis3dh_saul_info[i].name;
        saul_entries[i].driver = &lis3dh_saul_driver;
        saul_reg_add(&(saul_entries[i]));
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_LIS3DH */

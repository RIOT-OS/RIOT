/*
 * Copyright (C) 2016 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     auto_init_saul
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

#include "saul_reg.h"
#include "lis3dh.h"
#include "lis3dh_params.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief   Define the number of configured sensors
 */
#define LIS3DH_NUM    (sizeof(lis3dh_params)/sizeof(lis3dh_params[0]))

/**
 * @brief   Allocate memory for the device descriptors
 */
static lis3dh_t lis3dh_devs[LIS3DH_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[LIS3DH_NUM];

/**
 * @brief   Reference the driver struct
 */
extern saul_driver_t lis3dh_saul_driver;


void auto_init_lis3dh(void)
{
    for (int i = 0; i < LIS3DH_NUM; i++) {
        const lis3dh_params_t *p = &lis3dh_params[i];
        int res;

        DEBUG("[auto_init_saul] initializing lis3dh accelerometer\n");
        res = lis3dh_init(&lis3dh_devs[i], p->spi, p->cs, p->scale);
        if (res < 0) {
            DEBUG("[auto_init_saul] error during lis3dh_init\n");
            continue;
        }
        res = lis3dh_set_odr(&lis3dh_devs[i], p->odr);
        if (res < 0) {
            DEBUG("[auto_init_saul] error during lis3dh_set_odr\n");
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

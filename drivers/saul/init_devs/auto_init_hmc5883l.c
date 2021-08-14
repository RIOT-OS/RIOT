/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/*
 * @ingroup     sys_auto_init_saul
 * @brief       Auto initialization of HMC5883L 3-axis digital compass
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 */

#include "assert.h"
#include "log.h"
#include "saul_reg.h"

#include "hmc5883l.h"
#include "hmc5883l_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define HMC5883L_NUM    ARRAY_SIZE(hmc5883l_params)

/**
 * @brief   Allocate memory for the device descriptors
 */
static hmc5883l_t hmc5883l_devs[HMC5883L_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[HMC5883L_NUM];

/**
 * @brief   Define the number of saul info
 */
#define HMC5883L_INFO_NUM    ARRAY_SIZE(hmc5883l_saul_info)

/**
 * @brief   Reference the driver struct
 */
extern saul_driver_t hmc5883l_saul_driver;

void auto_init_hmc5883l(void)
{
    assert(HMC5883L_NUM == HMC5883L_INFO_NUM);

    for (unsigned int i = 0; i < HMC5883L_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing hmc5883l #%u\n", i);

        if (hmc5883l_init(&hmc5883l_devs[i], &hmc5883l_params[i]) < 0) {
            LOG_ERROR("[auto_init_saul] error initializing hmc5883l #%u\n", i);
            continue;
        }

        saul_entries[i].dev = &(hmc5883l_devs[i]);
        saul_entries[i].name = hmc5883l_saul_info[i].name;
        saul_entries[i].driver = &hmc5883l_saul_driver;
        saul_reg_add(&(saul_entries[i]));
    }
}

/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_vcnl40x0
 * @ingroup     sys_auto_init_saul
 * @brief       Auto initialization of Vishay VCNL40X0 proximity and ambient light sensor
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#ifdef MODULE_VCNL40X0

#include "assert.h"
#include "log.h"
#include "saul_reg.h"
#include "vcnl40x0.h"
#include "vcnl40x0_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define VCNL40X0_NUM    (sizeof(vcnl40x0_params) / sizeof(vcnl40x0_params[0]))

/**
 * @brief   Allocate memory for the device descriptors
 */
static vcnl40x0_t vcnl40x0_devs[VCNL40X0_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[VCNL40X0_NUM];

/**
 * @brief   Define the number of saul info
 */
#define VCNL40X0_INFO_NUM    (sizeof(vcnl40x0_saul_info) / sizeof(vcnl40x0_saul_info[0]))

/**
 * @name    Reference the driver structs
 * @{
 */
extern saul_driver_t vcnl40x0_saul_als_driver;
/** @} */

void auto_init_vcnl40x0(void)
{
    assert(VCNL40X0_NUM == VCNL40X0_INFO_NUM);

    for (unsigned int i = 0; i < VCNL40X0_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing vcnl40x0 #%u\n", i);

        if (vcnl40x0_init(&vcnl40x0_devs[i], &vcnl40x0_params[i]) < 0) {
            LOG_ERROR("[auto_init_saul] error initializing vcnl40x0 #%u\n", i);
            continue;
        }

        saul_entries[i].dev = &(vcnl40x0_devs[i]);
        saul_entries[i].name = vcnl40x0_saul_info[i].name;
        saul_entries[i].driver = &vcnl40x0_saul_als_driver;
        saul_reg_add(&(saul_entries[i]));
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_VCNL40X0 */

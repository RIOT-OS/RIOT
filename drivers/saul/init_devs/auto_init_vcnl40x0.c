/*
 * SPDX-FileCopyrightText: 2017 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of VCNL40X0 driver.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include "log.h"
#include "saul_reg.h"

#include "vcnl40x0_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define VCNL40X0_NUMOF    ARRAY_SIZE(vcnl40x0_params)

/**
 * @brief   Allocation of memory for device descriptors
 */
static vcnl40x0_t vcnl40x0_devs[VCNL40X0_NUMOF];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[VCNL40X0_NUMOF * 2];

/**
 * @brief   Reference the driver structs.
 * @{
 */
extern const saul_driver_t vcnl40x0_proximity_saul_driver;
extern const saul_driver_t vcnl40x0_illuminance_saul_driver;
/** @} */

void auto_init_vcnl40x0(void)
{
    for (unsigned i = 0; i < VCNL40X0_NUMOF; i++) {
        LOG_DEBUG("[auto_init_saul] initializing vcnl40x0 #%u\n", i);

        if (vcnl40x0_init(&vcnl40x0_devs[i],
                          &vcnl40x0_params[i]) != VCNL40X0_OK) {
            LOG_ERROR("[auto_init_saul] error initializing vcnl40x0 #%u\n", i);
            return;
        }

        /* proximity */
        saul_entries[(i * 2)].dev = &(vcnl40x0_devs[i]);
        saul_entries[(i * 2)].name = vcnl40x0_saul_reg_info[i].name;
        saul_entries[(i * 2)].driver = &vcnl40x0_proximity_saul_driver;

        /* illuminance */
        saul_entries[(i * 2) + 1].dev = &(vcnl40x0_devs[i]);
        saul_entries[(i * 2) + 1].name = vcnl40x0_saul_reg_info[i].name;
        saul_entries[(i * 2) + 1].driver = &vcnl40x0_illuminance_saul_driver;

        /* register to saul */
        saul_reg_add(&(saul_entries[(i * 2)]));
        saul_reg_add(&(saul_entries[(i * 2) + 1]));
    }
}

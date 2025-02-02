/*
 * SPDX-FileCopyrightText: 2025 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of GP8XXX driver.
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include <assert.h>

#include "log.h"
#include "saul_reg.h"
#include "gp8xxx.h"
#include "gp8xxx_params.h"

/**
 * @brief   Define the number of configured actuators
 */
#define GP8XXX_NUM          ARRAY_SIZE(gp8xxx_params)

/**
 * @brief   Define the number of configured SAUL actuators
 */
#define GP8XXX_SAUL_NUM     ARRAY_SIZE(gp8xxx_saul_info)

/**
 * @brief   Allocation of memory for device descriptors
 */
gp8xxx_t gp8xxx_devs[GP8XXX_NUM];

/**
 * @brief   Allocation of memory for SAUL device descriptors
 */
static gp8xxx_saul_t gp8xxx_saul_devs[GP8XXX_SAUL_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[GP8XXX_SAUL_NUM];

/**
 * @name    Reference the driver structs.
 */
#if GP8XXX_HAS_VDAC
extern const saul_driver_t gp8xxx_voltage_saul_driver;
#endif
#if GP8XXX_HAS_IDAC
extern const saul_driver_t gp8xxx_current_saul_driver;
#endif

void auto_init_gp8xxx(void)
{
    unsigned entry = 0;

    for (unsigned i = 0; i < GP8XXX_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing GP8XXX #%u\n", i);

        if (gp8xxx_init(&gp8xxx_devs[i], &gp8xxx_params[i]) != 0) {
            LOG_ERROR("[auto_init_saul] error initializing GP8XXX #%i\n", i);
            continue;
        }

        for (unsigned j = 0; j < gp8xxx_params[i].info->channels; j++) {
            assert(entry < GP8XXX_SAUL_NUM);

            gp8xxx_saul_devs[entry].dev = &gp8xxx_devs[i];
            gp8xxx_saul_devs[entry].channel = j;

            saul_entries[entry].dev = &gp8xxx_saul_devs[entry];
            saul_entries[entry].name = gp8xxx_saul_info[entry].name;

#if GP8XXX_HAS_VDAC
            if (gp8xxx_params[i].info->type == GP8XXX_INFO_TYPE_VDAC) {
                saul_entries[entry].driver = &gp8xxx_voltage_saul_driver;
            }
#endif
#if GP8XXX_HAS_IDAC
            if (gp8xxx_params[i].info->type == GP8XXX_INFO_TYPE_IDAC) {
                saul_entries[entry].driver = &gp8xxx_current_saul_driver;
            }
#endif

            saul_reg_add(&saul_entries[entry++]);
        }
    }
}

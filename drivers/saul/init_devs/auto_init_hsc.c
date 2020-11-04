/*
 * Copyright (C) 2020 Deutsches Zentrum für Luft- und Raumfahrt e.V. (DLR)
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
 * @brief       Auto initialization of HSC driver.
 *
 * @author      Quang Pham <phhr_quang@live.com>
 *
 * @}
 */

#ifdef MODULE_HSC

#include "assert.h"
#include "log.h"
#include "saul_reg.h"
#include "hsc.h"
#include "hsc_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define HSC_NUM      ARRAY_SIZE(hsc_params)

/**
 * @brief   Allocation of memory for device descriptors
 */
static hsc_t hsc_devs[HSC_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[HSC_NUM * 2];

/**
 * @brief   Define the number of saul info
 */
#define HSC_INFO_NUM ARRAY_SIZE(hsc_saul_info)

/**
 * @name    Reference the driver structs.
 * @{
 */
extern const saul_driver_t hsc_temperature_saul_driver;
extern const saul_driver_t hsc_pressure_saul_driver;
/** @} */

void auto_init_hsc(void)
{
    static_assert(HSC_INFO_NUM == HSC_NUM, "Number of descriptions and sensors must match");
    LOG_DEBUG("[auto_init_saul] Number of HSC devices = %d\n", HSC_NUM);

    for (unsigned i = 0; i < HSC_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing hsc #%u\n", i);

        if (hsc_init(&hsc_devs[i], &hsc_params[i]) != 0) {
            LOG_ERROR("[auto_init_saul] error initializing hsc #%u\n", i);
            continue;
        }

        /* temperature */
        saul_entries[(i * 2)].dev = &(hsc_devs[i]);
        saul_entries[(i * 2)].name = hsc_saul_info[i].name;
        saul_entries[(i * 2)].driver = &hsc_temperature_saul_driver;

        /* pressure */
        saul_entries[(i * 2) + 1].dev = &(hsc_devs[i]);
        saul_entries[(i * 2) + 1].name = hsc_saul_info[i].name;
        saul_entries[(i * 2) + 1].driver = &hsc_pressure_saul_driver;

        /* register to saul */
        saul_reg_add(&(saul_entries[(i * 2)]));
        saul_reg_add(&(saul_entries[(i * 2) + 1]));
    }
}
#else
typedef int dont_be_pedantic;
#endif /* MODULE_HSC */

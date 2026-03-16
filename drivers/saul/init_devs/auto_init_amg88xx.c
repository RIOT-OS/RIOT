/*
 * SPDX-FileCopyrightText: 2026 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of AMG88xx driver.
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include "assert.h"
#include "log.h"
#include "saul_reg.h"
#include "amg88xx.h"
#include "amg88xx_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define AMG88XX_NUM    ARRAY_SIZE(amg88xx_params)

/**
 * @brief   Allocation of memory for device descriptors
 */
static amg88xx_t amg88xx_devs[AMG88XX_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[AMG88XX_NUM];

/**
 * @brief   Define the number of saul info
 */
#define AMG88XX_INFO_NUM    ARRAY_SIZE(amg88xx_saul_info)

/**
 * @name    Reference the driver structs.
 * @{
 */
extern const saul_driver_t amg88xx_temperature_saul_driver;
/** @} */

void auto_init_amg88xx(void)
{
    assert(AMG88XX_INFO_NUM == AMG88XX_NUM);

    for (unsigned i = 0; i < AMG88XX_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing AMG88xx #%u\n", i);

        if (amg88xx_init(&amg88xx_devs[i], &amg88xx_params[i]) != 0) {
            LOG_ERROR("[auto_init_saul] error initializing AMG88xx #%i\n", i);
            continue;
        }

        /* temperature */
        saul_entries[i].dev = &amg88xx_devs[i];
        saul_entries[i].name = amg88xx_saul_info[i].name;
        saul_entries[i].driver = &amg88xx_temperature_saul_driver;

        saul_reg_add(&saul_entries[i]);
    }
}

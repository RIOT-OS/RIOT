/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_apds99xx
 * @{
 * @ingroup     sys_auto_init_saul
 * @brief       Auto initialization of Broadcom APDS99XX proximity and ambient light sensor
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @}
 */

#include "assert.h"
#include "log.h"
#include "saul_reg.h"
#include "apds99xx.h"
#include "apds99xx_params.h"

/**
 * @name    Reference the driver structs
 * @{
 */
extern saul_driver_t apds99xx_saul_prx_driver;
extern saul_driver_t apds99xx_saul_als_driver;
#if MODULE_APDS9900 || MODULE_APDS9901 || MODULE_APDS9930
extern saul_driver_t apds99xx_saul_lux_driver;
#endif
#if MODULE_APDS9950 || MODULE_APDS9960
extern saul_driver_t apds99xx_saul_rgb_driver;
#endif

#define APDS99XX_SAUL_ENTRY_NUM 3

/** @} */

/**
 * @brief   Define the number of configured sensors
 */
#define APDS99XX_NUM    (sizeof(apds99xx_params) / sizeof(apds99xx_params[0]))

/**
 * @brief   Allocate memory for the device descriptors
 */
static apds99xx_t apds99xx_devs[APDS99XX_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[APDS99XX_NUM * APDS99XX_SAUL_ENTRY_NUM];

/**
 * @brief   Define the number of saul info
 */
#define APDS99XX_INFO_NUM    (sizeof(apds99xx_saul_info) / sizeof(apds99xx_saul_info[0]))

void auto_init_apds99xx(void)
{
    assert(APDS99XX_NUM == APDS99XX_INFO_NUM);

    for (unsigned int i = 0; i < APDS99XX_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing apds99xx #%u\n", i);

        if (apds99xx_init(&apds99xx_devs[i], &apds99xx_params[i]) < 0) {
            LOG_ERROR("[auto_init_saul] error initializing apds99xx #%u\n", i);
            continue;
        }

        /* proximity */
        saul_entries[(i * APDS99XX_SAUL_ENTRY_NUM)].dev = &(apds99xx_devs[i]);
        saul_entries[(i * APDS99XX_SAUL_ENTRY_NUM)].name = apds99xx_saul_info[i].name;
        saul_entries[(i * APDS99XX_SAUL_ENTRY_NUM)].driver = &apds99xx_saul_prx_driver;
        saul_reg_add(&(saul_entries[(i * APDS99XX_SAUL_ENTRY_NUM)]));

        /* ambient light sensing */
        saul_entries[(i * APDS99XX_SAUL_ENTRY_NUM) + 1].dev = &(apds99xx_devs[i]);
        saul_entries[(i * APDS99XX_SAUL_ENTRY_NUM) + 1].name = apds99xx_saul_info[i].name;
        saul_entries[(i * APDS99XX_SAUL_ENTRY_NUM) + 1].driver = &apds99xx_saul_als_driver;
        saul_reg_add(&(saul_entries[(i * APDS99XX_SAUL_ENTRY_NUM) + 1]));

        #if MODULE_APDS9900 || MODULE_APDS9901 || MODULE_APDS9930
        /* illuminance */
        saul_entries[(i * APDS99XX_SAUL_ENTRY_NUM) + 2].dev = &(apds99xx_devs[i]);
        saul_entries[(i * APDS99XX_SAUL_ENTRY_NUM) + 2].name = apds99xx_saul_info[i].name;
        saul_entries[(i * APDS99XX_SAUL_ENTRY_NUM) + 2].driver = &apds99xx_saul_lux_driver;
        saul_reg_add(&(saul_entries[(i * APDS99XX_SAUL_ENTRY_NUM) + 2]));
        #endif

        #if MODULE_APDS9950 || MODULE_APDS9960
        /* RGB sensing */
        saul_entries[(i * APDS99XX_SAUL_ENTRY_NUM) + 2].dev = &(apds99xx_devs[i]);
        saul_entries[(i * APDS99XX_SAUL_ENTRY_NUM) + 2].name = apds99xx_saul_info[i].name;
        saul_entries[(i * APDS99XX_SAUL_ENTRY_NUM) + 2].driver = &apds99xx_saul_rgb_driver;
        saul_reg_add(&(saul_entries[(i * APDS99XX_SAUL_ENTRY_NUM) + 2]));
        #endif
    }
}

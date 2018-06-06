/*
 * Copyright (C) 2016 Inria
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
 * @brief       Auto initialization of BMP180 driver.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#ifdef MODULE_BMP180

#include "log.h"
#include "saul_reg.h"
#include "bmp180.h"
#include "bmp180_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define BMP180_NUM      (sizeof(bmp180_params) / sizeof(bmp180_params[0]))

/**
 * @brief   Allocation of memory for device descriptors
 */
static bmp180_t bmp180_devs[BMP180_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[BMP180_NUM * 2];

/**
 * @brief   Define the number of saul info
 */
#define BMP180_INFO_NUM (sizeof(bmp180_saul_info) / sizeof(bmp180_saul_info[0]))

/**
 * @name    Reference the driver structs.
 * @{
 */
extern const saul_driver_t bmp180_temperature_saul_driver;
extern const saul_driver_t bmp180_pressure_saul_driver;
/** @} */

void auto_init_bmp180(void)
{
    assert(BMP180_INFO_NUM == BMP180_NUM);

    for (unsigned i = 0; i < BMP180_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing bmp180 #%u\n", i);

        if (bmp180_init(&bmp180_devs[i],
                        &bmp180_params[i]) != BMP180_OK) {
            LOG_ERROR("[auto_init_saul] error initializing bmp180 #%u\n", i);
            continue;
        }

        /* temperature */
        saul_entries[(i * 2)].dev = &(bmp180_devs[i]);
        saul_entries[(i * 2)].name = bmp180_saul_info[i].name;
        saul_entries[(i * 2)].driver = &bmp180_temperature_saul_driver;

        /* atmospheric pressure */
        saul_entries[(i * 2) + 1].dev = &(bmp180_devs[i]);
        saul_entries[(i * 2) + 1].name = bmp180_saul_info[i].name;
        saul_entries[(i * 2) + 1].driver = &bmp180_pressure_saul_driver;

        /* register to saul */
        saul_reg_add(&(saul_entries[(i * 2)]));
        saul_reg_add(&(saul_entries[(i * 2) + 1]));
    }
}
#else
typedef int dont_be_pedantic;
#endif /* MODULE_BMP180 */

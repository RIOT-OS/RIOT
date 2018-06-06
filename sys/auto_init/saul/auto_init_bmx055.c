/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/*
 * @ingroup     sys_auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of bmx055 9-axis sensors
 *
 * @author      Semjon Kerner <semjon.kerner@fu-berlin.de>
 * @}
 */

#ifdef MODULE_BMX055

#include "log.h"
#include "saul_reg.h"
#include "bmx055.h"
#include "bmx055_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define BMX055_NUM          (sizeof(bmx055_params) / sizeof(bmx055_params[0]))

/**
 * @brief   Each sensor contains 3 individual i2c modules
 */
#define BMX055_SENSOR_NUM   (3U)

/**
 * @brief   Allocate memory for the device descriptors
 */
static bmx055_t bmx055_devs[BMX055_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[BMX055_NUM * BMX055_SENSOR_NUM];

/**
 * @brief   Reference the driver struct
 */
extern saul_driver_t bmx055_magnetometer_saul_driver;
extern saul_driver_t bmx055_accelerometer_saul_driver;
extern saul_driver_t bmx055_gyroscope_saul_driver;

void auto_init_bmx055(void)
{
    for (unsigned int i = 0; i < BMX055_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing bmx055 #%u\n", i);

        int res = bmx055_init(&bmx055_devs[i], &bmx055_params[i]);
        if (res != BMX055_OK) {
            LOG_ERROR("[auto_init_saul] error initializing bmx055 #%u\n", i);
            continue;
        }

        /* magnetometer */
        saul_entries[(i * 3)].dev = &(bmx055_devs[i]);
        saul_entries[(i * 3)].name = bmx055_saul_info[i][0].name;
        saul_entries[(i * 3)].driver = &bmx055_magnetometer_saul_driver;
        saul_reg_add(&(saul_entries[(i * 3)]));

        /* accelerometer */
        saul_entries[(i * 3) + 1].dev = &(bmx055_devs[i]);
        saul_entries[(i * 3) + 1].name = bmx055_saul_info[i][1].name;
        saul_entries[(i * 3) + 1].driver = &bmx055_accelerometer_saul_driver;
        saul_reg_add(&(saul_entries[(i * 3) + 1]));

        /* gyroscope */
        saul_entries[(i * 3) + 2].dev = &(bmx055_devs[i]);
        saul_entries[(i * 3) + 2].name = bmx055_saul_info[i][2].name;
        saul_entries[(i * 3) + 2].driver = &bmx055_gyroscope_saul_driver;
        saul_reg_add(&(saul_entries[(i * 3) + 2]));
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_BMX055 */

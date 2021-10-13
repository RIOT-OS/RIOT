/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
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
 * @brief       Auto initialization for INA2XX power/current monitors
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include "assert.h"
#include "log.h"
#include "saul_reg.h"
#include "ina2xx_params.h"
#include "ina2xx.h"

/**
 * @brief   Define the number of configured sensors
 */
#define INA2XX_NUM      ARRAY_SIZE(ina2xx_params)

/**
 * @brief   Allocate memory for the device descriptors
 */
static ina2xx_t ina2xx_devs[INA2XX_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[INA2XX_NUM * 3];

/**
 * @brief   Define the number of saul info
 */
#define INA2XX_INFO_NUM ARRAY_SIZE(ina2xx_saul_info)

/**
 * @name    Import SAUL endpoints
 * @{
 */
extern const saul_driver_t ina2xx_saul_current_driver;
extern const saul_driver_t ina2xx_saul_power_driver;
extern const saul_driver_t ina2xx_saul_voltage_driver;
/** @} */

void auto_init_ina2xx(void)
{
    assert(INA2XX_INFO_NUM == 3 * INA2XX_NUM);

    for (unsigned int i = 0; i < INA2XX_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing ina2xx #%u\n", i);

        if (ina2xx_init(&ina2xx_devs[i], &ina2xx_params[i])) {
            LOG_ERROR("[auto_init_saul] error initializing ina2xx #%u\n", i);
            continue;
        }

        for (unsigned int j = 0; j < 3; j++) {
            saul_entries[i * 3 + j].dev = &(ina2xx_devs[i]);
            saul_entries[i * 3 + j].name = ina2xx_saul_info[3 * i + j].name;
            saul_entries[i * 3 + j].driver = &ina2xx_saul_power_driver;
            saul_reg_add(&(saul_entries[i * 3 + j]));
        }
    }
}

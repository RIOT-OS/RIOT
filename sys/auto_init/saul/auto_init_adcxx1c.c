/*
 * Copyright (C) 2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     sys_auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of ADCXX1C ADC
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 *
 * @}
 */

#ifdef MODULE_ADCXX1C

#include "log.h"

#include "saul_reg.h"
#include "adcxx1c.h"
#include "adcxx1c_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define ADCXX1C_NUM   (sizeof(adcxx1c_params) / sizeof(adcxx1c_params[0]))

/**
 * @brief   Allocate memory for the device descriptors
 */
static adcxx1c_t adcxx1c_devs[ADCXX1C_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[ADCXX1C_NUM];

/**
 * @brief   Define the number of saul info
 */
#define ADCXX1C_INFO_NUM (sizeof(adcxx1c_saul_info) / sizeof(adcxx1c_saul_info[0]))

/**
 * @brief   Reference the driver struct
 */
extern saul_driver_t adcxx1c_saul_driver;

void auto_init_adcxx1c(void)
{
    assert(ADCXX1C_INFO_NUM == ADCXX1C_NUM);

    for (unsigned i = 0; i < ADCXX1C_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing adcxx1c #%d\n", i);
        if (adcxx1c_init(&adcxx1c_devs[i], &adcxx1c_params[i]) < 0) {
            LOG_ERROR("[auto_init_saul] error initializing adcxx1c #%d\n", i);
            continue;
        }

        saul_entries[i].dev = &(adcxx1c_devs[i]);
        saul_entries[i].name = adcxx1c_saul_info[i].name;
        saul_entries[i].driver = &adcxx1c_saul_driver;
        saul_reg_add(&(saul_entries[i]));
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_ADCXX1C */

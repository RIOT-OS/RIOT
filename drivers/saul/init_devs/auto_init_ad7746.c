/*
 * SPDX-FileCopyrightText: 2019 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of AD7746
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 *
 * @}
 */

#include "assert.h"
#include "log.h"

#include "saul_reg.h"
#include "ad7746.h"
#include "ad7746_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define AD7746_NUM   ARRAY_SIZE(ad7746_params)

/**
 * @brief   Allocate memory for the device descriptors.
 */
static ad7746_t ad7746_devs[AD7746_NUM];

/**
 * @brief   Memory for the SAUL registry entries. Each device provides
 *          3 SAUL entries (capacitance, temperature and voltage)
 */
static saul_reg_t saul_entries[AD7746_NUM * 3];

/**
 * @brief   Define the number of saul info
 */
#define AD7746_INFO_NUM ARRAY_SIZE(ad7746_saul_info)

/**
 * @brief   Reference the driver structs
 * @{
 */
extern saul_driver_t ad7746_saul_driver_cap;
extern saul_driver_t ad7746_saul_driver_temp;
extern saul_driver_t ad7746_saul_driver_volt;
/** @} */

void auto_init_ad7746(void)
{
    assert(AD7746_INFO_NUM == AD7746_NUM);

    for (unsigned i = 0; i < AD7746_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing ad7746 #%d\n", i);
        if (ad7746_init(&ad7746_devs[i], &ad7746_params[i]) < 0) {
            LOG_ERROR("[auto_init_saul] error initializing ad7746 #%d\n", i);
            continue;
        }
        /* add capacitance driver */
        saul_entries[i].dev = &(ad7746_devs[i]);
        saul_entries[i].name = ad7746_saul_info[i].name;
        saul_entries[i].driver = &ad7746_saul_driver_cap;
        saul_reg_add(&(saul_entries[i]));

        /* add temperature driver */
        saul_entries[i + 1].dev = &(ad7746_devs[i]);
        saul_entries[i + 1].name = ad7746_saul_info[i].name;
        saul_entries[i + 1].driver = &ad7746_saul_driver_temp;
        saul_reg_add(&(saul_entries[i + 1]));

        /* add voltage driver */
        saul_entries[i + 2].dev = &(ad7746_devs[i]);
        saul_entries[i + 2].name = ad7746_saul_info[i].name;
        saul_entries[i + 2].driver = &ad7746_saul_driver_volt;
        saul_reg_add(&(saul_entries[i + 2]));
    }
}

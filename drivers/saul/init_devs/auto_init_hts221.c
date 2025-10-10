/*
 * SPDX-FileCopyrightText: 2017 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/*
 * @ingroup     sys_auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization for HTS221 devices
 *
 * @author      Sebastian Meiling <s@mlng.net>
 *
 * @}
 */

#include "assert.h"
#include "log.h"
#include "saul_reg.h"
#include "hts221.h"
#include "hts221_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define HTS221_NUM      ARRAY_SIZE(hts221_params)

#define HTS221_SAUL_NUM ARRAY_SIZE(hts221_saul_info)
/**
 * @brief   Allocate memory for the device descriptors
 */
static hts221_t hts221_devs[HTS221_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[HTS221_NUM * 2];

/**
 * @brief   Reference the driver struct
 * @{
 */
extern saul_driver_t hts221_saul_temp_driver;
extern saul_driver_t hts221_saul_hum_driver;
/** @} */

void auto_init_hts221(void)
{
    assert(HTS221_NUM == HTS221_SAUL_NUM);

    for (unsigned i = 0; i < HTS221_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing hts221 #%u\n", i);

        hts221_t *dev = &hts221_devs[i];
        if (hts221_init(dev, &hts221_params[i]) != HTS221_OK) {
            LOG_ERROR("[auto_init_saul] error initializing hts221 (#%u)\n", i);
            continue;
        }
        if (hts221_power_on(dev) != HTS221_OK) {
            LOG_ERROR("[auto_init_saul] error set power on (#%u)!\n", i);
            continue;
        }
        if (hts221_set_rate(dev, dev->p.rate) != HTS221_OK) {
            LOG_ERROR("[auto_init_saul] error set continuous mode (#%u)!\n", i);
            continue;
        }

        saul_entries[(i * 2)].dev = &(hts221_devs[i]);
        saul_entries[(i * 2)].name = hts221_saul_info[i].name;
        saul_entries[(i * 2)].driver = &hts221_saul_temp_driver;
        saul_entries[(i * 2) + 1].dev = &(hts221_devs[i]);
        saul_entries[(i * 2) + 1].name = hts221_saul_info[i].name;
        saul_entries[(i * 2) + 1].driver = &hts221_saul_hum_driver;
        saul_reg_add(&(saul_entries[(i * 2)]));
        saul_reg_add(&(saul_entries[(i * 2) + 1]));
    }
}

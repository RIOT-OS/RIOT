/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
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
 * @brief       Auto initialization for INA3221 devices
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 *
 * @}
 */

#include "assert.h"
#include "log.h"
#include "saul_reg.h"
#include "ina3221_params.h"
#include "ina3221.h"

#define INA3221_NUM             ARRAY_SIZE(ina3221_params)

#define INA3221_SAUL_INFO_NUM   ARRAY_SIZE(ina3221_saul_info)

#define INA3221_SAUL_DRIVERS_NUM (4)
extern const saul_driver_t ina3221_saul_drivers[INA3221_SAUL_DRIVERS_NUM];

static ina3221_t ina3221_devs[INA3221_NUM];

static saul_reg_t saul_entries[INA3221_NUM * INA3221_SAUL_DRIVERS_NUM];

void auto_init_ina3221(void)
{
    assert(INA3221_SAUL_INFO_NUM == INA3221_SAUL_DRIVERS_NUM * INA3221_NUM);

    for (unsigned i = 0; i < INA3221_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing ina3221 #%u\n", i);
        int status = ina3221_init(&ina3221_devs[i], &ina3221_params[i]);
        if (status != 0) {
            LOG_ERROR("[auto_init_saul] error(%d) initializing ina3221 #%u\n",
                      status, i);
            continue;
        }
        for (unsigned j = 0; j < INA3221_SAUL_DRIVERS_NUM; j++) {
            saul_entries[i * INA3221_SAUL_DRIVERS_NUM + j] = (saul_reg_t) {
                .dev = &ina3221_devs[i],
                .name =
                    ina3221_saul_info[i * INA3221_SAUL_DRIVERS_NUM + j].name,
                .driver = &ina3221_saul_drivers[j]
            };
            saul_reg_add(&saul_entries[i * INA3221_SAUL_DRIVERS_NUM + j]);
        }
    }
}

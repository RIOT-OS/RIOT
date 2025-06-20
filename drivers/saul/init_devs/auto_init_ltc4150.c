/*
 * SPDX-FileCopyrightText: 2018 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/*
 * @ingroup     sys_auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization for LTC4150 coulomb counter
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include "assert.h"
#include "log.h"
#include "saul_reg.h"
#include "ltc4150_params.h"
#include "ltc4150.h"

/**
 * @brief   Define the number of configured sensors
 */
#define LTC4150_NUM     ARRAY_SIZE(ltc4150_params)

/**
 * @brief   Allocate memory for the device descriptors
 */
static ltc4150_dev_t ltc4150_devs[LTC4150_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[LTC4150_NUM * 2];

/**
 * @brief   Define the number of saul info
 */
#define LTC4150_INFO_NUM ARRAY_SIZE(ltc4150_saul_info)

/**
 * @name    Import SAUL endpoints
 * @{
 */
extern const saul_driver_t ltc4150_saul_charge_driver;
extern const saul_driver_t ltc4150_saul_current_driver;
/** @} */

void auto_init_ltc4150(void)
{
    assert(LTC4150_INFO_NUM == 2 * LTC4150_NUM);

    for (unsigned int i = 0; i < LTC4150_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing ltc4150 #%u\n", i);

        if (ltc4150_init(&ltc4150_devs[i], &ltc4150_params[i])) {
            LOG_ERROR("[auto_init_saul] error initializing ltc4150 #%u\n", i);
            continue;
        }

        saul_entries[i * 2    ].dev = &(ltc4150_devs[i]);
        saul_entries[i * 2    ].name = ltc4150_saul_info[2 * i    ].name;
        saul_entries[i * 2    ].driver = &ltc4150_saul_charge_driver;
        saul_entries[i * 2 + 1].dev = &(ltc4150_devs[i]);
        saul_entries[i * 2 + 1].name = ltc4150_saul_info[2 * i + 1].name;
        saul_entries[i * 2 + 1].driver = &ltc4150_saul_current_driver;
        saul_reg_add(&(saul_entries[i * 2    ]));
        saul_reg_add(&(saul_entries[i * 2 + 1]));
    }
}

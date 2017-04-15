/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/*
 * @ingroup     auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization for DHT temperature/humidity sensors
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#ifdef MODULE_DHT

#include "log.h"
#include "saul_reg.h"
#include "dht.h"
#include "dht_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define DHT_NUM    (sizeof(dht_params) / sizeof(dht_params[0]))

/**
 * @brief   Allocate memory for the device descriptors
 */
static dht_t dht_devs[DHT_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[DHT_NUM * 2];

void auto_init_dht(void)
{
    for (unsigned int i = 0; i < DHT_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing dht #%u\n", i);

        if (dht_init(&dht_devs[i], &dht_params[i]) != DHT_OK) {
            LOG_ERROR("[auto_init_saul] error initializing dht #%u\n", i);
        }
        else {
            saul_entries[(i * 2)].dev = &(dht_devs[i]);
            saul_entries[(i * 2)].name = dht_saul_info[i][0].name;
            saul_entries[(i * 2)].driver = &dht_temp_saul_driver;
            saul_entries[(i * 2) + 1].dev = &(dht_devs[i]);
            saul_entries[(i * 2) + 1].name = dht_saul_info[i][1].name;
            saul_entries[(i * 2) + 1].driver = &dht_hum_saul_driver;
            saul_reg_add(&(saul_entries[(i * 2)]));
            saul_reg_add(&(saul_entries[(i * 2) + 1]));
        }
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_DHT */

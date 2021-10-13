/*
 * Copyright (C) 2015 Freie Universität Berlin
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
 * @brief       Auto initialization of LPSXXX family of pressure sensors
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "assert.h"
#include "log.h"
#include "saul_reg.h"
#include "lpsxxx.h"
#include "lpsxxx_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define LPSXXX_NUM      ARRAY_SIZE(lpsxxx_params)

/**
 * @brief   Allocate memory for the device descriptors
 */
static lpsxxx_t lpsxxx_devs[LPSXXX_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[LPSXXX_NUM * 2];

/**
 * @brief   Define the number of saul info
 */
#define LPSXXX_INFO_NUM    ARRAY_SIZE(lpsxxx_saul_info)

/**
 * @brief   Reference the driver struct
 */
extern saul_driver_t lpsxxx_saul_pres_driver;
extern saul_driver_t lpsxxx_saul_temp_driver;

void auto_init_lpsxxx(void)
{
    assert(LPSXXX_NUM == LPSXXX_INFO_NUM);

    for (unsigned int i = 0; i < LPSXXX_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing %s #%u\n",
                  LPSXXX_SAUL_NAME, i);

        if (lpsxxx_init(&lpsxxx_devs[i], &lpsxxx_params[i]) < 0) {
            LOG_ERROR("[auto_init_saul] error initializing %s #%u\n",
                      LPSXXX_SAUL_NAME, i);
            continue;
        }

        saul_entries[i].dev = &(lpsxxx_devs[i]);
        saul_entries[i].name = lpsxxx_saul_info[i].name;
        saul_entries[i].driver = &lpsxxx_saul_pres_driver;
        saul_reg_add(&(saul_entries[i]));
        saul_entries[(i * 2) + 1].dev = &(lpsxxx_devs[i]);
        saul_entries[(i * 2) + 1].name = lpsxxx_saul_info[i].name;
        saul_entries[(i * 2) + 1].driver = &lpsxxx_saul_temp_driver;
        saul_reg_add(&(saul_entries[(i * 2) + 1]));
    }
}

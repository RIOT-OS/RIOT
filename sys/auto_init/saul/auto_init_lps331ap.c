/*
 * Copyright (C) 2015 Freie Universität Berlin
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
 * @brief       Auto initialization of LPS331AP pressure sensors
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#ifdef MODULE_LPS331AP

#include "log.h"
#include "saul_reg.h"
#include "lps331ap.h"
#include "lps331ap_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define LPS331AP_NUM    (sizeof(lps331ap_params)/sizeof(lps331ap_params[0]))

/**
 * @brief   Allocate memory for the device descriptors
 */
static lps331ap_t lps331ap_devs[LPS331AP_NUM];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[LPS331AP_NUM];

/**
 * @brief   Reference the driver struct
 */
extern saul_driver_t lps331ap_saul_driver;


void auto_init_lps331ap(void)
{
    for (unsigned int i = 0; i < LPS331AP_NUM; i++) {
        const lps331ap_params_t *p = &lps331ap_params[i];

        LOG_DEBUG("[auto_init_saul] initializing lps331ap #%u\n", i);

        int res = lps331ap_init(&lps331ap_devs[i], p->i2c, p->addr, p->rate);
        if (res < 0) {
            LOG_ERROR("[auto_init_saul] error initializing lps331ap #%u\n", i);
        }
        else {
            saul_entries[i].dev = &(lps331ap_devs[i]);
            saul_entries[i].name = lps331ap_saul_info[i].name;
            saul_entries[i].driver = &lps331ap_saul_driver;
            saul_reg_add(&(saul_entries[i]));
        }
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_LPS331AP */

/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of LPS25H driver.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#ifdef MODULE_LPS25H

#include "log.h"
#include "saul_reg.h"
#include "lps25h.h"
#include "lps25h_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define LPS25H_NUMOF    (sizeof(lps25h_params) / sizeof(lps25h_params[0]))

/**
 * @brief   Allocation of memory for device descriptors
 */
static lps25h_t lps25h_devs[LPS25H_NUMOF];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[LPS25H_NUMOF * 2];

/**
 * @brief   Reference the driver structs.
 * @{
 */
extern const saul_driver_t lps25h_temperature_saul_driver;
extern const saul_driver_t lps25h_pressure_saul_driver;
/** @} */

void auto_init_lps25h(void)
{
    for (unsigned i = 0; i < LPS25H_NUMOF; i++) {
        LOG_DEBUG("[auto_init_saul] initializing lps25h #%u\n", i);

        if (lps25h_init(&lps25h_devs[i],
                        &lps25h_params[i]) != LPS25H_OK) {
            LOG_ERROR("[auto_init_saul] error initializing lps25h #%u\n", i);
            continue;
        }

        /* temperature */
        saul_entries[(i * 2)].dev = &(lps25h_devs[i]);
        saul_entries[(i * 2)].name = lps25h_saul_reg_info[i].name;
        saul_entries[(i * 2)].driver = &lps25h_temperature_saul_driver;

        /* atmospheric pressure */
        saul_entries[(i * 2) + 1].dev = &(lps25h_devs[i]);
        saul_entries[(i * 2) + 1].name = lps25h_saul_reg_info[i].name;
        saul_entries[(i * 2) + 1].driver = &lps25h_pressure_saul_driver;

        /* register to saul */
        saul_reg_add(&(saul_entries[(i * 2)]));
        saul_reg_add(&(saul_entries[(i * 2) + 1]));
    }
}
#else
typedef int dont_be_pedantic;
#endif /* MODULE_LPS25H */

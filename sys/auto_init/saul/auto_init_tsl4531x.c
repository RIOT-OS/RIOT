/*
 * Copyright (C) 2016 Inria
 * Copyright (C) 2018 Freie Universität Berlin
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
 * @brief       Auto initialization of TSL4531x driver.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Daniel Petry <daniel.petry@fu-berlin.de>
 *
 * Adapted from auto_init for the TSL2561 driver.
 *
 * @}
 */

#ifdef MODULE_TSL4531X

#include "log.h"
#include "saul_reg.h"
#include "tsl4531x.h"
#include "tsl4531x_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define TSL4531X_NUMOF    (sizeof(tsl4531x_params) / sizeof(tsl4531x_params[0]))

/**
 * @brief   Allocation of memory for device descriptors
 */
static tsl4531x_t tsl4531x_devs[TSL4531X_NUMOF];
static tsl4531x_t *tsl4531x_devs_p[TSL4531X_NUMOF];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[TSL4531X_NUMOF];

/**
 * @brief   Define the number of saul info
 */
#define TSL4531X_INFO_NUMOF    (sizeof(tsl4531x_saul_info) / sizeof(tsl4531x_saul_info[0]))

/**
 * @brief   Reference the driver structs.
 */
extern const saul_driver_t tsl4531x_saul_driver;

void auto_init_tsl4531x(void)
{
    assert(TSL4531X_NUMOF == TSL4531X_INFO_NUMOF);

    for (unsigned i = 0; i < TSL4531X_NUMOF; i++) {
        printf("[auto_init_saul] initializing tsl4531x #%u\n", i);

        tsl4531x_devs_p[i] = &tsl4531x_devs[i];

        if (tsl4531x_init(&tsl4531x_devs[i],
                          &tsl4531x_params[i]) != 0) {
            LOG_ERROR("[auto_init_saul] error initializing tsl4531x #%u\n", i);
            continue;
        }

        /* Fill the saul_entries struct */
        saul_entries[i].dev = &(tsl4531x_devs_p[i]);
        saul_entries[i].name = tsl4531x_saul_info[i].name;
        saul_entries[i].driver = &tsl4531x_saul_driver;

        /* register to saul */
        saul_reg_add(&(saul_entries[i]));
    }
}
#else
typedef int dont_be_pedantic;
#endif /* MODULE_TSL4531X */

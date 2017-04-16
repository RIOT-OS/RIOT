/*
 * Copyright (C) 2016 Inria
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
 * @brief       Auto initialization of IO1 Xplained extension driver.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#ifdef MODULE_IO1_XPLAINED

#include "log.h"
#include "saul_reg.h"
#include "io1_xplained.h"
#include "io1_xplained_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define IO1_XPLAINED_NUMOF    (sizeof(io1_xplained_params) / sizeof(io1_xplained_params[0]))

/**
 * @brief   Allocation of memory for device descriptors
 */
static io1_xplained_t io1_xplained_devs[IO1_XPLAINED_NUMOF];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[IO1_XPLAINED_NUMOF * 4];

/**
 * @brief   Reference the driver structs.
 * @{
 */
extern const saul_driver_t _saul_driver;
extern const saul_driver_t io1_xplained_temperature_saul_driver;
/** @} */

void auto_init_io1_xplained(void)
{
    for (unsigned i = 0; i < IO1_XPLAINED_NUMOF; i++) {
        if (io1_xplained_init(&io1_xplained_devs[i],
                              &io1_xplained_params[i]) != IO1_XPLAINED_OK) {
            LOG_ERROR("Unable to initialize IO1 Xplained #%i\n", i);
            continue;
        }

        /* Temperature */
        saul_entries[i * 4].dev = &(io1_xplained_devs[i]);
        saul_entries[i * 4].name = io1_xplained_saul_reg_info[i][0].name;
        saul_entries[i * 4].driver = &io1_xplained_temperature_saul_driver;
        saul_reg_add(&(saul_entries[i * 4]));

        /* GPIOs */
        for (unsigned j = 1; j < 4; j++) {
            saul_entries[i * 4 + j].dev = &(io1_xplained_saul_gpios[j - 1]);
            saul_entries[i * 4 + j].name = io1_xplained_saul_reg_info[i][j - 1].name;
            saul_reg_add(&(saul_entries[i * 4 + j]));
        }
    }
}
#else
typedef int dont_be_pedantic;
#endif /* MODULE_IO1_XPLAINED */

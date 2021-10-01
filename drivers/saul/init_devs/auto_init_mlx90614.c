/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
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
 * @brief       Auto initialization of MLX90614.
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include "assert.h"
#include "log.h"
#include "saul_reg.h"
#include "mlx90614.h"
#include "mlx90614_params.h"

/**
 * @brief   Define the number of configured sensors
 */
#define MLX90614_NUMOF      ARRAY_SIZE(mlx90614_params)

/**
 * @brief   Allocation of memory for device descriptors
 */
static mlx90614_t mlx90614_devs[MLX90614_NUMOF];

/**
 * @brief   Memory for the SAUL registry entries
 */
static saul_reg_t saul_entries[MLX90614_NUMOF];

/**
 * @brief   Reference the driver structs.
 */
extern const saul_driver_t mlx90614_temperature_saul_driver;

void auto_init_mlx90614(void)
{
    for (unsigned i = 0; i < MLX90614_NUMOF; i++) {
        LOG_DEBUG("[auto_init_saul] initializing mlx90614 #%u\n", i);

        if (mlx90614_init(&mlx90614_devs[i], &mlx90614_params[i]) < 0) {
            LOG_ERROR("[auto_init_saul] error initializing mlx90614 #%u\n", i);
            continue;
        }

        /* temperature */
        saul_entries[i].dev = &mlx90614_devs[i];
        saul_entries[i].name = "mlx90614";
        saul_entries[i].driver = &mlx90614_temperature_saul_driver;

        /* register to saul */
        saul_reg_add(&(saul_entries[i]));
    }
}

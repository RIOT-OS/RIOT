/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mlx90614
 *
 * @{
 * @file
 * @brief       Test program for the driver of the MLX90614 temperature sensor
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include "mlx90614.h"
#include "mlx90614_params.h"
#include "xtimer.h"
#include "test_utils/expect.h"

#define MLX90614_NUMOF  ARRAY_SIZE(mlx90614_params)

int main(void)
{
    mlx90614_t dev[MLX90614_NUMOF];

    /* initialize all sensors */
    for (unsigned i = 0; i < MLX90614_NUMOF; ++i) {
        mlx90614_init(&dev[i], &mlx90614_params[i]);
    }

    while (1) {
        for (unsigned i = 0; i < MLX90614_NUMOF; ++i) {
            int res;
            uint32_t temp[3];
            res = mlx90614_read_temperature(&dev[i], MLX90614_TEMP_AMBIENT, &temp[0]);
            expect(res == 0);
            res = mlx90614_read_temperature(&dev[i], MLX90614_TEMP_OBJ1, &temp[1]);
            expect(res == 0);
            res = mlx90614_read_temperature(&dev[i], MLX90614_TEMP_OBJ2, &temp[2]);
            expect(res == 0);

            printf("DEV%u:\tambient: %.2f K\tOBJ1: %.2f K\tOBJ2: %.2f K\n",
                   i, temp[0] / 100.f, temp[1] / 100.f, temp[2] / 100.f);
        }
        xtimer_msleep(250);
    }

    return 0;
}

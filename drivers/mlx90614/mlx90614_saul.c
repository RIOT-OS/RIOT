/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mlx90614
 * @{
 *
 * @file
 * @brief       SAUL adaption for MLX90614
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include "saul.h"
#include "mlx90614.h"

static int _read_temperature(const void *dev, phydat_t *res)
{
    uint32_t tmp;

    mlx90614_read_temperature(dev, MLX90614_TEMP_AMBIENT, &tmp);
    res->val[0] = tmp / 10; /* phydat_t is 16 bit */

    mlx90614_read_temperature(dev, MLX90614_TEMP_OBJ1, &tmp);
    res->val[1] = tmp / 10; /* phydat_t is 16 bit */

    mlx90614_read_temperature(dev, MLX90614_TEMP_OBJ2, &tmp);
    res->val[2] = tmp / 10; /* phydat_t is 16 bit */

    res->unit = UNIT_TEMP_K;
    res->scale = -1;
    return 3;
}

const saul_driver_t mlx90614_temperature_saul_driver = {
    .read = _read_temperature,
    .write = saul_notsup,
    .type = SAUL_SENSE_TEMP
};

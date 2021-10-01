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
 * @brief       Driver for the MLX90614 ±0.02°C Accurate, 16-Bit Digital I2C
 *              Temperature Sensor from Melexis
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include "byteorder.h"
#include "mlx90614.h"

void mlx90614_init(mlx90614_t *dev, const mlx90614_params_t *params)
{
    dev->params = *params;
}

int mlx90614_read_temperature(const mlx90614_t *dev, mlx90614_chan_t chan, uint32_t *out)
{
    int res;
    le_uint16_t tmp;

    res = i2c_acquire(dev->params.bus);
    if (res) {
        return res;
    }

    res = i2c_read_regs(dev->params.bus, dev->params.addr, chan, &tmp, sizeof(tmp), 0);
    i2c_release(dev->params.bus);

    if (res) {
        return res;
    }

    /* register value is in 0.02K */
    *out = byteorder_ltohs(tmp) * 2;

    return 0;
}

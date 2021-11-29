/*
 * Copyright (C) 2020 Nalys
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_bh1900nux
 * @{
 *
 * @file
 * @brief       BH1900NUX temperature sensor driver implementation
 *
 * @author      Wouter Symons <wsymons@nalys-group.com>
 *
 * @}
 */

#include <stdio.h>

#include "bh1900nux.h"
#include "byteorder.h"

#define ENABLE_DEBUG 0
#include "debug.h"

int bh1900nux_init(bh1900nux_t *dev, const bh1900nux_params_t *params)
{
    if ((dev == NULL) | (params == NULL)) {
        return -1;
    }

    /* initialize the device descriptor */
    dev->i2c = params->i2c;
    dev->addr = params->addr;

    return BH1900NUX_OK;
}

int bh1900nux_read(const bh1900nux_t *dev, int16_t *temp)
{
    int ret = 0;
    int16_t raw;

    /* Read raw sensor value */
    DEBUG("[bh1900nux] read temperature\n");

    i2c_acquire(dev->i2c);
    ret = i2c_read_regs(dev->i2c, dev->addr, BH1900NUX_REG_ADDR, &raw, sizeof(raw), 0);
    i2c_release(dev->i2c);

    if (ret < 0) {
        return ret;
    }

    /* Calculate temperature */
    raw = (int16_t) ntohs(raw) >> 4;
    *temp = ((int32_t) raw * 1000) / 16;

    return BH1900NUX_OK;
}

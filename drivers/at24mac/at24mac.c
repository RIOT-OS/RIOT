/*
 * SPDX-FileCopyrightText: 2019 Benjamin Valentin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_at24mac
 *
 * @{
 * @file
 * @brief       Driver for AT24MAC unique ID chip.
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <errno.h>
#include <stdbool.h>

#include "kernel_defines.h"

#include "at24mac.h"
#include "at24mac_params.h"

#define CMD_READ_EUI48      (0x9A)
#define CMD_READ_EUI64      (0x98)
#define CMD_READ_ID128      (0x80)

static bool _is_valid(at24mac_type_t type, uint8_t reg)
{
    if (type == AT24MAC4XX && reg == CMD_READ_EUI64) {
        return false;
    }

    if (type == AT24MAC6XX && reg == CMD_READ_EUI48) {
        return false;
    }

    return true;
}

static int _read_reg(at24mac_t dev, uint8_t reg, void *dst, size_t size)
{
    if (dev >= ARRAY_SIZE(at24mac_params)) {
        return -ERANGE;
    }

    int res = 0;
    const at24mac_params_t *params = &at24mac_params[dev];

    if (!_is_valid(params->type, reg)) {
        return -ENOTSUP;
    }

    i2c_acquire(params->i2c_dev);

    res = i2c_read_regs(params->i2c_dev, params->i2c_addr,
                        reg, dst, size, 0);

    i2c_release(params->i2c_dev);

    return res;
}

int at24mac_get_eui48(at24mac_t dev, eui48_t *dst)
{
    return _read_reg(dev, CMD_READ_EUI48, dst, sizeof(*dst));
}

int at24mac_get_eui64(at24mac_t dev, eui64_t *dst)
{
    return _read_reg(dev, CMD_READ_EUI64, dst, sizeof(*dst));
}

int at24mac_get_id128(at24mac_t dev, void *dst)
{
    return _read_reg(dev, CMD_READ_ID128, dst, AT24MAC_ID_LEN);
}

at24mac_type_t at24mac_get_type(at24mac_t dev)
{
    if (dev >= ARRAY_SIZE(at24mac_params)) {
        return -ERANGE;
    }

    return at24mac_params[dev].type;
}

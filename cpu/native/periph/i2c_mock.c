/*
 * SPDX-FileCopyrightText: 2024 COGIP Robotics Association
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_native
 * @ingroup     drivers_periph_i2c
 * @{
 *
 * @file
 * @brief       Pluggable no-op I2C implementation
 *
 * By default, operations on this bus will always succeed (without actually performing any
 * action). Reads will produce zeros.
 *
 * To provide mocks of a particular connected device, application authors can provide any
 * of the functions themselves, because all the I2C functions in this implementation are
 * weak symbols. There is per-bus or per-address dispatch. That means that if multiple
 * buses or devices are to be mocked with non-zero implementations, the application needs
 * to provide a single implementation that fans out to the respective mocked device.
 *
 * @author      Gilles DOFFE <g.doffe@gmail.com>
 */

#include <string.h>

#include "periph/i2c.h"

__attribute__((weak)) void i2c_init(i2c_t dev)
{
    (void)dev;
}

__attribute__((weak)) void i2c_acquire(i2c_t dev)
{
    (void)dev;
}

__attribute__((weak)) void i2c_release(i2c_t dev)
{
    (void)dev;
}

__attribute__((weak)) int i2c_read_bytes(i2c_t dev, uint16_t addr,
                   void *data, size_t len, uint8_t flags)
{
    (void)dev;
    (void)addr;
    memset(data, 0, len);
    (void)len;
    (void)flags;

    return 0;
}

__attribute__((weak)) int i2c_write_bytes(i2c_t dev, uint16_t addr, const void *data,
                    size_t len, uint8_t flags)
{
    (void)dev;
    (void)addr;
    (void)data;
    (void)len;
    (void)flags;

    return 0;
}

/*
 * SPDX-FileCopyrightText: 2021 Otto-von-Guericke Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_rpx0xx
 * @ingroup     drivers_periph_i2c
 * @{
 *
 * @file
 * @brief       Low-level I2C driver implementation
 *
 * @note        This driver is so far only a dummy implementation
 *              to test the PIO I2C interface.
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 * @}
 */

#include <errno.h>

#include "periph_conf.h"
#include "periph/i2c.h"
#include "periph/pio/i2c.h"

#define ENABLE_DEBUG 0
#include "debug.h"

void i2c_init(i2c_t dev)
{
    if (IS_USED(MODULE_PIO_I2C) && dev >= I2C_NUMOF) {
        pio_i2c_bus_t *i2c = pio_i2c_get(dev - I2C_NUMOF);
        assert(i2c);
        pio_t pio = pio_i2c_config[dev - I2C_NUMOF].pio;
        if (pio_i2c_init_program(pio)) {
            DEBUG("[i2c] init: PIO program allocation failed\n");
            return;
        }
        pio_sm_t sm = pio_i2c_sm_lock(pio, i2c);
        if (sm < 0) {
            DEBUG("[i2c] init: PIO state machine allocation failed\n");
            return;
        }
        if (pio_i2c_init(i2c, pio_i2c_get_program(pio),
                         pio_i2c_config[dev - I2C_NUMOF].sda,
                         pio_i2c_config[dev - I2C_NUMOF].scl,
                         pio_i2c_config[dev - I2C_NUMOF].irq)) {
            DEBUG("[i2c] init: PIO I2C initialization failed\n");
            pio_i2c_sm_unlock(i2c);
            return;
        }
    }
}

void i2c_acquire(i2c_t dev)
{
    if (IS_USED(MODULE_PIO_I2C) && dev >= I2C_NUMOF) {
        pio_i2c_bus_t *i2c = pio_i2c_get(dev - I2C_NUMOF);
        if (i2c) {
            pio_i2c_acquire(i2c);
        }
    }
}

void i2c_release(i2c_t dev)
{
    if (IS_USED(MODULE_PIO_I2C) && dev >= I2C_NUMOF) {
        pio_i2c_bus_t *i2c = pio_i2c_get(dev - I2C_NUMOF);
        if (i2c) {
            pio_i2c_release(i2c);
        }
    }
}

int i2c_read_bytes(i2c_t dev, uint16_t addr, void *data,
                   size_t len, uint8_t flags)
{
    if (IS_USED(MODULE_PIO_I2C) && dev >= I2C_NUMOF) {
        pio_i2c_bus_t *i2c = pio_i2c_get(dev - I2C_NUMOF);
        return i2c ? pio_i2c_read_bytes(i2c->pio, i2c->sm, addr, data, len, flags) : -EINVAL;
    }
    return -EIO;
}

int i2c_read_regs(i2c_t dev, uint16_t addr, uint16_t reg,
                  void *data, size_t len, uint8_t flags)
{
    if (IS_USED(MODULE_PIO_I2C) && dev >= I2C_NUMOF) {
        pio_i2c_bus_t *i2c = pio_i2c_get(dev - I2C_NUMOF);
        return i2c ? pio_i2c_read_regs(i2c->pio, i2c->sm, addr, reg, data, len, flags) : -EINVAL;
    }
    return -EIO;
}

int i2c_read_reg(i2c_t dev, uint16_t addr, uint16_t reg,
                 void *data, uint8_t flags)
{
    return i2c_read_regs(dev, addr, reg, data, 1, flags);
}

int i2c_write_bytes(i2c_t dev, uint16_t addr, const void *data,
                    size_t len, uint8_t flags)
{
    if (IS_USED(MODULE_PIO_I2C) && dev >= I2C_NUMOF) {
        pio_i2c_bus_t *i2c = pio_i2c_get(dev - I2C_NUMOF);
        return i2c ? pio_i2c_write_bytes(i2c->pio, i2c->sm, addr, data, len, flags) : -EINVAL;
    }
    return -EIO;
}

int i2c_write_regs(i2c_t dev, uint16_t addr, uint16_t reg,
                   const void *data, size_t len, uint8_t flags)
{
    if (IS_USED(MODULE_PIO_I2C) && dev >= I2C_NUMOF) {
        pio_i2c_bus_t *i2c = pio_i2c_get(dev - I2C_NUMOF);
        return i2c ? pio_i2c_write_regs(i2c->pio, i2c->sm, addr, reg, data, len, flags) : -EINVAL;
    }
    return -EIO;
}

int i2c_write_reg(i2c_t dev, uint16_t addr, uint16_t reg,
                  uint8_t data, uint8_t flags)
{
    return i2c_write_regs(dev, addr, reg, &data, 1, flags);
}

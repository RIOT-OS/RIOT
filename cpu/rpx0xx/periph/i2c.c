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
 * @note        The hardware I2C peripherals are not supported yet,
 *              all I2C buses are emulated by the PIO I2C interface.
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

#if !defined(PIO_I2C_NUMOF)
#  error "The board must provide pio_i2c_config and PIO_I2C_NUMOF to use periph_i2c!"
#endif

void i2c_init(i2c_t dev)
{
    pio_i2c_bus_t *i2c = pio_i2c_get(dev);
    if (!i2c) {
        DEBUG("[i2c] init: no PIO I2C bus configured for this device\n");
        return;
    }
    pio_t pio = pio_i2c_config[dev].pio;
    if (pio_i2c_init_program(pio)) {
        DEBUG("[i2c] init: PIO program allocation failed\n");
        return;
    }
    pio_sm_t sm = pio_i2c_sm_lock(pio, i2c);
    if (sm < 0) {
        DEBUG("[i2c] init: PIO state machine allocation failed\n");
        pio_i2c_deinit_program(pio);
        return;
    }
    if (pio_i2c_init(i2c, pio_i2c_get_program(pio),
                     pio_i2c_config[dev].sda,
                     pio_i2c_config[dev].scl,
                     pio_i2c_config[dev].irq)) {
        DEBUG("[i2c] init: PIO I2C initialization failed\n");
        pio_i2c_sm_unlock(i2c);
        pio_i2c_deinit_program(pio);
        return;
    }
}

void i2c_acquire(i2c_t dev)
{
    pio_i2c_bus_t *i2c = pio_i2c_get(dev);
    if (i2c) {
        pio_i2c_acquire(i2c);
    }
}

void i2c_release(i2c_t dev)
{
    pio_i2c_bus_t *i2c = pio_i2c_get(dev);
    if (i2c) {
        pio_i2c_release(i2c);
    }
}

int i2c_read_bytes(i2c_t dev, uint16_t addr, void *data,
                   size_t len, uint8_t flags)
{
    pio_i2c_bus_t *i2c = pio_i2c_get(dev);
    if (!i2c || (i2c->sm < 0)) {
        return -EINVAL;
    }
    return pio_i2c_read_bytes(i2c->pio, i2c->sm, addr, data, len, flags);
}

int i2c_read_regs(i2c_t dev, uint16_t addr, uint16_t reg,
                  void *data, size_t len, uint8_t flags)
{
    pio_i2c_bus_t *i2c = pio_i2c_get(dev);
    if (!i2c || (i2c->sm < 0)) {
        return -EINVAL;
    }
    return pio_i2c_read_regs(i2c->pio, i2c->sm, addr, reg, data, len, flags);
}

int i2c_read_reg(i2c_t dev, uint16_t addr, uint16_t reg,
                 void *data, uint8_t flags)
{
    return i2c_read_regs(dev, addr, reg, data, 1, flags);
}

int i2c_write_bytes(i2c_t dev, uint16_t addr, const void *data,
                    size_t len, uint8_t flags)
{
    pio_i2c_bus_t *i2c = pio_i2c_get(dev);
    if (!i2c || (i2c->sm < 0)) {
        return -EINVAL;
    }
    return pio_i2c_write_bytes(i2c->pio, i2c->sm, addr, data, len, flags);
}

int i2c_write_regs(i2c_t dev, uint16_t addr, uint16_t reg,
                   const void *data, size_t len, uint8_t flags)
{
    pio_i2c_bus_t *i2c = pio_i2c_get(dev);
    if (!i2c || (i2c->sm < 0)) {
        return -EINVAL;
    }
    return pio_i2c_write_regs(i2c->pio, i2c->sm, addr, reg, data, len, flags);
}

int i2c_write_reg(i2c_t dev, uint16_t addr, uint16_t reg,
                  uint8_t data, uint8_t flags)
{
    return i2c_write_regs(dev, addr, reg, &data, 1, flags);
}

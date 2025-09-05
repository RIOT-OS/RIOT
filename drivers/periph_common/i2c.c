/*
 * SPDX-FileCopyrightText: 2018 Mesotic SAS <dylan.laduranty@mesotic.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_periph_i2c
 * @{
 *
 * @file
 * @brief       common I2C function fallback implementations
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 * @}
 */
#include <errno.h>

#include "board.h"
#include "cpu.h"
#include "periph/i2c.h"
#include "byteorder.h"

#ifdef PERIPH_I2C_NEED_READ_REG
int i2c_read_reg(i2c_t dev, uint16_t addr, uint16_t reg,
                 void *data, uint8_t flags)
{
    return i2c_read_regs(dev, addr, reg, data, 1, flags);
}
#endif /* PERIPH_I2C_NEED_READ_REG */

#ifdef PERIPH_I2C_NEED_READ_REGS
int i2c_read_regs(i2c_t dev, uint16_t addr, uint16_t reg,
                  void *data, size_t len, uint8_t flags)
{
    uint16_t reg_end = reg;

    if (flags & (I2C_NOSTOP | I2C_NOSTART)) {
        return -EOPNOTSUPP;
    }

    /* Handle endianness of register if 16 bit */
    if (flags & I2C_REG16) {
        reg_end = htons(reg); /* Make sure register is in big-endian on I2C bus */
    }

    /* First set ADDR and register with no stop */
    int ret = i2c_write_bytes(dev, addr, &reg_end, (flags & I2C_REG16) ? 2 : 1,
                              flags | I2C_NOSTOP);
    if (ret < 0) {
        return ret;
    }
    /* Then get the data from device */
    return i2c_read_bytes(dev, addr, data, len, flags);
}
#endif /* PERIPH_I2C_NEED_READ_REGS */

int i2c_read_byte(i2c_t dev, uint16_t addr, void *data, uint8_t flags)
{
    return i2c_read_bytes(dev, addr, data, 1, flags);
}

int i2c_write_byte(i2c_t dev, uint16_t addr, uint8_t data, uint8_t flags)
{
    return i2c_write_bytes(dev, addr, &data, 1, flags);
}

#ifdef PERIPH_I2C_NEED_WRITE_REG
int i2c_write_reg(i2c_t dev, uint16_t addr, uint16_t reg,
                  uint8_t data, uint8_t flags)
{
    return i2c_write_regs(dev, addr, reg, &data, 1, flags);
}
#endif /* PERIPH_I2C_NEED_WRITE_REG */

#ifdef PERIPH_I2C_NEED_WRITE_REGS
int i2c_write_regs(i2c_t dev, uint16_t addr, uint16_t reg,
                   const void *data, size_t len, uint8_t flags)
{
    uint16_t reg_end = reg;

    if (flags & (I2C_NOSTOP | I2C_NOSTART)) {
        return -EOPNOTSUPP;
    }

    /* Handle endianness of register if 16 bit */
    if (flags & I2C_REG16) {
        reg_end = htons(reg); /* Make sure register is in big-endian on I2C bus */
    }

    /* First set ADDR and register with no stop */
    int ret = i2c_write_bytes(dev, addr, &reg_end, (flags & I2C_REG16) ? 2 : 1,
                              flags | I2C_NOSTOP);
    if (ret < 0) {
        return ret;
    }
    /* Then write data to the device */
    return i2c_write_bytes(dev, addr, data, len, flags | I2C_NOSTART);
}
#endif /* PERIPH_I2C_NEED_WRITE_REGS */

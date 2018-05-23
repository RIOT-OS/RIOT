/*
 * Copyright (C) 2018 Mesotic SAS <dylan.laduranty@mesotic.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers
 * @{
 *
 * @file
 * @brief       common I2C function fallback implementations
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 * @}
 */

#include "board.h"
#include "cpu.h"
#include "periph/i2c.h"

#ifdef I2C_NUMOF

#ifdef PERIPH_I2C_NEED_READ_REG
int i2c_read_reg(ic2_t dev, uint16_t addr, uint16_t reg,
                 void *data, uint8_t flags)
{
	return i2c_read_regs(dev, addr, reg, data, 1, flags);
}
#endif /* PERIPH_I2C_NEED_READ_REG */

#ifdef PERIPH_I2C_NEED_READ_REGS
int i2c_read_regs(ic2_t dev, uint16_t addr, uint16_t reg,
                  void *data, size_t len, uint8_t flags);
{
	uint16_t data = reg;
	uint8_t err;
	/* First set ADDR and register with no stop */
	err = i2c_write(dev, addr, &data, (flags & I2C_REG16) ? 2 : 1,
					flags & I2C_NOSTOP );
	if(err != I2C_ACK)
	{
		return err;
	}
	/* Then get the data from device */
    return i2c_read(dev, addr, data, len, flags);
}
#endif /* PERIPH_I2C_NEED_READ_REGS */

#ifdef PERIPH_I2C_NEED_READ_BYTE
int i2c_read_byte(ic2_t dev, uint16_t addr, void *data, uint8_t flags)
{
	return i2c_read(dev, addr, data, 1, flags);
}
#endif /* PERIPH_I2C_NEED_READ_BYTE */

#ifdef PERIPH_I2C_NEED_READ_BYTES
int i2c_read_bytes(ic2_t dev, uint16_t addr,
                   void *data, size_t len, uint8_t flags)
{
	return i2c_read(dev, addr, data, len, flags);
}
#endif /* PERIPH_I2C_NEED_READ_BYTES */

#ifdef PERIPH_I2C_NEED_WRITE_BYTE
int i2c_write_byte(i2c_t dev, uint16_t addr, uint8_t data, uint8_t flags)
{
	return i2c_write(dev, addr, addr, 1, flags);
}
#endif /* PERIPH_I2C_NEED_WRITE_BYTE */

#ifdef PERIPH_I2C_NEED_WRITE_BYTES
int i2c_write_bytes(i2c_t dev, uint16_t addr, const void *data,
                    size_t len, uint8_t flags)
{
	return i2c_write(dev, addr, addr, len, flags);
}
#endif /* PERIPH_I2C_NEED_WRITE_BYTES */

#ifdef PERIPH_I2C_NEED_WRITE_REG
int i2c_write_reg(i2c_t dev, uint16_t addr, uint16_t reg,
                  const void *data, size_t len, uint8_t flags)
{
	return i2c_write_regs(dev, addr, reg, data, 1);
}
#endif /* PERIPH_I2C_NEED_WRITE_REG */

#ifdef PERIPH_I2C_NEED_WRITE_REGS
int i2c_write_regs(i2c_t dev, uint16_t addr, uint16_t reg,
                  const void *data, size_t len, uint8_t flags)
{
	uint16_t regp = reg;
	uint8_t err;
	/* First set ADDR and register with no stop */
	err = i2c_write(dev, addr, &regp, (flags & I2C_REG16) ? 2 : 1,
					flags & I2C_NOSTOP );
	if(err != I2C_ACK)
	{
		return err;
	}
	/* Then write data to the device */
    return i2c_write(dev, addr, data, len, flags);
}
#endif /* PERIPH_I2C_NEED_WRITE_REGS */

#endif /* I2C_NUMOF */

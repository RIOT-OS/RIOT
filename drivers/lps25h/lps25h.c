/*
 * Copyright (C) 2015 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_lps25h
 * @{
 *
 * @file
 * @brief       Device driver implementation for the LPS25H pressure sensor
 *
 * @note The current driver implementation is very basic and allows only for polling the
 *       devices temperature and pressure values. Threshold values and interrupts are not
 *       used.
 *
 * @author      René Herthel <rene-herthel@outlook.de>
 *
 * @}
 */

#include <stdint.h>

#include "lps25h.h"
#include "include/lps25h-internal.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief pressure divider for norming pressure output
 */
#define PRES_DIVIDER        (4096U)

/**
 * @brief temperature base value and divider for norming temperature output
 */
#define TEMP_BASE           (42.5f)
#define TEMP_DIVIDER        (480U)

int lps25h_init(lps25h_t *dev, i2c_t i2c, uint8_t address, lps25h_odr_t odr)
{
    char tmp;

    dev->i2c = i2c;
    dev->address = address;

    i2c_acquire(dev->i2c);

    if (i2c_init_master(dev->i2c, I2C_SPEED_FAST) < 0) {
        i2c_release(dev->i2c);
        return -1;
    }

    i2c_read_reg(dev->i2c, dev->address, LPS25H_REG_WHO_AM_I, &tmp);
    if (tmp & !LPS25H_CHIP_ID) {
        i2c_release(dev->i2c);
        return -1;
    }

    tmp = (LPS25H_MASK_CTRL_REG1_PD     /**< active mode        */
          | LPS25H_MASK_CTRL_REG1_BDU   /**< block data update  */
          | odr);                       /**< ouput data rate    */

    if (i2c_write_reg(dev->i2c, dev->address, LPS25H_REG_CTRL_REG1, tmp) != 1) {
        i2c_release(dev->i2c);
        return -1;
    }

    i2c_release(dev->i2c);

    return 0;
}

int lps25h_read_temp(lps25h_t *dev)
{
    char tmp[2] = {0, 0};
    int val = 0;
    float res;

    i2c_acquire(dev->i2c);
    i2c_read_regs(dev->i2c, dev->address, LPS25H_REG_TEMP_OUT_L, &tmp[0], 2);
    i2c_release(dev->i2c);

    val = (tmp[1] << 8) | tmp[0];

    DEBUG("LPS25H: raw temperature: %i\n", val);

    res = TEMP_BASE + ((float)val / TEMP_DIVIDER);

    return (int)res;
}

int lps25h_read_pres(lps25h_t *dev)
{
    char tmp[3] = {0, 0, 0};
    int val = 0;
    float res;

    i2c_acquire(dev->i2c);
    i2c_read_regs(dev->i2c, dev->address, LPS25H_REG_PRESS_POUT_XL, &tmp[0], 3);
    i2c_release(dev->i2c);

    val = (tmp[2] << 16) | (tmp[1] << 8) | tmp[0];

    DEBUG("LPS25H: raw pressure: %i\n", val);

    res = ((float)val) / PRES_DIVIDER;

    return (int)res;
}

int lps25h_enable(lps25h_t *dev)
{
    char tmp;

    i2c_acquire(dev->i2c);
    if (i2c_read_reg(dev->i2c, dev->address, LPS25H_REG_CTRL_REG1, &tmp) != 1) {
        i2c_release(dev->i2c);
        return -1;
    }

    tmp |= (LPS25H_MASK_CTRL_REG1_PD);
    i2c_write_reg(dev->i2c, dev->address, LPS25H_REG_CTRL_REG1, tmp);

    i2c_release(dev->i2c);

    return 0;
}

int lps25h_disable(lps25h_t *dev)
{
    char tmp;

    i2c_acquire(dev->i2c);
    if (i2c_read_reg(dev->i2c, dev->address, LPS25H_REG_CTRL_REG1, &tmp) != 1) {
        i2c_release(dev->i2c);
        return -1;
    }

    tmp &= ~(LPS25H_MASK_CTRL_REG1_PD);
    i2c_write_reg(dev->i2c, dev->address, LPS25H_REG_CTRL_REG1, tmp);

    i2c_release(dev->i2c);

    return 0;
}

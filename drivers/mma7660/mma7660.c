/*
 * Copyright (C) 2016 University of California, Berkeley
 *               2018 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     drivers_mma7660
 * @{
 *
 * @file
 * @brief       Driver for the Freescale MMA7660 accelerometer.
 *
 * @author      Michael Andersen <m.andersen@cs.berkeley.edu>
 * @author      Sebastian Meiling <s@mlng.net>
 *
 * @}
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "periph/i2c.h"

#include "mma7660.h"
#include "mma7660_reg.h"

#define ENABLE_DEBUG     (0)
#include "debug.h"

#define I2C_SPEED        I2C_SPEED_FAST

#define ALERT_MASK       (0x40)
#define SIGN_BIT         (0x20)
#define SIGN_EXTEND      (0xC0)
#define SR_FILT_SHIFT    (5U)

#define PDET_AXIS_MASK   (0xE0)

#define DEV_I2C          (dev->params.i2c)
#define DEV_ADDR         (dev->params.addr)

int mma7660_init(mma7660_t *dev, const mma7660_params_t *params)
{
    /* write device descriptor */
    memcpy(&dev->params, params, sizeof(mma7660_params_t));

    if (mma7660_set_mode(dev, 0, 0, 0, 0) != MMA7660_OK) {
        DEBUG("mma7660_set_mode failed!\n");
        return -MMA7660_I2C_WRITE_ERR;
    }

    if (mma7660_config_samplerate(dev, params->amsr,
                                  params->awsr, params->filt) != MMA7660_OK) {
        DEBUG("mma7660_config_samplerate failed!\n");
        return -MMA7660_I2C_WRITE_ERR;
    }

    if (mma7660_config_interrupts(dev, MMA7660_INTSOURCE_NONE) != MMA7660_OK) {
        DEBUG("mma7660_config_interrupts failed!\n");
        return -MMA7660_I2C_WRITE_ERR;
    }

    /* set device active after configuration */
    if (mma7660_set_mode(dev, 1, 0, 0, 0) != MMA7660_OK) {
        DEBUG("mma7660_set_mode failed!\n");
        return -MMA7660_I2C_WRITE_ERR;
    }

    return MMA7660_OK;
}

int mma7660_set_mode(const mma7660_t *dev, uint8_t active,
                     uint8_t autowake, uint8_t autosleep, uint8_t prescale)
{
    char reg;
    int rv;
    reg = (active << MODE_ACTIVE_SHIFT)       |
          (autowake << MODE_AUTOWAKE_SHIFT)   |
          (autosleep << MODE_AUTOSLEEP_SHIFT) |
          (prescale << MODE_PRESCALE_SHIFT)   |
          MODE_INTERRUPT_DEFAULT;
    i2c_acquire(DEV_I2C);
    rv = i2c_write_reg(DEV_I2C, DEV_ADDR, MMA7660_MODE, reg, 0);
    i2c_release(DEV_I2C);
    if (rv != 0) {
        return -MMA7660_I2C_WRITE_ERR;
    }

    return MMA7660_OK;
}

int mma7660_read_tilt(const mma7660_t *dev, uint8_t *res)
{
    int rv;
    i2c_acquire(DEV_I2C);
    rv = i2c_read_reg(DEV_I2C, DEV_ADDR, MMA7660_TILT, (char *)res, 0);
    i2c_release(DEV_I2C);
    if (rv != 0) {
        return -MMA7660_I2C_READ_ERR;
    }

    return MMA7660_OK;
}

int mma7660_write_sleep_count(const mma7660_t *dev, uint8_t sleep)
{
    int rv;
    i2c_acquire(DEV_I2C);
    rv = i2c_write_reg(DEV_I2C, DEV_ADDR, MMA7660_SPCNT, sleep, 0);
    i2c_release(DEV_I2C);
    if (rv != 0) {
        return -MMA7660_I2C_WRITE_ERR;
    }

    return MMA7660_OK;
}

int mma7660_config_interrupts(const mma7660_t *dev, uint8_t isource_flags)
{
    int rv;
    i2c_acquire(DEV_I2C);
    rv = i2c_write_reg(DEV_I2C, DEV_ADDR, MMA7660_INTSU, isource_flags, 0);
    i2c_release(DEV_I2C);
    if (rv != 0) {
        return -MMA7660_I2C_WRITE_ERR;
    }

    return MMA7660_OK;
}

int mma7660_config_samplerate(const mma7660_t *dev, uint8_t amsr, uint8_t awsr, uint8_t filt)
{
    int rv;
    char ch = amsr | awsr | (filt << SR_FILT_SHIFT);
    i2c_acquire(DEV_I2C);
    rv = i2c_write_reg(DEV_I2C, DEV_ADDR, MMA7660_SR, ch, 0);
    i2c_release(DEV_I2C);
    if (rv != 0) {
        return -MMA7660_I2C_WRITE_ERR;
    }

    return MMA7660_OK;
}

int mma7660_config_pdet(const mma7660_t *dev, uint8_t pdth, uint8_t enabled_axes) {
    int rv;
    char ch = pdth | ((~enabled_axes) & PDET_AXIS_MASK);
    i2c_acquire(DEV_I2C);
    rv = i2c_write_reg(DEV_I2C, DEV_ADDR, MMA7660_PDET, ch, 0);
    i2c_release(DEV_I2C);
    if (rv != 0) {
        return -MMA7660_I2C_WRITE_ERR;
    }

    return MMA7660_OK;
}

int mma7660_config_pd(const mma7660_t *dev, uint8_t pd) {
    int rv;
    i2c_acquire(DEV_I2C);
    rv = i2c_write_reg(DEV_I2C, DEV_ADDR, MMA7660_PD, (char)pd, 0);
    i2c_release(DEV_I2C);
    if (rv != 0) {
        return -MMA7660_I2C_WRITE_ERR;
    }

    return MMA7660_OK;
}

int mma7660_read_counts(const mma7660_t *dev, int8_t *x, int8_t *y, int8_t *z)
{
    int retries = 6;
    char t;
    i2c_acquire(DEV_I2C);

    while (retries > 0) {
        if (i2c_read_reg(DEV_I2C, DEV_ADDR, MMA7660_XOUT, &t, 0) != 0) {
            i2c_release(DEV_I2C);
            return -MMA7660_READ_ERR;
        }
        if ((t & ALERT_MASK) == 0) {
            break;
        }
        retries--;
    }

    if (t & SIGN_BIT) {
        t |= SIGN_EXTEND;
    }
    *x = (int8_t)t;

    while (retries > 0) {
        if (i2c_read_reg(DEV_I2C, DEV_ADDR, MMA7660_YOUT, &t, 0) != 0) {
            i2c_release(DEV_I2C);
            return -MMA7660_READ_ERR;
        }
        if ((t & ALERT_MASK) == 0) {
            break;
        }
        retries--;
    }

    if (t & SIGN_BIT) {
        t |= SIGN_EXTEND;
    }
    *y = (int8_t)t;

    while (retries > 0) {
        if (i2c_read_reg(DEV_I2C, DEV_ADDR, MMA7660_ZOUT, &t, 0) != 0) {
            i2c_release(DEV_I2C);
            return -MMA7660_READ_ERR;
        }
        if ((t & ALERT_MASK) == 0) {
            break;
        }
        retries--;
    }

    if (t & SIGN_BIT) t |= SIGN_EXTEND;
      *z = (int8_t)t;

    i2c_release(DEV_I2C);
    if (retries > 0) {
        return MMA7660_OK;
    }

    return -MMA7660_READ_ERR;
}

int mma7660_read(const mma7660_t *dev, mma7660_data_t *data)
{
    int8_t countx, county, countz;
    int rv;

    rv = mma7660_read_counts(dev, &countx, &county, &countz);
    if (rv) {
        return rv;
    }

    data->x = ((int16_t)countx) * MMA7660_MG_PER_COUNT;
    data->y = ((int16_t)county) * MMA7660_MG_PER_COUNT;
    data->z = ((int16_t)countz) * MMA7660_MG_PER_COUNT;

    return MMA7660_OK;
}

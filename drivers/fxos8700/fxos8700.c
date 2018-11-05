/*
 * Copyright (C) 2018 UC Berkeley
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 * @brief       Driver for the FXOS8700 3-axis accelerometer/magnetometer
 *
 * @author      Michael Andersen <m.andersen@cs.berkeley.edu>
 * @author      Hyung-Sin Kim <hs.kim@cs.berkeleyedu>
 */


#include "periph/i2c.h"
#include "xtimer.h"
#include "fxos8700.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define I2C_SPEED                I2C_SPEED_FAST

#define FXOS8700_FULLDATA_LENGTH (12)

static int fxos8700_read_regs(const fxos8700_t* dev, uint8_t reg, uint8_t* data, size_t len)
{
    i2c_acquire(dev->p.i2c);
    if (i2c_read_regs(dev->p.i2c, dev->p.addr, reg, (char*) data, len, 0) < 0) {
        DEBUG("[fxos8700] Can't read register 0x%x\n", reg);
        i2c_release(dev->p.i2c);
        return FXOS8700_BUSERR;
    }
    i2c_release(dev->p.i2c);

    return FXOS8700_OK;
}

static int fxos8700_write_regs(const fxos8700_t* dev, uint8_t reg, uint8_t* data, size_t len)
{
    i2c_acquire(dev->p.i2c);
    if (i2c_write_regs(dev->p.i2c, dev->p.addr, reg, (char*) data, len, 0) < 0) {
        DEBUG("[fxos8700] Can't write to register 0x%x\n", reg);
        i2c_release(dev->p.i2c);
        return FXOS8700_BUSERR;
    }
    i2c_release(dev->p.i2c);

    return FXOS8700_OK;
}

int fxos8700_init(fxos8700_t* dev, const fxos8700_params_t *params)
{
    uint8_t config;

    if ((params->addr < 0x1C) || (params->addr > 0x1F)) {
        DEBUG("[fxos8700] Invalid address\n");
        return FXOS8700_ADDRERR;
    }
    dev->p.addr = params->addr;
    dev->p.i2c = params->i2c;
    dev->p.acc_range = params->acc_range;
    dev->p.renew_interval = params->renew_interval;

    i2c_acquire(dev->p.i2c);

    if (i2c_read_regs(dev->p.i2c, dev->p.addr, FXOS8700_REG_WHO_AM_I, &config, 1, 0) < 0) {
        i2c_release(dev->p.i2c);
        DEBUG("[fxos8700] Could not read WHOAMI\n");
        return FXOS8700_NOBUS;
    }
    if (config != FXOS8700_WHO_AM_I_VAL) {
        i2c_release(dev->p.i2c);
        DEBUG("[fxos8700] WHOAMI is wrong (%2x)\n", config);
        return FXOS8700_NODEV;
    }
    i2c_release(dev->p.i2c);

    /* Configure the ODR to maximum (400Hz in hybrid mode) */
    config = FXOS8700_REG_CTRL_REG1_ODR__400HZ & FXOS8700_REG_CTRL_REG1_MASK__ODR;
    if (fxos8700_write_regs(dev, FXOS8700_REG_CTRL_REG1, &config, 1) != FXOS8700_OK) {
        return FXOS8700_BUSERR;
    }
    dev->config = config;
    /* Activate hybrid mode */
    config = FXOS8700_REG_M_CTRL_REG1_HMS__HYBRID & FXOS8700_REG_M_CTRL_REG1_MASK__HMS;
    if (fxos8700_write_regs(dev, FXOS8700_REG_M_CTRL_REG1, &config, 1) != FXOS8700_OK) {
        return FXOS8700_BUSERR;
    }
    /* Set burst read mode (accel + magnet together) */
    config = FXOS8700_REG_M_CTRL_REG2_MASK__HYB_AUTOINC_MODE;
    if (fxos8700_write_regs(dev, FXOS8700_REG_M_CTRL_REG2, &config, 1) != FXOS8700_OK) {
        return FXOS8700_BUSERR;
    }
    /* Set accelerator's full-scale range */
    if (fxos8700_read_regs(dev, FXOS8700_REG_XYZ_DATA_CFG, &config, 1) != FXOS8700_OK) {
        return FXOS8700_BUSERR;
    }
    config &= ~FXOS8700_REG_XYZ_DATA_CFG_MASK__FS;
    config |= dev->p.acc_range & FXOS8700_REG_XYZ_DATA_CFG_MASK__FS;
    if (fxos8700_write_regs(dev, FXOS8700_REG_XYZ_DATA_CFG, &config, 1) != FXOS8700_OK) {
        return FXOS8700_BUSERR;
    }

    /* initial read for caching operation */
    if (fxos8700_read(dev, &dev->acc_cached, &dev->mag_cached) != FXOS8700_OK) {
        return FXOS8700_BUSERR;
    }
    dev->last_read_time = xtimer_now_usec();

    return FXOS8700_OK;
}


int fxos8700_set_active(const fxos8700_t* dev)
{
    uint8_t config = dev->config | FXOS8700_REG_CTRL_REG1_MASK__ACTIVE;
    if (fxos8700_write_regs(dev, FXOS8700_REG_CTRL_REG1, &config, 1) != FXOS8700_OK) {
        return FXOS8700_BUSERR;
    }
    return FXOS8700_OK;
}

int fxos8700_set_idle(const fxos8700_t* dev)
{
    uint8_t config = dev->config & ~FXOS8700_REG_CTRL_REG1_MASK__ACTIVE;
    if (fxos8700_write_regs(dev, FXOS8700_REG_CTRL_REG1, &config, 1) != FXOS8700_OK) {
        return FXOS8700_BUSERR;
    }
    return FXOS8700_OK;
}

int fxos8700_read(const fxos8700_t* dev, fxos8700_measurement_t* acc,
                  fxos8700_measurement_t* mag)
{
    uint8_t data[12];
    uint8_t ready = 0;

    if (fxos8700_set_active(dev)) {
        return FXOS8700_BUSERR;
    }

    while (!(ready & FXOS8700_REG_STATUS_MASK__XYZ_READY)) {
        fxos8700_read_regs(dev, FXOS8700_REG_STATUS, &ready, 1);
    }
    while (!(ready & FXOS8700_REG_M_DR_STATUS_MASK__XYZ_READY)) {
        fxos8700_read_regs(dev, FXOS8700_REG_M_DR_STATUS, &ready, 1);
    }

    /* Read all data at once */
    if (fxos8700_read_regs(dev, FXOS8700_REG_OUT_X_MSB, &data[0], FXOS8700_FULLDATA_LENGTH)) {
        return FXOS8700_BUSERR;
    }

    if (fxos8700_set_idle(dev)) {
        return FXOS8700_BUSERR;
    }

    /* Read accelerometer */
    if (acc) {
#if FXOS8700_USE_ACC_RAW_VALUES
        acc->x = (int16_t) ((data[0] << 8) | data[1]) >> 2;
        acc->y = (int16_t) ((data[2] << 8) | data[3]) >> 2;
        acc->z = (int16_t) ((data[4] << 8) | data[5]) >> 2;
#else
        int32_t acc_raw_x = (int16_t) ((data[0] << 8) | data[1]) >> 2;
        int32_t acc_raw_y = (int16_t) ((data[2] << 8) | data[3]) >> 2;
        int32_t acc_raw_z = (int16_t) ((data[4] << 8) | data[5]) >> 2;
        switch(dev->p.acc_range) {
            case FXOS8700_REG_XYZ_DATA_CFG_FS__2G:
                acc->x = (int16_t) ((acc_raw_x * 244) / 100);
                acc->y = (int16_t) ((acc_raw_y * 244) / 100);
                acc->z = (int16_t) ((acc_raw_z * 244) / 100);
                break;
            case FXOS8700_REG_XYZ_DATA_CFG_FS__4G:
                acc->x = (int16_t) ((acc_raw_x * 488) / 1000);
                acc->y = (int16_t) ((acc_raw_y * 488) / 1000);
                acc->z = (int16_t) ((acc_raw_z * 488) / 1000);
                break;
            case FXOS8700_REG_XYZ_DATA_CFG_FS__8G:
                acc->x = (int16_t) ((acc_raw_x * 976) / 1000);
                acc->y = (int16_t) ((acc_raw_y * 976) / 1000);
                acc->z = (int16_t) ((acc_raw_z * 976) / 1000);
                break;
            default:
                return FXOS8700_NODEV;
        }
#endif
    }
    /* Read magnetometer */
    if (mag) {
        mag->x = (int16_t) ((data[6] << 8) | data[7]);
        mag->y = (int16_t) ((data[8] << 8) | data[9]);
        mag->z = (int16_t) ((data[10] << 8) | data[11]);
    }
    return FXOS8700_OK;
}

int fxos8700_read_cached(const void *dev, fxos8700_measurement_t* acc,
                         fxos8700_measurement_t* mag)
{
    fxos8700_t* fxos_dev = (fxos8700_t *)dev;
    uint32_t now = xtimer_now_usec();

    /* check if readings are outdated */
    if (now - fxos_dev->last_read_time > fxos_dev->p.renew_interval) {
        /* refresh cache and update last_read_time */
        if (fxos8700_read(fxos_dev, &fxos_dev->acc_cached, &fxos_dev->mag_cached)
            != FXOS8700_OK) {
            return FXOS8700_BUSERR;
        }
        fxos_dev->last_read_time = now;
    }

    /* Read cached data */
    if (acc) {
        acc->x = fxos_dev->acc_cached.x;
        acc->y = fxos_dev->acc_cached.y;
        acc->z = fxos_dev->acc_cached.z;
    }
    if (mag) {
        mag->x = fxos_dev->mag_cached.x;
        mag->y = fxos_dev->mag_cached.y;
        mag->z = fxos_dev->mag_cached.z;
    }
    return FXOS8700_OK;
}

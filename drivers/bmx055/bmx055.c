/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_bmx055
 * @{
 *
 * @file
 * @brief       Device driver interface for the BMX055 9-axis sensor
 *
 * @author      Semjon Kerner <semjon.kerner@fu-berlin.de>
 * @}
 */

#include <string.h>
#include <stdint.h>
#include <math.h>

#include "periph/i2c.h"
#include "periph/gpio.h"
#include "assert.h"

#include "bmx055.h"
#include "bmx055_internal.h"

#define ENABLE_DEBUG    0
#include "debug.h"

#define BUS             (dev->p.i2c)
#define ADDR_MAG        (dev->p.addr_mag)
#define ADDR_ACC        (dev->p.addr_acc)
#define ADDR_GYRO       (dev->p.addr_gyro)
#define GYRO_2000_DPS   (2000U)
#define GYRO_MAX_SCALE  (0x7FFFU)

/**
 * @brief Array of available range values of accelerometer
 */
static const uint8_t acc_ranges[] = {
    BIT_ACC_RANGE_2G,
    BIT_ACC_RANGE_4G,
    BIT_ACC_RANGE_8G,
    BIT_ACC_RANGE_16G,
};

int bmx055_init(bmx055_t *dev, const bmx055_params_t *params)
{
    assert(dev && params);

    uint8_t tmp;

    dev->p = *params;

    /* bring magnetometer from suspend mode to sleep mode just in case
     * and try to read magnetometer id
     * NOTE: this is necessary because the module id is 0x00 in suspend mode
     */
    i2c_acquire(BUS);
    if (i2c_write_reg(BUS, ADDR_MAG, REG_MAG_PWRCTRL, BIT_MAG_PWRCTRL_VAL, 0x0) < 0) {
        DEBUG("[bmx055] error: no connection to magnetometer\n");
        i2c_release(BUS);
        return BMX055_NODEV;
    }
    if (i2c_read_reg(BUS, ADDR_MAG, REG_MAG_CHIPID, &tmp, 0x0) < 0) {
        DEBUG("[bmx055] error: no connection to magnetometer\n");
        i2c_release(BUS);
        return BMX055_NODEV;
    }
    if (tmp != REG_MAG_CHIPID_VAL) {
        DEBUG("[bmx055] error: no connection to magnetometer\n");
        i2c_release(BUS);
        return BMX055_NODEV;
    }

    /* try to read accelerometer id */
    if (i2c_read_reg(BUS, ADDR_ACC, REG_ACC_CHIPID, &tmp, 0x0) < 0) {
        DEBUG("[bmx055] error: no connection to accelerometer\n");
        i2c_release(BUS);
        return BMX055_NODEV;
    }
    if (tmp != REG_ACC_CHIPID_VAL) {
        DEBUG("[bmx055] error: no connection to accelerometer\n");
        i2c_release(BUS);
        return BMX055_NODEV;
    }

    /* try to read gyroscope id */
    if (i2c_read_reg(BUS, ADDR_GYRO, REG_GYRO_CHIPID, &tmp, 0x0) < 0) {
        DEBUG("[bmx055] error: no connection to gyroscope\n");
        i2c_release(BUS);
        return BMX055_NODEV;
    }
    if (tmp != REG_GYRO_CHIPID_VAL) {
        DEBUG("[bmx055] error: no connection to gyroscope\n");
        i2c_release(BUS);
        return BMX055_NODEV;
    }

    /* Init Magnetometer
     *
     * set magnetometer to normal mode (Bits 1 & 2 = 0x00)
     * and set magnetometer sample rate (Bits 3 to 5)
     */
    if (i2c_write_reg(BUS, ADDR_MAG, REG_MAG_OPMODE, (dev->p.mag_rate << 3), 0x0) < 0) {
        DEBUG("[bmx055] error: setting magnetometer opmode\n");
        i2c_release(BUS);
        return BMX055_NOWRITE;
    }

    /* Init Accelerometer
     *
     * softreset to bring module to normal mode
     */
    if (i2c_write_reg(BUS, ADDR_ACC, 0x14, 0xB6, 0x0) < 0) {
        DEBUG("[bmx055] error: setting accelerometer opmode\n");
        i2c_release(BUS);
        return BMX055_NOWRITE;
    }

    /* setting acc range */
    if (i2c_write_reg(BUS, ADDR_ACC, REG_ACC_RANGE, acc_ranges[dev->p.acc_range], 0x0) < 0) {
        DEBUG("[bmx055] error: setting accelerometer range\n");
        i2c_release(BUS);
        return BMX055_NOWRITE;
    }

    /* enable acc shadowing */
    if (i2c_write_reg(BUS, ADDR_ACC, REG_ACC_SHDW, REG_ACC_SHDW_ENABLE, 0x0) < 0) {
        DEBUG("[bmx055] error: writing accelerometer shadowing bit\n");
        i2c_release(BUS);
        return BMX055_NOWRITE;
    }

    /* Init Gyroscope
     *
     * The preferred way to bring the module to normal mode is using softreset.
     * However, a softreset brings the module into an unknown state and
     * deadlocks it. Hence it is not the way to go and normal mode is entered
     * by writing into power mode control register.
     */
    if (i2c_write_reg(BUS, ADDR_GYRO, REG_GYRO_PWRMD, REG_GYRO_PWRMD_NORM, 0x0) < 0) {
        DEBUG("[bmx055] error: setting gyroscope opmode\n");
        i2c_release(BUS);
        return BMX055_NOWRITE;
    }

    /* setting gyro scale */
    if (i2c_write_reg(BUS, ADDR_GYRO, REG_GYRO_SCALE, dev->p.gyro_scale, 0x0) < 0) {
        DEBUG("[bmx055] error: setting gyroscope scale\n");
        i2c_release(BUS);
        return BMX055_NOWRITE;
    }

    /* enable gyro shadowing */
    if (i2c_write_reg(BUS, ADDR_GYRO, REG_GYRO_SHDW, REG_GYRO_SHDW_EN, 0x0) < 0) {
        DEBUG("[bmx055] error: setting gyroscope shadowing bit\n");
        i2c_release(BUS);
        return BMX055_NOWRITE;
    }

    i2c_release(BUS);
    return BMX055_OK;
}

int bmx055_mag_read(const bmx055_t *dev, int16_t *data)
{
    assert(dev && data);

    uint8_t tmp[7];

    /* reading magnetometer data */
    i2c_acquire(BUS);
    if (i2c_read_regs(BUS, ADDR_MAG, REG_MAG_DATA, &tmp, 7, 0x0) < 0) {
        DEBUG("[bmx055] error: reading magnetometer data\n");
        i2c_release(BUS);
        return BMX055_NOREAD;
    }
    i2c_release(BUS);

    /* checking if new data was available */
    if ((tmp[6] & BIT_MAG_DATARDY) != 1) {
        DEBUG("[bmx055] error: no magnetometer data ready\n");
        return BMX055_NOTREADY;
    }

    /* scaling raw mag data to mGs */
    data[0] = (int16_t) (((int16_t)tmp[1] << 8) | tmp[0]) >> 3;
    data[1] = (int16_t) (((int16_t)tmp[3] << 8) | tmp[2]) >> 3;
    data[2] = (int16_t) (((int16_t)tmp[5] << 8) | tmp[4]) >> 1;

    return BMX055_OK;
}

int bmx055_acc_read(const bmx055_t *dev, int16_t *data)
{
    assert(dev && data);
    uint8_t tmp[7];

    /* reading accelerometer data */
    i2c_acquire(BUS);
    if (i2c_read_regs(BUS, ADDR_ACC, REG_ACC_DATA, &tmp, 7, 0x0) < 0) {
        DEBUG("[bmx055] error: reading accelerometer data\n");
        i2c_release(BUS);
        return BMX055_NOREAD;
    }
    i2c_release(BUS);

    if (((tmp[0] & 1) == 0) || ((tmp[2] & 1) == 0) || ((tmp[4] & 1) == 0)) {
        DEBUG("[bmx055] error: no acceleration data ready\n");
        return BMX055_NOTREADY;
    }

    /* scaling raw acc data to g */
    for (int i = 0; i < 3; i++) {
        data[i] = (int16_t) (((int16_t)tmp[(i * 2) + 1] << 8) | (tmp[i * 2] & 0xf0)) >> 4;
        data[i] <<= (dev->p.acc_range);
    }

    return BMX055_OK;
}

int bmx055_gyro_read(const bmx055_t *dev, int16_t *data)
{
    assert(dev && data);

    uint8_t tmp[6];
    int16_t shift[3];
    int32_t compensation[3];
    uint16_t scale;

    /* converting scale info into real scaling values */
    scale = (GYRO_2000_DPS / pow(2, dev->p.gyro_scale));

    /* reading gyroscope data */
    i2c_acquire(BUS);
    if (i2c_read_regs(BUS, ADDR_GYRO, REG_GYRO_DATA, &tmp, 6, 0x0) < 0) {
        DEBUG("[bmx055] error: reading gyroscope data\n");
        i2c_release(BUS);
        return BMX055_NOREAD;
    }
    i2c_release(BUS);

    for (int i = 0; i < 3; i++) {
        /* shifting and casting register data */
        shift[i] = (((tmp[(i * 2) + 1] << 8) & 0xFF00) | (tmp[(i * 2)] & 0x00FF));
        compensation[i] = (int32_t) shift[i];

        /* scaling raw gyro data to dps */
        compensation[i] *= scale;
        compensation[i] /= GYRO_MAX_SCALE;

        data[i] = (int16_t) compensation[i];
    }

    return BMX055_OK;
}

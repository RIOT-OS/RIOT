/*
 * Copyright (C) 2026 Technische Universität Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_qmi8658
 * @{
 *
 * @file
 * @brief       Device driver implementation for the QMI8658 IMU
 *
 * @author      Yahia Abdella <yahia.abdella@tuhh.de>
 *
 * @}
 */

#include "periph/i2c.h"
#include "qmi8658.h"
#include "qmi8658_constants.h"
#include "qmi8658_params.h"
#include "ztimer.h"
#include <assert.h>
#include <stdint.h>
#include <sys/errno.h>
#include "log.h"

#define ENABLE_DEBUG 1
#include "debug.h"

#define BUS         (dev->params.i2c)
#define ADDR        (dev->params.addr)

/**
 * order in array [0, 1, 2, 3] is
 * QMI8658_ACC_FS_2G, QMI8658_ACC_FS_4G, QMI8658_ACC_FS_8G, QMI8658_ACC_FS_16G
 */
static const int16_t range_acc[] = { 2000, 4000, 8000, 16000 };

/**
 * order in array [0, 1, 2, 3] is
 * QMI8658_GYRO_FS_245DPS, QMI8658_GYRO_FS_500DPS,
 * QMI8658_GYRO_FS_1000DPS, QMI8658_GYRO_FS_2000DPS
 */
// static const int16_t range_gyro[] = { 2450, 5000, 10000, 20000 };

int qmi8658_init(qmi8658_t *dev, const qmi8658_params_t *params)
{
    uint8_t tmp;
    int res;

    assert(dev && params);

    dev->params = *params;

    DEBUG("[LOG] qmi8658_init: Using I2C device %i\n", params->i2c);

    i2c_acquire(BUS);

    /* Soft reset */
    res = i2c_write_reg(BUS, ADDR, QMI8658_REG_RESET, QMI8658_RESET_VALUE, 0);
    /* Reset process needs 15 ms */
    ztimer_sleep(ZTIMER_MSEC, 15);
    /* Reset success is represented by reg 0x4D */
    res += i2c_read_reg(BUS, ADDR, 0x4D, &tmp, 0);
    /* Check if reset was successful */
    if (res < 0 || tmp != 0x80) {
        i2c_release(BUS);
        DEBUG("[ERROR] qmi8658_init: Failed to reset device\n");
        return -EIO;
    }

    /* Check WHO_AM_I value */
    res = i2c_read_reg(BUS, ADDR, QMI8658_REG_WHO_AM_I, &tmp, 0);
    if (res < 0 || tmp != QMI8658_WHO_AM_I_VALUE) {
        i2c_release(BUS);
        DEBUG("[ERROR] qmi8658_init: WHO_AM_I\n");
        return -EIO;
    }

    /* Set acc odr / full scale */
    tmp = dev->params.acc_odr | (dev->params.acc_fs << QMI8658_CTRL_FS_SHIFT);
    res = i2c_write_reg(BUS, ADDR, QMI8658_REG_CTRL2, tmp, 0);
    /* Set gyro odr / full scale */
    tmp = dev->params.gyro_odr | (dev->params.gyro_fs << QMI8658_CTRL_FS_SHIFT);
    res += i2c_write_reg(BUS, ADDR, QMI8658_REG_CTRL3, tmp, 0);

    i2c_release(BUS);

    if (res < 0) {
        DEBUG("[ERROR] qmi8658_init: Config\n");
        return -EIO;
    }

    LOG_INFO("qmi8658_init(): QMI8658 initialized.\n");

    return 0;
}

int qmi8658_enable_sensors(qmi8658_t* dev, uint8_t sensor_flags)
{
    int res;

    /* If Accelerometer ODR is higher than 1kHz, Gyroscope hast to be enabled*/
    if ((dev->params.acc_odr < QMI8658_DATA_RATE_1KHZ) && (sensor_flags & QMI8658_ENABLE_ACC)) {
        LOG_INFO("qmi8658_enable_sensors(): High accelerometer ODR is set, automatically enabling gyroscope.\n");
        sensor_flags |= QMI8658_ENABLE_GYRO;
    }

    i2c_acquire(BUS);

    res = i2c_write_reg(BUS, ADDR, QMI8658_REG_CTRL7, sensor_flags & 0x03, 0);
    
    i2c_release(BUS);
    
    if (res < 0) {
        DEBUG("[ERROR] qmi8658_enable_sensors\n");
        return -EIO;
    }

    DEBUG("[LOG] qmi8658_enable_sensors: sensors enabled\n");

    return 0;
}

int qmi8658_read_acc(const qmi8658_t *dev, qmi8658_3d_data_t *data)
{
    int res;
    uint8_t tmp;

    i2c_acquire(BUS);

    /* Check if data is available */
    i2c_read_reg(BUS, ADDR, QMI8658_REG_STATUS0, &tmp, 0);
    DEBUG("[LOG] qmi8658_read_acc: status: %x\n", tmp);
    if (!(tmp & QMI8658_STATUS0_ACC_MASK)) {
        i2c_release(BUS);
        LOG_WARNING("qmi8658_read_acc(): No data available.\n");
        return -ENODATA;
    }

    res = i2c_read_reg(BUS, ADDR, QMI8658_REG_AX_L, &tmp, 0);
    data->x = tmp;
    res += i2c_read_reg(BUS, ADDR, QMI8658_REG_AX_H, &tmp, 0);
    data->x |= tmp << 8;
    res += i2c_read_reg(BUS, ADDR, QMI8658_REG_AY_L, &tmp, 0);
    data->y = tmp;
    res += i2c_read_reg(BUS, ADDR, QMI8658_REG_AY_H, &tmp, 0);
    data->y |= tmp << 8;
    res += i2c_read_reg(BUS, ADDR, QMI8658_REG_AZ_L, &tmp, 0);
    data->z = tmp;
    res += i2c_read_reg(BUS, ADDR, QMI8658_REG_AZ_H, &tmp, 0);
    data->z |= tmp << 8;

    i2c_release(BUS);

    if (res < 0) {
        DEBUG("[ERROR] qmi8658_read_acc\n");
        return -EIO;
    }

    data->x = ((int32_t)data->x * range_acc[dev->params.acc_fs]) / INT16_MAX;
    data->y = ((int32_t)data->y * range_acc[dev->params.acc_fs]) / INT16_MAX;
    data->z = ((int32_t)data->z * range_acc[dev->params.acc_fs]) / INT16_MAX;

    return 0;
}

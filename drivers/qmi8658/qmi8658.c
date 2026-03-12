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
 * order in array [0, 1, 2, 3, 4, 5, 6, 7] is
 *  QMI8658_GYRO_FS_16DPS, QMI8658_GYRO_FS_32DPS, QMI8658_GYRO_FS_64DPS,
 *  QMI8658_GYRO_FS_128DPS, QMI8658_GYRO_FS_256DPS, QMI8658_GYRO_FS_512DPS,
 *  QMI8658_GYRO_FS_1024DPS, QMI8658_GYRO_FS_2048DPS
 */
static const int16_t range_gyro[] = { 160, 320, 640, 1280, 2560, 5120, 10240, 20480 };

int qmi8658_init(qmi8658_t *dev, const qmi8658_params_t *params)
{
    assert(dev && params);
    
    uint8_t tmp;
    int res;

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

    /* Enable address auto increment */
    res = i2c_write_reg(BUS, ADDR, QMI8658_REG_CTRL1, QMI8658_CTRL1_ADDR_AI_MASK, 0);

    /* Set acc odr / full scale */
    tmp = dev->params.acc_odr | (dev->params.acc_fs << QMI8658_CTRL_FS_SHIFT);
    res += i2c_write_reg(BUS, ADDR, QMI8658_REG_CTRL2, tmp, 0);
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
    assert(dev);

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

static int _qmi8658_read_sensor(const qmi8658_t *dev, qmi8658_3d_data_t *data, uint8_t sensor)
{
    int res;
    uint8_t tmp;
    uint8_t tmp_arr[6];

    uint8_t status_mask;
    uint8_t data_reg;
    switch (sensor) {
        case QMI8658_SENSOR_ACC:
            status_mask = QMI8658_STATUS0_ACC_MASK;
            data_reg = QMI8658_REG_AX_L;
            break;
        case QMI8658_SENSOR_GYRO:
            status_mask = QMI8658_STATUS0_GYRO_MASK;
            data_reg = QMI8658_REG_GX_L;
            break;
        default:
            return -EINVAL;
    }

    i2c_acquire(BUS);

    /* Check if data is available */
    i2c_read_reg(BUS, ADDR, QMI8658_REG_STATUS0, &tmp, 0);
    DEBUG("[LOG] qmi8658_read_sensor: status: %x\n", tmp);
    if (!(tmp & status_mask)) {
        i2c_release(BUS);
        LOG_WARNING("qmi8658_read_sensor(): No data available.\n");
        return -ENODATA;
    }

    /* 6 byte burst read, register address is incremented automatically */
    res = i2c_read_regs(BUS, ADDR, data_reg, tmp_arr, 6, 0);
    
    i2c_release(BUS);
    
    if (res < 0) {
        DEBUG("[ERROR] qmi8658_read_sensor\n");
        return -EIO;
    }

    /* Write raw data to buffer */
    data->x = (int16_t)(tmp_arr[0] | (tmp_arr[1] << 8));
    data->y = (int16_t)(tmp_arr[2] | (tmp_arr[3] << 8));
    data->z = (int16_t)(tmp_arr[4] | (tmp_arr[5] << 8));

    return 0;
}

int qmi8658_read_acc(const qmi8658_t *dev, qmi8658_3d_data_t *data)
{
    assert(dev && data);

    int res = _qmi8658_read_sensor(dev, data, QMI8658_SENSOR_ACC);
    if (res < 0) {
        DEBUG("[ERROR] qmi8658_read_acc\n");
        return res;
    }

    /* Scale raw data to the correct range */
    data->x = (int16_t)(((int32_t)data->x * range_acc[dev->params.acc_fs]) / INT16_MAX);
    data->y = (int16_t)(((int32_t)data->y * range_acc[dev->params.acc_fs]) / INT16_MAX);
    data->z = (int16_t)(((int32_t)data->z * range_acc[dev->params.acc_fs]) / INT16_MAX);

    return 0;
}

int qmi8658_read_gyro(const qmi8658_t *dev, qmi8658_3d_data_t *data)
{
    assert(dev && data);

    int res = _qmi8658_read_sensor(dev, data, QMI8658_SENSOR_GYRO);
    if (res < 0) {
        DEBUG("[ERROR] qmi8658_read_gyro\n");
        return res;
    }

    /* Scale raw data to the correct range */
    data->x = (int16_t)(((int32_t)data->x * range_gyro[dev->params.gyro_fs]) / INT16_MAX);
    data->y = (int16_t)(((int32_t)data->y * range_gyro[dev->params.gyro_fs]) / INT16_MAX);
    data->z = (int16_t)(((int32_t)data->z * range_gyro[dev->params.gyro_fs]) / INT16_MAX);

    return 0;
}

int qmi8658_read_temp(const qmi8658_t *dev, int16_t *data)
{
    int res;
    uint8_t tmp[2];

    i2c_acquire(BUS);

    /* Read the 2 temperature bytes */
    res = i2c_read_regs(BUS, ADDR, QMI8658_REG_TEMP_L, tmp, 2, 0);

    i2c_release(BUS);

    if (res < 0) {
        DEBUG("[ERROR] qmi8658_read_temp\n");
        return -EIO;
    }

    /* Raw reading */
    *data = (int16_t)(tmp[0] | (tmp[1] << 8));
    /* Scale raw data to degC x 100 range */
    *data = (int16_t)(((int32_t)(*data) * 100) / QMI8658_TEMP_SCALE_FACTOR);

    return 0;
}

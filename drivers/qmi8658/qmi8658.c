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

#include <assert.h>
#include <stdint.h>
#include <sys/errno.h>
#include "periph/i2c.h"
#include "ztimer.h"
#include "log.h"
#include "qmi8658.h"
#include "qmi8658_constants.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define BUS         (dev->params.i2c)
#define ADDR        (dev->params.addr)

/*
 * order in array [0, 1, 2, 3] is
 * QMI8658_ACC_FS_2G, QMI8658_ACC_FS_4G, QMI8658_ACC_FS_8G, QMI8658_ACC_FS_16G
 */
static const int16_t range_acc[] = { 2000, 4000, 8000, 16000 };

/*
 * order in array [0, 1, 2, 3, 4, 5, 6, 7] is
 * QMI8658_GYRO_FS_16DPS, QMI8658_GYRO_FS_32DPS, QMI8658_GYRO_FS_64DPS,
 * QMI8658_GYRO_FS_128DPS, QMI8658_GYRO_FS_256DPS, QMI8658_GYRO_FS_512DPS,
 * QMI8658_GYRO_FS_1024DPS, QMI8658_GYRO_FS_2048DPS
 */
static const int16_t range_gyro[] = { 160, 320, 640, 1280, 2560, 5120, 10240, 20480 };

/* Forward declarations */
static int _qmi8658_read_sensor(const qmi8658_t *dev, qmi8658_3d_data_t *data,
                                qmi8658_sensor_id_t sensor);
static int _qmi8658_set_sensors(const qmi8658_t *dev, qmi8658_enable_flag_t sensor_enable_flags);

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

    i2c_release(BUS);

    if (res < 0) {
        DEBUG("[ERROR] qmi8658_init: Config\n");
        return -EIO;
    }

    LOG_INFO("qmi8658_init(): QMI8658 initialized.\n");

    return 0;
}

int qmi8658_set_mode(const qmi8658_t *dev, qmi8658_mode_t mode)
{
    assert(dev);

    DEBUG("[LOG] qmi8658_set_mode: %i\n", mode);

    uint8_t reg_ctrl2_value = 0;
    uint8_t reg_ctrl3_value = 0;
    qmi8658_enable_flag_t sensor_enable_flags = QMI8658_DISABLE_ALL;
    int res = 0;

    /* Disable sensors first */
    res = _qmi8658_set_sensors(dev, QMI8658_DISABLE_ALL);
    if (res < 0) {
        DEBUG("[ERROR] qmi8658_set_mode: Disable sensors\n");
        return -EIO;
    }

    i2c_acquire(BUS);

    switch (mode) {
    case QMI8658_POWER_DOWN:
        res = i2c_write_reg(BUS, ADDR, QMI8658_REG_CTRL1, QMI8658_CTRL1_SENSOR_DISABLE_MASK, 0);
        break;

    case QMI8658_NORMAL_ACC:
        /* Set acc odr / full scale */
        reg_ctrl2_value = dev->params.acc_odr | (dev->params.acc_fs << QMI8658_CTRL_FS_SHIFT);
        /* Enable Acc */
        sensor_enable_flags = QMI8658_ENABLE_ACC;
        /* If Accelerometer ODR is higher than 1kHz, Gyroscope has to be enabled */
        if (dev->params.acc_odr < QMI8658_DATA_RATE_1KHZ) {
            LOG_INFO(
                "qmi8658_enable_sensors(): High accelerometer ODR is set, automatically enabling gyroscope.\n");
            /* Set gyro odr / full scale */
            reg_ctrl3_value = dev->params.gyro_odr | (dev->params.gyro_fs << QMI8658_CTRL_FS_SHIFT);
            sensor_enable_flags |= QMI8658_ENABLE_GYRO;
        }
        break;

    case QMI8658_LOWPWR_ACC:
        /* Set acc low power odr / full scale */
        reg_ctrl2_value = dev->params.acc_lowpwr_odr | (dev->params.acc_fs <<
                                                            QMI8658_CTRL_FS_SHIFT);
        /* Enable Acc */
        sensor_enable_flags = QMI8658_ENABLE_ACC;
        break;

    case QMI8658_NORMAL_GYRO:
        /* Set gyro odr / full scale */
        reg_ctrl3_value = dev->params.gyro_odr | (dev->params.gyro_fs << QMI8658_CTRL_FS_SHIFT);
        /* Enable Gyro */
        sensor_enable_flags = QMI8658_ENABLE_GYRO;
        break;

    case QMI8658_NORMAL_ACC_GYRO:
        /* Set acc odr / full scale */
        reg_ctrl2_value = dev->params.acc_odr | (dev->params.acc_fs << QMI8658_CTRL_FS_SHIFT);
        /* Set gyro odr / full scale */
        reg_ctrl3_value = dev->params.gyro_odr | (dev->params.gyro_fs << QMI8658_CTRL_FS_SHIFT);
        /* Enable Acc and Gyro */
        sensor_enable_flags = QMI8658_ENABLE_ACC | QMI8658_ENABLE_GYRO;
        break;
    }

    if (mode != QMI8658_POWER_DOWN) {
        /* Clear sensor disable */
        res += i2c_write_reg(BUS, ADDR, QMI8658_REG_CTRL1, QMI8658_CTRL1_ADDR_AI_MASK, 0);
    }
    if (reg_ctrl2_value != 0) {
        res += i2c_write_reg(BUS, ADDR, QMI8658_REG_CTRL2, reg_ctrl2_value, 0);
    }
    if (reg_ctrl3_value != 0) {
        res += i2c_write_reg(BUS, ADDR, QMI8658_REG_CTRL3, reg_ctrl3_value, 0);
    }

    /* Filters need 3/ODR seconds to settle (Datasheet section 7.3)*/
    if (mode != QMI8658_LOWPWR_ACC) {
        /* Worst case in normal mode is around 100ms at ODR = 31.25Hz */
        ztimer_sleep(ZTIMER_MSEC, 100);
    }
    else {
        /* Worst case in low power mode is around 1s at ODR = 3Hz */
        ztimer_sleep(ZTIMER_MSEC, 1000);
    }

    i2c_release(BUS);

    if (res < 0) {
        DEBUG("[ERROR] qmi8658_set_mode\n");
        return -EIO;
    }

    res = _qmi8658_set_sensors(dev, sensor_enable_flags);
    if (res < 0) {
        DEBUG("[ERROR] qmi8658_set_mode: Disable sensors\n");
        return -EIO;
    }

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

static int _qmi8658_read_sensor(const qmi8658_t *dev, qmi8658_3d_data_t *data,
                                qmi8658_sensor_id_t sensor)
{
    int res;
    uint8_t tmp;
    uint8_t tmp_arr[6];

    uint8_t data_reg;

    switch (sensor) {
    case QMI8658_SENSOR_ACC:
        data_reg = QMI8658_REG_AX_L;
        break;
    case QMI8658_SENSOR_GYRO:
        data_reg = QMI8658_REG_GX_L;
        break;
    default:
        return EINVAL;
        break;
    }

    i2c_acquire(BUS);

    /* Check if data is available */
    i2c_read_reg(BUS, ADDR, QMI8658_REG_STATUS0, &tmp, 0);
    DEBUG("[LOG] qmi8658_read_sensor: status: %x\n", tmp);

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

static int _qmi8658_set_sensors(const qmi8658_t *dev, qmi8658_enable_flag_t sensor_enable_flags)
{
    int res;

    DEBUG("[LOG] Set sensor enable flags %x\n", sensor_enable_flags);

    i2c_acquire(BUS);

    res = i2c_write_reg(BUS, ADDR, QMI8658_REG_CTRL7, sensor_enable_flags & 0x03, 0);

    i2c_release(BUS);

    if (res < 0) {
        DEBUG("[ERROR] qmi8658_enable_sensors\n");
        return -EIO;
    }

    return 0;
}

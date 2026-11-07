/*
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
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
#include <errno.h>
#include "periph/i2c.h"
#include "ztimer.h"
#include "log.h"
#include "qmi8658.h"
#include "qmi8658_params.h"
#include "qmi8658_constants.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/** Helper macros */
#define QMI8658_BUS         (dev->params.i2c)
#define QMI8658_ADDR        (dev->params.addr)
/** Helper to mask out the bottom n bits of a byte */
#define QMI8658_MASK_BITS(n) ((1 << (n)) - 1)

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

/*
 * order in array [0, 1, 2, 3, 4, 5, 6, 7, 8] is
 * QMI8658_DATA_RATE_8KHZ, QMI8658_DATA_RATE_4KHZ, QMI8658_DATA_RATE_2KHZ, QMI8658_DATA_RATE_1KHZ,
 * QMI8658_DATA_RATE_500HZ, QMI8658_DATA_RATE_250HZ, QMI8658_DATA_RATE_125HZ,
 * QMI8658_DATA_RATE_62_5HZ, QMI8658_DATA_RATE_31_25HZ
 */
static const int16_t odrs[] = { 8000, 4000, 2000, 1000, 500, 250, 125, 63, 31 };

/* Forward declarations */
static int _qmi8658_read_sensor(const qmi8658_t *dev, qmi8658_3d_data_t *data,
                                qmi8658_sensor_id_t sensor);
static int _qmi8658_set_sensors(const qmi8658_t *dev, qmi8658_enable_flag_t sensor_enable_flags);
static int _qmi8658_write_cal_regs(const qmi8658_t *dev, uint8_t *data);
static int _qmi8658_ctrl9_cmd(const qmi8658_t *dev, uint8_t cmd);

/* Convert a time in ms to number of samples (based on ACC ODR) */
static inline uint16_t _qmi8658_ms_to_sample(const qmi8658_t *dev, uint16_t time)
{
    return (uint16_t)(((uint32_t)time * odrs[dev->params.acc_odr]) / 1000);
}

/* Scale tap alpha/gamma ratio to the correct range */
static inline uint16_t _qmi8658_tap_scale_ratio(uint16_t ratio)
{
    return (uint16_t)(((uint32_t)ratio * 128) / 10000);
}

/* Scale tap threshold to the correct range */
static inline uint16_t _qmi8658_tap_scale_thr(uint16_t thr)
{
    return (uint16_t)(((uint32_t)thr * 1024) / 1000);
}

int qmi8658_init(qmi8658_t *dev, const qmi8658_params_t *params)
{
    assert(dev && params);

    /* Make sure configuration is valid */
    assert(
        params->acc_odr < QMI8658_DATA_RATE_31_25HZ &&
        params->acc_lowpwr_odr < QMI8658_ACC_LOWPWR_DATA_RATE_3HZ &&
        params->acc_lowpwr_odr > QMI8658_ACC_LOWPWR_DATA_RATE_128HZ &&
        params->acc_fs < QMI8658_ACC_FS_16G &&
        params->gyro_odr < QMI8658_DATA_RATE_31_25HZ &&
        params->gyro_fs < QMI8658_GYRO_FS_2048DPS
        );

    uint8_t tmp;
    int res;

    dev->params = *params;
    dev->enable_flags = QMI8658_DISABLE_ALL;
    dev->initialized = false;

    DEBUG("[LOG] qmi8658_init: Using I2C device %i\n", params->i2c);

    i2c_acquire(QMI8658_BUS);

    /* Soft reset */
    res = i2c_write_reg(QMI8658_BUS, QMI8658_ADDR, QMI8658_REG_RESET, QMI8658_RESET_VALUE, 0);
    /* Reset process needs 15 ms */
    ztimer_sleep(ZTIMER_MSEC, 15);
    /* Reset success is represented by reg 0x4D */
    res += i2c_read_reg(QMI8658_BUS, QMI8658_ADDR, 0x4D, &tmp, 0);
    /* Check if reset was successful */
    if (res < 0 || tmp != 0x80) {
        i2c_release(QMI8658_BUS);
        DEBUG("[ERROR] qmi8658_init: Failed to reset device\n");
        return -EIO;
    }

    /* Check WHO_AM_I value */
    res = i2c_read_reg(QMI8658_BUS, QMI8658_ADDR, QMI8658_REG_WHO_AM_I, &tmp, 0);
    if (res < 0 || tmp != QMI8658_WHO_AM_I_VALUE) {
        i2c_release(QMI8658_BUS);
        DEBUG("[ERROR] qmi8658_init: Failed to read WHO_AM_I value (or read incorrect value)\n");
        return -EIO;
    }

    /* Enable address auto increment */
    res = i2c_write_reg(QMI8658_BUS, QMI8658_ADDR, QMI8658_REG_CTRL1, QMI8658_CTRL1_ADDR_AI_MASK,
                        0);

    i2c_release(QMI8658_BUS);

    if (res < 0) {
        DEBUG("[ERROR] qmi8658_init: Failed to configure serial interface\n");
        return -EIO;
    }

    dev->initialized = true;
    LOG_INFO("qmi8658_init(): QMI8658 initialized.\n");

    return 0;
}

int qmi8658_set_mode(qmi8658_t *dev, qmi8658_mode_t mode)
{
    assert(dev);

    if (!dev->initialized) {
        return -EPERM;
    }

    DEBUG("[LOG] qmi8658_set_mode: mode = %i\n", mode);

    uint8_t reg_ctrl2_value = 0;
    uint8_t reg_ctrl3_value = 0;
    uint8_t tmp;
    int res = 0;

    /* Disable sensors first */
    res = _qmi8658_set_sensors(dev, QMI8658_DISABLE_ALL);
    if (res < 0) {
        DEBUG("[ERROR] qmi8658_set_mode: Failed to disable sensors\n");
        return -EIO;
    }

    i2c_acquire(QMI8658_BUS);

    switch (mode) {
    case QMI8658_POWER_DOWN:
        res = i2c_read_reg(QMI8658_BUS, QMI8658_ADDR, QMI8658_REG_CTRL1, &tmp, 0);
        res += i2c_write_reg(QMI8658_BUS, QMI8658_ADDR, QMI8658_REG_CTRL1,
                             tmp | QMI8658_CTRL1_SENSOR_DISABLE_MASK, 0);
        dev->enable_flags = QMI8658_DISABLE_ALL;
        break;

    case QMI8658_NORMAL_ACC:
        /* Set acc odr / full scale */
        reg_ctrl2_value = dev->params.acc_odr | (dev->params.acc_fs << QMI8658_CTRL_FS_SHIFT);
        /* Enable Acc */
        dev->enable_flags = QMI8658_ENABLE_ACC;
        /* If Accelerometer ODR is higher than 1kHz, Gyroscope has to be enabled */
        if (dev->params.acc_odr < QMI8658_DATA_RATE_1KHZ) {
            LOG_INFO(
                "qmi8658_set_mode(): High accelerometer ODR is set, automatically enabling gyroscope.\n");
            /* Set gyro odr / full scale */
            reg_ctrl3_value = dev->params.gyro_odr | (dev->params.gyro_fs << QMI8658_CTRL_FS_SHIFT);
            dev->enable_flags |= QMI8658_ENABLE_GYRO;
        }
        break;

    case QMI8658_LOWPWR_ACC:
        /* Set acc low power odr / full scale */
        reg_ctrl2_value = dev->params.acc_lowpwr_odr | (dev->params.acc_fs <<
                                                        QMI8658_CTRL_FS_SHIFT);
        /* Enable Acc */
        dev->enable_flags = QMI8658_ENABLE_ACC;
        break;

    case QMI8658_NORMAL_GYRO:
        /* Set gyro odr / full scale */
        reg_ctrl3_value = dev->params.gyro_odr | (dev->params.gyro_fs << QMI8658_CTRL_FS_SHIFT);
        /* Enable Gyro */
        dev->enable_flags = QMI8658_ENABLE_GYRO;
        break;

    case QMI8658_NORMAL_ACC_GYRO:
        /* Set acc odr / full scale */
        reg_ctrl2_value = dev->params.acc_odr | (dev->params.acc_fs << QMI8658_CTRL_FS_SHIFT);
        /* Set gyro odr / full scale */
        reg_ctrl3_value = dev->params.gyro_odr | (dev->params.gyro_fs << QMI8658_CTRL_FS_SHIFT);
        /* Enable Acc and Gyro */
        dev->enable_flags = QMI8658_ENABLE_ACC | QMI8658_ENABLE_GYRO;
        break;
    default:
        i2c_release(QMI8658_BUS);
        LOG_ERROR("qmi8658_set_mode(): Invalid mode.\n");
        return -EINVAL;
    }

    if (mode != QMI8658_POWER_DOWN) {
        /* Clear sensor disable */
        res = i2c_read_reg(QMI8658_BUS, QMI8658_ADDR, QMI8658_REG_CTRL1, &tmp, 0);
        res += i2c_write_reg(QMI8658_BUS, QMI8658_ADDR, QMI8658_REG_CTRL1,
                             tmp & (~QMI8658_CTRL1_SENSOR_DISABLE_MASK), 0);
    }
    if (reg_ctrl2_value != 0) {
        res += i2c_write_reg(QMI8658_BUS, QMI8658_ADDR, QMI8658_REG_CTRL2, reg_ctrl2_value, 0);
    }
    if (reg_ctrl3_value != 0) {
        res += i2c_write_reg(QMI8658_BUS, QMI8658_ADDR, QMI8658_REG_CTRL3, reg_ctrl3_value, 0);
    }

    i2c_release(QMI8658_BUS);

    /* Filters need 3/ODR seconds to settle (Datasheet section 7.3) */
    if (mode != QMI8658_LOWPWR_ACC) {
        /* Worst case in normal mode is around 100ms at ODR = 31.25Hz */
        ztimer_sleep(ZTIMER_MSEC, QMI8658_NORMAL_FILTER_WAIT_MS);
    }
    else {
        /* Worst case in low power mode is around 1s at ODR = 3Hz */
        ztimer_sleep(ZTIMER_MSEC, QMI8658_LOWPWR_FILTER_WAIT_MS);
    }

    if (res < 0) {
        DEBUG("[ERROR] qmi8658_set_mode: Failed to write sensor settings\n");
        return -EIO;
    }

    res = _qmi8658_set_sensors(dev, dev->enable_flags);
    if (res < 0) {
        DEBUG("[ERROR] qmi8658_set_mode: Failed to enable sensors\n");
        return -EIO;
    }

    LOG_INFO("qmi8658_set_mode(): QMI8658 mode set.\n");

    return 0;
}

int qmi8658_enable_tap(const qmi8658_t *dev)
{
    assert(dev);

    if (!dev->initialized) {
        return -EPERM;
    }

    int res;
    uint8_t tmp;
    uint8_t tap_config[8];

    /* Disable sensors first */
    res = _qmi8658_set_sensors(dev, QMI8658_DISABLE_ALL);
    if (res < 0) {
        DEBUG("[ERROR] qmi8658_enable_tap: Failed to disable sensors\n");
        return -EIO;
    }

    /* First set of CAL configuration values */
    /* PeakWindow in samples */
    tap_config[0] = _qmi8658_ms_to_sample(dev, QMI8658_PARAM_TAP_PEAKWIN);
    /* Tap axis priority */
    tap_config[1] = QMI8658_PARAM_TAP_PRIORITY;
    /* TapWindow in samples */
    uint16_t tap_win = _qmi8658_ms_to_sample(dev, QMI8658_PARAM_TAP_TAPWIN);
    tap_config[2] = tap_win & 0xFF;
    tap_config[3] = tap_win >> 8;
    /* DTapWindow in samples */
    uint16_t dtap_win = _qmi8658_ms_to_sample(dev, QMI8658_PARAM_TAP_DTAPWIN);
    tap_config[4] = dtap_win & 0xFF;
    tap_config[5] = dtap_win >> 8;
    tap_config[6] = 0;                  /* N/A */
    tap_config[7] = 0x01;               /* This is the first command */
    res = _qmi8658_write_cal_regs(dev, tap_config);
    res += _qmi8658_ctrl9_cmd(dev, QMI8658_CTRL9_CMD_CONFIG_TAP);

    /* Second set of CAL configuration values */
    /* Alpha */
    tap_config[0] = _qmi8658_tap_scale_ratio(QMI8658_PARAM_TAP_ALPHA);
    /* Gamma */
    tap_config[1] = _qmi8658_tap_scale_ratio(QMI8658_PARAM_TAP_GAMMA);
    /* PeakMagThr */
    uint16_t peak_mag_thr = _qmi8658_tap_scale_thr(QMI8658_PARAM_TAP_PEAKMAGTHR);
    tap_config[2] = peak_mag_thr & 0xFF;
    tap_config[3] = peak_mag_thr >> 8;
    /* UDMThr */
    uint16_t udm_thr = _qmi8658_tap_scale_thr(QMI8658_PARAM_TAP_UDMTHR);
    tap_config[4] = udm_thr & 0xFF;
    tap_config[5] = udm_thr >> 8;
    tap_config[6] = 0;              /* N/A */
    tap_config[7] = 0x02;           /* This is the second command */
    res += _qmi8658_write_cal_regs(dev, tap_config);
    res += _qmi8658_ctrl9_cmd(dev, QMI8658_CTRL9_CMD_CONFIG_TAP);

    if (res < 0) {
        DEBUG("[ERROR] qmi8658_enable_tap: Failed to configure Tap detection\n");
        return -EIO;
    }

    DEBUG("[LOG] qmi8658_enable_tap: Tap detection configured.\n");

    /* Enable sensors */
    res = _qmi8658_set_sensors(dev, dev->enable_flags);

    i2c_acquire(QMI8658_BUS);

    /* Enable Tap engine */
    res += i2c_read_reg(QMI8658_BUS, QMI8658_ADDR, QMI8658_REG_CTRL8, &tmp, 0);
    res += i2c_write_reg(QMI8658_BUS, QMI8658_ADDR, QMI8658_REG_CTRL8,
                         tmp | QMI8658_CTRL8_TAP_EN_MASK, 0);

    i2c_release(QMI8658_BUS);

    if (res < 0) {
        DEBUG("[ERROR] qmi8658_enable_tap: Failed to enable Tap detection\n");
        return -EIO;
    }

    return 0;
}

int qmi8658_disable_tap(const qmi8658_t *dev)
{
    assert(dev);

    int res;
    uint8_t tmp;

    i2c_acquire(QMI8658_BUS);

    /* Disable Tap engine */
    res = i2c_read_reg(QMI8658_BUS, QMI8658_ADDR, QMI8658_REG_CTRL8, &tmp, 0);
    res += i2c_write_reg(QMI8658_BUS, QMI8658_ADDR, QMI8658_REG_CTRL8,
                         tmp & (~QMI8658_CTRL8_TAP_EN_MASK), 0);

    i2c_release(QMI8658_BUS);

    if (res < 0) {
        DEBUG("[ERROR] qmi8658_disable_tap: Failed to disable Tap detection\n");
        return -EIO;
    }

    return 0;
}

int qmi8658_read_acc(const qmi8658_t *dev, qmi8658_3d_data_t *data)
{
    assert(dev && data);

    int res = _qmi8658_read_sensor(dev, data, QMI8658_SENSOR_ACC);
    if (res < 0) {
        DEBUG("[ERROR] qmi8658_read_acc: failed to read sensor\n");
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
        DEBUG("[ERROR] qmi8658_read_gyro: failed to read sensor\n");
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
    assert(dev && data);

    int res;
    uint8_t tmp[2];

    i2c_acquire(QMI8658_BUS);

    /* Read the 2 temperature bytes */
    res = i2c_read_regs(QMI8658_BUS, QMI8658_ADDR, QMI8658_REG_TEMP_L, tmp, 2, 0);

    i2c_release(QMI8658_BUS);

    if (res < 0) {
        DEBUG("[ERROR] qmi8658_read_temp: failed to read sensor\n");
        return -EIO;
    }

    /* Raw reading */
    *data = (int16_t)(tmp[0] | (tmp[1] << 8));
    /* Scale raw data to degC x 100 range */
    *data = (int16_t)(((int32_t)(*data) * 100) / QMI8658_TEMP_SCALE_FACTOR);

    return 0;
}

int qmi8658_read_tap(const qmi8658_t *dev, qmi8658_tap_data_t *data)
{
    assert(dev && data);

    int res;
    uint8_t tmp;
    uint8_t status1;

    i2c_acquire(QMI8658_BUS);

    /* Read data availability */
    res = i2c_read_reg(QMI8658_BUS, QMI8658_ADDR, QMI8658_REG_STATUS1, &status1, 0);
    /* Read Tap data */
    res += i2c_read_reg(QMI8658_BUS, QMI8658_ADDR, QMI8658_REG_TAP_STATUS, &tmp, 0);

    i2c_release(QMI8658_BUS);

    if (res < 0) {
        DEBUG("[ERROR] qmi8658_read_tap: failed to read tap status\n");
        return -EIO;
    }

    if (!(status1 & QMI8658_STATUS1_TAP_MASK)) {
        /* No tap was detected, set number to 0 */
        data->num = 0;
        return 0;
    }

    /* Tap was detected */
    data->num = (tmp >> QMI8658_TAP_NUM_SHIFT) & QMI8658_MASK_BITS(2);
    data->axis = (tmp >> QMI8658_TAP_AXIS_SHIFT) & QMI8658_MASK_BITS(2);
    data->neg_polarity = (tmp >> QMI8658_TAP_POLARITY_SHIFT) & QMI8658_MASK_BITS(1);

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
        return -EINVAL;
        break;
    }

    i2c_acquire(QMI8658_BUS);

    /* Check if data is available */
    i2c_read_reg(QMI8658_BUS, QMI8658_ADDR, QMI8658_REG_STATUS0, &tmp, 0);
    DEBUG("[LOG] _qmi8658_read_sensor: status = %x\n", tmp);

    /* 6 byte burst read, register address is incremented automatically */
    res = i2c_read_regs(QMI8658_BUS, QMI8658_ADDR, data_reg, tmp_arr, 6, 0);

    i2c_release(QMI8658_BUS);

    if (res < 0) {
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
    uint8_t tmp;

    DEBUG("[LOG] _qmi8658_set_sensors: Set sensor enable flags %x\n", sensor_enable_flags);

    i2c_acquire(QMI8658_BUS);

    res = i2c_read_reg(QMI8658_BUS, QMI8658_ADDR, QMI8658_REG_CTRL7, &tmp, 0);
    res = i2c_write_reg(QMI8658_BUS, QMI8658_ADDR, QMI8658_REG_CTRL7, tmp | sensor_enable_flags, 0);

    i2c_release(QMI8658_BUS);

    if (res < 0) {
        DEBUG("[ERROR] _qmi8658_set_sensors: Failed to set sensor flags\n");
        return -EIO;
    }

    /* Wait for sensors to wakeup */
    ztimer_sleep(ZTIMER_MSEC,
                 sensor_enable_flags & QMI8658_ENABLE_GYRO ?
                 QMI8658_GYRO_WAKEUP_MS : QMI8658_ACC_WAKEUP_MS);

    return 0;
}

/* Set the CAL registers (before CTRL9 command) */
static int _qmi8658_write_cal_regs(const qmi8658_t *dev, uint8_t *data)
{
    int res;

    i2c_acquire(QMI8658_BUS);

    /* 8 byte burst write to all CAL registers, register address is incremented automatically */
    res = i2c_write_regs(QMI8658_BUS, QMI8658_ADDR, QMI8658_REG_CAL1_L, data, 8, 0);

    i2c_release(QMI8658_BUS);

    if (res < 0) {
        return -EIO;
    }

    return 0;
}

/* Send a CTRL9 command and ACK after execution */
static int _qmi8658_ctrl9_cmd(const qmi8658_t *dev, uint8_t cmd)
{
    int res;
    uint8_t tmp;

    i2c_acquire(QMI8658_BUS);

    /* Send command */
    res = i2c_write_reg(QMI8658_BUS, QMI8658_ADDR, QMI8658_REG_CTRL9, cmd, 0);
    if (res < 0) {
        i2c_release(QMI8658_BUS);
        return -EIO;
    }

    ztimer_now_t start = ztimer_now(ZTIMER_MSEC);
    ztimer_now_t elapsed;
    /* Wait for command execution to be done */
    do {
        res = i2c_read_reg(QMI8658_BUS, QMI8658_ADDR, QMI8658_REG_STATUSINT, &tmp, 0);
        if (res < 0) {
            i2c_release(QMI8658_BUS);
            return -EIO;
        }

        elapsed = ztimer_now(ZTIMER_MSEC) - start;
        if (elapsed > QMI8658_CTRL9_TIMEOUT_MS) {
            DEBUG("[ERROR] _qmi8658_ctrl9_cmd_ack: CTRL9 command timed out\n");
            i2c_release(QMI8658_BUS);
            return -ETIMEDOUT;
        }

        ztimer_sleep(ZTIMER_MSEC, 1);
    } while (!(tmp & QMI8658_CTRL9_DONE_MASK));

    /* Acknowledge command execution */
    res = i2c_write_reg(QMI8658_BUS, QMI8658_ADDR, QMI8658_REG_CTRL9, QMI8658_CTRL9_CMD_ACK, 0);

    i2c_release(QMI8658_BUS);

    return 0;
}

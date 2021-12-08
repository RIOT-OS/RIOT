/*
 * Copyright (C) 2015 Freie Universität Berlin
 *               2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mpu9x50
 * @{
 *
 * @file
 * @brief       Device driver implementation for the MPU-9X50 (MPU9150 and MPU9250) 9-Axis Motion Sensor
 *
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 * @author      Jannes Volkens <jannes.volkens@haw-hamburg.de>
 *
 * @}
 */

#include "mpu9x50.h"
#include "mpu9x50_regs.h"
#include "mpu9x50_internal.h"
#include "periph/i2c.h"
#include "ztimer.h"
#include "byteorder.h"

#define ENABLE_DEBUG        0
#include "debug.h"

#define REG_RESET           (0x00)
#define MAX_VALUE           (0x7FFF)

#define DEV_I2C             (dev->params.i2c)
#define DEV_ADDR            (dev->params.addr)
#define DEV_COMP_ADDR       (dev->params.comp_addr)

/* Default config settings */
static const mpu9x50_status_t DEFAULT_STATUS = {
    .accel_pwr = MPU9X50_SENSOR_PWR_ON,
    .gyro_pwr = MPU9X50_SENSOR_PWR_ON,
    .compass_pwr = MPU9X50_SENSOR_PWR_ON,
    .gyro_fsr = MPU9X50_GYRO_FSR_250DPS,
    .accel_fsr = MPU9X50_ACCEL_FSR_16G,
    .sample_rate = 0,
    .compass_sample_rate = 0,
    .compass_x_adj = 0,
    .compass_y_adj = 0,
    .compass_z_adj = 0,
};

/* Internal function prototypes */
static int compass_init(mpu9x50_t *dev);
static void conf_bypass(const mpu9x50_t *dev, uint8_t bypass_enable);
static void conf_lpf(const mpu9x50_t *dev, uint16_t rate);

/*---------------------------------------------------------------------------*
 *                          MPU9X50 Core API                                 *
 *---------------------------------------------------------------------------*/

int mpu9x50_init(mpu9x50_t *dev, const mpu9x50_params_t *params)
{
    dev->params = *params;

    uint8_t temp;

    dev->conf = DEFAULT_STATUS;

    /* Acquire exclusive access */
    i2c_acquire(DEV_I2C);

    /* Reset MPU9X50 registers and afterwards wake up the chip */
    i2c_write_reg(DEV_I2C, DEV_ADDR, MPU9X50_PWR_MGMT_1_REG, MPU9X50_PWR_RESET, 0);
    ztimer_sleep(ZTIMER_MSEC, MPU9X50_RESET_SLEEP_MS);
    i2c_write_reg(DEV_I2C, DEV_ADDR, MPU9X50_PWR_MGMT_1_REG, MPU9X50_PWR_WAKEUP, 0);

    /* Release the bus, it is acquired again inside each function */
    i2c_release(DEV_I2C);

    /* Set default full scale ranges and sample rate */
    mpu9x50_set_gyro_fsr(dev, MPU9X50_GYRO_FSR_2000DPS);
    mpu9x50_set_accel_fsr(dev, MPU9X50_ACCEL_FSR_2G);
    mpu9x50_set_sample_rate(dev, dev->params.sample_rate);

    /* Disable interrupt generation */
    i2c_acquire(DEV_I2C);
    i2c_write_reg(DEV_I2C, DEV_ADDR, MPU9X50_INT_ENABLE_REG, REG_RESET, 0);

    /* Initialize magnetometer */
    if (compass_init(dev)) {
        i2c_release(DEV_I2C);
        return -2;
    }
    /* Release the bus, it is acquired again inside each function */
    i2c_release(DEV_I2C);
    mpu9x50_set_compass_sample_rate(dev, 10);
    /* Enable all sensors */
    i2c_acquire(DEV_I2C);
    i2c_write_reg(DEV_I2C, DEV_ADDR, MPU9X50_PWR_MGMT_1_REG, MPU9X50_PWR_PLL, 0);
    i2c_read_reg(DEV_I2C, DEV_ADDR, MPU9X50_PWR_MGMT_2_REG, &temp, 0);
    temp &= ~(MPU9X50_PWR_ACCEL | MPU9X50_PWR_GYRO);
    i2c_write_reg(DEV_I2C, DEV_ADDR, MPU9X50_PWR_MGMT_2_REG, temp, 0);
    i2c_release(DEV_I2C);
    ztimer_sleep(ZTIMER_MSEC, MPU9X50_PWR_CHANGE_SLEEP_MS);

    return 0;
}

int mpu9x50_set_accel_power(mpu9x50_t *dev, mpu9x50_pwr_t pwr_conf)
{
    uint8_t pwr_1_setting, pwr_2_setting;

    if (dev->conf.accel_pwr == pwr_conf) {
        return 0;
    }

    /* Acquire exclusive access */
    i2c_acquire(DEV_I2C);

    /* Read current power management 2 configuration */
    i2c_read_reg(DEV_I2C, DEV_ADDR, MPU9X50_PWR_MGMT_2_REG, &pwr_2_setting, 0);
    /* Prepare power register settings */
    if (pwr_conf == MPU9X50_SENSOR_PWR_ON) {
        pwr_1_setting = MPU9X50_PWR_WAKEUP;
        pwr_2_setting &= ~(MPU9X50_PWR_ACCEL);
    }
    else {
        pwr_1_setting = BIT_PWR_MGMT1_SLEEP;
        pwr_2_setting |= MPU9X50_PWR_ACCEL;
    }
    /* Configure power management 1 register if needed */
    if ((dev->conf.gyro_pwr == MPU9X50_SENSOR_PWR_OFF)
            && (dev->conf.compass_pwr == MPU9X50_SENSOR_PWR_OFF)) {
        i2c_write_reg(DEV_I2C, DEV_ADDR, MPU9X50_PWR_MGMT_1_REG, pwr_1_setting, 0);
    }
    /* Enable/disable accelerometer standby in power management 2 register */
    i2c_write_reg(DEV_I2C, DEV_ADDR, MPU9X50_PWR_MGMT_2_REG, pwr_2_setting, 0);

    /* Release the bus */
    i2c_release(DEV_I2C);

    dev->conf.accel_pwr = pwr_conf;
    ztimer_sleep(ZTIMER_MSEC, MPU9X50_PWR_CHANGE_SLEEP_MS);

    return 0;
}

int mpu9x50_set_gyro_power(mpu9x50_t *dev, mpu9x50_pwr_t pwr_conf)
{
    uint8_t pwr_2_setting;

    if (dev->conf.gyro_pwr == pwr_conf) {
        return 0;
    }

    /* Acquire exclusive access */
    i2c_acquire(DEV_I2C);

    /* Read current power management 2 configuration */
    i2c_read_reg(DEV_I2C, DEV_ADDR, MPU9X50_PWR_MGMT_2_REG, &pwr_2_setting, 0);
    /* Prepare power register settings */
    if (pwr_conf == MPU9X50_SENSOR_PWR_ON) {
        /* Set clock to pll */
        i2c_write_reg(DEV_I2C, DEV_ADDR, MPU9X50_PWR_MGMT_1_REG, MPU9X50_PWR_PLL, 0);
        pwr_2_setting &= ~(MPU9X50_PWR_GYRO);
    }
    else {
        /* Configure power management 1 register */
        if ((dev->conf.accel_pwr == MPU9X50_SENSOR_PWR_OFF)
                && (dev->conf.compass_pwr == MPU9X50_SENSOR_PWR_OFF)) {
            /* All sensors turned off, put the MPU-9X50 to sleep */
            i2c_write_reg(DEV_I2C, DEV_ADDR,
                    MPU9X50_PWR_MGMT_1_REG, BIT_PWR_MGMT1_SLEEP, 0);
        }
        else {
            /* Reset clock to internal oscillator */
            i2c_write_reg(DEV_I2C, DEV_ADDR,
                    MPU9X50_PWR_MGMT_1_REG, MPU9X50_PWR_WAKEUP, 0);
        }
        pwr_2_setting |= MPU9X50_PWR_GYRO;
    }
    /* Enable/disable gyroscope standby in power management 2 register */
    i2c_write_reg(DEV_I2C, DEV_ADDR, MPU9X50_PWR_MGMT_2_REG, pwr_2_setting, 0);

    /* Release the bus */
    i2c_release(DEV_I2C);

    dev->conf.gyro_pwr = pwr_conf;
    ztimer_sleep(ZTIMER_MSEC, MPU9X50_PWR_CHANGE_SLEEP_MS);

    return 0;
}

int mpu9x50_set_compass_power(mpu9x50_t *dev, mpu9x50_pwr_t pwr_conf)
{
    uint8_t pwr_1_setting, usr_ctrl_setting, s1_do_setting;

    if (dev->conf.compass_pwr == pwr_conf) {
        return 0;
    }

    /* Acquire exclusive access */
    i2c_acquire(DEV_I2C);

    /* Read current user control configuration */
    i2c_read_reg(DEV_I2C, DEV_ADDR, MPU9X50_USER_CTRL_REG, &usr_ctrl_setting, 0);
    /* Prepare power register settings */
    if (pwr_conf == MPU9X50_SENSOR_PWR_ON) {
        pwr_1_setting = MPU9X50_PWR_WAKEUP;
        s1_do_setting = MPU9X50_COMP_SINGLE_MEASURE;
        usr_ctrl_setting |= BIT_I2C_MST_EN;
    }
    else {
        pwr_1_setting = BIT_PWR_MGMT1_SLEEP;
        s1_do_setting = MPU9X50_COMP_POWER_DOWN;
        usr_ctrl_setting &= ~(BIT_I2C_MST_EN);
    }
    /* Configure power management 1 register if needed */
    if ((dev->conf.gyro_pwr == MPU9X50_SENSOR_PWR_OFF)
            && (dev->conf.accel_pwr == MPU9X50_SENSOR_PWR_OFF)) {
        i2c_write_reg(DEV_I2C, DEV_ADDR, MPU9X50_PWR_MGMT_1_REG, pwr_1_setting, 0);
    }
    /* Configure mode writing by slave line 1 */
    i2c_write_reg(DEV_I2C, DEV_ADDR, MPU9X50_SLAVE1_DATA_OUT_REG, s1_do_setting, 0);
    /* Enable/disable I2C master mode */
    i2c_write_reg(DEV_I2C, DEV_ADDR, MPU9X50_USER_CTRL_REG, usr_ctrl_setting, 0);

    /* Release the bus */
    i2c_release(DEV_I2C);

    dev->conf.compass_pwr = pwr_conf;
    ztimer_sleep(ZTIMER_MSEC, MPU9X50_PWR_CHANGE_SLEEP_MS);

    return 0;
}

int mpu9x50_read_gyro(const mpu9x50_t *dev, mpu9x50_results_t *output)
{
    uint8_t data[6];
    int16_t temp;
    float fsr;

    switch (dev->conf.gyro_fsr) {
        case MPU9X50_GYRO_FSR_250DPS:
            fsr = 250.0;
            break;
        case MPU9X50_GYRO_FSR_500DPS:
            fsr = 500.0;
            break;
        case MPU9X50_GYRO_FSR_1000DPS:
            fsr = 1000.0;
            break;
        case MPU9X50_GYRO_FSR_2000DPS:
            fsr = 2000.0;
            break;
        default:
            return -2;
    }

    /* Acquire exclusive access */
    i2c_acquire(DEV_I2C);
    /* Read raw data */
    i2c_read_regs(DEV_I2C, DEV_ADDR, MPU9X50_GYRO_START_REG, data, 6, 0);
    /* Release the bus */
    i2c_release(DEV_I2C);

    /* Normalize data according to configured full scale range */
    temp = (data[0] << 8) | data[1];
    output->x_axis = (temp * fsr) / MAX_VALUE;
    temp = (data[2] << 8) | data[3];
    output->y_axis = (temp * fsr) / MAX_VALUE;
    temp = (data[4] << 8) | data[5];
    output->z_axis = (temp * fsr) / MAX_VALUE;

    return 0;
}

int mpu9x50_read_accel(const mpu9x50_t *dev, mpu9x50_results_t *output)
{
    uint8_t data[6];
    int16_t temp;
    float fsr;

    switch (dev->conf.accel_fsr) {
        case MPU9X50_ACCEL_FSR_2G:
            fsr = 2000.0;
            break;
        case MPU9X50_ACCEL_FSR_4G:
            fsr = 4000.0;
            break;
        case MPU9X50_ACCEL_FSR_8G:
            fsr = 8000.0;
            break;
        case MPU9X50_ACCEL_FSR_16G:
            fsr = 16000.0;
            break;
        default:
            return -2;
    }

    /* Acquire exclusive access */
    i2c_acquire(DEV_I2C);
    /* Read raw data */
    i2c_read_regs(DEV_I2C, DEV_ADDR, MPU9X50_ACCEL_START_REG, data, 6, 0);
    /* Release the bus */
    i2c_release(DEV_I2C);

    /* Normalize data according to configured full scale range */
    temp = (data[0] << 8) | data[1];
    output->x_axis = (temp * fsr) / MAX_VALUE;
    temp = (data[2] << 8) | data[3];
    output->y_axis = (temp * fsr) / MAX_VALUE;
    temp = (data[4] << 8) | data[5];
    output->z_axis = (temp * fsr) / MAX_VALUE;

    return 0;
}

int mpu9x50_read_compass(const mpu9x50_t *dev, mpu9x50_results_t *output)
{
    uint8_t data[6];

    /* Acquire exclusive access */
    i2c_acquire(DEV_I2C);
    /* Read raw data */
    i2c_read_regs(DEV_I2C, DEV_ADDR, MPU9X50_EXT_SENS_DATA_START_REG, data, 6, 0);
    /* Release the bus */
    i2c_release(DEV_I2C);

    output->x_axis = (data[1] << 8) | data[0];
    output->y_axis = (data[3] << 8) | data[2];
    output->z_axis = (data[5] << 8) | data[4];

    /* Compute sensitivity adjustment */
    output->x_axis = (int16_t) (((float)output->x_axis) *
            ((((dev->conf.compass_x_adj - 128) * 0.5) / 128.0) + 1));
    output->y_axis = (int16_t) (((float)output->y_axis) *
            ((((dev->conf.compass_y_adj - 128) * 0.5) / 128.0) + 1));
    output->z_axis = (int16_t) (((float)output->z_axis) *
            ((((dev->conf.compass_z_adj - 128) * 0.5) / 128.0) + 1));

    /* Normalize data according to full-scale range */
    output->x_axis = output->x_axis * 0.3;
    output->y_axis = output->y_axis * 0.3;
    output->z_axis = output->z_axis * 0.3;

    return 0;
}

int mpu9x50_read_temperature(const mpu9x50_t *dev, int32_t *output)
{
    uint16_t data;

    /* Acquire exclusive access */
    i2c_acquire(DEV_I2C);
    /* Read raw temperature value */
    i2c_read_regs(DEV_I2C, DEV_ADDR, MPU9X50_TEMP_START_REG, &data, 2, 0);
    /* Release the bus */
    i2c_release(DEV_I2C);

    data = htons(data);

    *output = (((int32_t)data * 1000LU) / MPU9X50_TEMP_SENSITIVITY) + (MPU9X50_TEMP_OFFSET * 1000LU);

    return 0;
}

int mpu9x50_set_gyro_fsr(mpu9x50_t *dev, mpu9x50_gyro_ranges_t fsr)
{
    if (dev->conf.gyro_fsr == fsr) {
        return 0;
    }

    switch (fsr) {
        case MPU9X50_GYRO_FSR_250DPS:
        case MPU9X50_GYRO_FSR_500DPS:
        case MPU9X50_GYRO_FSR_1000DPS:
        case MPU9X50_GYRO_FSR_2000DPS:
            i2c_acquire(DEV_I2C);
            i2c_write_reg(DEV_I2C, DEV_ADDR,
                    MPU9X50_GYRO_CFG_REG, (fsr << 3), 0);
            i2c_release(DEV_I2C);
            dev->conf.gyro_fsr = fsr;
            break;
        default:
            return -2;
    }

    return 0;
}

int mpu9x50_set_accel_fsr(mpu9x50_t *dev, mpu9x50_accel_ranges_t fsr)
{
    if (dev->conf.accel_fsr == fsr) {
        return 0;
    }

    switch (fsr) {
        case MPU9X50_ACCEL_FSR_2G:
        case MPU9X50_ACCEL_FSR_4G:
        case MPU9X50_ACCEL_FSR_8G:
        case MPU9X50_ACCEL_FSR_16G:
            i2c_acquire(DEV_I2C);
            i2c_write_reg(DEV_I2C, DEV_ADDR,
                    MPU9X50_ACCEL_CFG_REG, (fsr << 3), 0);
            i2c_release(DEV_I2C);
            dev->conf.accel_fsr = fsr;
            break;
        default:
            return -2;
    }

    return 0;
}

int mpu9x50_set_sample_rate(mpu9x50_t *dev, uint16_t rate)
{
    uint8_t divider;

    if ((rate < MPU9X50_MIN_SAMPLE_RATE) || (rate > MPU9X50_MAX_SAMPLE_RATE)) {
        return -2;
    }
    else if (dev->conf.sample_rate == rate) {
        return 0;
    }

    /* Compute divider to achieve desired sample rate and write to rate div register */
    divider = (1000 / rate - 1);

    i2c_acquire(DEV_I2C);
    i2c_write_reg(DEV_I2C, DEV_ADDR, MPU9X50_RATE_DIV_REG, divider, 0);

    /* Store configured sample rate */
    dev->conf.sample_rate = 1000 / (((uint16_t) divider) + 1);

    /* Always set LPF to a maximum of half the configured sampling rate */
    conf_lpf(dev, (dev->conf.sample_rate >> 1));
    i2c_release(DEV_I2C);

    return 0;
}

int mpu9x50_set_compass_sample_rate(mpu9x50_t *dev, uint8_t rate)
{
    uint8_t divider;

    if ((rate < MPU9X50_MIN_COMP_SMPL_RATE) || (rate > MPU9X50_MAX_COMP_SMPL_RATE)
            || (rate > dev->conf.sample_rate)) {
        return -2;
    }
    else if (dev->conf.compass_sample_rate == rate) {
        return 0;
    }

    /* Compute divider to achieve desired sample rate and write to slave ctrl register */
    divider = (dev->conf.sample_rate / rate - 1);

    i2c_acquire(DEV_I2C);
    i2c_write_reg(DEV_I2C, DEV_ADDR, MPU9X50_SLAVE4_CTRL_REG, divider, 0);
    i2c_release(DEV_I2C);

    /* Store configured sample rate */
    dev->conf.compass_sample_rate = dev->conf.sample_rate / (((uint16_t) divider) + 1);

    return 0;
}

/*------------------------------------------------------------------------------------*/
/*                                Internal functions                                  */
/*------------------------------------------------------------------------------------*/

/**
 * Initialize compass
 * Caution: This internal function does not acquire exclusive access to the I2C bus.
 *          Acquisation and release is supposed to be handled by the calling function.
 */
static int compass_init(mpu9x50_t *dev)
{
    uint8_t data[3];

    /* Enable Bypass Mode to speak to compass directly */
    conf_bypass(dev, 1);

    /* Check whether compass answers correctly */
    i2c_read_reg(DEV_I2C, DEV_COMP_ADDR, COMPASS_WHOAMI_REG, data, 0);
    if (data[0] != MPU9X50_COMP_WHOAMI_ANSWER) {
        DEBUG("[Error] Wrong answer from compass\n");
        return -1;
    }

    /* Configure Power Down mode */
    i2c_write_reg(DEV_I2C, DEV_COMP_ADDR, COMPASS_CNTL_REG, MPU9X50_COMP_POWER_DOWN, 0);
    ztimer_sleep(ZTIMER_MSEC, MPU9X50_COMP_MODE_SLEEP_MS);
    /* Configure Fuse ROM access */
    i2c_write_reg(DEV_I2C, DEV_COMP_ADDR, COMPASS_CNTL_REG, MPU9X50_COMP_FUSE_ROM, 0);
    ztimer_sleep(ZTIMER_MSEC, MPU9X50_COMP_MODE_SLEEP_MS);
    /* Read sensitivity adjustment values from Fuse ROM */
    i2c_read_regs(DEV_I2C, DEV_COMP_ADDR, COMPASS_ASAX_REG, data, 3, 0);
    dev->conf.compass_x_adj = data[0];
    dev->conf.compass_y_adj = data[1];
    dev->conf.compass_z_adj = data[2];
    /* Configure Power Down mode again */
    i2c_write_reg(DEV_I2C, DEV_COMP_ADDR, COMPASS_CNTL_REG, MPU9X50_COMP_POWER_DOWN, 0);
    ztimer_sleep(ZTIMER_MSEC, MPU9X50_COMP_MODE_SLEEP_MS);

    /* Disable Bypass Mode to configure MPU as master to the compass */
    conf_bypass(dev, 0);

    /* Configure MPU9X50 for single master mode */
    i2c_write_reg(DEV_I2C, DEV_ADDR, MPU9X50_I2C_MST_REG, BIT_WAIT_FOR_ES, 0);

    /* Set up slave line 0 */
    /* Slave line 0 reads the compass data */
    i2c_write_reg(DEV_I2C, DEV_ADDR,
            MPU9X50_SLAVE0_ADDR_REG, (BIT_SLAVE_RW | DEV_COMP_ADDR), 0);
    /* Slave line 0 read starts at compass data register */
    i2c_write_reg(DEV_I2C, DEV_ADDR, MPU9X50_SLAVE0_REG_REG, COMPASS_DATA_START_REG, 0);
    /* Enable slave line 0 and configure read length to 6 consecutive registers */
    i2c_write_reg(DEV_I2C, DEV_ADDR, MPU9X50_SLAVE0_CTRL_REG, (BIT_SLAVE_EN | 0x06), 0);

    /* Set up slave line 1 */
    /* Slave line 1 writes to the compass */
    i2c_write_reg(DEV_I2C, DEV_ADDR, MPU9X50_SLAVE1_ADDR_REG, DEV_COMP_ADDR, 0);
    /* Slave line 1 write starts at compass control register */
    i2c_write_reg(DEV_I2C, DEV_ADDR, MPU9X50_SLAVE1_REG_REG, COMPASS_CNTL_REG, 0);
    /* Enable slave line 1 and configure write length to 1 register */
    i2c_write_reg(DEV_I2C, DEV_ADDR, MPU9X50_SLAVE1_CTRL_REG, (BIT_SLAVE_EN | 0x01), 0);
    /* Configure data which is written by slave line 1 to compass control */
    i2c_write_reg(DEV_I2C, DEV_ADDR,
            MPU9X50_SLAVE1_DATA_OUT_REG, MPU9X50_COMP_SINGLE_MEASURE, 0);

    /* Slave line 0 and 1 operate at each sample */
    i2c_write_reg(DEV_I2C, DEV_ADDR,
            MPU9X50_I2C_DELAY_CTRL_REG, (BIT_SLV0_DELAY_EN | BIT_SLV1_DELAY_EN), 0);
    /* Set I2C bus to VDD */
    i2c_write_reg(DEV_I2C, DEV_ADDR, MPU9X50_YG_OFFS_TC_REG, BIT_I2C_MST_VDDIO, 0);

    return 0;
}

/**
 * Configure bypass mode
 * Caution: This internal function does not acquire exclusive access to the I2C bus.
 *          Acquisation and release is supposed to be handled by the calling function.
 */
static void conf_bypass(const mpu9x50_t *dev, uint8_t bypass_enable)
{
   uint8_t data;
   i2c_read_reg(DEV_I2C, DEV_ADDR, MPU9X50_USER_CTRL_REG, &data, 0);

   if (bypass_enable) {
       data &= ~(BIT_I2C_MST_EN);
       i2c_write_reg(DEV_I2C, DEV_ADDR, MPU9X50_USER_CTRL_REG, data, 0);
       ztimer_sleep(ZTIMER_MSEC, MPU9X50_BYPASS_SLEEP_MS);
       i2c_write_reg(DEV_I2C, DEV_ADDR, MPU9X50_INT_PIN_CFG_REG, BIT_I2C_BYPASS_EN, 0);
   }
   else {
       data |= BIT_I2C_MST_EN;
       i2c_write_reg(DEV_I2C, DEV_ADDR, MPU9X50_USER_CTRL_REG, data, 0);
       ztimer_sleep(ZTIMER_MSEC, MPU9X50_BYPASS_SLEEP_MS);
       i2c_write_reg(DEV_I2C, DEV_ADDR, MPU9X50_INT_PIN_CFG_REG, REG_RESET, 0);
   }
}

/**
 * Configure low pass filter
 * Caution: This internal function does not acquire exclusive access to the I2C bus.
 *          Acquisation and release is supposed to be handled by the calling function.
 */
static void conf_lpf(const mpu9x50_t *dev, uint16_t half_rate)
{
    mpu9x50_lpf_t lpf_setting;

    /* Get target LPF configuration setting */
    if (half_rate >= 188) {
        lpf_setting = MPU9X50_FILTER_188HZ;
    }
    else if (half_rate >= 98) {
        lpf_setting = MPU9X50_FILTER_98HZ;
    }
    else if (half_rate >= 42) {
        lpf_setting = MPU9X50_FILTER_42HZ;
    }
    else if (half_rate >= 20) {
        lpf_setting = MPU9X50_FILTER_20HZ;
    }
    else if (half_rate >= 10) {
        lpf_setting = MPU9X50_FILTER_10HZ;
    }
    else {
        lpf_setting = MPU9X50_FILTER_5HZ;
    }

    /* Write LPF setting to configuration register */
    i2c_write_reg(DEV_I2C, DEV_ADDR, MPU9X50_LPF_REG, lpf_setting, 0);
}

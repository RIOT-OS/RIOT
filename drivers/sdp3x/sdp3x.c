/*
 * Copyright (C) 2019 Dirk Ehmen
 *               2020 Jan Schlichter
 *               2020 Nishchay Agrawal
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_sdp3x
 * @{
 * @file
 * @brief       Sensirion SDP3x sensor driver implementation
 *
 * @author      Dirk Ehmen       <ehmen@ibr.cs.tu-bs.de>
 * @author      Jan Schlichter   <schlichter@ibr.cs.tu-bs.de>
 * @author      Nishchay Agrawal <f2016088@pilani.bits-pilani.ac.in>
 * @}
 */

#include <stdint.h>
#include <stdlib.h>
#include "sdp3x_params.h"
#include "sdp3x.h"
#include "checksum/crc8.h"
#include "thread.h"

#define ENABLE_DEBUG        0
#include "debug.h"

#define DEV_I2C             (dev->params.i2c_dev)
#define DEV_ADDR            (dev->params.i2c_addr)

#define DATA_READY_SLEEP_US (50 * US_PER_MS)

static bool _check_product_number(uint8_t *readData);
#ifdef MODULE_SDP3X_IRQ
static void _sdp3x_irq_callback(void *arg);
#endif
static int8_t _checkCRC(uint16_t value, uint8_t test);
static int8_t _SDP3x_read_data(const sdp3x_t *dev, int16_t *data);
static int32_t _SDP3x_convert_to_pascal(int16_t value,
                                        int16_t dp_scale_factor);
static int32_t _SDP3x_convert_to_celsius(int16_t value);
static int8_t _SDP3x_start_triggered(const sdp3x_t *dev, uint8_t flags);
static int32_t _SDP3x_read_temp(const sdp3x_t *dev);
static int32_t _SDP3x_read_pressure(const sdp3x_t *dev);

int sdp3x_init(sdp3x_t *dev, const sdp3x_params_t *params)
{
    DEBUG("[SDP3x] init: Initializing device\n");

    dev->params = *params;
    dev->continuous_measurement = false;

    sdp3x_soft_reset(dev);

    /* try to read product number to check if sensor is connected and working */
    int ret = 0;
    /* Command to get sensor product information */
    uint8_t cmd1[2] = { 0x36, 0x7C };
    uint8_t cmd2[2] = { 0xE1, 0x02 };
    i2c_acquire(DEV_I2C);
    ret = i2c_write_bytes(DEV_I2C, DEV_ADDR, cmd1, 2, I2C_NOSTOP);
    if (ret != 0) {
        i2c_release(DEV_I2C);
        return ret;
    }
    ret = i2c_write_bytes(DEV_I2C, DEV_ADDR, cmd2, 2, I2C_NOSTOP);
    if (ret != 0) {
        i2c_release(DEV_I2C);
        return ret;
    }

    /* Read information from the sensor to verify it's correct */
    uint8_t readData[5];
    ret = i2c_read_bytes(DEV_I2C, DEV_ADDR, readData, sizeof(readData), 0);
    i2c_release(DEV_I2C);

    if (ret != 0) {
        DEBUG("[SDP3x] read_data: ret %i\n", ret);
        return ret;
    }
    if (!_check_product_number(readData)) {
        DEBUG("[SDP3x] Sensor not working or incorrectly configured\n");
        return ret;
    }

#ifdef MODULE_SDP3X_IRQ
    /* check if current device has irq pin connected */
    if (params->irq_pin != GPIO_UNDEF) {
        mutex_init(&dev->mutex);
        /* lock mutex initially to be unlocked when interrupt is raised */
        mutex_lock(&dev->mutex);
        /* Interrupt set to trigger on falling edge of interrupt pin */
        gpio_init_int(params->irq_pin, GPIO_IN, GPIO_FALLING, _sdp3x_irq_callback,
                     dev);
    }
#endif

    DEBUG("[SDP3x] init: Init done\n");
    return 1;
}

int32_t sdp3x_read_single_temperature(sdp3x_t *dev, uint8_t flags)
{
    _SDP3x_start_triggered(dev, flags);
    if (!IS_USED(MODULE_SDP3X_IRQ) || dev->params.irq_pin == GPIO_UNDEF) {
        /* Wait for measurement to be ready if irq pin not used */
        xtimer_usleep(DATA_READY_SLEEP_US);
    }
    else {
        /* Try to lock mutex till the interrupt is raised or till timeut happens */
        xtimer_mutex_lock_timeout(&dev->mutex, DATA_READY_SLEEP_US);
    }
    return _SDP3x_read_temp(dev);
}

int32_t sdp3x_read_single_differential_pressure(sdp3x_t *dev,
                                               uint8_t flags)
{
    _SDP3x_start_triggered(dev, flags);
    if (!IS_USED(MODULE_SDP3X_IRQ) || dev->params.irq_pin == GPIO_UNDEF) {
        /* Wait for measurement to be ready if irq pin not used */
        xtimer_usleep(DATA_READY_SLEEP_US);
    }
    else {
        /* Try to lock mutex till the interrupt is raised or till timeut happens */
        xtimer_mutex_lock_timeout(&dev->mutex, DATA_READY_SLEEP_US);
    }
    return _SDP3x_read_pressure(dev);
}

int8_t sdp3x_read_single_measurement(sdp3x_t *dev, uint8_t flags,
                                     sdp3x_measurement_t *result)
{
    _SDP3x_start_triggered(dev, flags);
    if (!IS_USED(MODULE_SDP3X_IRQ) || dev->params.irq_pin == GPIO_UNDEF) {
        /* Wait for measurement to be ready if irq pin not used */
        xtimer_usleep(DATA_READY_SLEEP_US);
    }
    else {
        /* Try to lock mutex till the interrupt is raised or till timeut happens */
        xtimer_mutex_lock_timeout(&dev->mutex, DATA_READY_SLEEP_US);
    }
    /* read in sensor values here */
    int16_t data[3];
    uint8_t ret = _SDP3x_read_data(dev, data);
    if (ret != 0) {
        DEBUG("[SDP3x] Error reading data\n");
        return 0;
    }
    else {
        result->differential_pressure = _SDP3x_convert_to_pascal(data[0],
                                                                 data[2]);
        result->temperature = _SDP3x_convert_to_celsius(data[1]);
    }
    return 1;
}

int8_t  sdp3x_start_continuous(sdp3x_t *dev, uint8_t flags)
{
    int ret = 0;
    uint8_t cmd[2] = { 0x36, 0 };
    if (((flags >> 1) & 1) == 1) {
        if ((flags & 1) == 0) {
            cmd[1] = 0x08;
        }
        else {
            cmd[1] = 0x03;
        }
    }
    else {
        if ((flags & 1) == 0) {
            cmd[1] = 0x1E;
        }
        else {
            cmd[1] = 0x15;
        }
    }

    i2c_acquire(DEV_I2C);
    ret = i2c_write_bytes(DEV_I2C, DEV_ADDR, cmd, 2, 0);
    i2c_release(DEV_I2C);

    if (ret != 0) {
        DEBUG("[SDP3x] Error starting continuous measurement\n");
    }

    dev->continuous_measurement = true;
    return ret;
}

int8_t sdp3x_read_continuous(sdp3x_measurement_t *result, sdp3x_t *dev)
{
    /* read in sensor values here */
    int16_t data[3];
    uint8_t ret = _SDP3x_read_data(dev, data);

    if (ret != 0) {
        DEBUG("[SDP3x] Error reading date\n");
    }
    else {
        result->differential_pressure = _SDP3x_convert_to_pascal(data[0],
                                                                 data[2]);
        result->temperature = _SDP3x_convert_to_celsius(data[1]);
    }
    return ret;
}

/**
 *      intern function to start triggered measurement
 *      @param  dev          device
 *      @param flags         stores Temperature compensation
 *                           1st bit-> 0 = Differential pressure, 1 = Mass Flow
 *      @return i2c error code
 */
static int8_t _SDP3x_start_triggered(const sdp3x_t *dev, uint8_t flags)
{
    int ret = 0;
    uint8_t cmd[2];

    cmd[0] = 0x36;
    if (flags == SDP3X_FLAG_MASS_FLOW) {
        cmd[1] = 0x24;
    }
    else {
        cmd[1] = 0x2F;
    }

    i2c_acquire(DEV_I2C);
    ret = i2c_write_bytes(DEV_I2C, DEV_ADDR, cmd, 2, 0);
    i2c_release(DEV_I2C);
    return ret;
}

int8_t sdp3x_stop_continuous(sdp3x_t *dev, xtimer_t *continuous_timer)
{
    int ret = 0;
    uint8_t cmd[2] = { 0x3F, 0xF9 };

    DEBUG("[SDP3x] stop_continuous: Stopping continuous"
          " measurement on device %#X\n", DEV_ADDR);
    i2c_acquire(DEV_I2C);
    ret = i2c_write_bytes(DEV_I2C, DEV_ADDR, cmd, 2, 0);
    i2c_release(DEV_I2C);

    xtimer_remove(continuous_timer);
    dev->continuous_measurement = false;

    return ret;
}

int8_t sdp3x_soft_reset(const sdp3x_t *dev)
{
    int ret = 0;

    DEBUG("[SDP3x] soft_reset: Sending soft reset to all devices\n");
    i2c_acquire(DEV_I2C);
    /* General Call Reset */
    ret = i2c_write_byte(DEV_I2C, 0x00, 0x06, 0);
    i2c_release(DEV_I2C);
    /* Wait 20ms for the reset to be processed */
    xtimer_usleep(20000);
    DEBUG("[SDP3x] soft_reset: reset done\n");
    return ret;
}

int8_t sdp3x_enter_sleep(const sdp3x_t *dev)
{
    int ret = 0;
    uint8_t cmd[2] = { 0x36, 0x77 };

    DEBUG("[SDP3x] enter_sleep: Entering sleep mode on device %#X\n", DEV_ADDR);
    i2c_acquire(DEV_I2C);
    ret = i2c_write_bytes(DEV_I2C, DEV_ADDR, cmd, 2, 0);
    i2c_release(DEV_I2C);

    return ret;
}

int8_t sdp3x_exit_sleep(const sdp3x_t *dev)
{
    int ret = 0;
    uint8_t ptr[1] = { 0 };

    DEBUG("[SDP3x] exit_sleep: Exiting sleep mode on device %#X\n", DEV_ADDR);
    i2c_acquire(DEV_I2C);
    ret = i2c_write_bytes(DEV_I2C, DEV_ADDR, ptr, 0, 0);

    if (ret != 0) {
        DEBUG("[SDP3x] Error exiting sleep mode\n");
        i2c_release(DEV_I2C);
        return ret;
    }
    xtimer_usleep(2000);
    ret = i2c_write_bytes(DEV_I2C, DEV_ADDR, ptr, 0, 0);
    i2c_release(DEV_I2C);

    return ret;
}

/**
 *      intern function to read temperature
 *      @param  dev      device
 *      @return Temperature value in celsius
 */
static int32_t _SDP3x_read_temp(const sdp3x_t *dev)
{
    int16_t data[3];
    uint8_t ret = _SDP3x_read_data(dev, data);

    if (ret != 0) {
        DEBUG("[SDP3x] Error reading date\n");
        return ret;
    }
    DEBUG("[SDP3x] read temperature bare value %i\n", data[1]);
    return _SDP3x_convert_to_celsius(data[1]);
}

/**
 *      intern function to read differential pressure
 *      @param  dev      device
 *      @return Pressure value in pascals
 */
static int32_t _SDP3x_read_pressure(const sdp3x_t *dev)
{
    int16_t data[3];
    uint8_t ret = _SDP3x_read_data(dev, data);

    if (ret != 0) {
        DEBUG("[SDP3x] Error reading date\n");
        return ret;
    }
    return _SDP3x_convert_to_pascal(data[0], data[2]);
}

/** Read measurements
 *
 *      Data consists of:
 *      2 byte Differential Pressure,
 *      1 byte CRC
 *      2 byte Temperature,
 *      1 byte CRC
 *      2 byte Scale Factor differential pressure
 *      1 byte CRC
 *
 *      @param dev    sdp3x device
 *      @param data   Data will be stored here
 *
 *      @return       0 if data could be read, 1 if CRC-Error
 */
static int8_t _SDP3x_read_data(const sdp3x_t *dev, int16_t *data)
{
    int ret = 0;
    uint8_t readData[9];

    i2c_acquire(DEV_I2C);
    ret = i2c_read_bytes(DEV_I2C, DEV_ADDR, readData, sizeof(readData), 0);
    i2c_release(DEV_I2C);

    if (ret != 0) {
        DEBUG("[SDP3x] read_data: ret %i\n", ret);
        return ret;
    }
    data[2] = (readData[6] << 8) + readData[7];
    data[1] = (readData[3] << 8) + readData[4];
    data[0] = (readData[0] << 8) + readData[1];

    uint8_t correct = 1;

    correct &= _checkCRC(data[2], readData[8]);
    correct &= _checkCRC(data[1], readData[5]);
    correct &= _checkCRC(data[0], readData[2]);

    if (correct == 0) {
        DEBUG("[SDP3x] read_data: CRC-Failure\n");
        return SDP3x_CRCERROR;
    }

    return 0;
}

/**
 *      intern method to convert sensor value to pascal
 *      @param  value                 raw sensor value
 *      @param  dp_scale_factor scale factor for sensor
 *      @return pressure              value in centiPa
 */
static int32_t _SDP3x_convert_to_pascal(int16_t value, int16_t dp_scale_factor)
{
    /* Multiplying by 100 to convert to centiPa */
    return ((value * 100) / dp_scale_factor);
}

/**
 *      intern method to convert sensor value to celsius
 *      @param  value   raw sensor value
 *      @return temperature in centi°C
 */
static int32_t _SDP3x_convert_to_celsius(int16_t value)
{
    int16_t div = 200;
    /* Multiplying by 100 to convert the final temperature in 100*°C */
    return ((value * 100) / div);
}

/**
 *      check if crc is valid
 *      @param  value Number whose crc has to be calculated
 *      @param  test  CRC value with which it will compared
 *      @return 1 on success
 */
static int8_t _checkCRC(uint16_t value, uint8_t test)
{
    uint8_t crc;
    uint8_t data[2] = { value >> 8, value & 0x00FF };

    crc = crc8(data, 2, 0x31, 0xFF);

    return (crc == test ? 1 : 0);
}

/**
 *      Callback function to handle trigger on irq
 *      @param  arguments passed when interrupt is raised
 *              (in this case sdp3x dev)
 */
#ifdef MODULE_SDP3X_IRQ
static void _sdp3x_irq_callback(void *arg)
{
    sdp3x_t *dev = (sdp3x_t *)arg;
    if (!dev->continuous_measurement) {
        mutex_unlock(&(dev->mutex));
    }
}
#endif

/**
 *      Function to check if the product number set
 *      is the one we get from the sensor
 *      @param  data  data read from the sensor
 *      @return true  if values from sensor were correct
 */
static bool _check_product_number(uint8_t *readData)
{
    if (readData[0] != SDP31_PRODUCT_NO_BYTE_0) {
        return false;
    }
    if (readData[1] != SDP31_PRODUCT_NO_BYTE_1) {
        return false;
    }
    if ((readData[3] != SDP3X_MODEL_31) && (readData[3] != SDP3X_MODEL_32)) {
        return false;
    }
    if (readData[4] != SDP31_PRODUCT_NO_BYTE_3) {
        return false;
    }

    return true;
}

/*
 * Copyright (C) 2016 Kees Bakker, SODAQ
 *               2017 Inria
 *               2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_bmx280
 * @{
 *
 * @file
 * @brief       Device driver implementation for BME280 and BMP280 sensors
 *
 * @author      Kees Bakker <kees@sodaq.com>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <string.h>
#include <math.h>

#include "log.h"
#include "assert.h"
#include "bmx280.h"
#include "bmx280_internals.h"

#define ENABLE_DEBUG        0
#include "debug.h"

#ifdef BMX280_USE_SPI
#define BUS                 (dev->params.spi)
#define CS                  (dev->params.cs)
#define CLK                 (dev->params.clk)
#define MODE                SPI_MODE_0
#define WRITE_MASK          (0x7F)
#else
#define BUS                 (dev->params.i2c_dev)
#define ADDR                (dev->params.i2c_addr)
#endif

/* shortcut for accessing byte x of the latest sensor reading */
#define RAW_DATA            (dev->last_reading)

/* implementation for the driver's configured bus interface (I2C vs SPI) */
#ifdef BMX280_USE_SPI /* using SPI mode */
static inline int _acquire(const bmx280_t *dev)
{
    spi_acquire(BUS, CS, MODE, CLK);
    return BMX280_OK;
}

static inline void _release(const bmx280_t *dev)
{
    spi_release(BUS);
}

static int _read_reg(const bmx280_t *dev, uint8_t reg, uint8_t *data)
{
    *data = spi_transfer_reg(BUS, CS, reg, 0);
    return BMX280_OK;
}

static int _write_reg(const bmx280_t *dev, uint8_t reg, uint8_t data)
{
    (void)spi_transfer_reg(BUS, CS, (reg & WRITE_MASK), data);
    return BMX280_OK;
}

static int _read_burst(const bmx280_t *dev, uint8_t reg, void *buf, size_t len)
{
    spi_transfer_regs(BUS, CS, reg, NULL, buf, len);
    return BMX280_OK;
}

#else /* using I2C mode */

static inline int _acquire(const bmx280_t *dev)
{
    if (i2c_acquire(BUS) != 0) {
        return BMX280_ERR_BUS;
    }
    return BMX280_OK;
}

static inline void _release(const bmx280_t *dev)
{
    i2c_release(BUS);
}

static int _read_reg(const bmx280_t *dev, uint8_t reg, uint8_t *data)
{
    if (i2c_read_reg(BUS, ADDR, reg, data, 0) != 0) {
        return BMX280_ERR_BUS;
    }
    return BMX280_OK;
}

static int _write_reg(const bmx280_t *dev, uint8_t reg, uint8_t data)
{
    if (i2c_write_reg(BUS, ADDR, reg, data, 0) != 0) {
        return BMX280_ERR_BUS;
    }
    return BMX280_OK;
}

static int _read_burst(const bmx280_t *dev, uint8_t reg, void *buf, size_t len)
{
    if (i2c_read_regs(BUS, ADDR, reg, buf, len, 0) != 0) {
        return BMX280_ERR_BUS;
    }
    return BMX280_OK;
}

#endif /* bus mode selection */

static uint16_t _to_u16_le(const uint8_t *buffer, size_t offset)
{
    return (((uint16_t)buffer[offset + 1]) << 8) | buffer[offset];
}

static int16_t _to_i16_le(const uint8_t *buffer, size_t offset)
{
    return (((int16_t)buffer[offset + 1]) << 8) | buffer[offset];
}

/**
 * @brief   Read the calibration data from sensor ROM, it is in registers
 *          0x88..0x9F, 0xA1, and 0xE1..0xE7
 */
static int _read_calibration_data(bmx280_t *dev)
{
    /* no need to acquire a bus here, as this is done in the init function */

    /* allocate some memory to store the largest block of calibration data */
    uint8_t buf[CALIB_T_P_LEN];

    /* read humidity and temperature calibration data */
    if (_read_burst(dev, CALIB_T_P_BASE, buf, CALIB_T_P_LEN) != BMX280_OK) {
        return BMX280_ERR_BUS;
    }

    /* convert calibration values to little endian format and save them */
    dev->calibration.dig_T1 = _to_u16_le(buf, OFFSET_T_P(BMX280_DIG_T1_LSB_REG));
    dev->calibration.dig_T2 = _to_i16_le(buf, OFFSET_T_P(BMX280_DIG_T2_LSB_REG));
    dev->calibration.dig_T3 = _to_i16_le(buf, OFFSET_T_P(BMX280_DIG_T3_LSB_REG));

    dev->calibration.dig_P1 = _to_u16_le(buf, OFFSET_T_P(BMX280_DIG_P1_LSB_REG));
    dev->calibration.dig_P2 = _to_i16_le(buf, OFFSET_T_P(BMX280_DIG_P2_LSB_REG));
    dev->calibration.dig_P3 = _to_i16_le(buf, OFFSET_T_P(BMX280_DIG_P3_LSB_REG));
    dev->calibration.dig_P4 = _to_i16_le(buf, OFFSET_T_P(BMX280_DIG_P4_LSB_REG));
    dev->calibration.dig_P5 = _to_i16_le(buf, OFFSET_T_P(BMX280_DIG_P5_LSB_REG));
    dev->calibration.dig_P6 = _to_i16_le(buf, OFFSET_T_P(BMX280_DIG_P6_LSB_REG));
    dev->calibration.dig_P7 = _to_i16_le(buf, OFFSET_T_P(BMX280_DIG_P7_LSB_REG));
    dev->calibration.dig_P8 = _to_i16_le(buf, OFFSET_T_P(BMX280_DIG_P8_LSB_REG));
    dev->calibration.dig_P9 = _to_i16_le(buf, OFFSET_T_P(BMX280_DIG_P9_LSB_REG));

#if defined(MODULE_BME280_SPI) || defined(MODULE_BME280_I2C)
    /* read dig_H1 in a single read, as this value is not in the block with the
     * rest of the humidity calibration values */
    if (_read_reg(dev, BME280_DIG_H1_REG, &dev->calibration.dig_H1) != BMX280_OK) {
        return BMX280_ERR_BUS;
    }

    /* read the block with the rest of the values */
    if (_read_burst(dev, CALIB_H_BASE, buf, CALIB_H_LEN) != BMX280_OK) {
        return BMX280_ERR_BUS;
    }

    /* parse the humidity compensation and store in device descriptor */
    dev->calibration.dig_H2 = _to_i16_le(buf, OFFSET_H(BME280_DIG_H2_LSB_REG));
    dev->calibration.dig_H3 = buf[OFFSET_H(BME280_DIG_H3_REG)];
    dev->calibration.dig_H4 = ((((int16_t)buf[OFFSET_H(BME280_DIG_H4_MSB_REG)]) << 4) +
                               (buf[OFFSET_H(BME280_DIG_H4_H5_REG)] & 0x0F));
    dev->calibration.dig_H5 = ((((int16_t)buf[OFFSET_H(BME280_DIG_H5_MSB_REG)]) << 4) +
                               ((buf[OFFSET_H(BME280_DIG_H4_H5_REG)] & 0xF0) >> 4));
    dev->calibration.dig_H6 = buf[OFFSET_H(BME280_DIG_H6_REG)];
#endif

    return BMX280_OK;
}

/**
 * @brief   Trigger a new measurement (if applicable) and read raw data
 */
static int _do_measurement(bmx280_t *dev)
{
    uint8_t reg;

    /* get access to the bus */
    if (_acquire(dev) != BMX280_OK) {
        goto err;
    }

    /* if in FORCED mode, we need to manually trigger a measurement */
    if (dev->params.run_mode != BMX280_MODE_NORMAL) {
        reg = ((dev->params.temp_oversample << MEAS_OSRS_T_POS) |
               (dev->params.press_oversample << MEAS_OSRS_P_POS) |
               BMX280_MODE_FORCED);
        if (_write_reg(dev, BMX280_CTRL_MEAS_REG, reg) != BMX280_OK) {
            goto err;
        }
        do {
            if (_read_reg(dev, BMX280_STAT_REG, &reg) != BMX280_OK) {
                goto err;
            }
        } while (reg & STAT_MEASURING);
        /* results are ready now */
        DEBUG("[bmx280] _do_measurement: measurement data ready\n");
    }

    /* read all raw data registers into data buffer */
    if (_read_burst(dev, DATA_BASE, RAW_DATA, BMX280_RAW_LEN) != BMX280_OK) {
        goto err;
    }

    /* we are done reading from the device, so release the bus again */
    _release(dev);
    return BMX280_OK;

err:
    _release(dev);
    return BMX280_ERR_BUS;
}

int bmx280_init(bmx280_t *dev, const bmx280_params_t *params)
{
    assert(dev && params);

    dev->params = *params;
    uint8_t reg;


#ifdef BMX280_USE_SPI
    /* configure the chip-select pin */
    if (spi_init_cs(BUS, CS) != SPI_OK) {
        DEBUG("[bmx280] error: unable to configure chip the select pin\n");
        return BMX280_ERR_BUS;
    }
#endif

    /* acquire bus bus, this also tests the bus parameters in SPI mode */
    if (_acquire(dev) != BMX280_OK) {
        DEBUG("[bmx280] error: unable to acquire bus\n");
        return BMX280_ERR_BUS;
    }

    /* test the connection to the device by reading and verifying its chip ID */
    if (_read_reg(dev, BMX280_CHIP_ID_REG, &reg) != BMX280_OK) {
        DEBUG("[bmx280] error: unable to read chip ID from device\n");
        _release(dev);
        return BMX280_ERR_NODEV;
    }
    if (reg != BMX280_CHIP_ID_VAL) {
        DEBUG("[bmx280] error: invalid chip ID (0x%02x)\n", (int)reg);
        _release(dev);
        return BMX280_ERR_NODEV;
    }

    /* trigger a power-on reset sequence to reset all registers */
    if (_write_reg(dev, BMEX80_RST_REG, RESET_WORD) != BMX280_OK) {
        goto err;
    }
    /* wait for reset sequence to finish */
    do {
        if (_read_reg(dev, BMX280_STAT_REG, &reg) != BMX280_OK) {
            goto err;
        }
    } while (reg != 0);

    /* read the compensation data from the sensor's ROM */
    if (_read_calibration_data(dev) != BMX280_OK) {
        DEBUG("[bmx280] error: could not read calibration data\n");
        goto err;
    }

    /* write basic device configuration: t_sb and filter values */
    reg = (dev->params.t_sb | dev->params.filter);
    if (_write_reg(dev, BMX280_CONFIG_REG, reg) != BMX280_OK) {
        goto err;
    }

#if defined(MODULE_BME280_SPI) || defined(MODULE_BME280_I2C)
    /* ctrl_hum must be written before ctrl_meas for changes to become
     * effective */
    reg = dev->params.humid_oversample;
    if (_write_reg(dev, BME280_CTRL_HUM_REG, reg) != BMX280_OK) {
        goto err;
    }
#endif

    /* finally apply the temperature and pressure oversampling configuration and
     * configure the run mode */
    reg = ((dev->params.temp_oversample << MEAS_OSRS_T_POS) |
           (dev->params.press_oversample << MEAS_OSRS_P_POS) |
           (dev->params.run_mode));
    if (_write_reg(dev, BMX280_CTRL_MEAS_REG, reg) != BMX280_OK) {
        goto err;
    }

    _release(dev);
    return BMX280_OK;

err:
    _release(dev);
    DEBUG("[bmx280] init: bus error while initializing device\n");
    return BMX280_ERR_BUS;
}

int16_t bmx280_read_temperature(bmx280_t *dev)
{
    assert(dev);

    if (_do_measurement(dev) < 0) {
        return INT16_MIN;
    }

    const bmx280_calibration_t *cal = &dev->calibration; /* helper variable */

    /* Read the uncompensated temperature */
    int32_t adc_T = (((uint32_t)RAW_DATA[3 + 0]) << 12) |
        (((uint32_t)RAW_DATA[3 + 1]) << 4) |
        ((((uint32_t)RAW_DATA[3 + 2]) >> 4) & 0x0F);

    /*
     * Compensate the temperature value.
     * The following is code from Bosch's BME280_driver
     * bme280_compensate_temperature_int32(). The variable names and the many
     * defines have been modified to make the code more readable.
     */
    int32_t var1;
    int32_t var2;

    var1 = ((((adc_T >> 3) - ((int32_t)cal->dig_T1 << 1))) * ((int32_t)cal->dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((int32_t)cal->dig_T1)) * ((adc_T >> 4) - ((int32_t)cal->dig_T1))) >> 12) *
            ((int32_t)cal->dig_T3)) >> 14;

    /* calculate t_fine (used for pressure and humidity too) */
    dev->t_fine = var1 + var2;

    return (dev->t_fine * 5 + 128) >> 8;
}

uint32_t bmx280_read_pressure(bmx280_t *dev)
{
    assert(dev);

    bmx280_read_temperature(dev);
    const bmx280_calibration_t *cal = &dev->calibration; /* helper variable */

    /* Read the uncompensated pressure */
    int32_t adc_P = (((uint32_t)RAW_DATA[0 + 0]) << 12) |
        (((uint32_t)RAW_DATA[0 + 1]) << 4) |
        ((((uint32_t)RAW_DATA[0 + 2]) >> 4) & 0x0F);

    int64_t var1;
    int64_t var2;
    int64_t p_acc;

    /*
     * Compensate the pressure value.
     * The following is code from Bosch's BME280_driver
     * bme280_compensate_pressure_int64(). The variable names and the many
     * defines have been modified to make the code more readable.
     */
    var1 = ((int64_t)dev->t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)cal->dig_P6;
    var2 = var2 + ((var1 * (int64_t)cal->dig_P5) << 17);
    var2 = var2 + (((int64_t)cal->dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)cal->dig_P3) >> 8) + ((var1 * (int64_t)cal->dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)cal->dig_P1) >> 33;
    /* Avoid division by zero */
    if (var1 == 0) {
        return UINT32_MAX;
    }

    p_acc = 1048576 - adc_P;
    p_acc = (((p_acc << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)cal->dig_P9) * (p_acc >> 13) * (p_acc >> 13)) >> 25;
    var2 = (((int64_t)cal->dig_P8) * p_acc) >> 19;
    p_acc = ((p_acc + var1 + var2) >> 8) + (((int64_t)cal->dig_P7) << 4);

    return p_acc >> 8;
}

#if defined(MODULE_BME280_SPI) || defined(MODULE_BME280_I2C)
uint16_t bme280_read_humidity(bmx280_t *dev)
{
    assert(dev);

    bmx280_read_temperature(dev);
    const bmx280_calibration_t *cal = &dev->calibration; /* helper variable */

    /* Read the uncompensated pressure */
    int32_t adc_H = (((uint32_t)RAW_DATA[6 + 0]) << 8) |
        (((uint32_t)RAW_DATA[6 + 1]));

    /*
     * Compensate the humidity value.
     * The following is code from Bosch's BME280_driver
     * bme280_compensate_humidity_int32(). The variable names and the many
     * defines have been modified to make the code more readable.
     * The value is first computed as a value in %rH as unsigned 32bit integer
     * in Q22.10 format(22 integer 10 fractional bits).
     */
    int32_t var1;

    /* calculate x1*/
    var1 = (dev->t_fine - ((int32_t)76800));
    /* calculate x1*/
    var1 = (((((adc_H << 14) - (((int32_t)cal->dig_H4) << 20) - (((int32_t)cal->dig_H5) * var1)) +
              ((int32_t)16384)) >> 15) *
            (((((((var1 * ((int32_t)cal->dig_H6)) >> 10) *
                 (((var1 * ((int32_t)cal->dig_H3)) >> 11) + ((int32_t)32768))) >> 10) +
               ((int32_t)2097152)) * ((int32_t)cal->dig_H2) + 8192) >> 14));
    var1 = (var1 - (((((var1 >> 15) * (var1 >> 15)) >> 7) * ((int32_t)cal->dig_H1)) >> 4));
    var1 = (var1 < 0) ? 0 : var1;
    var1 = (var1 > 419430400) ? 419430400 : var1;
    /* First multiply to avoid losing the accuracy after the shift by ten */
    return (100 * ((uint32_t)var1 >> 12)) >> 10;
}
#endif

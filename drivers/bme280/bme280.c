/*
 * Copyright (C) 2016 Kees Bakker, SODAQ
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_bme280
 * @{
 *
 * @file
 * @brief       Device driver implementation for the BME280 temperature,
 *              pressure and humidity sensor.
 *
 * @author      Kees Bakker <kees@sodaq.com>
 *
 * @}
 */

#include <string.h>
#include <math.h>

#include "log.h"
#include "bme280.h"
#include "bme280_internals.h"
#include "bme280_params.h"
#include "periph/i2c.h"
#include "xtimer.h"

#define ENABLE_DEBUG        (0)
#include "debug.h"

static int read_calibration_data(bme280_t* dev);
static int do_measurement(bme280_t* dev);
static uint8_t get_ctrl_meas(bme280_t* dev);
static uint8_t get_status(bme280_t* dev);
static uint8_t read_u8_reg(bme280_t* dev, uint8_t reg);
static void write_u8_reg(bme280_t* dev, uint8_t reg, uint8_t b);
static uint16_t get_uint16_le(const uint8_t *buffer, size_t offset);
static int16_t get_int16_le(const uint8_t *buffer, size_t offset);

#if ENABLE_DEBUG
static void dump_buffer(const char *txt, uint8_t *buffer, size_t size);
#define DUMP_BUFFER(txt, buffer, size)  dump_buffer(txt, buffer, size)
#else
#define DUMP_BUFFER(txt, buffer, size)
#endif

/**
 * @brief   Fine resolution temperature value, also needed for pressure and humidity.
 */
static int32_t t_fine;

/**
 * @brief   The measurement registers, including temperature, pressure and humidity
 *
 * A temporary buffer for the memory map 0xF7..0xFE
 * These are read all at once and then used to compute the three sensor values.
 */
static uint8_t measurement_regs[8];

/*---------------------------------------------------------------------------*
 *                          BME280 Core API                                  *
 *---------------------------------------------------------------------------*/

int bme280_init(bme280_t* dev, const bme280_params_t* params)
{
    uint8_t chip_id;

    dev->params = *params;

    /* Initialize I2C interface */
    if (i2c_init_master(dev->params.i2c_dev, I2C_SPEED_NORMAL)) {
        DEBUG("[Error] I2C device not enabled\n");
        return BME280_ERR_I2C;
    }

    /* Read chip ID */
    chip_id = read_u8_reg(dev, BME280_CHIP_ID_REG);
    if (chip_id != BME280_CHIP_ID) {
        DEBUG("[Error] Did not detect a BME280 at address %02x (%02X != %02X)\n",
              dev->params.i2c_addr, chip_id, BME280_CHIP_ID);
        return BME280_ERR_NODEV;
    }

    /* Read compensation data, 0x88..0x9F, 0xA1, 0xE1..0xE7 */
    if (read_calibration_data(dev)) {
        DEBUG("[Error] Could not read calibration data\n");
        return BME280_ERR_NOCAL;
    }

    return BME280_OK;
}

/*
 * Returns temperature in DegC, resolution is 0.01 DegC.
 * t_fine carries fine temperature as global value
 */
int16_t bme280_read_temperature(bme280_t* dev)
{
    if (do_measurement(dev) < 0) {
        return INT16_MIN;
    }

    bme280_calibration_t *cal = &dev->calibration;      /* helper variable */

    /* Read the uncompensated temperature */
    int32_t adc_T = (((uint32_t)measurement_regs[3 + 0]) << 12) |
        (((uint32_t)measurement_regs[3 + 1]) << 4) |
        ((((uint32_t)measurement_regs[3 + 2]) >> 4) & 0x0F);

    /*
     * Compensate the temperature value.
     * The following is code from Bosch's BME280_driver bme280_compensate_temperature_int32()
     * The variable names and the many defines have been modified to make the code
     * more readable.
     */
    int32_t var1;
    int32_t var2;

    var1 = ((((adc_T >> 3) - ((int32_t)cal->dig_T1 << 1))) * ((int32_t)cal->dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((int32_t)cal->dig_T1)) * ((adc_T >> 4) - ((int32_t)cal->dig_T1))) >> 12) *
            ((int32_t)cal->dig_T3)) >> 14;

    /* calculate t_fine (used for pressure and humidity too) */
    t_fine = var1 + var2;

    return (t_fine * 5 + 128) >> 8;
}

/*
 * Returns pressure in Pa
 */
uint32_t bme280_read_pressure(bme280_t *dev)
{
    bme280_calibration_t *cal = &dev->calibration;      /* helper variable */

    /* Read the uncompensated pressure */
    int32_t adc_P = (((uint32_t)measurement_regs[0 + 0]) << 12) |
        (((uint32_t)measurement_regs[0 + 1]) << 4) |
        ((((uint32_t)measurement_regs[0 + 2]) >> 4) & 0x0F);

    int64_t var1;
    int64_t var2;
    int64_t p_acc;

    /*
     * Compensate the pressure value.
     * The following is code from Bosch's BME280_driver bme280_compensate_pressure_int64()
     * The variable names and the many defines have been modified to make the code
     * more readable.
     */
    var1 = ((int64_t)t_fine) - 128000;
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

uint16_t bme280_read_humidity(bme280_t *dev)
{
    bme280_calibration_t *cal = &dev->calibration;      /* helper variable */

    /* Read the uncompensated pressure */
    int32_t adc_H = (((uint32_t)measurement_regs[6 + 0]) << 8) |
        (((uint32_t)measurement_regs[6 + 1]));

    /*
     * Compensate the humidity value.
     * The following is code from Bosch's BME280_driver bme280_compensate_humidity_int32()
     * The variable names and the many defines have been modified to make the code
     * more readable.
     * The value is first computed as a value in %rH as unsigned 32bit integer
     * in Q22.10 format(22 integer 10 fractional bits).
     */
    int32_t var1;

    /* calculate x1*/
    var1 = (t_fine - ((int32_t)76800));
    /* calculate x1*/
    var1 = (((((adc_H << 14) - (((int32_t)cal->dig_H4) << 20) - (((int32_t)cal->dig_H5) * var1)) +
              ((int32_t)16384)) >> 15) *
            (((((((var1 * ((int32_t)cal->dig_H6)) >> 10) *
                 (((var1 * ((int32_t)cal->dig_H3)) >> 11) + ((int32_t)32768))) >> 10) +
               ((int32_t)2097152)) * ((int32_t)cal->dig_H2) + 8192) >> 14));
    var1 = (var1 - (((((var1 >> 15) * (var1 >> 15)) >> 7) * ((int32_t)cal->dig_H1)) >> 4));
    var1 = (var1 < 0 ? 0 : var1);
    var1 = (var1 > 419430400 ? 419430400 : var1);
    /* First multiply to avoid losing the accuracy after the shift by ten */
    return (100 * ((uint32_t)var1 >> 12)) >> 10;
}

/**
 * Read compensation data, 0x88..0x9F, 0xA1, 0xE1..0xE7
 *
 * This function reads all calibration bytes at once. These are
 * the registers DIG_T1_LSB (0x88) upto and including DIG_H6 (0xE7).
 */
static int read_calibration_data(bme280_t* dev)
{
    uint8_t buffer[128];        /* 128 should be enough to read all calibration bytes */
    int nr_bytes;
    int nr_bytes_to_read = (BME280_DIG_H6_REG - BME280_DIG_T1_LSB_REG) + 1;
    uint8_t offset = 0x88;

    memset(buffer, 0, sizeof(buffer));
    nr_bytes = i2c_read_regs(dev->params.i2c_dev, dev->params.i2c_addr, offset,
                             buffer, nr_bytes_to_read);
    if (nr_bytes != nr_bytes_to_read) {
        LOG_ERROR("Unable to read calibration data\n");
        return -1;
    }
    DUMP_BUFFER("Raw Calibration Data", buffer, nr_bytes);

    /* All little endian */
    dev->calibration.dig_T1 = get_uint16_le(buffer, BME280_DIG_T1_LSB_REG - offset);
    dev->calibration.dig_T2 = get_int16_le(buffer, BME280_DIG_T2_LSB_REG - offset);
    dev->calibration.dig_T3 = get_int16_le(buffer, BME280_DIG_T3_LSB_REG - offset);

    dev->calibration.dig_P1 = get_uint16_le(buffer, BME280_DIG_P1_LSB_REG - offset);
    dev->calibration.dig_P2 = get_int16_le(buffer, BME280_DIG_P2_LSB_REG - offset);
    dev->calibration.dig_P3 = get_int16_le(buffer, BME280_DIG_P3_LSB_REG - offset);
    dev->calibration.dig_P4 = get_int16_le(buffer, BME280_DIG_P4_LSB_REG - offset);
    dev->calibration.dig_P5 = get_int16_le(buffer, BME280_DIG_P5_LSB_REG - offset);
    dev->calibration.dig_P6 = get_int16_le(buffer, BME280_DIG_P6_LSB_REG - offset);
    dev->calibration.dig_P7 = get_int16_le(buffer, BME280_DIG_P7_LSB_REG - offset);
    dev->calibration.dig_P8 = get_int16_le(buffer, BME280_DIG_P8_LSB_REG - offset);
    dev->calibration.dig_P9 = get_int16_le(buffer, BME280_DIG_P9_LSB_REG - offset);

    dev->calibration.dig_H1 = buffer[BME280_DIG_H1_REG - offset];
    dev->calibration.dig_H2 = get_int16_le(buffer, BME280_DIG_H2_LSB_REG - offset);
    dev->calibration.dig_H3 = buffer[BME280_DIG_H3_REG - offset];
    dev->calibration.dig_H4 = (((int16_t)buffer[BME280_DIG_H4_MSB_REG - offset]) << 4) +
        (buffer[BME280_DIG_H4_H5_REG - offset] & 0x0F);
    dev->calibration.dig_H5 = (((int16_t)buffer[BME280_DIG_H5_MSB_REG - offset]) << 4) +
        ((buffer[BME280_DIG_H4_H5_REG - offset] & 0xF0) >> 4);
    dev->calibration.dig_H6 = buffer[BME280_DIG_H6_REG - offset];

    DEBUG("[INFO] Chip ID = 0x%02X\n", buffer[BME280_CHIP_ID_REG - offset]);

    /* Config is only be writable in sleep mode */
    (void)i2c_write_reg(dev->params.i2c_dev, dev->params.i2c_addr,
                        BME280_CTRL_MEAS_REG, 0);

    uint8_t b;

    /* Config Register */
    /* spi3w_en unused */
    b = ((dev->params.t_sb & 7) << 5) | ((dev->params.filter & 7) << 2);
    write_u8_reg(dev, BME280_CONFIG_REG, b);

    /*
     * Note from the datasheet about ctrl_hum: "Changes to this register only become effective
     * after a write operation to "ctrl_meas".
     * So, set ctrl_hum first.
     */
    b = dev->params.humid_oversample & 7;
    write_u8_reg(dev, BME280_CTRL_HUMIDITY_REG, b);

    b = ((dev->params.temp_oversample & 7) << 5) |
        ((dev->params.press_oversample & 7) << 2) |
        (dev->params.run_mode & 3);
    write_u8_reg(dev, BME280_CTRL_MEAS_REG, b);

    return 0;
}

/**
 * @brief Start a measurement and read the registers
 */
static int do_measurement(bme280_t* dev)
{
    /*
     * If settings has FORCED mode, then the device go to sleep after
     * it finished the measurement. To read again we have to set the
     * run_mode back to FORCED.
     */
    uint8_t ctrl_meas = get_ctrl_meas(dev);
    uint8_t run_mode = ctrl_meas & 3;
    if (run_mode != dev->params.run_mode) {
        /* Set the run_mode back to what we want. */
        ctrl_meas &= ~3;
        ctrl_meas |= dev->params.run_mode;
        write_u8_reg(dev, BME280_CTRL_MEAS_REG, ctrl_meas);

        /* Wait for measurement ready? */
        size_t count = 0;
        while (count < 10 && (get_status(dev) & 0x08) != 0) {
            ++count;
        }
        /* What to do when measuring is still on? */
    }
    int nr_bytes;
    int nr_bytes_to_read = sizeof(measurement_regs);
    uint8_t offset = BME280_PRESSURE_MSB_REG;

    nr_bytes = i2c_read_regs(dev->params.i2c_dev, dev->params.i2c_addr,
                             offset, measurement_regs, nr_bytes_to_read);
    if (nr_bytes != nr_bytes_to_read) {
        LOG_ERROR("Unable to read temperature data\n");
        return -1;
    }
    DUMP_BUFFER("Raw Sensor Data", measurement_regs, nr_bytes);

    return 0;
}

static uint8_t get_ctrl_meas(bme280_t* dev)
{
    return read_u8_reg(dev, BME280_CTRL_MEAS_REG);
}

static uint8_t get_status(bme280_t* dev)
{
    return read_u8_reg(dev, BME280_STAT_REG);
}

static uint8_t read_u8_reg(bme280_t* dev, uint8_t reg)
{
    uint8_t b;
    /* Assuming device is correct, it should return 1 (nr bytes) */
    (void)i2c_read_reg(dev->params.i2c_dev, dev->params.i2c_addr, reg, &b);
    return b;
}

static void write_u8_reg(bme280_t* dev, uint8_t reg, uint8_t b)
{
    /* Assuming device is correct, it should return 1 (nr bytes) */
    (void)i2c_write_reg(dev->params.i2c_dev, dev->params.i2c_addr, reg, b);
}

static uint16_t get_uint16_le(const uint8_t *buffer, size_t offset)
{
    return (((uint16_t)buffer[offset + 1]) << 8) + buffer[offset];
}

static int16_t get_int16_le(const uint8_t *buffer, size_t offset)
{
    return (((int16_t)buffer[offset + 1]) << 8) + buffer[offset];
}

#if ENABLE_DEBUG
static void dump_buffer(const char *txt, uint8_t *buffer, size_t size)
{
    size_t ix;
    DEBUG("%s\n", txt);
    for (ix = 0; ix < size; ix++) {
        DEBUG("%02X", buffer[ix]);
        if ((ix + 1) == size || (((ix + 1) % 16) == 0)) {
            DEBUG("\n");
        }
    }
}
#endif

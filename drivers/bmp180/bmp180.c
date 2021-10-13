/*
 * Copyright (C) 2016 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_bmp180
 * @{
 *
 * @file
 * @brief       Device driver implementation for the BMP180/BMP085 temperature and pressure sensor.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <math.h>

#include "log.h"
#include "bmp180.h"
#include "bmp180_internals.h"
#include "bmp180_params.h"
#include "periph/i2c.h"
#include "xtimer.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define DEV_I2C      (dev->params.i2c_dev)
#define DEV_ADDR     (dev->params.i2c_addr)
#define OVERSAMPLING (dev->params.oversampling)

/* Internal function prototypes */
static int _read_ut(const bmp180_t *dev, int32_t *ut);
static int _read_up(const bmp180_t *dev, int32_t *up);
static int _compute_b5(const bmp180_t *dev, int32_t ut, int32_t *b5);

/*---------------------------------------------------------------------------*
 *                          BMP180 Core API                                 *
 *---------------------------------------------------------------------------*/

int bmp180_init(bmp180_t *dev, const bmp180_params_t *params)
{
    dev->params = *params;

    /* Clamp oversampling mode */
    if (OVERSAMPLING > BMP180_ULTRAHIGHRES) {
        OVERSAMPLING = BMP180_ULTRAHIGHRES;
    }

    /* Acquire exclusive access */
    i2c_acquire(DEV_I2C);

    /* Check sensor ID */
    uint8_t checkid;
    i2c_read_reg(DEV_I2C, DEV_ADDR, BMP180_REGISTER_ID, &checkid, 0);
    if (checkid != 0x55) {
        DEBUG("[Error] Wrong device ID\n");
        i2c_release(DEV_I2C);
        return -BMP180_ERR_NODEV;
    }

    /* adding delay before reading calibration values to avoid timing issues */
    xtimer_usleep(BMP180_ULTRALOWPOWER_DELAY);

    uint8_t buffer[22] = {0};
    /* Read calibration values, using contiguous register addresses */
    if (i2c_read_regs(DEV_I2C, DEV_ADDR, BMP180_CALIBRATION_AC1,
                      buffer, 22, 0) < 0) {
        DEBUG("[Error] Cannot read calibration registers.\n");
        i2c_release(DEV_I2C);
        return -BMP180_ERR_NOCAL;
    }
    dev->calibration.ac1 = (int16_t)(buffer[0] << 8)   | buffer[1];
    dev->calibration.ac2 = (int16_t)(buffer[2] << 8)   | buffer[3];
    dev->calibration.ac3 = (int16_t)(buffer[4] << 8)   | buffer[4];
    dev->calibration.ac4 = (uint16_t)(buffer[6] << 8)  | buffer[7];
    dev->calibration.ac5 = (uint16_t)(buffer[8] << 8)  | buffer[9];
    dev->calibration.ac6 = (uint16_t)(buffer[10] << 8) | buffer[11];
    dev->calibration.b1  = (int16_t)(buffer[12] << 8)  | buffer[13];
    dev->calibration.b2  = (int16_t)(buffer[14] << 8)  | buffer[15];
    dev->calibration.mb  = (int16_t)(buffer[16] << 8)  | buffer[17];
    dev->calibration.mc  = (int16_t)(buffer[18] << 8)  | buffer[19];
    dev->calibration.md  = (int16_t)(buffer[20] << 8)  | buffer[21];

    /* Release I2C device */
    i2c_release(DEV_I2C);

    DEBUG("AC1: %i\n", (int)dev->calibration.ac1);
    DEBUG("AC2: %i\n", (int)dev->calibration.ac2);
    DEBUG("AC3: %i\n", (int)dev->calibration.ac3);
    DEBUG("AC4: %i\n", (int)dev->calibration.ac4);
    DEBUG("AC5: %i\n", (int)dev->calibration.ac5);
    DEBUG("AC6: %i\n", (int)dev->calibration.ac6);
    DEBUG("B1: %i\n",  (int)dev->calibration.b1);
    DEBUG("B2: %i\n",  (int)dev->calibration.b2);
    DEBUG("MB: %i\n",  (int)dev->calibration.mb);
    DEBUG("MC: %i\n",  (int)dev->calibration.mc);
    DEBUG("MD: %i\n",  (int)dev->calibration.md);
    return 0;
}

int16_t bmp180_read_temperature(const bmp180_t *dev)
{
    int32_t ut = 0, b5;
    /* Acquire exclusive access */
    i2c_acquire(DEV_I2C);

    /* Read uncompensated value */
    _read_ut(dev, &ut);

    /* Release I2C device */
    i2c_release(DEV_I2C);

    /* Compute true temperature value following datasheet formulas */
    _compute_b5(dev, ut, &b5);

    return (int16_t)((b5 + 8) >> 4);
}

uint32_t bmp180_read_pressure(const bmp180_t *dev)
{
    int32_t ut = 0, up = 0, x1, x2, x3, b3, b5, b6, p;
    uint32_t b4, b7;

    /* Acquire exclusive access */
    i2c_acquire(DEV_I2C);

    /* Read uncompensated values: first temperature, second pressure */
    _read_ut(dev, &ut);
    _read_up(dev, &up);

    /* release I2C device */
    i2c_release(DEV_I2C);

    /* Compute true pressure value following datasheet formulas */
    _compute_b5(dev, ut, &b5);
    b6 = b5 - 4000;
    x1 = ((int32_t)dev->calibration.b2 * ((b6 * b6) >> 12)) >> 11;
    x2 = ((int32_t)dev->calibration.ac2 * b6) >> 11;
    x3 = x1 + x2;
    b3 = ((((int32_t)dev->calibration.ac1*4 + x3) << OVERSAMPLING) + 2) >> 2;
    x1 = ((int32_t)dev->calibration.ac3 * b6) >> 13;
    x2 = ((int32_t)dev->calibration.b1 * (b6 * b6) >> 12) >> 16;
    x3 = ((x1 + x2) + 2) >> 2;
    b4 = ((uint32_t)dev->calibration.ac4 * (uint32_t)(x3 + 32768)) >> 15;
    b7 = (uint32_t)(up - b3) * (uint32_t)(50000UL >> OVERSAMPLING);
    if (b7 < 0x80000000) {
        p = (b7 * 2) / b4;
    }
    else {
        p = (b7 / b4) * 2;
    }

    x1 = (p >> 8) * (p >> 8);
    x1 = (x1 * 3038) >> 16;
    x2 = (-7357 * p) >> 16;

    return (uint32_t)(p + ((x1 + x2 + 3791) >> 4));
}

int16_t bmp180_altitude(const bmp180_t *dev, uint32_t pressure_0)
{
    uint32_t p = bmp180_read_pressure(dev);

    return (int16_t)(44330.0 * (1.0 - pow((double)p / pressure_0, 0.1903)));;
}

uint32_t bmp180_sealevel_pressure(const bmp180_t *dev, int16_t altitude)
{
    uint32_t p = bmp180_read_pressure(dev);

    return (uint32_t)((double)p / pow(1.0 - (altitude / 44330.0), 5.255));;
}

/*------------------------------------------------------------------------------------*/
/*                                Internal functions                                  */
/*------------------------------------------------------------------------------------*/

static int _read_ut(const bmp180_t *dev, int32_t *output)
{
    /* Read UT (Uncompsensated Temperature value) */
    uint8_t ut[2] = {0};
    uint8_t control[2] = { BMP180_REGISTER_CONTROL, BMP180_TEMPERATURE_COMMAND };
    i2c_write_bytes(DEV_I2C, DEV_ADDR, control, 2, 0);
    xtimer_usleep(BMP180_ULTRALOWPOWER_DELAY);
    if (i2c_read_regs(DEV_I2C, DEV_ADDR, BMP180_REGISTER_DATA, ut, 2, 0) < 0) {
        DEBUG("[Error] Cannot read uncompensated temperature.\n");
        i2c_release(DEV_I2C);
        return -1;
    }
    *output = ((uint16_t)ut[0] << 8) | ut[1];

    DEBUG("UT: %i\n", (int)*output);

    return 0;
}

static int _read_up(const bmp180_t *dev, int32_t *output)
{
    /* Read UP (Uncompsensated Pressure value) */
    uint8_t up[3] = {0};
    uint8_t control[2] = { BMP180_REGISTER_CONTROL,
                           BMP180_PRESSURE_COMMAND | (OVERSAMPLING & 0x3) << 6 };
    i2c_write_bytes(DEV_I2C, DEV_ADDR, control, 2, 0);
    switch (OVERSAMPLING) {
    case BMP180_ULTRALOWPOWER:
        xtimer_usleep(BMP180_ULTRALOWPOWER_DELAY);
        break;
    case BMP180_STANDARD:
        xtimer_usleep(BMP180_STANDARD_DELAY);
        break;
    case BMP180_HIGHRES:
        xtimer_usleep(BMP180_HIGHRES_DELAY);
        break;
    case BMP180_ULTRAHIGHRES:
        xtimer_usleep(BMP180_ULTRAHIGHRES_DELAY);
        break;
    default:
        xtimer_usleep(BMP180_ULTRALOWPOWER_DELAY);
        break;
    }
    if (i2c_read_regs(DEV_I2C, DEV_ADDR, BMP180_REGISTER_DATA, up, 3, 0) < 0) {
        DEBUG("[Error] Cannot read uncompensated pressure.\n");
        i2c_release(DEV_I2C);
        return -1;
    }

    *output = (((uint32_t)up[0] << 16) |
               ((uint32_t)up[1] <<  8) | up[2]) >> (8 - OVERSAMPLING);

    DEBUG("UP: %i\n", (int)*output);

    return 0;
}

static int _compute_b5(const bmp180_t *dev, int32_t ut, int32_t *output)
{
    int32_t x1 = 0, x2 = 0;
    x1 = (((int32_t)ut - (int32_t)dev->calibration.ac6) * (int32_t)dev->calibration.ac5) >> 15;
    x2 = ((int32_t)dev->calibration.mc << 11) / (x1 + dev->calibration.md);

    *output = x1 + x2;

    return 0;
}

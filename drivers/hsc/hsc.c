/*
 * Copyright (C) 2020 Deutsches Zentrum für Luft- und Raumfahrt e.V. (DLR)
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_hsc
 * @{
 *
 * @file
 * @brief       Device driver implementation for the Honeywell HSC
 *              temperature and pressure sensor.
 *
 * @author      Quang Pham <phhr_quang@live.com>
 *
 * @}
 */

#include "hsc.h"
#include "errno.h"
#include "hsc_internals.h"
#include "hsc_params.h"
#include "periph/i2c.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define DEV_I2C      (dev->params.i2c_dev)
#define DEV_ADDR     (dev->params.i2c_addr)
#define PRESS_RANGE  (dev->params.hsc_range)

/* Internal function prototypes */
static int _read_ut(const hsc_t *dev, int32_t *ut);
static int _read_up(const hsc_t *dev, int32_t *up);

/*---------------------------------------------------------------------------*
 *                          HSC Core API                                     *
 *---------------------------------------------------------------------------*/

int hsc_init(hsc_t *dev, const hsc_params_t *params)
{
    dev->params = *params;
    uint8_t buf[HSC_FULL_DATA_LENGTH];

    /* Acquire exclusive access */
    i2c_acquire(DEV_I2C);

    if (i2c_read_bytes(DEV_I2C, DEV_ADDR, buf, sizeof(buf), 0) < 0) {
        i2c_release(DEV_I2C);
        DEBUG("[hsc] read_bytes fails\n");

        return -EIO;
    }

    i2c_release(DEV_I2C);

    /* Check the status of the sensor */
    uint8_t status = buf[0];

    switch (status & HSC_STATUS_MASK) {
    case HSC_STATUS_OK:
        break;
    case HSC_STATUS_STALE_DATA:
        return -EAGAIN;
    default:
        return -EIO;
    }

    return 0;
}

int hsc_read_temperature(const hsc_t *dev, int16_t *dest)
{
    int32_t ut = 0;
    int retval;

    /* Read uncompensated temperature value */
    if ((retval = _read_ut(dev, &ut)) != 0) {
        return retval;
    }

    /* Formular from section 4.0 in
     * https://sensing.honeywell.com/i2c-comms-digital-output-pressure-sensors-tn-008201-3-en-final-30may12.pdf */
    *dest = ut * 2000 / 2047 - 500;

    return 0;
}

int hsc_read_pressure(const hsc_t *dev, int32_t *dest)
{
    int32_t up = 0;
    int retval;

    /* Read uncompensated pressure value */
    if ((retval = _read_up(dev, &up)) != 0) {
        return retval;
    }

    /* Formular from section 3.0 in
     * https://sensing.honeywell.com/i2c-comms-digital-output-pressure-sensors-tn-008201-3-en-final-30may12.pdf */
    const int32_t output_max = 14745;
    const int32_t output_min = 1636;
    *dest = ((up - output_min) * 2000 * HSC_PARAM_RANGE / (output_max - output_min) - 1000 * HSC_PARAM_RANGE);

    return 0;
}

/*---------------------------------------------------------------------------*
 *                       Internal functions                                  *
 *---------------------------------------------------------------------------*/

static int _read_ut(const hsc_t *dev, int32_t *output)
{
    /* Read UT (Uncompsensated Temperature value) */
    uint8_t buf[HSC_FULL_DATA_LENGTH];

    /* Acquire exclusive access */
    i2c_acquire(DEV_I2C);

    if (i2c_read_bytes(DEV_I2C, DEV_ADDR, buf, sizeof(buf), 0) < 0) {
        i2c_release(DEV_I2C);
        DEBUG("[hsc] read_bytes fails\n");

        return -EIO;
    }

    i2c_release(DEV_I2C);

    *output = (((buf[2] << 8) | buf[3]) >> HSC_TEMPERATURE_SHIFT);

    return 0;
}

static int _read_up(const hsc_t *dev, int32_t *output)
{
    /* Read UP (Uncompsensated Pressure value) */
    uint8_t buf[HSC_FULL_DATA_LENGTH];

    /* Acquire exclusive access */
    i2c_acquire(DEV_I2C);

    if (i2c_read_bytes(DEV_I2C, DEV_ADDR, buf, sizeof(buf), 0) < 0) {
        i2c_release(DEV_I2C);
        DEBUG("[hsc] read_bytes fails\n");

        return -EIO;
    }

    i2c_release(DEV_I2C);

    *output = ((buf[0] << 8) | buf[1]) & HSC_PRESSURE_MASK;

    return 0;
}

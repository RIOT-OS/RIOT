/*
 * Copyright (C) 2023 TU Braunschweig Institut für Betriebssysteme und Rechnerverbund
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_sen5x
 * @{
 *
 * @file
 * @brief       Device driver implementation for the Sensirion Embedded I2C SEN5x Driver
 *
 * @author      Daniel Prigoshij <prigoshi@ibr.cs.tu-bs.de>
 *
 * @}
 */

#include "sen5x.h"
#include "sen5x_constants.h"
#include "sen5x_params.h"
#include "sensirion_i2c_hal.h"
#include "sen5x_i2c.h"

int sen5x_init(sen5x_t *dev, const sen5x_params_t *params)
{
    /* check parameters */
    assert(dev && params);

    dev->params = *params;

    i2c_init(dev->params.i2c_dev);
    return sen5x_reset(dev);
}

int sen5x_reset(const sen5x_t *dev)
{
    assert(dev);
    i2c_acquire(dev->params.i2c_dev);

    int result = sen5x_device_reset();

    i2c_release(dev->params.i2c_dev);
    return result;
}

void sen5x_wake(const sen5x_t *dev)
{
    assert(dev);
    i2c_acquire(dev->params.i2c_dev);

    sen5x_start_measurement();

    i2c_release(dev->params.i2c_dev);
}

void sen5x_sleep(const sen5x_t *dev)
{
    assert(dev);
    i2c_acquire(dev->params.i2c_dev);

    sen5x_stop_measurement();

    i2c_release(dev->params.i2c_dev);
}

void sen5x_clean_fan(const sen5x_t *dev)
{
    assert(dev);
    i2c_acquire(dev->params.i2c_dev);

    sen5x_start_fan_cleaning();

    i2c_release(dev->params.i2c_dev);
}

void sen5x_read_values(const sen5x_t *dev, sen5x_measurement_t *values)
{
    assert(dev && values);
    i2c_acquire(dev->params.i2c_dev);

    sen5x_read_measured_values(
        &values->mass_concentration_pm1p0, &values->mass_concentration_pm2p5,
        &values->mass_concentration_pm4p0, &values->mass_concentration_pm10p0,
        &values->ambient_humidity, &values->ambient_temperature, &values->voc_index,
        &values->nox_index);

    i2c_release(dev->params.i2c_dev);
}

void sen5x_read_pm_values(const sen5x_t *dev, sen5x_measurement_t *values)
{
    assert(dev && values);
    i2c_acquire(dev->params.i2c_dev);

    sen5x_read_measured_pm_values(
        &values->mass_concentration_pm1p0, &values->mass_concentration_pm2p5,
        &values->mass_concentration_pm4p0, &values->mass_concentration_pm10p0,
        &values->number_concentration_pm0p5, &values->number_concentration_pm1p0,
        &values->number_concentration_pm2p5, &values->number_concentration_pm4p0,
        &values->number_concentration_pm10p0, &values->typical_particle_size);

    i2c_release(dev->params.i2c_dev);
}

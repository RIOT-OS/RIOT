/*
 * SPDX-FileCopyrightText: 2023 TU Braunschweig Institut für Betriebssysteme und Rechnerverbund
 * SPDX-License-Identifier: LGPL-2.1-only
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

bool sen5x_data_ready_flag(const sen5x_t *dev)
{
    assert(dev);
    i2c_acquire(dev->params.i2c_dev);

    bool* status = NULL;
    sen5x_read_data_ready(status);

    i2c_release(dev->params.i2c_dev);
    return status;
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

void sen5x_set_temperature_offset(const sen5x_t *dev, int16_t temp_offset,
                                    int16_t slope, uint16_t time_constant)
{
    assert(dev);
    i2c_acquire(dev->params.i2c_dev);

    sen5x_set_temperature_offset_parameters(temp_offset, slope, time_constant);

    i2c_release(dev->params.i2c_dev);
}

void sen5x_get_temperature_offset(const sen5x_t *dev, int16_t *temp_offset,
                                    int16_t *slope, uint16_t *time_constant)
{
    assert(dev && temp_offset && slope && time_constant);
    i2c_acquire(dev->params.i2c_dev);

    sen5x_get_temperature_offset_parameters(temp_offset, slope, time_constant);

    i2c_release(dev->params.i2c_dev);
}

void sen5x_set_warm_start(const sen5x_t *dev, uint16_t warm_start)
{
    assert(dev);
    i2c_acquire(dev->params.i2c_dev);

    sen5x_set_warm_start_parameter(warm_start);

    i2c_release(dev->params.i2c_dev);
}

void sen5x_get_warm_start(const sen5x_t *dev, uint16_t *warm_start)
{
    assert(dev && warm_start);
    i2c_acquire(dev->params.i2c_dev);

    sen5x_get_warm_start_parameter(warm_start);

    i2c_release(dev->params.i2c_dev);
}

void sen5x_set_voc_algorithm_tuning(
    const sen5x_t *dev, int16_t index_offset, int16_t learning_time_offset_hours,
    int16_t learning_time_gain_hours, int16_t gating_max_duration_minutes,
    int16_t std_initial, int16_t gain_factor)
{
    assert(dev);
    i2c_acquire(dev->params.i2c_dev);

    sen5x_set_voc_algorithm_tuning_parameters(
        index_offset, learning_time_offset_hours,
        learning_time_gain_hours, gating_max_duration_minutes,
        std_initial, gain_factor);

    i2c_release(dev->params.i2c_dev);
}

void sen5x_get_voc_algorithm_tuning(
    const sen5x_t *dev, int16_t *index_offset, int16_t *learning_time_offset_hours,
    int16_t *learning_time_gain_hours, int16_t *gating_max_duration_minutes,
    int16_t *std_initial, int16_t *gain_factor)
{
    assert(dev && index_offset && learning_time_offset_hours && learning_time_gain_hours
        && gating_max_duration_minutes && std_initial && gain_factor);
    i2c_acquire(dev->params.i2c_dev);

    sen5x_get_voc_algorithm_tuning_parameters(
        index_offset, learning_time_offset_hours,
        learning_time_gain_hours, gating_max_duration_minutes,
        std_initial, gain_factor);

    i2c_release(dev->params.i2c_dev);
}

void sen5x_set_nox_algorithm_tuning(
    const sen5x_t *dev, int16_t index_offset, int16_t learning_time_offset_hours,
    int16_t learning_time_gain_hours, int16_t gating_max_duration_minutes,
    int16_t std_initial, int16_t gain_factor)
{
    assert(dev);
    i2c_acquire(dev->params.i2c_dev);

    sen5x_set_nox_algorithm_tuning_parameters(
        index_offset, learning_time_offset_hours,
        learning_time_gain_hours, gating_max_duration_minutes,
        std_initial, gain_factor);

    i2c_release(dev->params.i2c_dev);
}

void sen5x_get_nox_algorithm_tuning(
    const sen5x_t *dev, int16_t *index_offset, int16_t *learning_time_offset_hours,
    int16_t *learning_time_gain_hours, int16_t *gating_max_duration_minutes,
    int16_t *std_initial, int16_t *gain_factor)
{
    assert(dev && index_offset && learning_time_offset_hours && learning_time_gain_hours
        && gating_max_duration_minutes && std_initial && gain_factor);
    i2c_acquire(dev->params.i2c_dev);

    sen5x_get_nox_algorithm_tuning_parameters(
        index_offset, learning_time_offset_hours,
        learning_time_gain_hours, gating_max_duration_minutes,
        std_initial, gain_factor);

    i2c_release(dev->params.i2c_dev);
}

void sen5x_set_rht_acceleration(const sen5x_t *dev, uint16_t mode)
{
    assert(dev);
    i2c_acquire(dev->params.i2c_dev);

    sen5x_set_rht_acceleration_mode(mode);

    i2c_release(dev->params.i2c_dev);
}

void sen5x_get_rht_acceleration(const sen5x_t *dev, uint16_t *mode)
{
    assert(dev && mode);
    i2c_acquire(dev->params.i2c_dev);

    sen5x_get_rht_acceleration_mode(mode);

    i2c_release(dev->params.i2c_dev);
}

void sen5x_set_voc_state(const sen5x_t *dev, const uint8_t *state, uint8_t state_size)
{
    assert(dev && state);
    i2c_acquire(dev->params.i2c_dev);

    sen5x_set_voc_algorithm_state(state, state_size);

    i2c_release(dev->params.i2c_dev);
}

void sen5x_get_voc_state(const sen5x_t *dev, uint8_t *state, uint8_t state_size)
{
    assert(dev && state);
    i2c_acquire(dev->params.i2c_dev);

    sen5x_get_voc_algorithm_state(state, state_size);

    i2c_release(dev->params.i2c_dev);
}

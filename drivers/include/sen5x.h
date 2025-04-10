/*
 * Copyright (C) 2023 TU Braunschweig Institut für Betriebssysteme und Rechnerverbund
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    drivers_sen5x Sensirion Embedded I2C SEN5x Driver
 * @ingroup     drivers_sensors
 * @ingroup     drivers_saul
 * @brief       Driver for I2C communication to SEN5x devices.
 *
 * @{
 *
 * @file
 *
 * @author      Daniel Prigoshij <prigoshi@ibr.cs.tu-bs.de>
 */
#include <stdbool.h>
#include <stdint.h>

#include "periph/i2c.h"
#include "saul.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Wrapper for measured values
 */
typedef struct {
    uint16_t mass_concentration_pm1p0;      /**< raw value is scaled with factor 10:
                                                 PM1.0 [µg/m³] = value / 10 */
    uint16_t mass_concentration_pm2p5;      /**< raw value is scaled with factor 10:
                                                 PM2.5 [µg/m³] = value / 10 */
    uint16_t mass_concentration_pm4p0;      /**< raw value is scaled with factor 10:
                                                 PM4.0 [µg/m³] = value / 10 */
    uint16_t mass_concentration_pm10p0;     /**< raw value is scaled with factor 10:
                                                 PM10.0 [µg/m³] = value / 10 */
    uint16_t number_concentration_pm0p5;    /**< raw value is scaled with factor 10:
                                                 PM0.5 [#/cm³] = value / 10 */
    uint16_t number_concentration_pm1p0;    /**< raw value is scaled with factor 10:
                                                 PM1.0 [#/cm³] = value / 10 */
    uint16_t number_concentration_pm2p5;    /**< raw value is scaled with factor 10:
                                                 PM2.5 [#/cm³] = value / 10 */
    uint16_t number_concentration_pm4p0;    /**< raw value is scaled with factor 10:
                                                 PM4.0 [#/cm³] = value / 10 */
    uint16_t number_concentration_pm10p0;   /**< raw value is scaled with factor 10:
                                                 PM10.0 [#/cm³] = value / 10 */
    uint16_t typical_particle_size;         /**< raw value is scaled with factor 1000:
                                                 Size [µm] = value / 1000*/
    int16_t ambient_humidity;               /**< raw value is scaled with factor 100:
                                                 RH [%] = value / 100 */
    int16_t ambient_temperature;            /**< raw value is scaled with factor 200:
                                                 T [°C] = value / 200 */
    int16_t voc_index;                      /**< raw value is scaled with factor 10:
                                                 VOC Index = value / 10 */
    int16_t nox_index;                      /**< raw value is scaled with factor 10:
                                                 NOx Index = value / 10 */
} sen5x_measurement_t;

/**
 * @brief   Device initialization parameters
 */
typedef struct {
    i2c_t i2c_dev;                  /**< I2C device which is used */
    uint8_t i2c_addr;               /**< I2C address */
} sen5x_params_t;

/**
 * @brief   Device descriptor for the driver
 */
typedef struct {
    sen5x_params_t params;                  /**< Device initialization parameters */
} sen5x_t;

/**
 * @brief   Initialize the given device
 *
 * @param[in,out] dev       Device descriptor of the driver
 * @param[in]     params    Initialization parameters
 *
 * @return 0 on success
 * @return > 0 on error
 */
int sen5x_init(sen5x_t *dev, const sen5x_params_t *params);

/**
 * @brief   Execute a reset on the given device
 *
 * @param[in,out] dev       Device descriptor of the driver
 *
 * @return 0 on success
 * @return > 0 on error
 */
int sen5x_reset(const sen5x_t *dev);

/**
 * @brief   Starts a continuous measurement
 *
 * @param[in,out] dev       Device descriptor of the driver
 */
void sen5x_wake(const sen5x_t *dev);

/**
 * @brief   Starts a continuous measurement without PM. Only humidity,
 *          temperature, VOC and NOx are measured.
 *
 * @param[in,out] dev       Device descriptor of the driver
 */
void sen5x_wake_no_pm(const sen5x_t *dev);

/**
 * @brief   Stops the measurement and returns to idle mode
 *
 * @param[in,out] dev       Device descriptor of the driver
 */
void sen5x_sleep(const sen5x_t *dev);

/**
 * @brief   Sets the fan to maximum speed, to clean it within 10 seconds
 *
 * @param[in,out] dev       Device descriptor of the driver
 */
void sen5x_clean_fan(const sen5x_t *dev);

/**
 * @brief   Sets the fan to maximum speed, to clean it within 10 seconds
 *
 * @param[in,out] dev       Device descriptor of the driver
 *
 * @return 0 if no new measurements are available
 * @return 1 if new measuremtns are ready to be read
 */
bool sen5x_data_ready_flag(const sen5x_t *dev);

/**
 * @brief   Read measured mass concentration, humidity and temperature values
 *
 * @param[in,out] dev       Device descriptor of the driver
 * @param[out]    values    Pointer to wrapper containing all measured values
 */
void sen5x_read_values(const sen5x_t *dev, sen5x_measurement_t *values);

/**
 * @brief   Read measured particle matter values
 *
 * @param[in,out] dev       Device descriptor of the driver
 * @param[out]    values    Pointer to wrapper containing all measured values
 */
void sen5x_read_pm_values(const sen5x_t *dev, sen5x_measurement_t *values);

/**
 * @brief   Set a custom temperature offset to the ambient temperature
 *
 * @param[in,out] dev           Device descriptor of the driver
 * @param[in]     temp_offset   Temperature offset in °C
 * @param[in]     slope         Normalized temperature offset slope
 * @param[in]     time_constant Time constant in seconds
 */
void sen5x_set_temperature_offset(const sen5x_t *dev, int16_t temp_offset,
                                    int16_t slope, uint16_t time_constant);

/**
 * @brief   Set a custom temperature offset to the ambient temperature
 *
 * @param[in,out] dev            Device descriptor of the driver
 * @param[out]    temp_offset    Temperature offset in °C
 * @param[out]    slope          Normalized temperature offset slope
 * @param[out]    time_constant  Time constant in seconds
 */
void sen5x_get_temperature_offset(const sen5x_t *dev, int16_t *temp_offset,
                                    int16_t *slope, uint16_t *time_constant);

/**
 * @brief   Set the parameter for a warm start on the device, to improve initial
 *          accuracy of the ambient temperature output
 *
 * @param[in,out] dev           Device descriptor of the driver
 * @param[in]     warm_start    Warm start behavior as a value in the range from
 *                              0 (cold start, default) to 65535 (warm start).
 */
void sen5x_set_warm_start(const sen5x_t *dev, uint16_t warm_start);

/**
 * @brief   Get the warm start parameter
 *
 * @param[in,out] dev           Device descriptor of the driver
 * @param[out]    warm_start    Warm start behavior as a value in the range from
 *                              0 (cold start, default) to 65535 (warm start).
 */
void sen5x_get_warm_start(const sen5x_t *dev, uint16_t *warm_start);

/**
 * @brief   Set the parameters for the VOC Algorithm tuning
 *
 * @param[in,out] dev                           Device descriptor of the driver
 * @param[in]     index_offset                  VOC index representing typical (average) conditions
 * @param[in]     learning_time_offset_hours    Time constant to estimate the VOC
 *                                              algorithm offset from the history in hours
 * @param[in]     learning_time_gain_hours      Time constant to estimate the VOC
 *                                              algorithm gain from the history in hours
 * @param[in]     gating_max_duration_minutes   Maximum duration of gating in minutes
 * @param[in]     std_initial                   Initial estimate for standard deviation
 * @param[in]     gain_factor                   Gain factor to amplify or to attenuate
 *                                              the VOC index output
 */
void sen5x_set_voc_algorithm_tuning(
    const sen5x_t *dev, int16_t index_offset, int16_t learning_time_offset_hours,
    int16_t learning_time_gain_hours, int16_t gating_max_duration_minutes,
    int16_t std_initial, int16_t gain_factor);

/**
 * @brief   Get the VOC Algorithm tuning parameters
 *
 * @param[in,out] dev                            Device descriptor of the driver
 * @param[out]    index_offset                   VOC index representing typical
 *                                               (average) conditions
 * @param[out]    learning_time_offset_hours     Time constant to estimate the VOC
 *                                               algorithm offset from the history in hours
 * @param[out]    learning_time_gain_hours       Time constant to estimate the VOC
 *                                               algorithm gain from the history in hours
 * @param[out]    gating_max_duration_minutes    Maximum duration of gating in minutes
 * @param[out]    std_initial                    Initial estimate for standard deviation
 * @param[out]    gain_factor                    Gain factor to amplify or to attenuate
 *                                               the VOC index output
 */
void sen5x_get_voc_algorithm_tuning(
    const sen5x_t *dev, int16_t *index_offset, int16_t *learning_time_offset_hours,
    int16_t *learning_time_gain_hours, int16_t *gating_max_duration_minutes,
    int16_t *std_initial, int16_t *gain_factor);

/**
 * @brief   Set the parameters for the NOx Algorithm tuning
 *
 * @param[in,out] dev                           Device descriptor of the driver
 * @param[in]     index_offset                  NOx index representing typical (average) conditions
 * @param[in]     learning_time_offset_hours    Time constant to estimate the NOx algorithm offset
 *                                              from the history in hours
 * @param[in]     learning_time_gain_hours      The time constant to estimate the
 *                                              NOx algorithm gain from the history has no
 *                                              impact for NOx. This parameter is still in place
 *                                              for consistency reasons with the VOC tuning
 *                                              parameters command.
 *                                              This parameter must always be set to 12 hours
 * @param[in]     gating_max_duration_minutes   Maximum duration of gating in minutes
 * @param[in]     std_initial                   Initial estimate for standard deviation
 * @param[in]     gain_factor                   Gain factor to amplify or to attenuate
 *                                              the NOx index output
 */
void sen5x_set_nox_algorithm_tuning(
    const sen5x_t *dev, int16_t index_offset, int16_t learning_time_offset_hours,
    int16_t learning_time_gain_hours, int16_t gating_max_duration_minutes,
    int16_t std_initial, int16_t gain_factor);

/**
 * @brief   Get the NOx Algorithm tuning parameters
 *
 * @param[in,out] dev                            Device descriptor of the driver
 * @param[out]    index_offset                   NOx index representing typical(average) conditions
 * @param[out]    learning_time_offset_hours     Time constant to estimate the NOx algorithm
 *                                               offset from the history in hours
 * @param[out]    learning_time_gain_hours       The time constant to estimate the
 *                                               NOx algorithm gain from the history has no
 *                                               impact for NOx. This parameter is still in place
 *                                               for consistency reasons with the VOC tuning
 *                                               parameters command.
 *                                               This parameter must always be set to 12 hours
 * @param[out]    gating_max_duration_minutes    Maximum duration of gating in minutes
 * @param[out]    std_initial                    Initial estimate for standard deviation
 * @param[out]    gain_factor                    Gain factor to amplify or to attenuate
 *                                               the NOx index output
 */
void sen5x_get_nox_algorithm_tuning(
    const sen5x_t *dev, int16_t *index_offset, int16_t *learning_time_offset_hours,
    int16_t *learning_time_gain_hours, int16_t *gating_max_duration_minutes,
    int16_t *std_initial, int16_t *gain_factor);

/**
 * @brief   Set the mode for the RH/T acceleration algorithm
 *
 * @param[in,out] dev       Device descriptor of the driver
 * @param[in]     mode      RH/T acceleration mode:
 *                              = 0: Low Acceleration
 *                              = 1: High Acceleration
 *                              = 2: Medium Acceleration
 */
void sen5x_set_rht_acceleration(const sen5x_t *dev, uint16_t mode);

/**
 * @brief   Get the mode for the RH/T acceleration algorithm
 *
 * @param[in,out] dev       Device descriptor of the driver
 * @param[out]    mode      RH/T acceleration mode:
 *                              = 0: Low Acceleration
 *                              = 1: High Acceleration
 *                              = 2: Medium Acceleration
 */
void sen5x_get_rht_acceleration(const sen5x_t *dev, uint16_t *mode);

/**
 * @brief   Get the VOC Algorithm state
 *
 * @param[in,out] dev           Device descriptor of the driver
 * @param[in]     state         VOC Algorithm state
 * @param[in]     state_size    Size of the VOC Algorithm state
 */
void sen5x_set_voc_state(const sen5x_t *dev, const uint8_t *state, uint8_t state_size);

/**
 * @brief   Set the VOC Algorithm state
 *
 * @param[in,out] dev           Device descriptor of the driver
 * @param[out]    state         VOC Algorithm state
 * @param[in]     state_size    Size of the VOC Algorithm state
 */
void sen5x_get_voc_state(const sen5x_t *dev, uint8_t *state, uint8_t state_size);

#ifdef __cplusplus
}
#endif

/** @} */

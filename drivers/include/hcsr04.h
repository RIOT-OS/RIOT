/*
 * Copyright (C) 2021 AUTH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_hcsr04 HC-SR04
 * @ingroup     drivers_sensor
 * @ingroup     drivers_saul
 * @brief       This is a sensor which uses ultrasonic waves in order to measure the distance
 *
 * @{
 *
 * @file
 * @brief       This is a sensor which uses ultrasonic waves in order to measure the distance
 *
 * @author      Evripidis Chondromatidis <eurichon1996@gmail.com>
 */

#ifndef HCSR04_H
#define HCSR04_H

#include <stdint.h>
#include "hcsr04_constants.h"
#include "periph/gpio.h"


/* Add header includes here */

#ifdef __cplusplus
extern "C" {
#endif

/* Declare the API of the driver */

/**
 * @brief   Device initialization parameters for hcsr04 sensor
 */
typedef struct {
    uint16_t temperature;
    gpio_t trigger_pin;
    gpio_t echo_pin;
} hcsr04_params_t;

/**
 * @brief   Device descriptor of a HC-SR04 sensor
 */
typedef struct {
    uint16_t distance;
    uint16_t sound_speed;
    uint32_t pre_trig_t_us;
    uint32_t pre_meas_t_us;
    hcsr04_params_t params;
} hcsr04_t;

/**
 * @brief   Initialize an HC-SR04 sensor
 *
 * @param[inout] dev        Device descriptor of the driver
 * @param[in]    params     Initialization parameters
 *
 * @return                  EINVAL of invalid parameters
 * @return                  EIO of hardware initialization failure
 * @return                  ECON if connection issue is detected
 * @return                  0 on success
 *
 * After called it checks the parameters, calculates the sound speed
 * based on the default temperature, initializes the hardware and
 * performs a check reading to see if the sensor's echo pin works
 */
int hcsr04_init(hcsr04_t *dev, const hcsr04_params_t *params);

/**
 * @brief   Starts a measurement by triggering the HC-SR04 device
 *
 * @param[inout] dev        Device descriptor of the driver
 *
 * @return                  EFREQ if the frequency it is called is too high
 * @return                  0 on success
 */
int hcsr04_trigger(hcsr04_t *dev);

/**
 * @brief   Gets the last measurement reading
 *
 * @param[inout] dev        Device descriptor of the driver
 * @param[inout] distance   Measurement distance
 *
 * This function returns the distance in milli meters  
 * 
 * @return                  ECON if the measured value if invalid
 * @return                  0 on success
 *
 * @warning Before read a value @ref hcsr04_trigger has to be called
 *
 * @note This function will retrieve the calculated distance after the last trigger
 */
int hcsr04_read(hcsr04_t *dev, uint16_t *distance);

/**
 * @brief   Function that calculates the speed of sound
 *
 * @param[in] temp          Temperature in mili Celsious   
 *
 * It uses the following formula: v = s * sqrt(1 + T/To)
 * Where:
 *      - s is the speed of sound at 0 Celsious or 273 Kelvin
 *      - T is the given air temperature
 *      - To is the reference temperature 273 Kelvin
 *
 *  @warning Temperature must be given in milli Celsious
 */ 
uint16_t hcsr04_temp_to_sound_speed(uint16_t temp);

/**
 * @brief   Function that sets the temperature
 *
 * @param[inout] arg        Device descriptor of the driver
 * @param[in] new_temp      New temperature in mili Celsious   
 *
 * Internally it calls @ref hcsr04_temp_to_sound_speed to calculate
 * the speed of sound so the distance calculation is more accurate     
 */ 
void hcsr04_set_temp(hcsr04_t *dev, uint16_t new_temp);

/**
 * @brief   Function that measures the current distance
 *
 * @param[inout] arg        Device descriptor of the driver
 * @param[inout] distance   Measured distance
 *
 * @return                  EFREQ if trigger fails
 * @return                  ECON if read fails
 * @return                  0 on success
 *
 * Internally it calls the @ref hcsr04_trigger &  @ref hcsr04_read
 * with the ideal minimum time interval based on the datasheet
 * It also performs error checking
 */ 
int hcsr04_get_distance(hcsr04_t *dev, uint16_t *distance);

/**
 * @brief   Function that free's the resources of an hcsr04 sensor
 *
 * @param[inout] arg        Device descriptor of the driver
 *
 * @return                  EFREQ if trigger fails
 * @return                  ECON if read fails
 * @return                  0 on success
 *
 * Internally it calls the @ref hcsr04_trigger &  @ref hcsr04_read
 * with the ideal minimum time interval based on the datasheet
 * It also performs error checking
 */ 
void hcsr04_deinit(hcsr04_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* HCSR04_H */
/** @} */


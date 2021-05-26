/*
 * Copyright (C) 2021 AUTH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_hcsr04
 * @{
 *
 * @file
 * @brief       Internal addresses, registers and constants
 *
 * @author      Evripidis Chondromatidis <eurichon1996@gmail.com>
 */

#ifndef HCSR04_CONSTANTS_H
#define HCSR04_CONSTANTS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    HC-SR04 freq error value
 * @{
 */

/**
 * @brief   Integer representing error in reading frequency
 */
#define EFREQ                                (12345U)

/**
 * @name    HC-SR04 connection error value
 * @{
 */

/**
 * @brief   Integer representing error in hardware connection
 */
#define ECON                                (1234U)

/**
 * @name    HC-SR04 trigger duration
 * @{
 */

/**
 * @brief   Duration time in us
 */
#define TRIGGER_TIME                        (10U)

/**
 * @name    Distance limit parameters for HCSR04 ultrasonic sensor
 * @{
 */

/**
 * @brief   Min distance possible in mm 
 */
#define HCSR04_MIN_DISTANCE_MM              (30U)

/**
 * @name    Distance limit parameters for HCSR04 ultrasonic sensor
 * @{
 */

/**
 * @brief   Max distance possible in mm 
 */
#define HCSR04_MAX_DISTANCE_MM              (4000U)

/**
 * @name    Reference temperature 
 * @{
 */

/**
 * @brief   Temperature in m Kelvin
 */
#define REF_TEMPERATURE                     (273.15F)

/**
 * @name    Speed of sound at the ref temperature for dry air
 * @{
 */

/**
 * @brief   Speed of sound in m/ms
 */
#define REF_SPEED_SOUND                     (331.30F)

/**
 * @name    Temperature coefficient
 * @{
 */

/**
 * @brief   Converts data to appropriate form
 */
#define COEFFICIENT                        (REF_SPEED_SOUND / sqrt(REF_TEMPERATURE))

#ifdef __cplusplus
}
#endif

#endif /* HCSR04_CONSTANTS_H */
/** @} */

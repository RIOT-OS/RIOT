/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_saul [S]ensor [A]ctuator [U]ber [L]ayer
 * @ingroup     drivers
 * @brief       Generic sensor/actuator abstraction layer for RIOT
 *
 * SAUL is a generic actuator/sensor interface in RIOT. Its purpose is to
 * enable unified interaction with a wide range of sensors and actuators through
 * a set of defined access functions and a common data structure.
 *
 * Each device driver implementing this interface has to expose a set of
 * predefined functions and it has to register itself to the central SAUL
 * registry. From here devices can be found, listed, and accessed.
 *
 * Each device has further to expose a name and its type. This information can
 * be used for automated searching and matching of devices (e.g. connect light
 * sensor automatically with the color of an RGB LED...).
 *
 * The SAUL module enables further the automated initialization of preconfigured
 * actuators/sensor via auto_init and the access to all available devices via
 * one unified shell command.
 *
 * @todo        So far, the interface only supports simple read and set
 *              operations. It probably needs to be extended to handling events,
 *              thresholds, and so on.
 *
 * @{
 *
 * @file
 * @brief       Definition of the generic [S]ensor [A]ctuator [U]ber [L]ayer
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef SAUL_H
#define SAUL_H

#include <stdint.h>

#include "phydat.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Definition of device classes
 *
 * This list contains a collections of available device classes. Each device
 * must be part of one, but can be part of multiple of these classes. When
 * belonging to more than one class, a device must however expose one driver
 * for each class it belongs to, and it has to register each driver with a
 * separate entry at the SAUL registry.
 *
 * Classes are identified by 8-bit unsigned integers.
 *
 * For searching and filtering purposes, the device classes are further split
 * into two top-level classes: sensors and actuators. For identification, all
 * actuator classes start with 0b01xxxxxx, all sensor classes start with
 * 0b10xxxxxx.
 *
 * This list is not exhaustive, extend to your needs!
 */
enum {
    SAUL_CLASS_UNDEF    = 0x00,     /**< device class undefined */
    SAUL_ACT_ANY        = 0x40,     /**< any actuator - wildcard */
    SAUL_ACT_LED_RGB    = 0x42,     /**< actuator: RGB LED */
    SAUL_ACT_SERVO      = 0x43,     /**< actuator: servo motor */
    SAUL_ACT_MOTOR      = 0x44,     /**< actuator: motor */
    SAUL_ACT_SWITCH     = 0x45,     /**< actuator: simple on/off switch */
    SAUL_ACT_DIMMER     = 0x46,     /**< actuator: dimmable switch */
    SAUL_SENSE_ANY      = 0x80,     /**< any sensor - wildcard */
    SAUL_SENSE_BTN      = 0x81,     /**< sensor: simple button */
    SAUL_SENSE_TEMP     = 0x82,     /**< sensor: temperature */
    SAUL_SENSE_HUM      = 0x83,     /**< sensor: humidity */
    SAUL_SENSE_LIGHT    = 0x84,     /**< sensor: light */
    SAUL_SENSE_ACCEL    = 0x85,     /**< sensor: accelerometer */
    SAUL_SENSE_MAG      = 0x86,     /**< sensor: magnetometer */
    SAUL_SENSE_GYRO     = 0x87,     /**< sensor: gyroscope */
    SAUL_SENSE_COLOR    = 0x88,     /**< sensor: (light) color */
    SAUL_SENSE_PRESS    = 0x89,     /**< sensor: pressure */
    SAUL_SENSE_ANALOG   = 0x8a,     /**< sensor: raw analog value */
    SAUL_CLASS_ANY      = 0xff      /**< any device - wildcard */
    /* extend this list as needed... */
};

/**
 * @brief   Read a value (a set of values) from a device
 *
 * Simple sensors, as e.g. a temperature sensor, will return exactly one value
 * together with the values scale and unit. Some sensors might return a touple
 * or triple of data (e.g. a 3-axis accelerometer).
 *
 * Actuators can chose to either just return -ENOTSUP or to return their current
 * set value (e.g. useful for reading back the current state of a switch)
 *
 * @param[in] dev       device descriptor of the target device
 * @param[out] res      data read from the device
 *
 * @return  number of values written into to result data structure [1-3]
 * @return  -ENOTSUP if the device does not support this operation
 * @return  -ECANCELED on other errors
 */
typedef int(*saul_read_t)(void *dev, phydat_t *res);

/**
 * @brief   Write a value (a set of values) to a device
 *
 * Most sensors will probably just return -ENOTSUP, as writing values to a
 * sensor is often without purpose. The interface can however be used to
 * configure sensors, e.g. to switch a sensor's unit type by writing the
 * newly selected type to it.
 *
 * For actuators this function is used to influence the actuators state, e.g.
 * switching a switch or setting the speed of a motor.
 *
 * @param[in] dev       device descriptor of the target device
 * @param[in] data      data to write to the device
 *
 * @return  number of values actually processed by the device [1-3]
 * @return  -ENOTSUP if the device does not support this operation
 * @return  -ECANCELED on other errors
 */
typedef int(*saul_write_t)(void *dev, phydat_t *data);

/**
 * @brief   Definition of the RIOT actuator/sensor interface
 */
typedef struct {
    saul_read_t read;       /**< read function pointer */
    saul_write_t write;     /**< write function pointer */
    uint8_t type;           /**< device class the device belongs to */
} saul_driver_t;

/**
 * @brief   Default not supported function
 */
int saul_notsup(void *dev, phydat_t *dat);

/**
 * @brief   Helper function converts a class ID to a string
 *
 * @param[in] class_id      device class ID
 *
 * @return      string representation of the device class
 * @return      NULL if class ID is not known
 */
const char *saul_class_to_str(uint8_t class_id);

#ifdef __cplusplus
}
#endif

#endif /* SAUL_H */
/** @} */

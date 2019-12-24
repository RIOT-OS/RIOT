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
 * @see @ref sys_saul_reg
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
#include <errno.h>

#include "phydat.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Definition of device classes
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
 * into top-level categories: sensors and actuators. For identification, the
 * two most significant bits of a class specify the device category (SAUL
 * category ID) and the six least significant bits identify the class within its
 * category (SAUL intra-category ID): Therefore, all actuator classes start with
 * 0b01xxxxxx, all sensor classes start with 0b10xxxxxx.
 *
 * This list is not exhaustive, extend to your needs!
 * @{
 */
/**
 * @brief   Definition of SAUL categories
 *
 * These entries are meant to be combined via bitwise or with the six least
 * significant bits identifying the class within its category to retrieve the
 * class ID.
 */
enum {
    SAUL_CAT_UNDEF        = 0x00,   /**< device class undefined */
    SAUL_CAT_ACT          = 0x40,   /**< Actuator device class */
    SAUL_CAT_SENSE        = 0x80,   /**< Sensor device class */
};

/**
 * @brief   Definition of actuator intra-category IDs
 */
enum {
    SAUL_ACT_ID_ANY,                  /**< any actuator - wildcard */
    SAUL_ACT_ID_LED_RGB,              /**< actuator: RGB LED */
    SAUL_ACT_ID_SERVO,                /**< actuator: servo motor */
    SAUL_ACT_ID_MOTOR,                /**< actuator: motor */
    SAUL_ACT_ID_SWITCH,               /**< actuator: simple on/off switch */
    SAUL_ACT_ID_DIMMER,               /**< actuator: dimmable switch */
    SAUL_ACT_NUMOF                  /**< Number of actuators supported */
    /* Extend this list as needed, but keep SAUL_ACT_ID_ANY the first and
     * SAUL_ACT_NUMOF the last entry
     */
};

/**
 * @brief   Definition of sensor intra-category IDs
 */
enum {
    SAUL_SENSE_ID_ANY,              /**< any sensor - wildcard */
    SAUL_SENSE_ID_BTN,              /**< sensor: simple button */
    SAUL_SENSE_ID_TEMP,             /**< sensor: temperature */
    SAUL_SENSE_ID_HUM,              /**< sensor: humidity */
    SAUL_SENSE_ID_LIGHT,            /**< sensor: light */
    SAUL_SENSE_ID_ACCEL,            /**< sensor: accelerometer */
    SAUL_SENSE_ID_MAG,              /**< sensor: magnetometer */
    SAUL_SENSE_ID_GYRO,             /**< sensor: gyroscope */
    SAUL_SENSE_ID_COLOR,            /**< sensor: (light) color */
    SAUL_SENSE_ID_PRESS,            /**< sensor: pressure */
    SAUL_SENSE_ID_ANALOG,           /**< sensor: raw analog value */
    SAUL_SENSE_ID_UV,               /**< sensor: UV index */
    SAUL_SENSE_ID_OBJTEMP,          /**< sensor: object temperature */
    SAUL_SENSE_ID_COUNT,            /**< sensor: pulse counter */
    SAUL_SENSE_ID_DISTANCE,         /**< sensor: distance */
    SAUL_SENSE_ID_CO2,              /**< sensor: CO2 Gas */
    SAUL_SENSE_ID_TVOC,             /**< sensor: TVOC Gas */
    SAUL_SENSE_ID_OCCUP,            /**< sensor: occupancy */
    SAUL_SENSE_ID_PROXIMITY,        /**< sensor: proximity */
    SAUL_SENSE_ID_RSSI,             /**< sensor: RSSI */
    SAUL_SENSE_ID_CHARGE,           /**< sensor: coulomb counter */
    SAUL_SENSE_ID_CURRENT,          /**< sensor: ammeter */
    SAUL_SENSE_ID_PM,               /**< sensor: particulate matter */
    SAUL_SENSE_ID_CAPACITANCE,      /**< sensor: capacitance */
    SAUL_SENSE_ID_VOLTAGE,          /**< sensor: voltage */
    SAUL_SENSE_ID_PH,               /**< sensor: pH */
    SAUL_SENSE_ID_POWER,            /**< sensor: power */
    SAUL_SENSE_NUMOF                /**< Number of actuators supported */
    /* Extend this list as needed, but keep SAUL_SENSE_ID_ANY the first and
     * SAUL_SENSE_NUMOF the last entry
     */
};

/**
 * @brief   Definition of SAUL actuator and sensor classes
 *
 * These values consists of the SAUL category ID (two most significant bits)
 * and the SAUL intra-category ID (six least significant bits).
 */
enum {
    SAUL_ACT_ANY            = SAUL_CAT_ACT | SAUL_ACT_ID_ANY,               /**< any actuator - wildcard */
    SAUL_ACT_LED_RGB        = SAUL_CAT_ACT | SAUL_ACT_ID_LED_RGB,           /**< actuator: RGB LED */
    SAUL_ACT_SERVO          = SAUL_CAT_ACT | SAUL_ACT_ID_SERVO,             /**< actuator: servo motor */
    SAUL_ACT_MOTOR          = SAUL_CAT_ACT | SAUL_ACT_ID_MOTOR,             /**< actuator: motor */
    SAUL_ACT_SWITCH         = SAUL_CAT_ACT | SAUL_ACT_ID_SWITCH,            /**< actuator: simple on/off switch */
    SAUL_ACT_DIMMER         = SAUL_CAT_ACT | SAUL_ACT_ID_DIMMER,            /**< actuator: dimmable switch */
    SAUL_SENSE_ANY          = SAUL_CAT_SENSE | SAUL_SENSE_ID_ANY,           /**< any sensor - wildcard */
    SAUL_SENSE_BTN          = SAUL_CAT_SENSE | SAUL_SENSE_ID_BTN,           /**< sensor: simple button */
    SAUL_SENSE_TEMP         = SAUL_CAT_SENSE | SAUL_SENSE_ID_TEMP,          /**< sensor: temperature */
    SAUL_SENSE_HUM          = SAUL_CAT_SENSE | SAUL_SENSE_ID_HUM,           /**< sensor: humidity */
    SAUL_SENSE_LIGHT        = SAUL_CAT_SENSE | SAUL_SENSE_ID_LIGHT,         /**< sensor: light */
    SAUL_SENSE_ACCEL        = SAUL_CAT_SENSE | SAUL_SENSE_ID_ACCEL,         /**< sensor: accelerometer */
    SAUL_SENSE_MAG          = SAUL_CAT_SENSE | SAUL_SENSE_ID_MAG,           /**< sensor: magnetometer */
    SAUL_SENSE_GYRO         = SAUL_CAT_SENSE | SAUL_SENSE_ID_GYRO,          /**< sensor: gyroscope */
    SAUL_SENSE_COLOR        = SAUL_CAT_SENSE | SAUL_SENSE_ID_COLOR,         /**< sensor: (light) color */
    SAUL_SENSE_PRESS        = SAUL_CAT_SENSE | SAUL_SENSE_ID_PRESS,         /**< sensor: pressure */
    SAUL_SENSE_ANALOG       = SAUL_CAT_SENSE | SAUL_SENSE_ID_ANALOG,        /**< sensor: raw analog value */
    SAUL_SENSE_UV           = SAUL_CAT_SENSE | SAUL_SENSE_ID_UV,            /**< sensor: UV index */
    SAUL_SENSE_OBJTEMP      = SAUL_CAT_SENSE | SAUL_SENSE_ID_OBJTEMP,       /**< sensor: object temperature */
    SAUL_SENSE_COUNT        = SAUL_CAT_SENSE | SAUL_SENSE_ID_COUNT,         /**< sensor: pulse counter */
    SAUL_SENSE_DISTANCE     = SAUL_CAT_SENSE | SAUL_SENSE_ID_DISTANCE,      /**< sensor: distance */
    SAUL_SENSE_CO2          = SAUL_CAT_SENSE | SAUL_SENSE_ID_CO2,           /**< sensor: CO2 Gas */
    SAUL_SENSE_TVOC         = SAUL_CAT_SENSE | SAUL_SENSE_ID_TVOC,          /**< sensor: TVOC Gas */
    SAUL_SENSE_OCCUP        = SAUL_CAT_SENSE | SAUL_SENSE_ID_OCCUP,         /**< sensor: occupancy */
    SAUL_SENSE_PROXIMITY    = SAUL_CAT_SENSE | SAUL_SENSE_ID_PROXIMITY,     /**< sensor: proximity */
    SAUL_SENSE_RSSI         = SAUL_CAT_SENSE | SAUL_SENSE_ID_RSSI,          /**< sensor: RSSI */
    SAUL_SENSE_CHARGE       = SAUL_CAT_SENSE | SAUL_SENSE_ID_CHARGE,        /**< sensor: coulomb counter */
    SAUL_SENSE_CURRENT      = SAUL_CAT_SENSE | SAUL_SENSE_ID_CURRENT,       /**< sensor: ammeter */
    SAUL_SENSE_PM           = SAUL_CAT_SENSE | SAUL_SENSE_ID_PM,            /**< sensor: particulate matter */
    SAUL_SENSE_CAPACITANCE  = SAUL_CAT_SENSE | SAUL_SENSE_ID_CAPACITANCE,   /**< sensor: capacitance */
    SAUL_SENSE_VOLTAGE      = SAUL_CAT_SENSE | SAUL_SENSE_ID_VOLTAGE,       /**< sensor: voltage */
    SAUL_SENSE_PH           = SAUL_CAT_SENSE | SAUL_SENSE_ID_PH,            /**< sensor: pH */
    SAUL_SENSE_POWER        = SAUL_CAT_SENSE | SAUL_SENSE_ID_POWER,         /**< sensor: power */
    SAUL_CLASS_ANY          = 0xff                                      /**< any device - wildcard */
    /* extend this list as needed... */
};

/**
 * @brief   Bitmask to retrieve the class ID and intra-category ID from a SAUL
 *          class
 */
enum {
    SAUL_CAT_MASK           = 0xc0, /**< Bitmask to obtain the category ID */
    SAUL_ID_MASK            = 0x3f, /**< Bitmask to obtain the intra-category ID */
};
/** @} */

/**
 * @brief   Read a value (a set of values) from a device
 *
 * Simple sensors, as e.g. a temperature sensor, will return exactly one value
 * together with the values scale and unit. Some sensors might return a tuple
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
typedef int(*saul_read_t)(const void *dev, phydat_t *res);

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
typedef int(*saul_write_t)(const void *dev, phydat_t *data);

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
int saul_notsup(const void *dev, phydat_t *dat);

/**
 * @brief   Helper function converts a class ID to a string
 *
 * @param[in] class_id      device class ID
 *
 * @return      string representation of the device class
 * @return      NULL if class ID is not known
 */
const char *saul_class_to_str(const uint8_t class_id);

#ifdef __cplusplus
}
#endif

#endif /* SAUL_H */
/** @} */

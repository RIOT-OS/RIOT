/*
 * Copyright (C) 2018 Beduino Master Projekt - University of Bremen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "lwm2mclient.h"

/*
 * RIOT debugging
 */
#define ENABLE_DEBUG (0)
#include "debug.h"

int lwm2m_saul_type_to_obj_id(uint8_t saul_type)
{
    switch (saul_type) {
        case SAUL_SENSE_ANY:
        case SAUL_SENSE_COUNT:
            return LWM2M_GENERIC_SENSOR_ID;
        case SAUL_SENSE_OBJTEMP:
        case SAUL_SENSE_TEMP:
            return LWM2M_TEMPERATURE_SENSOR_ID;
        case SAUL_SENSE_HUM:
            return LWM2M_HUMIDITY_SENSOR_ID;
        case SAUL_SENSE_UV:
        case SAUL_SENSE_LIGHT:
            return LWM2M_ILLUMINANCE_SENSOR_ID;
        case SAUL_SENSE_PRESS:
            return LWM2M_PRESSURE_SENSOR_ID;
        case SAUL_SENSE_ANALOG:
            return LWM2M_ANALOG_SENSOR_ID;
        case SAUL_SENSE_DISTANCE:
            return LWM2M_DISTANCE_SENSOR_ID;
        case SAUL_SENSE_BTN:
            return LWM2M_DIGITAL_SENSOR_ID;

        /* Needed for project, but no driver yet */
        case SAUL_SENSE_CO2:

        /* Need data types other than float or bool */
        case SAUL_SENSE_ACCEL:
        case SAUL_SENSE_MAG:
        case SAUL_SENSE_GYRO:
        case SAUL_SENSE_COLOR:

        /* Actuators aren't in yet */
        case SAUL_ACT_ANY:
        case SAUL_ACT_LED_RGB:
        case SAUL_ACT_SERVO:
        case SAUL_ACT_MOTOR:
        case SAUL_ACT_SWITCH:
        case SAUL_ACT_DIMMER:

        case SAUL_CLASS_ANY:
        case SAUL_CLASS_UNDEF:
        default:
            return -1;
    }
}

const char *lwm2m_object_id_to_str(const uint16_t id)
{
    switch (id) {
        case LWM2M_GENERIC_SENSOR_ID:       return "GENERIC_SENSOR";
        case LWM2M_ILLUMINANCE_SENSOR_ID:   return "ILLUMINANCE_SENSOR";
        case LWM2M_ANALOG_SENSOR_ID:        return "ANALOG_SENSOR";
        case LWM2M_TEMPERATURE_SENSOR_ID:   return "TEMPERATURE_SENSOR";
        case LWM2M_HUMIDITY_SENSOR_ID:      return "HUMIDITY_SENSOR";
        case LWM2M_BAROMETER_SENSOR_ID:     return "BAROMETER_SENSOR";
        case LWM2M_VOLTAGE_SENSOR_ID:       return "VOLTAGE_SENSOR";
        case LWM2M_CURRENT_SENSOR_ID:       return "CURRENT_SENSOR";
        case LWM2M_FREQUENCY_SENSOR_ID:     return "FREQUENCY_SENSOR";
        case LWM2M_DEPTH_SENSOR_ID:         return "DEPTH_SENSOR";
        case LWM2M_PERCENTAGE_SENSOR_ID:    return "PERCENTAGE_SENSOR";
        case LWM2M_ALTITUDE_SENSOR_ID:      return "ALTITUDE_SENSOR";
        case LWM2M_LOAD_SENSOR_ID:          return "LOAD_SENSOR";
        case LWM2M_PRESSURE_SENSOR_ID:      return "PRESSURE_SENSOR";
        case LWM2M_LOUDNESS_SENSOR_ID:      return "LOUDNESS_SENSOR";
        case LWM2M_CONCENTRATION_SENSOR_ID: return "CONCENTRATION_SENSOR";
        case LWM2M_ACIDITY_SENSOR_ID:       return "ACIDITY_SENSOR";
        case LWM2M_CONDUCTIVITY_SENSOR_ID:  return "CONDUCTIVITY_SENSOR";
        case LWM2M_POWER_SENSOR_ID:         return "POWER_SENSOR";
        case LWM2M_POWER_FACTOR_SENSOR_ID:  return "POWER_FACTOR_SENSOR";
        case LWM2M_DISTANCE_SENSOR_ID:      return "DISTANCE_SENSOR";
        case LWM2M_RATE_SENSOR_ID:          return "RATE_SENSOR";
        case LWM2M_DIGITAL_SENSOR_ID:       return "DIGITAL_SENSOR";
        default:                            return "UNDEFINED_OBJECT";
    }
}

/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_saul
 * @{
 *
 * @file
 * @brief       SAUL string functions
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdint.h>

#include "saul.h"

/*
 * This is surely not the most beautiful implementation of a stringification
 * function, but works...
 */
const char *saul_class_to_str(const uint8_t class_id)
{
    switch (class_id) {
        case SAUL_CLASS_UNDEF:       return "CLASS_UNDEF";
        case SAUL_ACT_ANY:           return "ACT_ANY";
        case SAUL_ACT_LED_RGB:       return "ACT_LED_RGB";
        case SAUL_ACT_SERVO:         return "ACT_SERVO";
        case SAUL_ACT_MOTOR:         return "ACT_MOTOR";
        case SAUL_ACT_SWITCH:        return "ACT_SWITCH";
        case SAUL_ACT_DIMMER:        return "ACT_DIMMER";
        case SAUL_SENSE_ANY:         return "SENSE_ANY";
        case SAUL_SENSE_BTN:         return "SENSE_BTN";
        case SAUL_SENSE_TEMP:        return "SENSE_TEMP";
        case SAUL_SENSE_HUM:         return "SENSE_HUM";
        case SAUL_SENSE_LIGHT:       return "SENSE_LIGHT";
        case SAUL_SENSE_ACCEL:       return "SENSE_ACCEL";
        case SAUL_SENSE_MAG:         return "SENSE_MAG";
        case SAUL_SENSE_GYRO:        return "SENSE_GYRO";
        case SAUL_SENSE_COLOR:       return "SENSE_COLOR";
        case SAUL_SENSE_PRESS:       return "SENSE_PRESS";
        case SAUL_SENSE_ANALOG:      return "SENSE_ANALOG";
        case SAUL_SENSE_UV:          return "SENSE_UV";
        case SAUL_SENSE_OBJTEMP:     return "SENSE_OBJTEMP";
        case SAUL_SENSE_COUNT:       return "SENSE_PULSE_COUNT";
        case SAUL_SENSE_DISTANCE:    return "SENSE_DISTANCE";
        case SAUL_SENSE_CO2:         return "SENSE_CO2";
        case SAUL_SENSE_TVOC:        return "SENSE_TVOC";
        case SAUL_SENSE_PROXIMITY:   return "SENSE_PROXIMITY";
        case SAUL_SENSE_RSSI:        return "SENSE_RSSI";
        case SAUL_SENSE_CHARGE:      return "SENSE_CHARGE";
        case SAUL_SENSE_CURRENT:     return "SENSE_CURRENT";
        case SAUL_SENSE_OCCUP:       return "SENSE_OCCUP";
        case SAUL_SENSE_PM:          return "SENSE_PM";
        case SAUL_SENSE_CAPACITANCE: return "SENSE_CAPACITANCE";
        case SAUL_SENSE_VOLTAGE:     return "SENSE_VOLTAGE";
        case SAUL_SENSE_PH:          return "SENSE_PH";
        case SAUL_SENSE_POWER:       return "SENSE_POWER";
        case SAUL_CLASS_ANY:         return "CLASS_ANY";
        default:                     return "CLASS_UNKNOWN";
    }
}

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
const char *saul_class_to_str(uint8_t class_id)
{
    switch (class_id) {
        case SAUL_CLASS_UNDEF:  return "CLASS_UNDEF";
        case SAUL_ACT_ANY:      return "ACT_ANY";
        case SAUL_ACT_LED_RGB:  return "ACT_LED_RGB";
        case SAUL_ACT_SERVO:    return "ACT_SERVO";
        case SAUL_ACT_MOTOR:    return "ACT_MOTOR";
        case SAUL_ACT_SWITCH:   return "ACT_SWITCH";
        case SAUL_ACT_DIMMER:   return "ACT_DIMMER";
        case SAUL_SENSE_ANY:    return "SENSE_ANY";
        case SAUL_SENSE_BTN:    return "SENSE_BTN";
        case SAUL_SENSE_TEMP:   return "SENSE_TEMP";
        case SAUL_SENSE_HUM:    return "SENSE_HUM";
        case SAUL_SENSE_LIGHT:  return "SENSE_LIGHT";
        case SAUL_SENSE_ACCEL:  return "SENSE_ACCEL";
        case SAUL_SENSE_MAG:    return "SENSE_MAG";
        case SAUL_SENSE_GYRO:   return "SENSE_GYRO";
        case SAUL_SENSE_COLOR:  return "SENSE_COLOR";
        case SAUL_SENSE_PRESS:  return "SENSE_PRESS";
        case SAUL_CLASS_ANY:    return "CLASS_ANY";
        default:                return "CLASS_UNKNOWN";
    }
}

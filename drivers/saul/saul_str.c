/*
 * Copyright (C) 2015 Freie Universität Berlin
 *               2019 Otto-von-Guericke-Universität Magdeburg
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
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <stddef.h>
#include <stdint.h>

#include "saul.h"

static const char *actuators[] = {
    [SAUL_ACT_ID_ANY]           = "ACT_ANY",
    [SAUL_ACT_ID_LED_RGB]       = "ACT_LED_RGB",
    [SAUL_ACT_ID_SERVO]         = "ACT_SERVO",
    [SAUL_ACT_ID_MOTOR]         = "ACT_MOTOR",
    [SAUL_ACT_ID_SWITCH]        = "ACT_SWITCH",
    [SAUL_ACT_ID_DIMMER]        = "ACT_DIMMER",
};

static const char *sensors[] = {
    [SAUL_SENSE_ID_ANY]         = "SENSE_ANY",
    [SAUL_SENSE_ID_BTN]         = "SENSE_BTN",
    [SAUL_SENSE_ID_TEMP]        = "SENSE_TEMP",
    [SAUL_SENSE_ID_HUM]         = "SENSE_HUM",
    [SAUL_SENSE_ID_LIGHT]       = "SENSE_LIGHT",
    [SAUL_SENSE_ID_ACCEL]       = "SENSE_ACCEL",
    [SAUL_SENSE_ID_MAG]         = "SENSE_MAG",
    [SAUL_SENSE_ID_GYRO]        = "SENSE_GYRO",
    [SAUL_SENSE_ID_COLOR]       = "SENSE_COLOR",
    [SAUL_SENSE_ID_PRESS]       = "SENSE_PRESS",
    [SAUL_SENSE_ID_ANALOG]      = "SENSE_ANALOG",
    [SAUL_SENSE_ID_UV]          = "SENSE_UV",
    [SAUL_SENSE_ID_OBJTEMP]     = "SENSE_OBJTEMP",
    [SAUL_SENSE_ID_COUNT]       = "SENSE_PULSE_COUNT",
    [SAUL_SENSE_ID_DISTANCE]    = "SENSE_DISTANCE",
    [SAUL_SENSE_ID_CO2]         = "SENSE_CO2",
    [SAUL_SENSE_ID_TVOC]        = "SENSE_TVOC",
    [SAUL_SENSE_ID_GAS]         = "SENSE_GAS",
    [SAUL_SENSE_ID_PROXIMITY]   = "SENSE_PROXIMITY",
    [SAUL_SENSE_ID_RSSI]        = "SENSE_RSSI",
    [SAUL_SENSE_ID_CHARGE]      = "SENSE_CHARGE",
    [SAUL_SENSE_ID_CURRENT]     = "SENSE_CURRENT",
    [SAUL_SENSE_ID_OCCUP]       = "SENSE_OCCUP",
    [SAUL_SENSE_ID_PM]          = "SENSE_PM",
    [SAUL_SENSE_ID_CAPACITANCE] = "SENSE_CAPACITANCE",
    [SAUL_SENSE_ID_VOLTAGE]     = "SENSE_VOLTAGE",
    [SAUL_SENSE_ID_PH]          = "SENSE_PH",
    [SAUL_SENSE_ID_POWER]       = "SENSE_POWER",
    [SAUL_SENSE_ID_SIZE]        = "SENSE_SIZE",
};

const char *saul_class_to_str(const uint8_t class_id)
{
    const char *result = NULL;
    uint8_t id = class_id & SAUL_ID_MASK;
    uint8_t cat = class_id & SAUL_CAT_MASK;
    switch (cat) {
        case SAUL_CAT_UNDEF:
            return "CLASS_UNDEF";
        case SAUL_CAT_ACT:
            if (id < SAUL_ACT_NUMOF) {
                result = actuators[id];
            }
            break;
        case SAUL_CAT_SENSE:
            if (id < SAUL_SENSE_NUMOF) {
                result = sensors[id];
            }
            break;
        default:
            if (class_id == SAUL_CLASS_ANY) {
                return "CLASS_ANY";
            }
            break;
    }

    if (result == NULL) {
        result = "CLASS_UNKNOWN";
    }

    return result;
}

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

#include "flash_utils.h"
#include "saul.h"

static FLASH_ATTR const char _act_id_any[] = "ACT_ANY";
static FLASH_ATTR const char _act_id_led_rgb[] = "ACT_LED_RGB";
static FLASH_ATTR const char _act_id_servo[] = "ACT_SERVO";
static FLASH_ATTR const char _act_id_motor[] = "ACT_MOTOR";
static FLASH_ATTR const char _act_id_switch[] = "ACT_SWITCH";
static FLASH_ATTR const char _act_id_dimmer[] = "ACT_DIMMER";

static FLASH_ATTR const char * FLASH_ATTR const actuators[] = {
    [SAUL_ACT_ID_ANY]           = _act_id_any,
    [SAUL_ACT_ID_LED_RGB]       = _act_id_led_rgb,
    [SAUL_ACT_ID_SERVO]         = _act_id_servo,
    [SAUL_ACT_ID_MOTOR]         = _act_id_motor,
    [SAUL_ACT_ID_SWITCH]        = _act_id_switch,
    [SAUL_ACT_ID_DIMMER]        = _act_id_dimmer,
};

static FLASH_ATTR const char _sense_any[] = "SENSE_ANY";
static FLASH_ATTR const char _sense_btn[] = "SENSE_BTN";
static FLASH_ATTR const char _sense_temp[] = "SENSE_TEMP";
static FLASH_ATTR const char _sense_hum[] = "SENSE_HUM";
static FLASH_ATTR const char _sense_light[] = "SENSE_LIGHT";
static FLASH_ATTR const char _sense_accel[] = "SENSE_ACCEL";
static FLASH_ATTR const char _sense_mag[] = "SENSE_MAG";
static FLASH_ATTR const char _sense_gyro[] = "SENSE_GYRO";
static FLASH_ATTR const char _sense_color[] = "SENSE_COLOR";
static FLASH_ATTR const char _sense_press[] = "SENSE_PRESS";
static FLASH_ATTR const char _sense_analog[] = "SENSE_ANALOG";
static FLASH_ATTR const char _sense_uv[] = "SENSE_UV";
static FLASH_ATTR const char _sense_objtemp[] = "SENSE_OBJTEMP";
static FLASH_ATTR const char _sense_pulse_count[] = "SENSE_PULSE_COUNT";
static FLASH_ATTR const char _sense_distance[] = "SENSE_DISTANCE";
static FLASH_ATTR const char _sense_co2[] = "SENSE_CO2";
static FLASH_ATTR const char _sense_tvoc[] = "SENSE_TVOC";
static FLASH_ATTR const char _sense_gas[] = "SENSE_GAS";
static FLASH_ATTR const char _sense_proximity[] = "SENSE_PROXIMITY";
static FLASH_ATTR const char _sense_rssi[] = "SENSE_RSSI";
static FLASH_ATTR const char _sense_charge[] = "SENSE_CHARGE";
static FLASH_ATTR const char _sense_current[] = "SENSE_CURRENT";
static FLASH_ATTR const char _sense_occup[] = "SENSE_OCCUP";
static FLASH_ATTR const char _sense_pm[] = "SENSE_PM";
static FLASH_ATTR const char _sense_capacitance[] = "SENSE_CAPACITANCE";
static FLASH_ATTR const char _sense_voltage[] = "SENSE_VOLTAGE";
static FLASH_ATTR const char _sense_ph[] = "SENSE_PH";
static FLASH_ATTR const char _sense_power[] = "SENSE_POWER";
static FLASH_ATTR const char _sense_size[] = "SENSE_SIZE";
static FLASH_ATTR const char _sense_speed[] = "SENSE_SPEED";

static FLASH_ATTR const char * FLASH_ATTR const sensors[] = {
    [SAUL_SENSE_ID_ANY]         = _sense_any,
    [SAUL_SENSE_ID_BTN]         = _sense_btn,
    [SAUL_SENSE_ID_TEMP]        = _sense_temp,
    [SAUL_SENSE_ID_HUM]         = _sense_hum,
    [SAUL_SENSE_ID_LIGHT]       = _sense_light,
    [SAUL_SENSE_ID_ACCEL]       = _sense_accel,
    [SAUL_SENSE_ID_MAG]         = _sense_mag,
    [SAUL_SENSE_ID_GYRO]        = _sense_gyro,
    [SAUL_SENSE_ID_COLOR]       = _sense_color,
    [SAUL_SENSE_ID_PRESS]       = _sense_press,
    [SAUL_SENSE_ID_ANALOG]      = _sense_analog,
    [SAUL_SENSE_ID_UV]          = _sense_uv,
    [SAUL_SENSE_ID_OBJTEMP]     = _sense_objtemp,
    [SAUL_SENSE_ID_COUNT]       = _sense_pulse_count,
    [SAUL_SENSE_ID_DISTANCE]    = _sense_distance,
    [SAUL_SENSE_ID_CO2]         = _sense_co2,
    [SAUL_SENSE_ID_TVOC]        = _sense_tvoc,
    [SAUL_SENSE_ID_GAS]         = _sense_gas,
    [SAUL_SENSE_ID_PROXIMITY]   = _sense_proximity,
    [SAUL_SENSE_ID_RSSI]        = _sense_rssi,
    [SAUL_SENSE_ID_CHARGE]      = _sense_charge,
    [SAUL_SENSE_ID_CURRENT]     = _sense_current,
    [SAUL_SENSE_ID_OCCUP]       = _sense_occup,
    [SAUL_SENSE_ID_PM]          = _sense_pm,
    [SAUL_SENSE_ID_CAPACITANCE] = _sense_capacitance,
    [SAUL_SENSE_ID_VOLTAGE]     = _sense_voltage,
    [SAUL_SENSE_ID_PH]          = _sense_ph,
    [SAUL_SENSE_ID_POWER]       = _sense_power,
    [SAUL_SENSE_ID_SIZE]        = _sense_size,
    [SAUL_SENSE_ID_SPEED]       = _sense_speed,
};

static FLASH_ATTR const char _class_undef[] = "CLASS_UNDEF";
static FLASH_ATTR const char _class_any[] = "CLASS_ANY";
static FLASH_ATTR const char _class_unknown[] = "CLASS_UNKNOWN";

const char *saul_class_to_str(const uint8_t class_id)
{
#if IS_ACTIVE(HAS_FLASH_UTILS_ARCH)
    /* Yeah, this is as bad as it looks... The function is deprecated for this
     * reason and it will only affect AVR users, for whom this is a good
     * trade-off. */
    static char buf[32]; /* yes, whopping 32 byte ... */
    ssize_t len = saul_class_write(buf, sizeof(buf) - 1, class_id);
    if (len < 0) {
        flash_memcpy(buf, _class_unknown, sizeof(_class_unknown));
        len = sizeof(_class_unknown);
    }
    buf[len] = '\0';
    return buf;
#else
    const char *result = NULL;
    uint8_t id = class_id & SAUL_ID_MASK;
    uint8_t cat = class_id & SAUL_CAT_MASK;
    switch (cat) {
    case SAUL_CAT_UNDEF:
        return _class_undef;
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
            return _class_any;
        }
        break;
    }

    if (result == NULL) {
        result = _class_unknown;
    }

    return result;
#endif
}

void saul_class_print(uint8_t class_id)
{
    uint8_t id = class_id & SAUL_ID_MASK;
    uint8_t cat = class_id & SAUL_CAT_MASK;
    FLASH_ATTR const char *str = NULL;

    switch (cat) {
    case SAUL_CAT_UNDEF:
        str = _class_undef;
        break;
    case SAUL_CAT_ACT:
        if (id < SAUL_ACT_NUMOF) {
            str = actuators[id];
        }
        break;
    case SAUL_CAT_SENSE:
        if (id < SAUL_SENSE_NUMOF) {
            str = sensors[id];
        }
        break;
    default:
        if (class_id == SAUL_CLASS_ANY) {
            str = _class_any;
        }
        break;
    }

    if (str) {
        flash_print_str(str);
    }
}

ssize_t saul_class_write(char *dest, size_t max_size, uint8_t class_id)
{
    uint8_t id = class_id & SAUL_ID_MASK;
    uint8_t cat = class_id & SAUL_CAT_MASK;
    FLASH_ATTR const char *str = NULL;

    switch (cat) {
    case SAUL_CAT_UNDEF:
        str = _class_undef;
        break;
    case SAUL_CAT_ACT:
        if (id < SAUL_ACT_NUMOF) {
            str = actuators[id];
        }
        break;
    case SAUL_CAT_SENSE:
        if (id < SAUL_SENSE_NUMOF) {
            str = sensors[id];
        }
        break;
    default:
        if (class_id == SAUL_CLASS_ANY) {
            str = _class_any;
        }
        break;
    }

    if (!str) {
        return -EINVAL;
    }
    size_t len = flash_strlen(str);
    if (dest) {
        if (len > max_size) {
            return -EOVERFLOW;
        }
        flash_memcpy(dest, str, len);
    }

    return len;
}

/*
 * Copyright (C) 2024 Orange
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_digit7seg
 * @file
 * @brief       Device driver for less than 5 digits of 7 segments without IC
 * @author      Pierre Le Meur <pierre1.lemeur@orange.com>
 */

#include <assert.h>

#define ENABLE_DEBUG        0 
#include "debug.h"

#include "digit7seg.h"

#define PIN_A               (dev->params.data_a)
#define PIN_B               (dev->params.data_b)
#define PIN_C               (dev->params.data_c)
#define PIN_D               (dev->params.data_d)
#define PIN_E               (dev->params.data_e)
#define PIN_F               (dev->params.data_f)
#define PIN_G               (dev->params.data_g)
#define PIN_DP              (dev->params.data_dp)
#define PIN_DIG1            (dev->params.dig1)
#define PIN_DIG2            (dev->params.dig2)
#define PIN_DIG3            (dev->params.dig3)
#define PIN_DIG4            (dev->params.dig4)

/** The 7 segments + decimal point + the number of digits */
#define NB_PIN              (8+dev->params.digits)
#define BYTE_BITS           (0x08)

static void _set_pin_value(digit7seg_t *dev)
{
    const gpio_t pins[] =
    {
        PIN_A, PIN_B, PIN_C, PIN_D, PIN_E, PIN_F, PIN_G, PIN_DP
    };

    uint8_t mask = 0xFF;
    uint8_t current_value = (uint8_t)(dev->value >> (dev->current_digit * 8) & mask);

    for (int i = 0; i < BYTE_BITS; i++) {
        if (current_value & (1 << i)) {
            gpio_set(pins[i]);
        }
        else {
            gpio_clear(pins[i]);
        }
    }
}

static void _shift_display(void *arg, int chan)
{
    (void)chan;
    digit7seg_t *dev = (digit7seg_t*)arg;

    const gpio_t digit_pins[] =
    {
        PIN_DIG1, PIN_DIG2, PIN_DIG3, PIN_DIG4
    };

    gpio_clear(digit_pins[dev->current_digit]);
    dev->current_digit += 1;
    dev->current_digit = dev->current_digit % dev->params.digits;
    gpio_set(digit_pins[dev->current_digit]);

    _set_pin_value(dev);
}

int digit7seg_init(digit7seg_t *dev, const digit7seg_params_t *params)
{
    dev->params = *params;
    dev->current_digit = 0;
    dev->value = 0;

    if (dev->params.digits <= 0 || dev->params.digits > DIGIT7SEG_MAX_DIGITS) {
        DEBUG("[Error] Invalid number of digit.\n");
        return -DIGIT7SEG_ERR_DIGITS;
    }

    const gpio_t pins[] =
    {
        PIN_A, PIN_B, PIN_C, PIN_D, PIN_E, PIN_F, PIN_G, PIN_DP,
        PIN_DIG1, PIN_DIG2, PIN_DIG3, PIN_DIG4
    };

    const char* pins_debug[] =
    {
        "A", "B", "C", "D", "E", "F", "G", "DP",
        "DIG1", "DIG2", "DIG3", "DIG4"
    };

    for (int i = 0; i < NB_PIN; i++) {
        DEBUG("[DIGIT7SEG] Trying the pin %s:...", pins_debug[i]);
        assert(gpio_init(pins[i], GPIO_OUT) == 0);
        gpio_clear(pins[i]);
        DEBUG("Worked\n");
    }

    return DIGIT7SEG_OK;
}

void digit7seg_set_all_value(digit7seg_t *dev, uint32_t value)
{
    dev->value = value;
}

int digit7seg_set_value(digit7seg_t *dev, int index, uint8_t value)
{
    if (index < 0 || index >= dev->params.digits) {
        return -1;
    }

    uint32_t temp_value = value << (index * BYTE_BITS);
    uint32_t up_value = dev->value >> ((index + 1) * BYTE_BITS);
    up_value <<= ((index + 1) * BYTE_BITS);
    uint32_t down_value = ((1 << (index * BYTE_BITS)) - 1) & dev->value;

    dev->value = up_value | temp_value | down_value;

    return 0;
}

#ifdef MODULE_DIGIT7SEG_INT
int digit7seg_set_int_all_value(digit7seg_t *dev, uint16_t value)
{
    if (value > 9999) return -1;

    for (int i = 0; i < 4 && value > 0; i++) {
        digit7seg_set_value(dev, i, digit7seg_bitfield[value % 10]);
        value = value / 10;
    }

    return 0;
}

int digit7seg_set_int_value(digit7seg_t *dev, int index, uint8_t value)
{
    if (value > 9) return -1;

    return digit7seg_set_value(dev, index, digit7seg_bitfield[value]);
}
#endif

#ifdef MODULE_DIGIT7SEG_FLOAT
int digit7seg_set_float_value(digit7seg_t *dev, float value, int precision)
{
    if (value < 0 || value > 999.9) return -1;
    if (precision != 10 && precision != 100 && precision != 1000) return -1;

    double log_val = log10(precision);
    int dp_pos = (int)log_val;
    int int_value = value * precision;
    int_value = int_value % 10000;

    int res = digit7seg_set_int_all_value(dev, int_value);
    if (res != 0) {
        DEBUG("[Error] Not possible to set float value.\n");
        return res;
    }

    /* Set the decimal point on the precision */
    dev->value |= (0b10000000 << (BYTE_BITS * dp_pos));
    return 0;
}
#endif

int digit7seg_poweron(digit7seg_t *dev)
{
    if (timer_init(dev->params.timer, DIGIT7SEG_TIMER_HZ, _shift_display, dev) != 0) {
        DEBUG("[Error] Not possible to init timer.\n");
        return -1;
    }

    timer_set_periodic(dev->params.timer, 0, DIGIT7SEG_DELAY, TIM_FLAG_RESET_ON_MATCH);
    return 0;
}

void digit7seg_poweroff(digit7seg_t *dev)
{
    timer_stop(dev->params.timer);
}

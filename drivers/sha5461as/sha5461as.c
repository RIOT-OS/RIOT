/*
 * Copyright (C) 2024 Orange
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_sha5461as
 * @{
 *
 * @file
 * @brief       Device driver interface for the SHA5461AS 7 segments display
 *
 * @author      Pierre Le Meur <pierre1.lemeur@orange.com>
 *
 * @}
 */
#include <inttypes.h>

#include "sha5461as.h"

#include "board.h"
#include "periph/gpio.h"
#include "periph/timer.h"

#define ENABLE_DEBUG        0
#include "debug.h"

#define PIN_A               (dev->params.a)
#define PIN_B               (dev->params.b)
#define PIN_C               (dev->params.c)
#define PIN_D               (dev->params.d)
#define PIN_E               (dev->params.e)
#define PIN_F               (dev->params.f)
#define PIN_G               (dev->params.g)
#define PIN_DP              (dev->params.dp)
#define PIN_DIG1            (dev->params.dig1)
#define PIN_DIG2            (dev->params.dig2)
#define PIN_DIG3            (dev->params.dig3)
#define PIN_DIG4            (dev->params.dig4)

#define NB_PIN              (12)

static void _set_pin_value(sha5461as_t *dev)
{
    const gpio_t pins[] =
    {
        PIN_A, PIN_B, PIN_C, PIN_D, PIN_E, PIN_F, PIN_G, PIN_DP
    };

    uint8_t mask = 0xFF;
    uint8_t current_value = (uint8_t)(dev->value >> (dev->current_digit * 8) & mask);

    for (int i = 1; i <= 8; i++) {
        if (current_value & (1 << (8 - i))) {
            gpio_set(pins[i - 1]);
            DEBUG("PIN SET\n");
        }
        else {
            gpio_clear(pins[i - 1]);
            DEBUG("PIN CLEAR\n");
        }
    }
}

static void _shift_display(void *arg, int chan)
{
    (void)chan;
    sha5461as_t *dev = (sha5461as_t *)arg;

    const gpio_t digit_pins[] =
    {
        PIN_DIG1, PIN_DIG2, PIN_DIG3, PIN_DIG4
    };

    gpio_clear(digit_pins[dev->current_digit]);
    dev->current_digit += 1;
    dev->current_digit = dev->current_digit % dev->digits;
    gpio_set(digit_pins[dev->current_digit]);

    _set_pin_value(dev);
}

int sha5461as_init(sha5461as_t *dev, const sha5461as_params_t *params)
{
    dev->params = *params;
    dev->current_digit = 0;

    if (dev->digits <= 0 || dev->digits > SHA5461AS_MAX_DIGITS) {
        return -SHA5461AS_ERR_DIGITS;
    }

    const gpio_t pins[] =
    {
        PIN_A, PIN_B, PIN_C, PIN_D, PIN_E, PIN_F, PIN_G, PIN_DP,
        PIN_DIG1, PIN_DIG2, PIN_DIG3, PIN_DIG4
    };

    const int pin_errs[] =
    {
        SHA5461AS_ERR_A_GPIO, SHA5461AS_ERR_B_GPIO, SHA5461AS_ERR_C_GPIO, SHA5461AS_ERR_D_GPIO,
        SHA5461AS_ERR_E_GPIO, SHA5461AS_ERR_F_GPIO, SHA5461AS_ERR_G_GPIO, SHA5461AS_ERR_DP_GPIO,
        SHA5461AS_ERR_DIG1_GPIO, SHA5461AS_ERR_DIG2_GPIO, SHA5461AS_ERR_DIG3_GPIO,
        SHA5461AS_ERR_DIG4_GPIO
    };

    for (int i = 0; i < NB_PIN; i++) {
        if (gpio_init(pins[i], GPIO_OUT) < 0) {
            DEBUG("[Error] Initializing error");
            return pin_errs[i];
        }
        gpio_clear(pins[i]);
    }

    return SHA5461AS_OK;
}

int sha5461as_shift(sha5461as_t *dev)
{
    _shift_display(dev, 0);
    return 1;
}

int sha5461as_set_all_value(sha5461as_t *dev, uint32_t value)
{
    dev->value = value;
    return 1;
}

int sha5461as_set_value(sha5461as_t *dev, int index, uint8_t value)
{
    value <<= (index * 8);
    uint32_t up_value = dev->value >> ((index + 1) * 8);
    uint32_t down_value = ((0b00000001 << (index * 8)) - 1) & dev->value;

    dev->value = up_value | value | down_value;

    return 1;
}

int sha5461as_display(sha5461as_t *dev)
{
    if (dev->status == TIMER_RUNNING) {
        timer_stop(XTIMER_DEV);
        dev->status = TIMER_STOPPED;
        return 0;
    }

    if (timer_init(XTIMER_DEV, XTIMER_HZ, _shift_display, dev) != 0) {
        return -1;
    }

    timer_set_periodic(XTIMER_DEV, 0, SHA5461AS_DELAY, TIM_FLAG_RESET_ON_MATCH);

    dev->status = TIMER_RUNNING;
    return 0;
}

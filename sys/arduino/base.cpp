/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     sys_arduino
 * @{
 *
 * @file
 * @brief       Implementation of Arduino core functionality
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

extern "C" {
#include "xtimer.h"
#include "periph/gpio.h"
}

#include "arduino.hpp"

static inline gpio_dir_t _dir(int mode)
{
    return (mode == OUTPUT) ? GPIO_DIR_OUT : GPIO_DIR_IN;
}

static inline gpio_pp_t _pr(int mode)
{
    return (mode == INPUT_PULLUP) ? GPIO_PULLUP : GPIO_NOPULL;
}

void pinMode(int pin, int mode)
{
    gpio_init(arduino_pinmap[pin], _dir(mode), _pr(mode));
}

void digitalWrite(int pin, int state)
{
    gpio_write(arduino_pinmap[pin], state);
}

int digitalRead(int pin)
{
    if (gpio_read(arduino_pinmap[pin])) {
        return HIGH;
    }
    else {
        return LOW;
    }
}

void delay(int msec)
{
    xtimer_usleep(1000 * msec);
}

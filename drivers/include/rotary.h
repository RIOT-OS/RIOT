/*
 * Copyright (C) 2016 Marc Poulhiès
 * Copyright (C) 2011 Ben Buxton <bb@cactii.net>
 *
 * Licensed under the GNU GPL Version 3
 */

/**
 * @defgroup     drivers_rotary_encoder
 * @{
 *
 * @file
 * @brief       Low-level rotary encoder implementation
 *
 * @author      Marc Poulhiès <dkm@kataplop.net>
 *
 * @}
 */

#ifndef ROTARY_ENC_H
#define ROTARY_ENC_H

#include "kernel_types.h"
#include <periph/gpio.h>

typedef struct {
    gpio_t pin1, pin2;
    kernel_pid_t listener;
    unsigned int state;
} rotary_t;

int rotary_init(rotary_t *dev, gpio_t pin1, gpio_t pin2);
void rotary_register(rotary_t *dev, kernel_pid_t pid);

#define ROTARY_MSG_EVENT 1

#define ROTARY_MSG_CLICK 2

enum rotary_dir_t {
    DIR_NONE = 0x0,     /* No complete step yet. */
    DIR_CW   = 0x10,    /* Clockwise step. */
    DIR_CCW  = 0x20,   /* Anti-clockwise step. */
};
#endif

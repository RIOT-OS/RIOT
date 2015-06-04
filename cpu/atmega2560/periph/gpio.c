/*
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_periph
 * @{
 *
 * @file
 * @brief       Low-level GPIO driver implementation
 *
 * @author      Hauke Petersen <mail@haukepetersen.de>
 *
 * @}
 */

/*
 * TODO: Implement GPIO interface
 */

#include "cpu.h"
#include "periph/gpio.h"
#include "periph_conf.h"

#if GPIO_NUMOF

typedef struct {
    void (*cb)(void);
} gpio_state_t;

/*static gpio_state_t config[GPIO_NUMOF]; */



int gpio_init(gpio_t dev, gpio_dir_t dir, gpio_pp_t pushpull)
{
    return -1;
}

int gpio_init_exti(gpio_t dev, gpio_pp_t pushpull, gpio_flank_t flank, gpio_cb_t cb, void *arg)
{
    return -1;
}

int gpio_read(gpio_t dev)
{
    return -1;
}

void gpio_set(gpio_t dev)
{
}

void  gpio_clear(gpio_t dev)
{
}

void gpio_toggle(gpio_t dev)
{
}

void gpio_write(gpio_t dev, int value)
{
}

#endif /* GPIO_NUMOF */

/*
 * Copyright (C) 2021 AUTH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_hcsr04
 * @{
 *
 * @file
 * @brief       Device driver implementation for the HC-SR04
 *
 * @author      Evripidis Chondromatidis <eurichon1996@gmail.com>
 *
 * @}
 */

#include "hcsr04.h"
#include "hcsr04_constants.h"
#include "hcsr04_params.h"

#include "periph/gpio.h"
#include "xtimer.h"

#include <stdint.h>
#include "assert.h"
#include <errno.h>
#include <math.h>
#include "mutex.h"


void hcsr04_int_callback(void *arg)
{
    uint32_t now = xtimer_now_usec();
    hcsr04_t *p_dev = (hcsr04_t *)arg;

    if (gpio_read(p_dev->params.echo_pin)) {
        p_dev->pre_meas_t_us = now;
    } else {
        uint32_t dt_us_one_way = (now - p_dev->pre_meas_t_us) / 2;
        p_dev->distance = p_dev->sound_speed * dt_us_one_way / 1000;
    }
}

int hcsr04_init(hcsr04_t *dev, const hcsr04_params_t *params)
{
    assert(dev && params);

    dev->params = *params;

    dev->distance = 0;
    dev->pre_trig_t_us = xtimer_now_usec();
    dev->pre_meas_t_us = xtimer_now_usec();
    hcsr04_set_temp(dev, params->temperature);

    if (gpio_init(dev->params.trigger_pin, GPIO_OUT)) {
        return -EIO;
    }

    if (gpio_init_int(dev->params.echo_pin, GPIO_IN, GPIO_BOTH, hcsr04_int_callback, dev)) {
        return -EIO;
    }

    // xtimer_msleep(30);

    // uint16_t distance;
    // int status = hcsr04_get_distance(dev, &distance);

    // if (dev->distance == 0) {
    //     return -ECON;
    // }    
    // initialize without the need to take a measurement

    return status;
}

int hcsr04_trigger(hcsr04_t *dev)
{
    uint32_t now = xtimer_now_usec();
    uint16_t min_delay = 1000 * HCSR04_MAX_DISTANCE_MM / dev->sound_speed;
    
    if ((now - dev->pre_trig_t_us) < min_delay) {
        return -EFREQ;
    }
    
    dev->pre_trig_t_us = now;
    gpio_set(dev->params.trigger_pin);
    xtimer_usleep(TRIGGER_TIME);
    gpio_clear(dev->params.trigger_pin);
    
    return 0;
}

int hcsr04_read(hcsr04_t *dev, uint16_t *distance)
{
    if (dev->pre_meas_t_us < dev->pre_trig_t_us) {
        *distance = 0;
        return -ECON;
    }

    if (dev->distance < HCSR04_MIN_DISTANCE_MM) {
        *distance = HCSR04_MIN_DISTANCE_MM;
    } else if (dev->distance > HCSR04_MAX_DISTANCE_MM) {
        *distance = HCSR04_MAX_DISTANCE_MM;
    } else {
        *distance = dev->distance;
    }

    return 0;
}

uint16_t hcsr04_temp_to_sound_speed(uint16_t temp) 
{
    return COEFFICIENT * sqrt(REF_TEMPERATURE + temp / 1000);
}

void hcsr04_set_temp(hcsr04_t *dev, uint16_t new_temp)
{
    dev->params.temperature = new_temp;
    dev->sound_speed = hcsr04_temp_to_sound_speed(new_temp);
}

int hcsr04_get_distance(hcsr04_t *dev, uint16_t *distance) 
{
    uint16_t min_delay = 2 * HCSR04_MAX_DISTANCE_MM / dev->sound_speed;
    
    int status = hcsr04_trigger(dev);
    if (status < 0) {
        *distance = 0;
        return status;
    }

    xtimer_msleep(min_delay);

    status = hcsr04_read(dev, distance);
    if (status < 0) {
        return status; 
    }
        
    return 0;
}

void hcsr04_deinit(hcsr04_t *dev) 
{
    gpio_clear(dev->params.trigger_pin);
    gpio_irq_disable(dev->params.echo_pin);

    dev->params.temperature = 0;
    dev->params.trigger_pin = 0;
    dev->params.echo_pin = 0;

    dev->distance = 0;
    dev->sound_speed = 0;
    dev->pre_trig_t_us = 0;
    dev->pre_meas_t_us = 0;
}

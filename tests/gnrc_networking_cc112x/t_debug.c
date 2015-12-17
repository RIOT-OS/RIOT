/*
 * t_debug.c
 *
 *  Created on: 14 gru 2015
 *      Author: mateusz
 */

/* My own implementation of timestamps */
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include "t_debug.h"
#include "periph/gpio.h"

struct timeval time_ref;
char time_stamp[20];

static void debug_timeref_reset(void){
    gettimeofday(&time_ref, 0);
}

static void debug_callback(void *arg){
    gpio_irq_disable(GPIO_T(GPIO_PORT_A, 13));
    debug_timeref_reset();
    gpio_irq_enable(GPIO_T(GPIO_PORT_A, 13));
}

void debug_timeref_init(void){
    gpio_init_int(GPIO_T(GPIO_PORT_A, 13), GPIO_PULLUP, GPIO_FALLING, &debug_callback, 0);
    gpio_irq_enable(GPIO_T(GPIO_PORT_A, 13));
}

void debug_timestamp(void){
    struct timeval time_tmp;
    gettimeofday(&time_tmp, 0);
    timersub(&time_tmp, &time_ref, &time_tmp);
    snprintf(time_stamp, 20, "%4d.%06d\t>", (int)time_tmp.tv_sec, (int)time_tmp.tv_usec);
}


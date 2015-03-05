/*
 * Copyright (C) 2014 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test for low-level PWM drivers
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>

#include "cpu.h"
#include "board.h"
#include "vtimer.h"
#include "periph/pwm.h"
#include "periph/gpio.h"

#define GPIO_DEBUG  GPIO_0
#define DEV         PWM_0
#define DEV_CHAN    (0)
#define MODE_1      PWM_LEFT
#define MODE_2      PWM_RIGHT
#define FREQU       (1000U)
#define STEPS       (1000U)
#define DUTY        (100)


int main(void)
{
    puts("\nRIOT PWM test");

    gpio_init_out(GPIO_DEBUG, GPIO_NOPULL);

    int res = pwm_init(DEV, MODE_1, FREQU, STEPS);
    if (res == 0) {
        puts("PWM successfully initialized.\n");
    }
    else {
        puts("Errors while initializing PWM");
        return -1;
    }
    printf("PER value 1: %i\n", PWM_0_DEV->PER.bit.PER);
    printf("Counter directrion value 1: %i\n", PWM_0_DEV->CTRLBCLR.reg);

    vtimer_usleep(10000);

    gpio_set(GPIO_DEBUG);
    pwm_set(DEV, DEV_CHAN, DUTY);
    gpio_clear(GPIO_DEBUG);

    vtimer_usleep(10000);

    printf("\n");
    res = pwm_init(DEV, MODE_2, FREQU, STEPS);
    if (res == 0) {
        puts("PWM successfully initialized.\n");
    }
    else {
        puts("Errors while initializing PWM");
        return -1;
    }
    printf("PER value 2: %i\n", PWM_0_DEV->PER.bit.PER);
    printf("Counter directrion value 2: %i\n", PWM_0_DEV->CTRLBCLR.reg);

    vtimer_usleep(10000);

    gpio_set(GPIO_DEBUG);
    pwm_set(DEV, DEV_CHAN, DUTY);
    gpio_clear(GPIO_DEBUG);

    return 0;
}

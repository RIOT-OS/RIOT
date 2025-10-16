/*
 * Copyright (C) 2018 Gilles DOFFE <g.doffe@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       High-level driver for DC motors test application
 *
 * @author      Gilles DOFFE <g.doffe@gmail.com>
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "motor_config.h"

/* RIOT includes */
#include "log.h"
#include "motor_driver.h"
#include "test_utils/expect.h"
#include "xtimer.h"

/* set interval to 20 milli-second */
#define INTERVAL (3000 * US_PER_MS)

#define MOTOR_0_ID  0
#define MOTOR_1_ID  1

void motors_control(int32_t duty_cycle)
{
    char str[6];

    if (duty_cycle >= 0) {
        strncpy(str, "CW", 3);
    }
    else {
        strncpy(str, "CCW", 4);
    }

    printf("Duty cycle = %" PRId32 "   Direction = %s\n", duty_cycle, str);

    if (motor_set(&motor_driver_config[MOTOR_DRIVER_DEV(0)],
                  MOTOR_0_ID, duty_cycle)) {
        printf("Cannot set PWM duty cycle for motor %" PRIu32 "\n", \
               (uint32_t)MOTOR_0_ID);
    }
    if (motor_set(&motor_driver_config[MOTOR_DRIVER_DEV(0)],
                  MOTOR_1_ID, duty_cycle)) {
        printf("Cannot set PWM duty cycle for motor %" PRIu32 "\n", \
               (uint32_t)MOTOR_1_ID);
    }
}

void motors_brake(void)
{
    puts("Brake motors !!!");

    if (motor_brake(&motor_driver_config[MOTOR_DRIVER_DEV(0)], MOTOR_0_ID)) {
        printf("Cannot brake motor %" PRIu32 "\n", (uint32_t)MOTOR_0_ID);
    }
    if (motor_brake(&motor_driver_config[MOTOR_DRIVER_DEV(0)], MOTOR_1_ID)) {
        printf("Cannot brake motor %" PRIu32 "\n", (uint32_t)MOTOR_1_ID);
    }
}

void motion_control(void)
{
    int8_t dir = 1;
    int ret = 0;
    xtimer_ticks32_t last_wakeup;
    int32_t pwm_res = motor_driver_config[MOTOR_DRIVER_DEV(0)].pwm_resolution;

    ret = motor_driver_init(&motor_driver_config[MOTOR_DRIVER_DEV(0)]);
    if (ret) {
        LOG_ERROR("motor_driver_init failed with error code %d\n", ret);
    }
    expect(ret == 0);

    for (;;) {
        /* BRAKE - duty cycle 100% */
        last_wakeup = xtimer_now();
        motors_brake();
        xtimer_periodic_wakeup(&last_wakeup, INTERVAL);

        /* CW - duty cycle 50% */
        last_wakeup = xtimer_now();
        motors_control(dir * pwm_res / 2);
        xtimer_periodic_wakeup(&last_wakeup, INTERVAL);

        /* Disable motor during INTERVAL µs (motor driver must have enable
           feature */
        last_wakeup = xtimer_now();
        motor_disable(&motor_driver_config[MOTOR_DRIVER_DEV(0)], MOTOR_0_ID);
        motor_disable(&motor_driver_config[MOTOR_DRIVER_DEV(0)], MOTOR_1_ID);
        xtimer_periodic_wakeup(&last_wakeup, INTERVAL);
        motor_enable(&motor_driver_config[MOTOR_DRIVER_DEV(0)], MOTOR_0_ID);
        motor_enable(&motor_driver_config[MOTOR_DRIVER_DEV(0)], MOTOR_1_ID);

        /* CW - duty cycle 100% */
        last_wakeup = xtimer_now();
        motors_control(dir * pwm_res);
        xtimer_periodic_wakeup(&last_wakeup, INTERVAL);

        /* Reverse direction */
        dir = dir * -1;
    }
}

int main(void)
{
    motion_control();

    return 0;
}

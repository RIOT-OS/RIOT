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

/* RIOT includes */
#include "init_dev.h"
#include "log.h"
#include "motor_driver.h"
#include "motor_driver_params.h"
#include "test_utils/expect.h"
#include "xtimer.h"


void motor_driver_callback_example(
    const motor_driver_t *motor_driver, uint8_t motor_id,
    int32_t pwm_duty_cycle)
{
    LOG_DEBUG("MOTOR-DRIVER=%p" \
        "    MOTOR_ID = %"PRIu8     \
        "    PWM_VALUE = %"PRIu32"\n", \
        (void*)motor_driver, motor_id,
        pwm_duty_cycle);
}

/* set interval to 20 milli-second */
#define INTERVAL (3000 * US_PER_MS)

#define MOTOR_0_ID  0
#define MOTOR_1_ID  1

static motor_driver_t motor_driver;

void motors_control(int32_t duty_cycle)
{
    char str[4];

    if (duty_cycle >= 0) {
        strncpy(str, "CW", 3);
    }
    else {
        strncpy(str, "CCW", 4);
    }

    puts("\nActuate Motors");

    if (motor_set(&motor_driver, MOTOR_0_ID, duty_cycle)) {
        printf("Cannot set PWM duty cycle for motor %" PRIu32 "\n", \
               (uint32_t)MOTOR_0_ID);
    }
    if (motor_set(&motor_driver, MOTOR_1_ID, duty_cycle)) {
        printf("Cannot set PWM duty cycle for motor %" PRIu32 "\n", \
               (uint32_t)MOTOR_1_ID);
    }
}

void motors_brake(void)
{
    puts("\nBrake motors");

    if (motor_brake(&motor_driver, MOTOR_0_ID)) {
        printf("Cannot brake motor %" PRIu32 "\n", (uint32_t)MOTOR_0_ID);
    }
    if (motor_brake(&motor_driver, MOTOR_1_ID)) {
        printf("Cannot brake motor %" PRIu32 "\n", (uint32_t)MOTOR_1_ID);
    }
}

void motion_control(void)
{
    int8_t dir = 1;
    int ret = 0;
    xtimer_ticks32_t last_wakeup /*, start*/;
    int32_t pwm_res = motor_driver_params->pwm_resolution;

    ret = motor_driver_init(&motor_driver, &motor_driver_params[0]);
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
        puts("\nDisable motors");
        motor_disable(&motor_driver, MOTOR_0_ID);
        motor_disable(&motor_driver, MOTOR_1_ID);
        xtimer_periodic_wakeup(&last_wakeup, INTERVAL);
        puts("\nEnable motors");
        motor_enable(&motor_driver, MOTOR_0_ID);
        motor_enable(&motor_driver, MOTOR_1_ID);
        xtimer_periodic_wakeup(&last_wakeup, INTERVAL);

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

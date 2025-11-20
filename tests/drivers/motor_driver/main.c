/*
 * SPDX-FileCopyrightText: 2018 Gilles DOFFE <g.doffe@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
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
#include "time_units.h"
#include "ztimer.h"

void motor_driver_callback_example(
    const motor_driver_t *motor_driver, uint8_t motor_id,
    int32_t pwm_duty_cycle)
{
    LOG_DEBUG("MOTOR-DRIVER=%p" \
        "    MOTOR_ID = %"PRIu8     \
        "    PWM_VALUE = %"PRIi32"\n", \
        (void*)motor_driver, motor_id,
        pwm_duty_cycle);
}

/* Set interval to 3 seconds */
#define INTERVAL (3 * MS_PER_SEC)

#define MOTOR_0_ID  ((uint8_t)0)
#define MOTOR_1_ID  ((uint8_t)1)

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
        printf("Cannot set PWM duty cycle for motor %" PRIu8 "\n", \
               MOTOR_0_ID);
    }
    if (motor_set(&motor_driver, MOTOR_1_ID, duty_cycle)) {
        printf("Cannot set PWM duty cycle for motor %" PRIu8 "\n", \
               MOTOR_1_ID);
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
    int32_t pwm_res = motor_driver_params->pwm_resolution;

    ret = motor_driver_init(&motor_driver, &motor_driver_params[0]);
    if (ret) {
        LOG_ERROR("motor_driver_init failed with error code %d\n", ret);
    }
    expect(ret == 0);

    while (1) {
        /* BRAKE - duty cycle 100% */
        motors_brake();
        ztimer_sleep(ZTIMER_MSEC, INTERVAL);

        /* CW - duty cycle 50% */
        motors_control(dir * pwm_res / 2);
        ztimer_sleep(ZTIMER_MSEC, INTERVAL);

        /* Disable motor during INTERVAL µs (motor driver must have enable
         * feature) */
        puts("\nDisable motors");
        motor_disable(&motor_driver, MOTOR_0_ID);
        motor_disable(&motor_driver, MOTOR_1_ID);
        ztimer_sleep(ZTIMER_MSEC, INTERVAL);
        puts("\nEnable motors");
        motor_enable(&motor_driver, MOTOR_0_ID);
        motor_enable(&motor_driver, MOTOR_1_ID);
        ztimer_sleep(ZTIMER_MSEC, INTERVAL);

        /* CW - duty cycle 100% */
        motors_control(dir * pwm_res);
        ztimer_sleep(ZTIMER_MSEC, INTERVAL);

        /* Reverse direction */
        dir *= -1;
    }
}

int main(void)
{
    motion_control();

    return 0;
}

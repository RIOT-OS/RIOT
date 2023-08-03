/*
 * Copyright (C) 2018 Gilles DOFFE <g.doffe@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_motor_driver
 * @{
 *
 * @file
 * @brief       High-level driver for DC motors
 *
 * @author      Gilles DOFFE <g.doffe@gmail.com>
 * @}
 */

#include <errno.h>

/* RIOT includes */
#include <assert.h>
#include <board.h>
#include <irq.h>
#include <log.h>
#include <motor_driver.h>

#define ENABLE_DEBUG 0
#include <debug.h>

/**
 * @brief Callback to set direction on two directions pins driver
 *
 * @param   motor       motor
 * @param   direction   direction
 */
static void _motor_set_two_dirs(const motor_t *motor, motor_direction_t direction);

/**
 * @brief Callback to set direction on one direction pin driver
 *
 * @param   motor       motor
 * @param   direction   direction
 */

static void _motor_set_one_dir(const motor_t *motor, motor_direction_t direction);

static void _motor_brake_two_dirs(const motor_t *motor, bool brake);
static void _motor_brake_one_dir_brake(const motor_t *motor, bool brake);

int motor_driver_init(motor_driver_t *motor_driver, const motor_driver_params_t *motor_driver_params)
{
    int err = 0;

    uint32_t ret_pwm = pwm_init(motor_driver_params->pwm_dev,
                                motor_driver_params->pwm_mode,
                                motor_driver_params->pwm_frequency,
                                motor_driver_params->pwm_resolution);
    if (ret_pwm != motor_driver_params->pwm_frequency) {
        err = EINVAL;
        LOG_ERROR("pwm_init failed\n");
        goto motor_init_err;
    }

    /* Init GPIO */
    err = 0;
    for (uint8_t i = 0; i < motor_driver_params->nb_motors; i++) {
        /* Init motor GPIOs, if one fails, motor is not setup */
        if (gpio_is_valid(motor_driver_params->motors[i].gpio_dir0)) {
            if (gpio_init(motor_driver_params->motors[i].gpio_dir0,
                          GPIO_OUT)) {
                err = EIO;
                LOG_ERROR("gpio_dir0 init failed for motor %d\n", i);
                goto motor_init_err;
            }
        }
        if (gpio_is_valid(motor_driver_params->motors[i].gpio_dir1_or_brake)) {
            if (gpio_init(motor_driver_params->motors[i].gpio_dir1_or_brake,
                          GPIO_OUT)) {
                err = EIO;
                LOG_ERROR("gpio_dir1_or_brake init failed for motor %d\n", i);
                goto motor_init_err;
            }
        }
        if (gpio_is_valid(motor_driver_params->motors[i].gpio_enable)) {
            if (gpio_init(motor_driver_params->motors[i].gpio_enable,
                          GPIO_OUT)) {
                err = EIO;
                LOG_ERROR("gpio_enable init failed for motor %d\n", i);
                goto motor_init_err;
            }
        }
    }

    /* Set callbacks according to driver type */
    switch(motor_driver_params->mode) {
        /* Two direction GPIO, handling brake */
        case MOTOR_DRIVER_2_DIRS:
        motor_driver->motor_set_cb = _motor_set_two_dirs;
        motor_driver->motor_brake_cb = _motor_brake_two_dirs;
        break;
        /* Single direction GPIO */
        case MOTOR_DRIVER_1_DIR:
        motor_driver->motor_set_cb = _motor_set_one_dir;
        motor_driver->motor_set_cb = NULL;
        break;
        /* Single direction GPIO and brake GPIO */
        case MOTOR_DRIVER_1_DIR_BRAKE:
        motor_driver->motor_set_cb = _motor_set_one_dir;
        motor_driver->motor_brake_cb = _motor_brake_one_dir_brake;
        break;
        /* Error */
        default:
            err = EINVAL;
            goto motor_init_err;
            break;
    };

    motor_driver->params = motor_driver_params;

motor_init_err:
    return -err;
}

static void _motor_set_two_dirs(const motor_t *motor, motor_direction_t direction)
{
    if ((gpio_is_valid(motor->gpio_dir0))
       && (gpio_is_valid(motor->gpio_dir1_or_brake))) {
        gpio_write(motor->gpio_dir0, direction);
        gpio_write(motor->gpio_dir1_or_brake, direction ^ 0x1);
    }
}

static void _motor_set_one_dir(const motor_t *motor, motor_direction_t direction)
{
    if (gpio_is_valid(motor->gpio_dir0)) {
        gpio_write(motor->gpio_dir0, direction);
    }
}

int motor_set(const motor_driver_t *motor_driver, uint8_t motor_id, \
              int32_t pwm_duty_cycle)
{
    int err = 0;

    if (motor_id >= motor_driver->params->nb_motors) {
        err = EINVAL;
        LOG_ERROR("Motor ID %u greater than number of motors %u\n",
                  motor_id, motor_driver->params->nb_motors);
        goto motor_set_err;
    }

    const motor_t *motor = &motor_driver->params->motors[motor_id];

    motor_direction_t direction = (pwm_duty_cycle < 0) ? MOTOR_CCW : MOTOR_CW;
    direction = direction ^ motor->gpio_dir_reverse;

    switch (direction) {
        case MOTOR_CW:
        case MOTOR_CCW:
            break;
        default:
            pwm_duty_cycle = 0;
            break;
    }

    /* Absolute value of pwm_duty_cycle */
    int32_t pwm_duty_cycle_abs = pwm_duty_cycle;
    pwm_duty_cycle_abs *= (pwm_duty_cycle < 0) ? -1 : 1;

    /* Critical section */
    int state = irq_disable();

    /* Set direction */
    if (motor_driver->motor_set_cb) {
        motor_driver->motor_set_cb(motor,
                                   direction);
    }

    /* Apply PWM duty cycle */
    pwm_set(motor_driver->params->pwm_dev, motor->pwm_channel, \
            (uint16_t)pwm_duty_cycle_abs);

    /* Remove brake */
    if (motor_driver->motor_brake_cb) {
        motor_driver->motor_brake_cb(motor, motor_driver->params->brake_inverted);
    }

    /* End of critical section */
    irq_restore(state);

    if (motor_driver->params->motor_set_post_cb) {
        motor_driver->params->motor_set_post_cb(motor_driver, motor_id, pwm_duty_cycle);
    }

    return 0;

motor_set_err:
    return -err;
}

static void _motor_brake_two_dirs(const motor_t *motor, bool brake)
{
    if ((gpio_is_valid(motor->gpio_dir0))
       && (gpio_is_valid(motor->gpio_dir1_or_brake))) {
        gpio_write(motor->gpio_dir0, brake);
        gpio_write(motor->gpio_dir1_or_brake, brake);
    }
}

static void _motor_brake_one_dir_brake(const motor_t *motor, bool brake)
{
    if (gpio_is_valid(motor->gpio_dir1_or_brake)) {
        gpio_write(motor->gpio_dir1_or_brake, brake);
    }
}

int motor_brake(const motor_driver_t *motor_driver, uint8_t motor_id)
{
    int err = 0;

    if (motor_id >= motor_driver->params->nb_motors) {
        err = EINVAL;
        LOG_ERROR("Motor ID %u greater than number of motors %u\n",
                  motor_id, motor_driver->params->nb_motors);
        goto motor_brake_err;
    }

    const motor_t *motor = &motor_driver->params->motors[motor_id];

    /* Critical section */
    int state = irq_disable();

    /* Apply brake */
    if (motor_driver->motor_brake_cb) {
        motor_driver->motor_brake_cb(motor, !motor_driver->params->brake_inverted);
    }

    /* Reset PWM duty cycle */
    pwm_set(motor_driver->params->pwm_dev, motor->pwm_channel, 0);

    irq_restore(state);

    return 0;

motor_brake_err:
    return -err;
}

void motor_enable(const motor_driver_t *motor_driver, uint8_t motor_id)
{
    if (motor_id >= motor_driver->params->nb_motors) {
        LOG_ERROR("Motor ID greater than number of motors\n");
        return;
    }

    const motor_t *motor = &motor_driver->params->motors[motor_id];

    if (gpio_is_valid(motor->gpio_enable)) {
        gpio_write(motor->gpio_enable, !motor_driver->params->enable_inverted);
    }
    else {
        LOG_WARNING("Enable GPIO is not valid for motor %u, skipping enable\n", motor_id);
    }
}

void motor_disable(const motor_driver_t *motor_driver, uint8_t motor_id)
{
    if (motor_id >= motor_driver->params->nb_motors) {
        LOG_ERROR("Motor ID greater than number of motors\n");
        return;
    }

    const motor_t *motor = &motor_driver->params->motors[motor_id];

    if (gpio_is_valid(motor->gpio_enable)) {
        gpio_write(motor->gpio_enable, motor_driver->params->enable_inverted);
    }
    else {
        LOG_WARNING("Enable GPIO is not valid for motor %u, skipping disable\n", motor_id);
    }
}

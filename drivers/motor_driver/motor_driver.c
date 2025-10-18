/*
 * SPDX-FileCopyrightText: 2018 Gilles DOFFE <g.doffe@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
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

int motor_driver_init(motor_driver_t *motor_driver, const motor_driver_params_t *params)
{
    int err = 0;

    uint32_t ret_pwm = pwm_init(params->pwm_dev,
                                params->pwm_mode,
                                params->pwm_frequency,
                                params->pwm_resolution);
    if (ret_pwm == 0) {
        err = EINVAL;
        LOG_ERROR("pwm_init failed\n");
        goto motor_init_err;
    }

    /* Init GPIO */
    err = 0;
    for (uint8_t i = 0; i < params->nb_motors; i++) {
        /* Init motor GPIOs, if one fails, motor is not setup */
        if (gpio_is_valid(params->motors[i].gpio_dir0)) {
            if (gpio_init(params->motors[i].gpio_dir0,
                          GPIO_OUT)) {
                err = EIO;
                LOG_ERROR("gpio_dir0 init failed for motor %d\n", i);
                goto motor_init_err;
            }
        }
        if (gpio_is_valid(params->motors[i].gpio_dir1)) {
            if (gpio_init(params->motors[i].gpio_dir1,
                          GPIO_OUT)) {
                err = EIO;
                LOG_ERROR("gpio_dir1/brake init failed for motor %d\n", i);
                goto motor_init_err;
            }
        }
        if (gpio_is_valid(params->motors[i].gpio_enable)) {
            if (gpio_init(params->motors[i].gpio_enable,
                          GPIO_OUT)) {
                err = EIO;
                LOG_ERROR("gpio_enable init failed for motor %d\n", i);
                goto motor_init_err;
            }
        }
    }

    motor_driver->params = params;

motor_init_err:
    return -err;
}

static void _motor_set_two_dirs(const motor_t *motor, motor_direction_t direction)
{
    if ((gpio_is_valid(motor->gpio_dir0))
       && (gpio_is_valid(motor->gpio_dir1))) {
        gpio_write(motor->gpio_dir0, direction);
        gpio_write(motor->gpio_dir1, direction ^ 1);
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

    if (direction != MOTOR_CW && direction != MOTOR_CCW) {
            pwm_duty_cycle = 0;
    }

    /* Absolute value of pwm_duty_cycle */
    int32_t pwm_duty_cycle_abs = pwm_duty_cycle;
    pwm_duty_cycle_abs *= (pwm_duty_cycle < 0) ? -1 : 1;

    /* Critical section */
    int state = irq_disable();

    /* Set direction */
    switch (motor_driver->params->mode) {
    /* Two direction GPIO, handling brake */
    case MOTOR_DRIVER_2_DIRS:
        _motor_set_two_dirs(motor, direction);
        break;
    /* Single direction GPIO */
    case MOTOR_DRIVER_1_DIR:
    case MOTOR_DRIVER_1_DIR_BRAKE:
        _motor_set_one_dir(motor, direction);
        break;
    /* Error */
    default:
        LOG_ERROR("Invalid mode to set direction %u\n",
                  motor_driver->params->mode);
        err = EINVAL;
        goto motor_set_err;
        break;
    }

    /* Apply PWM duty cycle */
    pwm_set(motor_driver->params->pwm_dev, motor->pwm_channel, \
            (uint16_t)pwm_duty_cycle_abs);

    /* Remove brake */
    switch (motor_driver->params->mode) {
    /* Two direction GPIO, handling brake */
    case MOTOR_DRIVER_2_DIRS:
        _motor_brake_two_dirs(motor, motor_driver->params->brake_inverted);
        break;
    case MOTOR_DRIVER_1_DIR:
        break;
    case MOTOR_DRIVER_1_DIR_BRAKE:
        _motor_brake_one_dir_brake(motor, motor_driver->params->brake_inverted);
        break;
    /* Error */
    default:
        LOG_ERROR("Invalid mode to unbrake %u\n", motor_driver->params->mode);
        err = EINVAL;
        goto motor_set_err;
        break;
    };

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
       && (gpio_is_valid(motor->gpio_dir1))) {
        gpio_write(motor->gpio_dir0, brake);
        gpio_write(motor->gpio_dir1, brake);
    }
}

static void _motor_brake_one_dir_brake(const motor_t *motor, bool brake)
{
    if (gpio_is_valid(motor->gpio_brake)) {
        gpio_write(motor->gpio_brake, brake);
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
    switch (motor_driver->params->mode) {
    /* Two direction GPIO, handling brake */
    case MOTOR_DRIVER_2_DIRS:
        _motor_brake_two_dirs(motor, !motor_driver->params->brake_inverted);
        break;
    /* Single direction GPIO */
    case MOTOR_DRIVER_1_DIR:
        DEBUG("%s: cannot brake with only one direction pin, just set PWM to 0\n", __func__);
        break;
    case MOTOR_DRIVER_1_DIR_BRAKE:
        _motor_brake_one_dir_brake(motor, !motor_driver->params->brake_inverted);
        break;
    /* Error */
    default:
        LOG_ERROR("Invalid mode to brake %u\n", motor_driver->params->mode);
        err = EINVAL;
        goto motor_brake_err;
        break;
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

/*
 * SPDX-FileCopyrightText: 2018 Gilles DOFFE <g.doffe@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_motor
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

int motor_driver_init(motor_driver_t motor_driver)
{
    int err = 0;

    assert(motor_driver < MOTOR_DRIVER_NUMOF);

    const motor_driver_config_t *motor_driver_conf = \
        &motor_driver_config[motor_driver];

    pwm_t pwm_dev = motor_driver_conf->pwm_dev;
    pwm_mode_t mode = motor_driver_conf->pwm_mode;
    uint32_t freq = motor_driver_conf->pwm_frequency;
    uint16_t resol = motor_driver_conf->pwm_resolution;

    uint32_t pwm_freq = pwm_init(pwm_dev, mode, freq, resol);
    if (pwm_freq == 0) {
        err = EINVAL;
        LOG_ERROR("pwm_init failed\n");
        goto motor_init_err;
    }

    for (uint8_t i = 0; i < motor_driver_conf->nb_motors; i++) {
        if (gpio_is_valid(motor_driver_conf->motors[i].gpio_dir0)
            && (gpio_init(motor_driver_conf->motors[i].gpio_dir0,
                          GPIO_OUT))) {
            err = EIO;
            LOG_ERROR("gpio_dir0 init failed\n");
            goto motor_init_err;
        }
        if (gpio_is_valid(motor_driver_conf->motors[i].gpio_dir1_or_brake)
            && (gpio_init(motor_driver_conf->motors[i].gpio_dir1_or_brake,
                          GPIO_OUT))) {
            err = EIO;
            LOG_ERROR("gpio_dir1_or_brake init failed\n");
            goto motor_init_err;
        }
        if (gpio_is_valid(motor_driver_conf->motors[i].gpio_enable)) {
            if (gpio_init(motor_driver_conf->motors[i].gpio_enable,
                          GPIO_OUT)) {
                err = EIO;
                LOG_ERROR("gpio_enable init failed\n");
                goto motor_init_err;
            }
            motor_enable(motor_driver, i);
        }
    }

    return 0;

motor_init_err:
    return -err;
}

int motor_set(const motor_driver_t motor_driver, uint8_t motor_id, \
              int32_t pwm_duty_cycle)
{
    int err = 0;

    assert(motor_driver < MOTOR_DRIVER_NUMOF);

    const motor_driver_config_t *motor_driver_conf =
        &motor_driver_config[motor_driver];

    assert(motor_id < motor_driver_conf->nb_motors);

    const motor_t *dev = &motor_driver_conf->motors[motor_id];

    int gpio_dir0_value = 0;
    int gpio_dir1_or_brake_value = 0;

    motor_direction_t direction = (pwm_duty_cycle < 0) ? MOTOR_CCW : MOTOR_CW;

    direction = direction ^ dev->gpio_dir_reverse;

    /* Two direction GPIO, handling brake */
    if (motor_driver_conf->mode == MOTOR_DRIVER_2_DIRS) {
        if (!gpio_is_valid(dev->gpio_dir0) || \
            !gpio_is_valid(dev->gpio_dir1_or_brake)) {
            err = ENODEV;
            goto motor_set_err;
        }
        switch (direction) {
            case MOTOR_CW:
            case MOTOR_CCW:
                /* Direction */
                gpio_dir0_value = direction;
                gpio_dir1_or_brake_value = direction ^ 0x1;
                break;
            default:
                pwm_duty_cycle = 0;
                break;
        }
    }
    /* Single direction GPIO */
    else if (motor_driver_conf->mode == MOTOR_DRIVER_1_DIR) {
        if (!gpio_is_valid(dev->gpio_dir0)) {
            err = ENODEV;
            goto motor_set_err;
        }
        switch (direction) {
            case MOTOR_CW:
            case MOTOR_CCW:
                /* Direction */
                gpio_dir0_value = direction;
                break;
            default:
                pwm_duty_cycle = 0;
                break;
        }
    }
    /* Single direction GPIO and brake GPIO */
    else if (motor_driver_conf->mode == MOTOR_DRIVER_1_DIR_BRAKE) {
        if (!gpio_is_valid(dev->gpio_dir0) || \
            !gpio_is_valid(dev->gpio_dir1_or_brake)) {
            err = ENODEV;
            goto motor_set_err;
        }
        switch (direction) {
            case MOTOR_CW:
            case MOTOR_CCW:
                /* Direction */
                gpio_dir0_value = direction;
                /* No brake */
                gpio_dir1_or_brake_value = dev->gpio_brake_invert;
                break;
            default:
                pwm_duty_cycle = 0;
                break;
        }
    }
    else {
        err = EINVAL;
        goto motor_set_err;
    }

    /* Absolute value of pwm_duty_cycle */
    int32_t pwm_duty_cycle_abs = pwm_duty_cycle;
    pwm_duty_cycle_abs *= (pwm_duty_cycle < 0) ? -1 : 1;

    unsigned irqstate = irq_disable();
    gpio_write(dev->gpio_dir0, gpio_dir0_value);
    gpio_write(dev->gpio_dir1_or_brake, gpio_dir1_or_brake_value);
    pwm_set(motor_driver_conf->pwm_dev, dev->pwm_channel, \
            (uint16_t)pwm_duty_cycle_abs);
    irq_restore(irqstate);

    motor_driver_cb_t cb = motor_driver_conf->cb;
    if (cb) {
        cb(motor_driver, motor_id, pwm_duty_cycle);
    }

    return 0;

motor_set_err:
    return -err;
}

int motor_brake(const motor_driver_t motor_driver, uint8_t motor_id)
{
    int err = 0;

    assert(motor_driver < MOTOR_DRIVER_NUMOF);

    const motor_driver_config_t *motor_driver_conf =
        &motor_driver_config[motor_driver];

    assert(motor_id < motor_driver_conf->nb_motors);

    const motor_t *dev = &motor_driver_conf->motors[motor_id];

    int gpio_dir0_value = 0;
    int gpio_dir1_or_brake_value = 0;

    /* Two direction GPIO, handling brake */
    if (motor_driver_conf->mode == MOTOR_DRIVER_2_DIRS) {
        if (!gpio_is_valid(dev->gpio_dir0) || \
            !gpio_is_valid(dev->gpio_dir1_or_brake)) {
            err = ENODEV;
            goto motor_brake_err;
        }
        /* Brake */
        gpio_dir0_value =
            motor_driver_conf->mode_brake;
        gpio_dir1_or_brake_value =
            motor_driver_conf->mode_brake;
    }
    /* Single direction GPIO */
    else if (motor_driver_conf->mode == MOTOR_DRIVER_1_DIR) {
        /* Nothing to do here */
    }
    /* Single direction GPIO and brake GPIO */
    else if (motor_driver_conf->mode == MOTOR_DRIVER_1_DIR_BRAKE) {
        if (!gpio_is_valid(dev->gpio_dir1_or_brake)) {
            err = ENODEV;
            goto motor_brake_err;
        }
        /* Brake */
        gpio_dir1_or_brake_value = 1 ^ dev->gpio_brake_invert;
    }
    else {
        err = EINVAL;
        goto motor_brake_err;
    }

    unsigned irqstate = irq_disable();
    gpio_write(dev->gpio_dir0, gpio_dir0_value);
    gpio_write(dev->gpio_dir1_or_brake, gpio_dir1_or_brake_value);
    pwm_set(motor_driver_conf->pwm_dev, dev->pwm_channel, 0);
    irq_restore(irqstate);

    return 0;

motor_brake_err:
    return -err;
}

void motor_enable(const motor_driver_t motor_driver, uint8_t motor_id)
{
    assert(motor_driver < MOTOR_DRIVER_NUMOF);

    const motor_driver_config_t *motor_driver_conf =
        &motor_driver_config[motor_driver];

    assert(motor_id < motor_driver_conf->nb_motors);

    const motor_t *dev = &motor_driver_conf->motors[motor_id];

    assert(gpio_is_valid(dev->gpio_enable));

    gpio_write(dev->gpio_enable, 1 ^ dev->gpio_enable_invert);
}

void motor_disable(const motor_driver_t motor_driver, uint8_t motor_id)
{
    assert(motor_driver < MOTOR_DRIVER_NUMOF);

    const motor_driver_config_t *motor_driver_conf =
        &motor_driver_config[motor_driver];

    assert(motor_id < motor_driver_conf->nb_motors);

    const motor_t *dev = &motor_driver_conf->motors[motor_id];

    assert(gpio_is_valid(dev->gpio_enable));

    gpio_write(dev->gpio_enable, dev->gpio_enable_invert);
}

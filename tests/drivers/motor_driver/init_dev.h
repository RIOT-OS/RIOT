 /*
 * Copyright (C) 2023 COGIP Robotics association
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
 * @brief       Motor driver test header file.
 *
 * @author      Gilles DOFFE <g.doffe@gmail.com>
 *
 */

#ifndef INIT_DEV_H
#define INIT_DEV_H

#include <motor_driver.h>

#ifndef MOTOR_DRIVER_PARAM_MOTOR_SET_POST_CALLBACK
/** Default callback called at end of motor_set() */
#define MOTOR_DRIVER_PARAM_MOTOR_SET_POST_CALLBACK  motor_driver_callback_example
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Simple motor_set post callback example.
 * Just print current PWM value.
 *
 * @param[in] motor_driver      motor driver to which motor is attached
 * @param[in] motor_id          motor ID on driver
 * @param[in] pwm_duty_cycle    Signed PWM duty_cycle to set motor speed and direction
 */
void motor_driver_callback_example(
    const motor_driver_t *motor_driver, uint8_t motor_id,
    int32_t pwm_duty_cycle);

#ifdef __cplusplus
}
#endif

#endif /* INIT_DEV_H */
/** @} */

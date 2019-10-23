/*
 * Copyright (C) 2018 Gilles DOFFE <g.doffe@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_motor DC Motor Driver
 * @ingroup     drivers_actuators
 * @brief       High-level driver for DC motors
 *
 * This API aims to handle DC motor analogic driver.
 * Driver boards using serial communication protocols (I2C, UART, etc...) are not in the
 * scope of this driver.
 * Mainly designed for H-bridge, it could also drive some brushless drivers.
 *
 * Some H-bridge driver circuits handle several motors.
 * Maximum motor number by H-bridge is set to 2 with MOTOR_DRIVER_MAX macro.
 * This macro can be overridden to support H-bridge drivers with more outputs.
 * However, MOTOR_DRIVER_MAX should not exceed PWM channels number.
 *
 * motor_driver_t structure represents an H-bridge.
 * As several H-bridge can share a same PWM device, motor_driver_t can
 * represent a group of H-bridge.
 *
 * Most of H-bridge boards uses the following I/Os for each motor :
 * - Enable/disable GPIO
 * - One or two direction GPIOs
 * - A PWM signal
 *
 * @verbatim
 *
 * Each motor direction is controlled (assuming it is enabled) according to
 * the following truth table :
 *  __________________________
 * | DIR0 | DIR1 |  BEHAVIOR  |
 * |--------------------------|
 * |  0   |  0   | BRAKE LOW  |
 * |  0   |  1   |     CW     |
 * |  1   |  0   |     CCW    |
 * |  1   |  1   | BRAKE HIGH |
 * |______|______|____________|
 *
 * In case of single GPIO for direction, only DIR0 is used without brake
 * capability :
 *  ___________________
 * | DIR0 |  BEHAVIOR  |
 * |-------------------|
 * |   0  |     CW     |
 * |   1  |     CCW    |
 * |______|____________|
 *
 * Some boards add a brake pin with single direction GPIO :
 *  ________________________
 * | DIR | BRAKE | BEHAVIOR |
 * |------------------------|
 * |  0  |   0   |    CW    |
 * |  0  |   1   |   BRAKE  |
 * |  1  |   0   |    CCW   |
 * |  1  |   1   |   BRAKE  |
 * |_____|_______|__________|
 *
 * @endverbatim
 *
 * From this truth tables we can extract two direction states :
 * - CW (ClockWise)
 * - CCW (Counter ClockWise)
 * and a brake capability
 *
 * BRAKE LOW is functionally the same than BRAKE HIGH but some H-bridge only
 * brake on BRAKE HIGH due to hardware.
 * In case of single direction GPIO, there is no BRAKE, PWM duty cycle is set
 * to 0.

 * @{
 * @file
 * @brief       High-level driver for DC motors
 *
 * @author      Gilles DOFFE <g.doffe@gmail.com>
 */

#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H

#include "periph/pwm.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Maximum number of motors by motor driver
 */
#ifndef MOTOR_DRIVER_MAX
#define MOTOR_DRIVER_MAX    (2)
#endif /* MOTOR_DRIVER_MAX */

/**
 * @brief Macro to return motor driver id
 */
#define MOTOR_DRIVER_DEV(x) (x)

/**
 * @brief Describe DC motor driver modes
 */
typedef enum {
    MOTOR_DRIVER_2_DIRS         = 0,            /**< 2 GPIOS for direction, \
                                                      handling BRAKE */
    MOTOR_DRIVER_1_DIR          = 1,            /**< Single GPIO for direction, \
                                                      no BRAKE */
    MOTOR_DRIVER_1_DIR_BRAKE    = 2             /**< Single GPIO for direction, \
                                                      Single GPIO for BRAKE */
} motor_driver_mode_t;

/**
 * @brief Describe DC motor driver brake modes
 */
typedef enum {
    MOTOR_BRAKE_LOW     = 0,        /**< Low stage brake */
    MOTOR_BRAKE_HIGH    = 1,        /**< High stage brake */
} motor_driver_mode_brake_t;

/**
 * @brief Describe DC motor direction states
 */
typedef enum {
    MOTOR_CW    = 0,            /**< clockwise */
    MOTOR_CCW   = 1,            /**< counter clockwise */
} motor_direction_t;

/**
 * @brief Describe DC motor with PWM channel and GPIOs
 */
typedef struct {
    int pwm_channel;            /**< PWM channel the motor is connected to */
    gpio_t gpio_enable;         /**< GPIO to enable/disable motor */
    gpio_t gpio_dir0;           /**< GPIO to control rotation direction */
    gpio_t gpio_dir1_or_brake;  /**< GPIO to control rotation direction */
    uint8_t gpio_dir_reverse;   /**< flag to reverse direction */
    uint8_t gpio_enable_invert; /**< flag to set enable GPIO inverted mode */
    uint8_t gpio_brake_invert;  /**< flag to make brake active low */
} motor_t;

/**
 * @brief   Default motor driver type definition
 */
typedef unsigned int motor_driver_t;

/**
 * @brief   Motor callback. It is called at end of motor_set()
 */
typedef void (*motor_driver_cb_t)(const motor_driver_t motor_driver,
                                  uint8_t motor_id,
                                  int32_t pwm_duty_cycle);

/**
 * @brief Describe DC motor driver with PWM device and motors array
 */
typedef struct {
    pwm_t pwm_dev;                          /**< PWM device driving motors */
    motor_driver_mode_t mode;               /**< driver mode */
    motor_driver_mode_brake_t mode_brake;   /**< driver brake mode */
    pwm_mode_t pwm_mode;                    /**< PWM mode */
    uint32_t pwm_frequency;                 /**< PWM device frequency */
    uint32_t pwm_resolution;                /**< PWM device resolution */
    uint8_t nb_motors;                      /**< number of moros */
    motor_t motors[MOTOR_DRIVER_MAX];       /**< motors array */
    motor_driver_cb_t cb;                   /**< callback on motor_set */
} motor_driver_config_t;

/**
 * @brief Initialize DC motor driver board
 *
 * @param[out] motor_driver     motor driver to initialize
 *
 * @return                      0 on success
 * @return                      -1 on error with errno set
 */
int motor_driver_init(const motor_driver_t motor_driver);

/**
 * @brief Set motor speed and direction
 *
 * @param[in] motor_driver      motor driver to which motor is attached
 * @param[in] motor_id          motor ID on driver
 * @param[in] pwm_duty_cycle    Signed PWM duty_cycle to set motor speed and direction
 *
 * @return                      0 on success
 * @return                      -1 on error with errno set
 */
int motor_set(const motor_driver_t motor_driver, uint8_t motor_id, \
              int32_t pwm_duty_cycle);

/**
 * @brief Brake the motor of a given motor driver
 *
 * @param[in] motor_driver      motor driver to which motor is attached
 * @param[in] motor_id          motor ID on driver
 *
 * @return                      0 on success
 * @return                      -1 on error with errno set
 */
int motor_brake(const motor_driver_t motor_driver, uint8_t motor_id);

/**
 * @brief Enable a motor of a given motor driver
 *
 * @param[in] motor_driver      motor driver to which motor is attached
 * @param[in] motor_id          motor ID on driver
 *
 * @return
 */
void motor_enable(const motor_driver_t motor_driver, uint8_t motor_id);

/**
 * @brief Disable a motor of a given motor driver
 *
 * @param[in] motor_driver      motor driver to which motor is attached
 * @param[in] motor_id          motor ID on driver
 *
 * @return
 */
void motor_disable(const motor_driver_t motor_driver, uint8_t motor_id);

#ifdef __cplusplus
}
#endif

#endif /* MOTOR_DRIVER_H */
/** @} */

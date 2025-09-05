/*
 * SPDX-FileCopyrightText: 2020 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    drivers_pca9633 PCA9633 I2C PWM controller
 * @ingroup     drivers_actuators
 * @brief       Device driver for the NXP PCA9633
 *
 * @{
 *
 * @author      Hendrik van Essen <hendrik.ve@fu-berlin.de>
 * @file
 */

#ifdef __cplusplus
extern "C"
{
#endif

#include "stdbool.h"
#include "periph/i2c.h"

#include "pca9633_regs.h"

/**
 * @brief Blinking period with a maximum duration of ~10.73 s
 */
#define PCA9633_BLINKING_PERIOD_MAX_MS  (10625)

/**
 * @brief Ration between on/ off in blinking mode is balanced.
 *
 * 128 = 255 / 2
 */
#define PCA9633_BLINKING_RATIO_BALANCED 128

/**
 * @brief   PCA9633 device initialization parameters
 */
typedef struct {
    i2c_t i2c_dev;              /**< I2C device */
    uint16_t i2c_addr;          /**< I2C address of device */

    uint8_t reg_pwm_red;        /**< Register for red color */
    uint8_t reg_pwm_green;      /**< Register for green color */
    uint8_t reg_pwm_blue;       /**< Register for blue color */
    uint8_t reg_pwm_amber;      /**< Register for amber color */

    bool has_amber_channel;      /**< Whether PCA9633 has fourth channel */
} pca9633_params_t;

/**
 * @brief   PCA9633 PWM device data structure type
 */
typedef struct {
    pca9633_params_t params;     /**< Device initialization parameters */
    uint8_t stored_reg_ledout;   /**< Stored register content of LEDOUT */
} pca9633_t;

/**
 * @brief   PCA9633 driver error codes
 */
enum {
    PCA9633_OK              = 0,    /**< Success */
    PCA9633_ERROR_I2C       = 1,    /**< I2C communication error */
};

/**
 * @brief   PCA9633 PWM channel definitions
 */
typedef enum {
    PCA9633_PWM_CHANNEL_0 = PCA9633_REG_PWM0,   /**< PWM channel 0 */
    PCA9633_PWM_CHANNEL_1 = PCA9633_REG_PWM1,   /**< PWM channel 1 */
    PCA9633_PWM_CHANNEL_2 = PCA9633_REG_PWM2,   /**< PWM channel 2 */
    PCA9633_PWM_CHANNEL_3 = PCA9633_REG_PWM3,   /**< PWM channel 3 */
} pca9633_pwm_channel_t;

/**
 * @brief   LED driver output state, LEDOUT (page 14, below table 13)
 */
typedef enum {
    /**
     * @brief LED driver x is off
     */
    PCA9633_LDR_STATE_OFF,

    /**
     * @brief LED driver x is fully on (individual brightness and group
     *        dimming/ blinking not controlled)
     */
    PCA9633_LDR_STATE_ON,

    /**
     * @brief LED driver x individual brightness can be controlled through its
     *        PWMx register
     */
    PCA9633_LDR_STATE_IND,

    /**
     * @brief LED driver x individual brightness and group dimming/ blinking can
     *        be controlled through its PWMx register and the GRPPWM registers.
     *        If using PCA9633_LDR_STATE_IND_GRP the controller takes the
     *        minimum value of PWM* and GRPPWM register
     */
    PCA9633_LDR_STATE_IND_GRP,
} pca9633_ldr_state_t;

/**
 * @brief   Auto-Increment options (page 10, table 6)
 */
typedef enum {
    /**
     * @brief No Auto-Increment
     */
    PCA9633_AI_DISABLED,

    /**
     * @brief Auto-Increment for all registers. D3, D2, D1, D0 roll over to
     *        ‘0000’ after the last register (1100) is accessed.
     */
    PCA9633_AI_ALL,

    /**
     * @brief Auto-Increment for individual brightness registers only.
     *        D3, D2, D1, D0 roll over to ‘0010’ after the last register (0101)
     *        is accessed.
     */
    PCA9633_AI_IND,

    /**
     * @brief Auto-Increment for global control registers only. D3, D2, D1, D0
     *        roll over to ‘0110’ after the last register (0111) is accessed.
     */
    PCA9633_AI_GBL,

    /**
     * @brief Auto-Increment for individual and global control registers only.
     *        D3, D2, D1, D0 roll over to ‘0010’ after the last register (0111)
     *        is accessed.
     */
    PCA9633_AI_IND_GBL,
} pca9633_auto_inc_option_t;

/**
 * @brief   PCA9633 group control modes
 */
typedef enum {
    /**
     * @brief Control mode for blinking
     */
    PCA9633_GROUP_CONTROL_MODE_BLINKING,

    /**
     * @brief Control mode for dimming
     */
    PCA9633_GROUP_CONTROL_MODE_DIMMING,
} pca9633_group_control_mode_t;

/**
 * @brief Initialization.
 *
 * @param[in] dev       Device descriptor of the PCA9633
 * @param[in] params    Parameters for device initialization
 *
 * @return  PCA9633_OK on success
 * @return  -PCA9633_ERROR_I2C if acquiring of I2C bus fails
 * @return  -EIO When slave device doesn't ACK the byte
 * @return  -ENXIO When no devices respond on the address sent on the bus
 * @return  -ETIMEDOUT When timeout occurs before device's response
 * @return  -EINVAL When an invalid argument is given
 * @return  -EOPNOTSUPP When MCU driver doesn't support the flag operation
 * @return  -EAGAIN When a lost bus arbitration occurs
 */
int pca9633_init(pca9633_t *dev, const pca9633_params_t *params);

/**
 * @brief Turn on all LEDs. Restores settings saved at pca9633_turn_off().
 *
 * WARNING: If you call pca9633_turn_off() twice, without calling
 *          pca9633_turn_on() in between, then the restored state will be
 *          PCA9633_LDR_STATE_OFF!
 *
 * @param[in] dev       Device descriptor of the PCA9633
 */
void pca9633_turn_on(pca9633_t* dev);

/**
 * @brief Turn off all LEDs. Saves current settings for pca9633_turn_on().
 *        For power saving, see pca9633_sleep().
 *
 * WARNING: If you call pca9633_turn_off() twice, without calling
 *          pca9633_turn_on() in between, then the restored state will be
 *          PCA9633_LDR_STATE_OFF!
 *
 * @param[in] dev       Device descriptor of the PCA9633
 */
void pca9633_turn_off(pca9633_t* dev);

/**
 * @brief Switch to normal mode.
 *
 * @param[in] dev       Device descriptor of the PCA9633
 */
void pca9633_wakeup(pca9633_t* dev);

/**
 * @brief Switch to low power mode.
 *
 * @param[in] dev       Device descriptor of the PCA9633
 */
void pca9633_sleep(pca9633_t* dev);

/**
 * @brief Set individual PWM signal for a given channel.
 *
 * @param[in] dev           Device descriptor of the PCA9633
 * @param[in] pwm_channel   PWM channel
 * @param[in] pwm           PWM value
 */
void pca9633_set_pwm(pca9633_t* dev,
                     pca9633_pwm_channel_t pwm_channel, uint8_t pwm);

/**
 * @brief Set global PWM signal.
 *
 * @param[in] dev       Device descriptor of the PCA9633
 * @param[in] pwm       PWM value
 */
void pca9633_set_grp_pwm(pca9633_t* dev, uint8_t pwm);

/**
 * @brief Set up values for blinking mode. Blinking mode needs to be activated
 *        manually by calling
 *        pca9633_set_group_control_mode(GROUP_CONTROL_MODE_BLINKING).
 *
 * @param[in] dev               Device descriptor of the PCA9633
 * @param[in] blink_period_ms   Period in ms for one blink (turning off and on).
 *                              Maximum period possible is
 *                              PCA9633_BLINKING_PERIOD_MAX_MS ≈ 10.73 s. All
 *                              values above this maximum will we capped to it.
 * @param[in] on_off_ratio      Value between 0 and 255, where e.g. a value of
 *                              64 (255/4) means 1/4 of the time the LEDs are on
 *                              and 3/4 of the time the LEDs are off.
 */
void pca9633_set_blinking(pca9633_t* dev, uint16_t blink_period_ms,
                          uint8_t on_off_ratio);

/**
 * @brief Set PWM values for RGB.
 *
 * @param[in] dev       Device descriptor of the PCA9633
 * @param[in] r         Value for red color channel
 * @param[in] g         Value for green color channel
 * @param[in] b         Value for blue color channel
 */
void pca9633_set_rgb(pca9633_t* dev, uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Set PWM values for RGBA.
 *
 * @param[in] dev       Device descriptor of the PCA9633
 * @param[in] r         Value for red color channel
 * @param[in] g         Value for green color channel
 * @param[in] b         Value for blue color channel
 * @param[in] w         Value for amber color channel
 */
void pca9633_set_rgba(pca9633_t* dev, uint8_t r, uint8_t g, uint8_t b, uint8_t w);

/**
 * @brief Set the LED driver output state for a given channel.
 *        There are four states:
 *          - PCA9633_LDR_STATE_OFF
 *          - PCA9633_LDR_STATE_ON
 *          - PCA9633_LDR_STATE_IND
 *          - PCA9633_LDR_STATE_IND_GRP
 *
 * @param[in] dev           Device descriptor of the PCA9633
 * @param[in] state         One of the four possible states
 * @param[in] pwm_channel   PWM channel belonging to LDR
 */
void pca9633_set_ldr_state(pca9633_t* dev,
                           pca9633_ldr_state_t state,
                           pca9633_pwm_channel_t pwm_channel);

/**
 * @brief Set the LED driver output state for all channels.
 *        There are four states:
 *          - PCA9633_LDR_STATE_OFF
 *          - PCA9633_LDR_STATE_ON
 *          - PCA9633_LDR_STATE_IND
 *          - PCA9633_LDR_STATE_IND_GRP
 *
 * @param[in] dev       Device descriptor of the PCA9633
 * @param[in] state     One of the four possible states
 */
void pca9633_set_ldr_state_all(pca9633_t* dev, pca9633_ldr_state_t state);

/**
 * @brief Set an option for auto increment.
 *        There are five options:
 *          - PCA9633_AI_DISABLED
 *          - PCA9633_AI_ALL
 *          - PCA9633_AI_IND
 *          - PCA9633_AI_GBL
 *          - PCA9633_AI_IND_GBL
 *
 * @param[in] dev       Device descriptor of the PCA9633
 * @param[in] option    One of the possible five options
 */
void pca9633_set_auto_increment(pca9633_t* dev, pca9633_auto_inc_option_t option);

/**
 * @brief Set the group control mode.
 *        There are two modes:
 *          - PCA9633_GROUP_CONTROL_MODE_BLINKING
 *          - PCA9633_GROUP_CONTROL_MODE_DIMMING
 *
 * @param[in] dev       Device descriptor of the PCA9633
 * @param[in] mode      One of the two possible modes
 */
void pca9633_set_group_control_mode(pca9633_t* dev,
                                    pca9633_group_control_mode_t mode);

#ifdef __cplusplus
}
#endif

/** @} */

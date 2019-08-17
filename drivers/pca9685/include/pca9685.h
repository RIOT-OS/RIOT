/*
 * Copyright (C) 2019 Marius <marius@twostairs.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_pca9685 PCA9685 16-channel, 12-bit PWM Fm+ I²C-bus controller
 * @ingroup     drivers_pca9685
 * @brief       Device driver interface for the PCA9685 16-channel, 12-bit PWM Fm+ I²C-bus controller
 * @{
 *
 * @file
 * @brief       Device driver interface for the PCA9685 16-channel, 12-bit PWM Fm+ I²C-bus controller.
 *
 * @author      Marius <marius@twostairs.com>
 *
 * @}
 */

#ifndef PCA9685_H
#define PCA9685_H

#include <stdio.h>

#include "periph/i2c.h"

#define PCA9685_ADDR (0x43)

#define PCA9685_SUBADR1 (0x2) /**< i2c bus address 1 */
#define PCA9685_SUBADR2 (0x3) /**< i2c bus address 2 */
#define PCA9685_SUBADR3 (0x4) /**< i2c bus address 3 */

#define PCA9685_MODE1 (0x0) /**< Mode Register 1 */
#define PCA9685_MODE2 (0x1) /**< Mode Register 2 */
#define PCA9685_PRESCALE (0xFE) /**< Prescaler for PWM output frequency */

#define LED0_ON_L (0x6) /**< LED0 output and brightness control byte 0 */
#define LED0_ON_H (0x7) /**< LED0 output and brightness control byte 1 */
#define LED0_OFF_L (0x8) /**< LED0 output and brightness control byte 2 */
#define LED0_OFF_H (0x9) /**< LED0 output and brightness control byte 3 */

#define ALLLED_ON_L (0xFA) /**< load all the LEDn_ON registers, byte 0 */
#define ALLLED_ON_H (0xFB) /**< load all the LEDn_ON registers, byte 1 */
#define ALLLED_OFF_L (0xFC) /**< load all the LEDn_OFF registers, byte 0 */
#define ALLLED_OFF_H (0xFD) /**< load all the LEDn_OFF registers, byte 1 */

#ifndef PCA9685_PARAM_I2C_DEV
#define PCA9685_PARAM_I2C_DEV         I2C_DEV(0)
#endif
#ifndef PCA9685_PARAM_I2C_ADDR
#define PCA9685_PARAM_I2C_ADDR        PCA9685_ADDR
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Device initialization parameters
 */
typedef struct {
    i2c_t i2c_dev;                              /**< I2C device which is used */
    uint8_t i2c_addr;                           /**< I2C address */
    float pwm_freq;                             /**< PWM frequency */
} pca9685_params_t;

/**
 * @brief   Device descriptor for the PCA9685
 */
typedef struct {
    pca9685_params_t      params;               /**< Device initialization parameters */
} pca9685_t;

/**
 * @brief   Status and error return codes
 */
enum {
    PCA9685_OK = 0,                              /**< everything was fine */
    PCA9685_ERR_NODEV,                           /**< did not detect device */
    PCA9685_ERR_SETFREQ,                         /**< error when setting the frequency */
    PCA9685_ERR_SETEXTCLK,                       /**< error when setting the external clock */
    PCA9685_ERR_SETPWM,                          /**< error when setting the PWM */
    PCA9685_ERR_WAKEUP,                          /**< error when waking up */
    PCA9685_ERR_SLEEP,                           /**< error when putting to sleep */
};

int pca9685_init(pca9685_t *dev, const pca9685_params_t *params);
int pca9685_set_ext_clk(pca9685_t *dev, uint8_t prescale);
int pca9685_set_pwm_freq(pca9685_t *dev, float freq);

int pca9685_set_pwm(pca9685_t *dev, uint8_t num, uint16_t on, uint16_t off, bool acquire, bool keep_acquired);
uint16_t pca9685_get_pwm(pca9685_t *dev, uint8_t num);

int pca9685_wakeup(pca9685_t *dev);
int pca9685_sleep(pca9685_t *dev);

#ifdef __cplusplus
}
#endif

#endif

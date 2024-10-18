/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_pca9685
 * @brief       Register definitions for the PCA9685 I2C PWM controller
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#ifndef PCA9685_REGS_H
#define PCA9685_REGS_H

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name    Register addresses
 * @{
 */
#define PCA9685_REG_MODE1           (0x00) /**< Mode1 register */
#define PCA9685_REG_MODE2           (0x01) /**< Mode2 register */
#define PCA9685_REG_SUBADR1         (0x02) /**< I2C bus subaddress 1 register */
#define PCA9685_REG_SUBADR2         (0x03) /**< I2C bus subaddress 2 register */
#define PCA9685_REG_SUBADR3         (0x04)  /**< I2C bus subaddress 3 register */
#define PCA9685_REG_ALLCALLADDR     (0x05)  /**< LED All Call I 2 C-bus address */
#define PCA9685_REG_LED0_ON_L       (0x06)  /**< LED0 ON control low byte */

#define PCA9685_REG_ALL_LED_ON_L    (0xfa) /**< Load all LEDn_OFF register low byte */
#define PCA9685_REG_ALL_LED_ON_H    (0xfb) /**< Load all LEDn_OFF register high byte */
#define PCA9685_REG_ALL_LED_OFF_L   (0xfc) /**< Load all LEDn_OFF register low byte */
#define PCA9685_REG_ALL_LED_OFF_H   (0xfd) /**< Load all LEDn_OFF register high byte */

#define PCA9685_REG_ALL_LED_ON      (0xfa) /**< Load all LEDn_OFF register word */
#define PCA9685_REG_ALL_LED_OFF     (0xfc) /**< Load all LEDn_OFF register word */

#define PCA9685_REG_PRE_SCALE       (0xfe) /**< Prescaler for PWM output frequency */
#define PCA9685_REG_TEST_MODE       (0xff) /**< Enter test mode register */

#define PCA9685_REG_LED_ON(n)       (0x06 + (n << 2))   /**< LEDn ON control word */
#define PCA9685_REG_LED_OFF(n)      (0x08 + (n << 2))   /**< LEDn OFF control word */

#define PCA9685_REG_LED_ON_L(n)     (0x06 + (n << 2))   /**< LEDn ON control low byte */
#define PCA9685_REG_LED_ON_H(n)     (0x07 + (n << 2))   /**< LEDn ON control high byte */
#define PCA9685_REG_LED_OFF_L(n)    (0x08 + (n << 2))   /**< LEDn OFF control low byte */
#define PCA9685_REG_LED_OFF_H(n)    (0x09 + (n << 2))   /**< LEDn OFF control high byte */
/** @} */

/**
 * @name    Register structures
 * @{
 */

/* PCA9685_REG_MODE1 */
#define PCA9685_MODE1_RESTART   (0x80)  /**< State of restart logic, write 1 to clear */
#define PCA9685_MODE1_EXTCLK    (0x40)  /**< Use EXTCLK pin */
#define PCA9685_MODE1_AI        (0x20)  /**< Enable register auto-increment*/
#define PCA9685_MODE1_SLEEP     (0x10)  /**< Enter low power mode, PWM is off */
#define PCA9685_MODE1_SUB1      (0x08)  /**< Enable I2C subaddress 1 */
#define PCA9685_MODE1_SUB2      (0x04)  /**< Enable I2C subaddress 2 */
#define PCA9685_MODE1_SUB3      (0x02)  /**< Enable I2C subaddress 3 */
#define PCA9685_MODE1_ALLCALL   (0x01)  /**< Enable I2C all call address */

/* PCA9685_REG_MODE2 */
#define PCA9685_MODE2_INVERT    (0x10)  /**< Invert outputs */
#define PCA9685_MODE2_OCH       (0x08)  /**< Output change change configuration */
#define PCA9685_MODE2_OUTDRV    (0x04)  /**< Output driver configuration */
#define PCA9685_MODE2_OUTNE     (0x03)  /**< Output enabled configuration */
/** @} */

/**
 * @name    Register value definitions
 * @{
 */
#define PCA9685_LED_ON          (0x1000)    /* LEDs on word */
#define PCA9685_LED_OFF         (0x1000)    /* LEDs off word */
#define PCA9685_LED_ON_H        (0x10)      /* LEDs on high byte */
#define PCA9685_LED_OFF_H       (0x10)      /* LEDs off high byte */
#define PCA9685_ALL_LED_ON      (0x1000)    /* All LEDs on word */
#define PCA9685_ALL_LED_OFF     (0x1000)    /* All LEDs off word */
#define PCA9685_ALL_LED_ON_H    (0x10)      /* All LEDs on high byte */
#define PCA9685_ALL_LED_OFF_H   (0x10)      /* All LEDs off word */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* PCA9685_REGS_H */

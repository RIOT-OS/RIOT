/*
 * Copyright (C) 2020 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_pca9633
 * @brief       Register definitions for the PCA9633 I2C PWM controller
 * @author      Hendrik van Essen <hendrik.ve@fu-berlin.de>
 * @file
 * @{
 */


#include "bitarithm.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* Register definitions (page 11, table 7) */

/**
 * @brief Mode register 1
 */
#define PCA9633_REG_MODE1       0x00

/**
 * @brief Mode register 2
 */
#define PCA9633_REG_MODE2       0x01

/**
 * @brief Brightness control LED0
 */
#define PCA9633_REG_PWM0        0x02

/**
 * @brief Brightness control LED1
 */
#define PCA9633_REG_PWM1        0x03

/**
 * @brief Brightness control LED2
 */
#define PCA9633_REG_PWM2        0x04

/**
 * @brief Brightness control LED3
 */
#define PCA9633_REG_PWM3        0x05

/**
 * @brief Group duty cycle control
 */
#define PCA9633_REG_GRPPWM      0x06

/**
 * @brief Group frequency
 */
#define PCA9633_REG_GRPFREQ     0x07

/**
 * @brief LED output state
 */
#define PCA9633_REG_LEDOUT      0x08

/**
 * @brief I2C-bus subaddress 1
 */
#define PCA9633_REG_SUBADR1     0x09

/**
 * @brief I2C-bus subaddress 2
 */
#define PCA9633_REG_SUBADR2     0x0A

/**
 * @brief I2C-bus subaddress 3
 */
#define PCA9633_REG_SUBADR3     0x0B

/**
 * @brief LED All Call I2C-bus address
 */
#define PCA9633_REG_ALLCALLADR  0x0C

/* Bits in REG_MODE1 (page 12, table 8) */

/**
 * @brief Bit for register Auto-Increment
 *        0 = disabled
 *        1 = enabled
 */
#define PCA9633_BIT_AI2     BIT7

/**
 * @brief Bit for Auto-Increment bit1
 */
#define PCA9633_BIT_AI1     BIT6

/**
 * @brief Bit for Auto-Increment bit0
 */
#define PCA9633_BIT_AI0     BIT5

/**
 * @brief 0 = Normal mode
 *        1 = Low power mode. Oscillator off
 */
#define PCA9633_BIT_SLEEP   BIT4

/**
 * @brief 0 = PCA9633 does not respond to I2C-bus subaddress 1
 *        1 = PCA9633 responds to I2C-bus subaddress 1
 */
#define PCA9633_BIT_SUB1    BIT3

/**
 * @brief 0 = PCA9633 does not respond to I2C-bus subaddress 2
 *        1 = PCA9633 responds to I2C-bus subaddress 2
 */
#define PCA9633_BIT_SUB2    BIT2

/**
 * @brief 0 = PCA9633 does not respond to I2C-bus subaddress 3
 *        1 = PCA9633 responds to I2C-bus subaddress 3
 */
#define PCA9633_BIT_SUB3    BIT1

/**
 * @brief 0 = PCA9633 does not respond to LED All Call I2C-bus address
 *        1 = PCA9633 responds to LED All Call I2C-bus address
 */
#define PCA9633_BIT_ALLCALL BIT0

/* Bits in REG_MODE2 (page 12-13, table 9) */

/**
 * @brief Bit for group control; 0=dimming, 1=blinking
 */
#define PCA9633_BIT_DMBLNK      BIT5

/**
 * @brief 0 = Output logic state not inverted. Value to use when no external driver used
 *        1 = Output logic state inverted. Value to use when external driver used
 */
#define PCA9633_BIT_INVRT       BIT4

/**
 * @brief 0 = Outputs change on STOP command
 *        1 = Outputs change on ACK
 */
#define PCA9633_BIT_OCH         BIT3

/**
 * @brief 0 = The 4 LED outputs are configured with an open-drain structure
 *        1 = The 4 LED outputs are configured with a totem pole structure
 */
#define PCA9633_BIT_OUTDRV      BIT2

/**
 * @brief 00 = When OE = 1 (output drivers not enabled), LEDn = 0.
 *        01* = When OE = 1 (output drivers not enabled):
 *           LEDn = 1 when OUTDRV = 1
 *           LEDn = high-impedance when OUTDRV = 0 (same as OUTNE[1:0] = 10)
 *        10 When OE = 1 (output drivers not enabled), LEDn = high-impedance.
 *        11 reserved
 */
#define PCA9633_BITMASK_OUTNE   0x03

/* Bits in REG_LEDOUT (page 14, table 13) */

/**
 * @brief Bitmask for LDR3
 */
#define PCA9633_BITMASK_LDR3    0xC0

/**
 * @brief Bitmask for LDR2
 */
#define PCA9633_BITMASK_LDR2    0x30

/**
 * @brief Bitmask for LDR1
 */
#define PCA9633_BITMASK_LDR1    0x0C

/**
 * @brief Bitmask for LDR0
 */
#define PCA9633_BITMASK_LDR0    0x03

#ifdef __cplusplus
}
#endif

/** @} */

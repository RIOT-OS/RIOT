/*
 * SPDX-FileCopyrightText: 2020 iosabi
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_si1133
 * @brief       Internal addresses, registers, constants for the Si1133 sensors
 *              family.
 * @{
 *
 * @file
 * @brief       Internal addresses, registers, constants for the Si1133 sensor.
 *
 * @author      iosabi <iosabi@protonmail.com>
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Si1133 I2C address
 */
#define SI1133_I2C_ADDRESS        (0x52)  /* or 0x55 */

/**
 * @name Si1133 registers
 * @{
 */
#define SI1133_REG_PART_ID        (0x00)
#define SI1133_REG_HW_ID          (0x01)
#define SI1133_REG_REV_ID         (0x02)
#define SI1133_REG_INFO0          (0x03)
#define SI1133_REG_INFO1          (0x04)
#define SI1133_REG_HOSTIN0        (0x0a)
#define SI1133_REG_COMMAND        (0x0b)
#define SI1133_REG_IRQENABLE      (0x0f)  /* Also RESET in the datasheet.*/
#define SI1133_REG_RESPONSE1      (0x10)
#define SI1133_REG_RESPONSE0      (0x11)
#define SI1133_REG_IRQ_STATUS     (0x12)
#define SI1133_REG_HOSTOUTx       (0x13)  /* Ranges from 0x13 to 0x2c */
/** @} */

/**
 * @name Si1133 commands
 * @{
 */
#define SI1133_CMD_RESET_CMD_CTR  (0x00)
#define SI1133_CMD_RESET_SW       (0x01)
#define SI1133_CMD_FORCE          (0x11)
#define SI1133_CMD_PAUSE          (0x12)
#define SI1133_CMD_START          (0x13)
#define SI1133_CMD_PARAM_QUERY    (0x40)    /* Add to SI1133_PARAM_* */
#define SI1133_CMD_PARAM_SET      (0x80)    /* Add to SI1133_PARAM_* */
/** @} */

/**
 * @brief Si1133 channel parameters.
 *
 * These parameters define how a "channel" is sampled, ADC settings, resolution
 * timing, etc. These four register values define a single channel. This is a
 * convenience struct to handle them together in the same order as they appear
 * in the parameter list below.
 */
typedef struct __attribute__((packed)) _si1133_channel_params {
    uint8_t adcconfig;      /**< ADCCONFIGx register. */
    uint8_t adcsens;        /**< ADCSENSx register. */
    uint8_t adcpost;        /**< ADCPOSTx register. */
    uint8_t measconfig;     /**< MEASCONFIGx register. */
} si1133_channel_params_t;

/**
 * @name Si1133 parameters
 *
 * @note These parameters are not accessible directly from the I2C registers.
 *       Instead, to access these parameters SI1133_CMD_PARAM_QUERY and
 *       SI1133_CMD_PARAM_SET commands should be used.
 * @{
 */
#define SI1133_PARAM_I2C_ADDR     (0x00)
#define SI1133_PARAM_CHAN_LIST    (0x01)
#define SI1133_PARAM_ADCCONFIG0   (0x02)
#define SI1133_PARAM_ADCSENS0     (0x03)
#define SI1133_PARAM_ADCPOST0     (0x04)
#define SI1133_PARAM_MEASCONFIG0  (0x05)
#define SI1133_PARAM_ADCCONFIG1   (0x06)
#define SI1133_PARAM_ADCSENS1     (0x07)
#define SI1133_PARAM_ADCPOST1     (0x08)
#define SI1133_PARAM_MEASCONFIG1  (0x09)
#define SI1133_PARAM_ADCCONFIG2   (0x0a)
#define SI1133_PARAM_ADCSENS2     (0x0b)
#define SI1133_PARAM_ADCPOST2     (0x0c)
#define SI1133_PARAM_MEASCONFIG2  (0x0d)
#define SI1133_PARAM_ADCCONFIG3   (0x0e)
#define SI1133_PARAM_ADCSENS3     (0x0f)
#define SI1133_PARAM_ADCPOST3     (0x10)
#define SI1133_PARAM_MEASCONFIG3  (0x11)
#define SI1133_PARAM_ADCCONFIG4   (0x12)
#define SI1133_PARAM_ADCSENS4     (0x13)
#define SI1133_PARAM_ADCPOST4     (0x14)
#define SI1133_PARAM_MEASCONFIG4  (0x15)
#define SI1133_PARAM_ADCCONFIG5   (0x16)
#define SI1133_PARAM_ADCSENS5     (0x17)
#define SI1133_PARAM_ADCPOST5     (0x18)
#define SI1133_PARAM_MEASCONFIG5  (0x19)
#define SI1133_PARAM_MEASRATE_H   (0x1a)
#define SI1133_PARAM_MEASRATE_L   (0x1b)
#define SI1133_PARAM_MEASCOUNT0   (0x1c)
#define SI1133_PARAM_MEASCOUNT1   (0x1d)
#define SI1133_PARAM_MEASCOUNT2   (0x1e)
#define SI1133_PARAM_THRESHOLD0_H (0x25)
#define SI1133_PARAM_THRESHOLD0_L (0x26)
#define SI1133_PARAM_THRESHOLD1_H (0x27)
#define SI1133_PARAM_THRESHOLD1_L (0x28)
#define SI1133_PARAM_THRESHOLD2_H (0x29)
#define SI1133_PARAM_THRESHOLD2_L (0x2a)
#define SI1133_PARAM_BURST        (0x2b)
/** @} */

/**
 * @name Si1133 RESPONSE0 register constants
 * @{
 */
#define SI1133_RESP0_COUNTER_MASK (0x0f)
#define SI1133_RESP0_CMD_ERR_MASK (0x10)
#define SI1133_RESP0_SLEEP_MASK   (0x20)
#define SI1133_RESP0_SUSPEND_MASK (0x40)
#define SI1133_RESP0_RUNNING_MASK (0x80)

/* Possible error values if SI1133_RESP0_CMD_ERR_MASK is set. */
#define SI1133_RESP0_ERR_INVALID_COMMAND    (0x01)
#define SI1133_RESP0_ERR_INVALID_PARAM_ADDR (0x80)
#define SI1133_RESP0_ERR_ADC_OVERFLOW       (0x88)
#define SI1133_RESP0_ERR_BUFFER_OVERFLOW    (0x89)
/** @} */

/**
 * @name Si1133 Channel configuration constants
 * @{
 */
#define SI1133_ADCCONFIG_DECIM_RATE_MASK    (0x60)
#define SI1133_ADCCONFIG_DECIM_RATE_SHIFT   (5u)
#define SI1133_ADCCONFIG_ADCMUX_MASK        (0x1f)
#define SI1133_ADCCONFIG_ADCMUX_SHIFT       (0u)

#define SI1133_ADCSENS_HSIG_MASK            (0x80)
#define SI1133_ADCSENS_SW_GAIN_MASK         (0x70)
#define SI1133_ADCSENS_SW_GAIN_SHIFT        (4u)
#define SI1133_ADCSENS_HW_GAIN_MASK         (0x0f)
#define SI1133_ADCSENS_HW_GAIN_SHIFT        (0u)

#define SI1133_ADCPOST_24BIT_OUT_MASK       (0x40)
#define SI1133_ADCPOST_POSTSHIFT_MASK       (0x38)
#define SI1133_ADCPOST_POSTSHIFT_SHIFT      (3u)
#define SI1133_ADCPOST_THRESH_SEL_MASK      (0x03)
#define SI1133_ADCPOST_THRESH_SEL_SHIFT     (0u)

#define SI1133_MEASCONFIG_COUNTER_IDX_MASK  (0xc0)
#define SI1133_MEASCONFIG_COUNTER_IDX_SHIFT (6u)
/** @} */

/**
 * @name Si1133 photodiode selection values for ADCMUX field.
 * @{
 */
#define SI1133_ADCMUX_SMALL_IR    (0u)
#define SI1133_ADCMUX_MEDIUM_IR   (1u)
#define SI1133_ADCMUX_LARGE_IR    (2u)
#define SI1133_ADCMUX_WHITE       (11u)
#define SI1133_ADCMUX_LARGE_WHITE (13u)
#define SI1133_ADCMUX_UV          (24u)
#define SI1133_ADCMUX_DEEP_UV     (25u)
/** @} */

/**
 * @name Si1133 constants
 * @{
 */
#define SI1133_ID                              (0x33)
#define SI1133_STARTUP_TIME_MS                 (25u)
#define SI1133_NUM_CHANNELS                    (6u)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */

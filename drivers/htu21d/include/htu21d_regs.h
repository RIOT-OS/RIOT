/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_htu21d
 * @brief       Definitions for HTU21D humidity and temperature sensor
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#ifndef HTU21D_REGS_H
#define HTU21D_REGS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name    Commands
 */
#define HTU21D_TMP_MEAS_WH    0xe3  /**< trigger temperature measurement with hold */
#define HTU21D_HUM_MEAS_WH    0xe5  /**< trigger humidity measurement with hold */
#define HTU21D_TMP_MEAS       0xf3  /**< trigger temperature measurement */
#define HTU21D_HUM_MEAS       0xf5  /**< trigger humidity measurement */
#define HTU21D_WRITE_USER_REG 0xe6  /**< write user register */
#define HTU21D_READ_USER_REG  0xe7  /**< read user register */
#define HTU21D_SOFT_RESET     0xfe  /**< soft reset */

/** User register structure */
typedef union {
    struct {
        uint8_t resolution_0: 1;    /**< measurement resolution bit 0 */
        uint8_t disable_otp : 1;    /**< disable opt_reload */
        uint8_t enable_heat : 1;    /**< enable heater */
        uint8_t reserved    : 3;    /**< reserved */
        uint8_t battery_stat: 1;    /**< status end of battery */
        uint8_t resolution_1: 1;    /**< measurement resolution bit 0 */
    };
    uint8_t val;                    /**< register as 8 bit value */
} htu21d_user_reg_t;

#ifdef __cplusplus
}
#endif

#endif /* HTU21D_REGS_H */

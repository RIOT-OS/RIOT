/*
 * Copyright (C) 2016-2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_nrf52
 * @{
 *
 * @file
 * @brief       Default I2C config for nRF52 based boards
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 */

#ifndef CFG_I2C_DEFAULT_H
#define CFG_I2C_DEFAULT_H

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    I2C configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        .dev = NRF_TWIM1,
        .scl = 27,
        .sda = 26,
        .speed = I2C_SPEED_NORMAL
    },
#ifdef BOARD_NRF52840DK
    {
        .dev = NRF_TWIM0,
        .scl = 28,
        .sda = 29,
        .speed = I2C_SPEED_NORMAL
    }
#endif
};
#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CFG_I2C_DEFAULT_H */
/** @} */

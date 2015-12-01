/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_saul
 * @{
 *
 * @file
 * @brief       Parameter definitions for mapping peripherals directly to SAUL
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef SAUL_PERIPH_H
#define SAUL_PERIPH_H

#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Direct mapped GPIO configuration values
 */
typedef struct {
    const char *name;       /**< name of the device connected to this pin */
    gpio_t pin;             /**< GPIO pin to initialize and expose */
    gpio_dir_t dir;         /**< use GPIO as input or output */
} saul_gpio_params_t;

#ifdef __cplusplus
}
#endif

#endif /* SAUL_PERIPH_H */
/** @} */

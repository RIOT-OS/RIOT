/*
 * SPDX-FileCopyrightText: 2014-2016 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2015 Zolertia SL
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_common_remote
 * @{
 *
 * @file
 * @brief       Common default I2C configuration for the RE-Mote board revision A
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Antonio Lignan <alinan@zolertia.com>
 * @author      Sebastian Meiling <s@mlng.net>
 */

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name I2C configuration
 * @{
 */
#define I2C_IRQ_PRIO            1

static const i2c_conf_t i2c_config[] = {
    {
        .speed = I2C_SPEED_FAST,    /**< bus speed */
        .scl_pin = GPIO_PIN(PORT_C, 3),  /**< GPIO_PC3 */
        .sda_pin = GPIO_PIN(PORT_C, 2)   /**< GPIO_PC2 */
    },
};

#define I2C_NUMOF               ARRAY_SIZE(i2c_config)
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/** @} */

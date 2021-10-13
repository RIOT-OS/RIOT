/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_pca9685
 * @brief       PCA9685 adaption to the RIOT actuator/sensor interface
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 */
#if MODULE_SAUL

#include <string.h>

#include "saul.h"
#include "pca9685.h"

extern pca9685_t pca9685_devs[];

static int set(const void *dev, phydat_t *data)
{
    const pca9685_saul_pwm_params_t *p = (const pca9685_saul_pwm_params_t *)dev;
    pca9685_pwm_set(&pca9685_devs[p->dev], p->channel, (uint16_t)data->val[0]);
    return 1;
}

const saul_driver_t pca9685_pwm_saul_driver = {
    .read = saul_notsup,
    .write = set,
    .type = SAUL_ACT_SERVO
};
#endif /* MODULE_SAUL */

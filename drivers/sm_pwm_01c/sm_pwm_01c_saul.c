/*
 * SPDX-FileCopyrightText: 2021 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_sm_pwm_01c
 * @{
 * @file
 * @brief       SAUL adaption of the SM_PWM_01C dust sensor driver
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "phydat.h"
#include "saul.h"
#include "sm_pwm_01c.h"
#include "sm_pwm_01c_params.h"

static int read_mc_pm_2p5(const void *_dev, phydat_t *data)
{
    sm_pwm_01c_data_t values;
    sm_pwm_01c_t *dev = (sm_pwm_01c_t *)_dev;

    sm_pwm_01c_read_data(dev, &values);
    data->unit = UNIT_GPM3;
    data->scale = -6;
    uint32_t value = values.mc_pm_2p5;
    phydat_fit(data, (int32_t *)&value, 1);
    return 1;
}

static int read_mc_pm_10(const void *_dev, phydat_t *data)
{
    sm_pwm_01c_data_t values;
    sm_pwm_01c_t *dev = (sm_pwm_01c_t *)_dev;

    sm_pwm_01c_read_data(dev, &values);
    data->unit = UNIT_GPM3;
    data->scale = -6;
    uint32_t value = values.mc_pm_10;
    phydat_fit(data, (int32_t *)&value, 1);
    return 1;
}

const saul_driver_t sm_pwm_01c_saul_driver_mc_pm_10 = {
    .read = read_mc_pm_10,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_PM,
};

const saul_driver_t sm_pwm_01c_saul_driver_mc_pm_2p5 = {
    .read = read_mc_pm_2p5,
    .write = saul_write_notsup,
    .type = SAUL_SENSE_PM,
};

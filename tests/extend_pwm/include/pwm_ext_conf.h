/*
 * Copyright (C) 2018 Gunar Schorcht
 * Copyright (C) 2018 Acutam Automation, LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 *
 * @{
 *
 * @file
 * @brief       PWM extension test list
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * @}
 */

#ifndef PWM_EXT_CONF_H
#define PWM_EXT_CONF_H

#include <stddef.h>

#include "extend/pwm.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Reference the driver structs
 *
 * @{
 */
extern pwm_ext_driver_t tests_extend_pwm_driver;
extern pwm_ext_driver_t pwm_ext_notsup_driver;
/** @} */

/**
 * @brief   PWM extension test list
 */
static const pwm_ext_t pwm_ext_list[] =
{
    {
        .driver = &tests_extend_pwm_driver,
        .dev = (void *)0xbeef,
    },
    {
        .driver = &pwm_ext_notsup_driver,
        .dev = NULL,
    },
};

#ifdef __cplusplus
}
#endif

#endif /* PWM_EXT_CONF_H */
/** @} */

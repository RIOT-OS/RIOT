/*
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
 * @brief       GPIO expansion test list of expanders
 *
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * @}
 */

#ifndef GPIO_EXP_CONF_H
#define GPIO_EXP_CONF_H

#include <stddef.h>

#include "gpio_exp.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Reference the driver structs
 *
 * @{
 */
extern gpio_exp_driver_t test_gpio_exp_test_driver;
extern gpio_exp_driver_t test_gpio_exp_notsup_driver;
/** @} */

/**
 * @brief   GPIO expansion test list
 */
static const gpio_exp_t gpio_exp_list[] =
{
    {
        .driver = &test_gpio_exp_test_driver,
        .dev = (void *)0xbeef
    },
    {
        .driver = &test_gpio_exp_notsup_driver,
        .dev = NULL
    }
};

#ifdef __cplusplus
}
#endif

#endif /* GPIO_EXP_CONF_H */
/** @} */

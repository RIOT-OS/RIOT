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
 * @brief       GPIO extension test list
 *
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * @}
 */

#ifndef GPIO_EXT_CONF_H
#define GPIO_EXT_CONF_H

#include <stddef.h>

#include "extend/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Reference the driver structs
 *
 * @{
 */
extern const gpio_ext_driver_t tests_extend_gpio_driver;
extern const gpio_ext_driver_t gpio_ext_notsup_driver;
/** @} */

/**
 * @brief   GPIO extension test list
 */
static const gpio_ext_dev_t gpio_ext[] =
{
    {
        .driver = &tests_extend_gpio_driver,
        .dev = (void *)0xbeef,
    },
    {
        .driver = &gpio_ext_notsup_driver,
        .dev = NULL,
    },
};

#ifdef __cplusplus
}
#endif

#endif /* GPIO_EXT_CONF_H */
/** @} */

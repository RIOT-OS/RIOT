/*
 * Copyright (C) 2018 Acutam Automation, LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_extend_gpio
 *
 * @{
 *
 * @file
 * @brief       GPIO extension default / example list
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
 * @brief   Reference the driver struct
 */
extern gpio_ext_driver_t gpio_ext_notsup_driver;

/**
 * @brief   GPIO expansion default list if not defined
 */
static const gpio_ext_t gpio_ext_list[] =
{
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

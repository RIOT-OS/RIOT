/*
 * Copyright (C) 2018 Acutam Automation, LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_gpio_exp
 *
 * @{
 *
 * @file
 * @brief       GPIO expansion default list of expanders
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
 * @brief   GPIO expansion default notsup driver
 */
static const gpio_exp_driver_t gpio_exp_def_driver = {
    .init = &gpio_exp_init_notsup,
    .init_int = &gpio_exp_init_int_notsup,
    .irq = &gpio_exp_irq_notsup,
    .read = &gpio_exp_read_notsup,
    .write = &gpio_exp_write_notsup
};

/**
 * @brief   GPIO expansion default list if not defined
 */
static const gpio_exp_t gpio_exp_list[] =
{
    {
        .driver = &gpio_exp_def_driver,
        .dev = NULL
    }
};

#ifdef __cplusplus
}
#endif

#endif /* GPIO_EXP_CONF_H */
/** @} */

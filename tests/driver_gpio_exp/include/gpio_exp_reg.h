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
 * @brief       GPIO expansion test registry
 *
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * @}
 */

#ifndef GPIO_EXP_REG_H
#define GPIO_EXP_REG_H

#include <stddef.h>

#include "gpio_exp.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   GPIO expansion test functions
 */
int test_gpio_exp_init(void *dev, gpio_t pin, gpio_mode_t mode);
int test_gpio_exp_init_int(void *dev, gpio_t pin, gpio_mode_t mode,
                           gpio_flank_t flank, gpio_cb_t cb, void *arg);
void test_gpio_exp_irq(void *dev, gpio_t pin, int state);
int test_gpio_exp_read(void *dev, gpio_t pin);
void test_gpio_exp_write(void *dev, gpio_t pin, int value);

/**
 * @brief   GPIO expansion test drivers
 */
static const gpio_exp_driver_t test_gpio_exp_driver0 = {
    .init = &test_gpio_exp_init,
    .init_int = &test_gpio_exp_init_int,
    .irq = &test_gpio_exp_irq,
    .read = &test_gpio_exp_read,
    .write = &test_gpio_exp_write
};
static const gpio_exp_driver_t test_gpio_exp_driver1 = {
    .init = &gpio_exp_init_notsup,
    .init_int = &gpio_exp_init_int_notsup,
    .irq = &gpio_exp_irq_notsup,
    .read = &gpio_exp_read_notsup,
    .write = &gpio_exp_write_notsup
};

/**
 * @brief   GPIO expansion test registry
 */
static const gpio_exp_t gpio_exp_reg[] =
{
    {
        .driver = &test_gpio_exp_driver0,
        .dev = (void *)0xbeef
    },
    {
        .driver = &test_gpio_exp_driver1,
        .dev = NULL
    }
};

#ifdef __cplusplus
}
#endif

#endif /* GPIO_EXP_REG_H */
/** @} */

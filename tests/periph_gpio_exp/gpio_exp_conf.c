/*
 * Copyright (C) 2020 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests_periph_gpio_exp
 * @{
 *
 * @file
 * @brief       Example GPIO expander configuration
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#include "gpio_exp_conf.h"

/**
 * @brief   Definition of example GPIO expander devices
 */
foo_gpio_exp_t foo_gpio_exp_1 = { .name = "foo1" };
foo_gpio_exp_t foo_gpio_exp_2 = { .name = "foo2" };

bar_exp_t bar_exp = { .name = "bar" };
bar_gpio_exp_t bar_gpio_exp_1 = { .dev = &bar_exp, .port = 0 };
bar_gpio_exp_t bar_gpio_exp_2 = { .dev = &bar_exp, .port = 1 };
bar_gpio_exp_t bar_gpio_exp_3 = { .dev = &bar_exp, .port = 2 };
bar_gpio_exp_t bar_gpio_exp_4 = { .dev = &bar_exp, .port = 3 };

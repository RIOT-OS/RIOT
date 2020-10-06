/*
 * Copyright (C) 2020 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests_periph_gpio_exp
 *
 * @{
 *
 * @file
 * @brief       Example GPIO expander configuration
 *
 * The configuration of GPIO expanders is either part of the board definition
 * or of the application. In the latter case, the application's `Makefile` must
 * add the according include PATH for the configuration before it includes the
 * default `$(RIOTBASE)/Makefile.include`.
 * ```
 * INCLUDES += -I$(APPDIR)
 * ```
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef GPIO_EXP_CONF_H
#define GPIO_EXP_CONF_H

#include <stddef.h>

#include "periph/gpio.h"

#include "foo_gpio_exp.h" /* include header files of GPIO expander drivers */
#include "bar_gpio_exp.h" /* include header files of GPIO expander drivers */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   Defined the expander device ports
 * @{
 */
extern foo_gpio_exp_t foo_gpio_exp_1; /**< Single port GPIO expander 1 */
extern foo_gpio_exp_t foo_gpio_exp_2; /**< Single port GPIO expander 2 */
extern bar_gpio_exp_t bar_gpio_exp_1; /**< Multiple port GPIO expander, Port0 */
extern bar_gpio_exp_t bar_gpio_exp_2; /**< Multiple port GPIO expander, Port1 */
extern bar_gpio_exp_t bar_gpio_exp_3; /**< Multiple port GPIO expander, Port2 */
extern bar_gpio_exp_t bar_gpio_exp_4; /**< Multiple port GPIO expander, Port3 */
/** @} */

/**
 * @brief   GPIO expansion default list if not defined
 */
#define GPIO_EXP_PORTS \
    { .dev = &foo_gpio_exp_1, .driver = &foo_gpio_exp_driver }, \
    { .dev = &foo_gpio_exp_2, .driver = &foo_gpio_exp_driver }, \
    { .dev = &bar_gpio_exp_1, .driver = &bar_gpio_exp_driver }, \
    { .dev = &bar_gpio_exp_2, .driver = &bar_gpio_exp_driver }, \
    { .dev = &bar_gpio_exp_3, .driver = &bar_gpio_exp_driver }, \
    { .dev = &bar_gpio_exp_4, .driver = &bar_gpio_exp_driver },

#ifdef __cplusplus
}
#endif

#endif /* GPIO_EXP_CONF_H */
/** @} */

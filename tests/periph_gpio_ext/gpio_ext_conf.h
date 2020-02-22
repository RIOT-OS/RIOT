/*
 * Copyright (C) 2020 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests_periph_gpio_ext
 * @{
 *
 * @file
 * @brief       Example GPIO extender configuration
 *
 * The configuration of GPIO extenders is either part of the board definition
 * or of the application. In the latter case, the application's `Makefile` must
 * add the according include PATH for the configuration before it includes the
 * default `$(RIOTBASE)/Makefile.include`.
 * ```
 * INCLUDES += -I$(APPDIR)
 * ```
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef GPIO_EXT_CONF_H
#define GPIO_EXT_CONF_H

#include "periph/gpio.h"

#include "foo_gpio_ext.h" /* include header of `foo` GPIO extender driver */
#include "bar_gpio_ext.h" /* include header of `bar` GPIO extender driver */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Declaration of example GPIO extender devices
 */
extern foo_gpio_ext_t foo_gpio_ext_1;
extern foo_gpio_ext_t foo_gpio_ext_2;
extern bar_gpio_ext_t bar_gpio_ext;

/**
 * @brief   Example GPIO extender configuration
 */
#define GPIO_EXT_PORTS \
    { .port.dev = &foo_gpio_ext_1, .driver = &foo_gpio_ext_driver }, \
    { .port.dev = &foo_gpio_ext_2, .driver = &foo_gpio_ext_driver }, \
    { .port.dev = &bar_gpio_ext  , .driver = &bar_gpio_ext_driver },

#ifdef __cplusplus
}
#endif

#endif /* GPIO_EXT_CONF_H */
/** @} */

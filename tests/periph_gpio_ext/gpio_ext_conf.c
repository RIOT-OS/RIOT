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

#ifdef MODULE_EXTEND_GPIO

#include "gpio_ext_conf.h"

/**
 * @brief   Definition of example GPIO extender devices
 */
foo_gpio_ext_t foo_gpio_ext_1 = { .name = "foo1" };
foo_gpio_ext_t foo_gpio_ext_2 = { .name = "foo2" };
bar_gpio_ext_t bar_gpio_ext   = { .name = "bar" };

#endif /* MODULE_EXTEND_GPIO */

/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @brief       GPIO extension for Microchip MCP23x17 I/O expanders
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 */

#ifndef GPIO_EXT_CONF_H
#define GPIO_EXT_CONF_H

#if MODULE_EXTEND_GPIO

#include <stddef.h>

#include "extend/gpio.h"
#include "mcp23x17_params.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Reference to MCP23x17 device driver struct
 */
extern const gpio_ext_driver_t mcp23x17_extend_gpio_driver;
/** @} */

/**
 * @brief   References to the MCP23x17 devices
 */
extern mcp23x17_t mcp23x17_dev[];

/**
 * @brief   GPIO extension list of MCP23x17 expanders
 */
static const gpio_ext_t gpio_ext_list[] =
{
    {
        .driver = &mcp23x17_extend_gpio_driver,
        .dev = (void *)&mcp23x17_dev[0],
    },
};

#ifdef __cplusplus
}
#endif

#endif /* MODULE_EXTEND_GPIO */

#endif /* GPIO_EXT_CONF_H */
/** @} */

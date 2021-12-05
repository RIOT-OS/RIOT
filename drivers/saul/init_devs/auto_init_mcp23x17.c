/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mcp23x17
 * @ingroup     sys_auto_init_saul
 * @brief       Auto initialization for Microchip MCP23x17 I/O expanders
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#if MODULE_MCP23X17 && MODULE_SAUL_GPIO

#include "assert.h"
#include "log.h"

#include "saul_reg.h"
#include "saul/periph.h"

#include "mcp23x17.h"
#include "mcp23x17_params.h"

/**
 * @brief   Number of configured MCP23x17 I/O expander devices
 */
#define MCP23X17_NUM                ARRAY_SIZE(mcp23x17_params)

/**
 * @brief   Number of configured SAUL MCP23x17 I/O pins
 */
#define MCP23X17_SAUL_GPIO_NUMOF    ARRAY_SIZE(mcp23x17_saul_gpio_params)

/**
 * @brief   Number of saul info
 */
#define MCP23X17_INFO_NUM           ARRAY_SIZE(mcp23x17_saul_info)

/**
 * @brief   Allocate the memory for the MCP23x17 I/O expander device descriptors
 */
mcp23x17_t mcp23x17_devs[MCP23X17_NUM];

/**
 * @brief   Allocate the memory for MCP23x17 I/O expander SAUL registry entries
 */
static saul_reg_t mcp23x17_saul_reg_entries[MCP23X17_SAUL_GPIO_NUMOF];

/**
 * @brief   Reference the MCP23x17 I/O expander input mode driver struct
 */
extern saul_driver_t mcp23x17_gpio_in_saul_driver;

/**
 * @brief   Reference to the MCP23x17 I/O expander output mode driver struct
 */
extern saul_driver_t mcp23x17_gpio_out_saul_driver;

void auto_init_mcp23x17(void)
{
    for (unsigned int i = 0; i < MCP23X17_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing MCP23x17 I/O expander dev #%u\n", i);
        mcp23x17_init(&mcp23x17_devs[i], &mcp23x17_params[i]);
    }

    for (unsigned int i = 0; i < MCP23X17_SAUL_GPIO_NUMOF; i++) {
        const mcp23x17_saul_gpio_params_t *p = &mcp23x17_saul_gpio_params[i];

        LOG_DEBUG("[auto_init_saul] initializing MCP23x17 GPIO #%u\n", i);

        /* check the MCP23x17 device index */
        assert(p->dev < MCP23X17_NUM);
        /* check the MCP23x17 device index */
        assert(p->gpio.pin < MCP23X17_GPIO_PIN_NUM);

        mcp23x17_saul_reg_entries[i].dev = (void *)p;
        mcp23x17_saul_reg_entries[i].name = p->gpio.name;
        if ((p->gpio.mode == GPIO_IN) ||
            (p->gpio.mode == GPIO_IN_PD) ||
            (p->gpio.mode == GPIO_IN_PU)) {
            mcp23x17_saul_reg_entries[i].driver = &mcp23x17_gpio_in_saul_driver;
        }
        else {
            mcp23x17_saul_reg_entries[i].driver = &mcp23x17_gpio_out_saul_driver;
        }
        /* initialize the MCP23x17 pin */
        mcp23x17_gpio_init(&mcp23x17_devs[p->dev], p->gpio.pin, p->gpio.mode);
        /* set initial pin MCP23x17 state if configured */
        if (p->gpio.flags & (SAUL_GPIO_INIT_CLEAR | SAUL_GPIO_INIT_SET)) {
            phydat_t s;
            s.val[0] = (p->gpio.flags & SAUL_GPIO_INIT_SET);
            mcp23x17_saul_reg_entries[i].driver->write(p, &s);
        }
        /* add to registry */
        saul_reg_add(&(mcp23x17_saul_reg_entries[i]));
    }
}
#else
typedef int dont_be_pedantic;
#endif /* MODULE_MCP23x17 && MODULE_SAUL_GPIO */

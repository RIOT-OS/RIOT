/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_pcf857x
 * @ingroup     sys_auto_init_saul
 * @brief       Auto initialization of Texas Instruments PCF857X I2C I/O expanders
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#if MODULE_PCF857X && MODULE_SAUL_GPIO

#include "assert.h"
#include "log.h"

#include "saul_reg.h"
#include "saul/periph.h"

#include "pcf857x.h"
#include "pcf857x_params.h"

/**
 * @brief   Number of configured PCF857X I/O expander devices
 */
#define PCF857X_NUM             ARRAY_SIZE(pcf857x_params)

/**
 * @brief   Number of configured SAUL PCF857X I/O pins
 */
#define PCF857X_SAUL_GPIO_NUMOF ARRAY_SIZE(pcf857x_saul_gpio_params)

/**
 * @brief   Number of saul info
 */
#define PCF857X_INFO_NUM        ARRAY_SIZE(pcf857x_saul_info)

/**
 * @brief   Allocate the memory for the PCF857X I/O expander device descriptors
 */
pcf857x_t pcf857x_devs[PCF857X_NUM];

/**
 * @brief   Allocate the memory for PCF857X I/O expander SAUL registry entries
 */
static saul_reg_t pcf857x_saul_reg_entries[PCF857X_SAUL_GPIO_NUMOF];

/**
 * @brief   Reference the PCF857X I/O expander input mode driver struct
 */
extern saul_driver_t pcf857x_gpio_in_saul_driver;

/**
 * @brief   Reference to the PCF857X I/O expander output mode driver struct
 */
extern saul_driver_t pcf857x_gpio_out_saul_driver;

void auto_init_pcf857x(void)
{
    for (unsigned int i = 0; i < PCF857X_NUM; i++) {
        LOG_DEBUG("[auto_init_saul] initializing PCF857X I/O expander dev #%u\n", i);
        pcf857x_init(&pcf857x_devs[i], &pcf857x_params[i]);
    }

    for (unsigned int i = 0; i < PCF857X_SAUL_GPIO_NUMOF; i++) {
        const pcf857x_saul_gpio_params_t *p = &pcf857x_saul_gpio_params[i];

        LOG_DEBUG("[auto_init_saul] initializing PCF857X GPIO #%u\n", i);

        /* check the PCF857X device index */
        assert(p->dev < PCF857X_NUM);
        /* check the PCF857X device index */
        assert(p->gpio.pin < PCF857X_GPIO_PIN_NUM);

        pcf857x_saul_reg_entries[i].dev = (void *)p;
        pcf857x_saul_reg_entries[i].name = p->gpio.name;
        if ((p->gpio.mode == GPIO_IN) ||
            (p->gpio.mode == GPIO_IN_PD) ||
            (p->gpio.mode == GPIO_IN_PU)) {
            pcf857x_saul_reg_entries[i].driver = &pcf857x_gpio_in_saul_driver;
        }
        else {
            pcf857x_saul_reg_entries[i].driver = &pcf857x_gpio_out_saul_driver;
        }
        /* initialize the PCF857X pin */
        pcf857x_gpio_init(&pcf857x_devs[p->dev], p->gpio.pin, p->gpio.mode);
        /* set initial pin PCF857X state if configured */
        if (p->gpio.flags & (SAUL_GPIO_INIT_CLEAR | SAUL_GPIO_INIT_SET)) {
            phydat_t s;
            s.val[0] = (p->gpio.flags & SAUL_GPIO_INIT_SET);
            pcf857x_saul_reg_entries[i].driver->write(p, &s);
        }
        /* add to registry */
        saul_reg_add(&(pcf857x_saul_reg_entries[i]));
    }
}
#else
typedef int dont_be_pedantic;
#endif /* MODULE_PCF857X && MODULE_SAUL_GPIO */

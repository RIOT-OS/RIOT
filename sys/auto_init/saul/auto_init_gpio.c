/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/*
 * @ingroup     auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of GPIO pins directly mapped to SAUL reg
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#ifdef MODULE_SAUL_GPIO

#include "log.h"
#include "saul_reg.h"
#include "saul/periph.h"
#include "gpio_params.h"
#include "periph/gpio.h"

/**
 * @brief   Define the number of configured sensors
 */
#define SAUL_GPIO_NUMOF    (sizeof(saul_gpio_params)/sizeof(saul_gpio_params[0]))

/**
 * @brief   Allocate memory for the device descriptors
 */
static gpio_t saul_gpios[SAUL_GPIO_NUMOF];

/**
 * @brief   Memory for the registry entries
 */
static saul_reg_t saul_reg_entries[SAUL_GPIO_NUMOF];

void auto_init_gpio(void)
{
    for (unsigned int i = 0; i < SAUL_GPIO_NUMOF; i++) {
        const saul_gpio_params_t *p = &saul_gpio_params[i];

        LOG_DEBUG("[auto_init_saul] initializing GPIO #%u\n", i);

        saul_gpios[i] = p->pin;
        saul_reg_entries[i].dev = &(saul_gpios[i]);
        saul_reg_entries[i].name = p->name;
        /* set correct driver */
        switch (p->type) {
            case SAUL_GPIO_IN:
                saul_reg_entries[i].driver = &gpio_in_saul_driver;
                break;
            case SAUL_GPIO_IN_INV:
                saul_reg_entries[i].driver = &gpio_in_inv_saul_driver;
                break;
            case SAUL_GPIO_OUT:
                saul_reg_entries[i].driver = &gpio_out_saul_driver;
                break;
            case SAUL_GPIO_OUT_INV:
                saul_reg_entries[i].driver = &gpio_out_inv_saul_driver;
                break;
        }
        /* initialize the GPIO pin */
        gpio_init(p->pin, p->mode);
        /* set initial pin state if configured */
        if (p->state != SAUL_GPIO_SKIP) {
            phydat_t s;
            s.val[0] = p->state;
            saul_reg_entries[i].driver->write(saul_reg_entries[i].dev, &s);
        }
        /* add to registry */
        saul_reg_add(&(saul_reg_entries[i]));
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_SAUL_GPIO */

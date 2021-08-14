/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/*
 * @ingroup     sys_auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of GPIO pins directly mapped to SAUL reg
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "log.h"
#include "saul_reg.h"
#include "saul/periph.h"
#include "gpio_params.h"
#include "periph/gpio.h"
#include "kernel_defines.h"

/**
 * @brief   Define the number of configured sensors
 */
#if defined(SAUL_GPIO_NUMOF) && !(SAUL_GPIO_NUMOF > 0)
void auto_init_gpio(void)
{
    /* do nothing, no GPIO configured for SAUL */
    LOG_DEBUG("[auto_init_saul] no SAUL GPIO configured!\n");
}
#else
#define SAUL_GPIO_NUMOF ARRAY_SIZE(saul_gpio_params)

/**
 * @brief   Memory for the registry entries
 */
static saul_reg_t saul_reg_entries[SAUL_GPIO_NUMOF];

/**
 * @brief   Reference the input mode driver struct
 */
extern saul_driver_t gpio_in_saul_driver;

/**
 * @brief   Reference to the output mode driver struct
 */
extern saul_driver_t gpio_out_saul_driver;

void auto_init_gpio(void)
{
    for (unsigned int i = 0; i < SAUL_GPIO_NUMOF; i++) {
        const saul_gpio_params_t *p = &saul_gpio_params[i];

        LOG_DEBUG("[auto_init_saul] initializing GPIO #%u\n", i);

        saul_reg_entries[i].dev = (void *)p;
        saul_reg_entries[i].name = p->name;
        if ((p->mode == GPIO_IN) || (p->mode == GPIO_IN_PD) ||
            (p->mode == GPIO_IN_PU)) {
            saul_reg_entries[i].driver = &gpio_in_saul_driver;
        }
        else {
            saul_reg_entries[i].driver = &gpio_out_saul_driver;
        }
        /* initialize the GPIO pin */
        gpio_init(p->pin, p->mode);
        /* set initial pin state if configured */
        if (p->flags & (SAUL_GPIO_INIT_CLEAR | SAUL_GPIO_INIT_SET)) {
            phydat_t s;
            s.val[0] = (p->flags & SAUL_GPIO_INIT_SET);
            saul_reg_entries[i].driver->write(p, &s);
        }
        /* add to registry */
        saul_reg_add(&(saul_reg_entries[i]));
    }
}
#endif

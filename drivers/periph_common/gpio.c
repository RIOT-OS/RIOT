/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_periph_gpio
 * @{
 *
 * @file
 * @brief       Common GPIO driver functions/definitions
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#ifdef MODULE_PERIPH_GPIO_EXP

#include "periph_cpu.h"
#include "periph/gpio.h"

#ifdef MODULE_GPIO_EXP
#include "gpio_exp_conf.h"
#endif

/* the CPU low level GPIO driver */
const gpio_driver_t gpio_cpu_driver = {
    .init = (gpio_dev_init_t)gpio_cpu_init,
#ifdef MODULE_PERIPH_GPIO_IRQ
    .init_int = (gpio_dev_init_int_t)gpio_cpu_init_int,
    .irq_enable = (gpio_dev_irq_enable_t)gpio_cpu_irq_enable,
    .irq_disable = (gpio_dev_irq_disable_t)gpio_cpu_irq_disable,
#endif /* MODULE_PERIPH_GPIO_IRQ */
    .read = (gpio_dev_read_t)gpio_cpu_read,
    .set = (gpio_dev_set_t)gpio_cpu_set,
    .clear = (gpio_dev_clear_t)gpio_cpu_clear,
    .toggle = (gpio_dev_toggle_t)gpio_cpu_toggle,
    .write = (gpio_dev_write_t)gpio_cpu_write,
};

#ifdef MODULE_GPIO_EXP
const gpio_dev_t gpio_devs[] = {
#ifdef GPIO_EXP_PORTS
    GPIO_EXP_PORTS
#endif
};

#endif /* MODULE_GPIO_EXP */

int gpio_port_numof(void)
{
#ifdef MODULE_GPIO_EXP
    return GPIO_EXP_PORT + ARRAY_SIZE(gpio_devs);
#else
    return GPIO_EXP_PORT;
#endif
}

int gpio_port_num(gpio_t gpio)
{
#ifdef MODULE_GPIO_EXP
    if (GPIO_CPU_PORT_IS(gpio.port)) {
        return GPIO_CPU_PORT_NUM(gpio.port.reg);
    }
#ifdef GPIO_EXP_PORTS
    for (unsigned i = 0; i < ARRAY_SIZE(gpio_devs); i++) {
        if (gpio.port.dev == gpio_devs[i].dev) {
            return i;
        }
    }
#endif /* GPIO_EXP_PORTS */
    return -1;
#else
    return GPIO_CPU_PORT_NUM(gpio.port.reg);
#endif /* MODULE_GPIO_EXP */
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_PERIPH_GPIO_EXP */

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

#ifdef MODULE_PERIPH_GPIO_EXT

#include "periph_cpu.h"
#include "periph/gpio.h"

#ifdef MODULE_EXTEND_GPIO
#include "gpio_ext_conf.h"
#endif

#ifdef MODULE_EXTEND_GPIO
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
#endif /* MODULE_EXTEND_GPIO */

const gpio_port_table_t gpio_ports[] = {
#if defined(MODULE_PERIPH_GPIO)
    GPIO_CPU_PORTS
#endif
#if defined(MODULE_EXTEND_GPIO) && defined(GPIO_EXT_PORTS)
    GPIO_EXT_PORTS
#endif
};

int gpio_port_numof(void)
{
    return ARRAY_SIZE(gpio_ports);
}

int gpio_port_num(gpio_t gpio)
{
    for (unsigned i = 0; i < ARRAY_SIZE(gpio_ports); i++) {
        if (GPIO_PORT(gpio).dev == gpio_ports[i].port.dev) {
            return i;
        }
    }
    return 0;
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_PERIPH_GPIO_EXT */

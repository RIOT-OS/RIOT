/*
 * Copyright (C) 2015 Loci Controls Inc.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup cpu_cc2538
 * @{
 *
 * @file            cc2538-gpio.c
 * @brief           cc2538 GPIO controller driver implementation
 *
 * @author          Ian Martin <ian@locicontrols.com>
 * @{
 */

#include "cc2538_gpio.h"

void cc2538_gpio_port_init(cc2538_gpio_t* gpio_port) {
    /* Disable power-up interrupts */
    gpio_port->PI_IEN = 0;

    /* Disable normal interrupts */
    gpio_port->IE = 0;

    /* Clear any pending interrupts */
    gpio_port->IC = 0xff;
}

void cc2538_gpio_init(void) {
    cc2538_gpio_port_init(GPIO_A);
    cc2538_gpio_port_init(GPIO_B);
    cc2538_gpio_port_init(GPIO_C);
    cc2538_gpio_port_init(GPIO_D);
}

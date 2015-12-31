/*
 * Copyright (C) 2015 Dinh Nguyen <nqdinhddt@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cc3200
 * @{
 *
 * @file        cc3200_gpio.c
 * @brief       Implementation of the GPIO control
 *
 * @author      Dinh Nguyen <nqdinhddt@gmail.com>
 * @}
 */

#include "cpu.h"

void gpio_set(uint32_t gpio, uint32_t value)
{
    uint32_t ulReg[]= {
        GPIOA0_BASE,
        GPIOA1_BASE,
        GPIOA2_BASE,
        GPIOA3_BASE,
        GPIOA4_BASE
    };
    uint32_t pin = (1 << (gpio % PINS_IN_PORT));
    uint32_t port = (ulReg[gpio / PINS_IN_PORT]);
    
    MAP_GPIOPinWrite(port, pin, (value << (gpio % PINS_IN_PORT)));
}
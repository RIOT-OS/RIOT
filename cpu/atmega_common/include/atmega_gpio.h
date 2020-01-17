/*
 * Copyright (C) 2015 HAW Hamburg
 *               2016 INRIA

 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atmega_common
 * @ingroup     drivers_periph_gpio
 * @{
 *
 * @file
 * @brief       Macros and inline functions for accessing GPIOs of the ATmega
 *              family
 *
 * @author      René Herthel <rene-herthel@outlook.de>
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 * @author      Laurent Navet <laurent.navet@gmail.com>
 */

#ifndef ATMEGA_GPIO_H
#define ATMEGA_GPIO_H
#include <stdio.h>

#include <avr/interrupt.h>

#include "cpu.h"
#include "board.h"
#include "periph/gpio.h"
#include "periph_conf.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ATMEGA_GPIO_BASE_PORT_A     (0x20)
#define ATMEGA_GPIO_OFFSET_PORT_H   (0xCB)
#define ATMEGA_GPIO_OFFSET_PIN_PORT (0x02)
#define ATMEGA_GPIO_OFFSET_PIN_PIN  (0x03)


/**
 * @brief     Extract the pin number of the given pin
 */
static inline uint8_t atmega_pin_num(gpio_pin_t pin)
{
    return (pin & 0x07);
}

/**
 * @brief     Generate the PORTx address of the give port.
 */
static inline gpio_reg_t atmega_port_addr(gpio_port_t port)
{
    return port.reg;
}

/**
 * @brief     Generate the DDRx address of the given port.
 */
static inline gpio_reg_t atmega_ddr_addr(gpio_port_t port)
{
    return (atmega_port_addr(port) - 0x01);
}

/**
 * @brief     Generate the PINx address of the given port.
 */
static inline gpio_reg_t atmega_pin_addr(gpio_port_t port)
{
    return (atmega_port_addr(port) - 0x02);
}

/**
 * @brief     Generate the port number of the given port
 */
static inline uint8_t atmega_port_num(gpio_port_t port)
{
    gpio_reg_t port_addr = atmega_port_addr(port);

#ifdef PORTG
    if (port_addr > ATMEGA_GPIO_OFFSET_PORT_H) {
        port_addr -= ATMEGA_GPIO_OFFSET_PORT_H;
    }
#endif
    port_addr -= ATMEGA_GPIO_OFFSET_PIN_PORT;
    port_addr -= ATMEGA_GPIO_BASE_PORT_A;

    return port_addr / ATMEGA_GPIO_OFFSET_PIN_PIN;
}

#ifdef __cplusplus
}
#endif

#endif /* ATMEGA_GPIO_H */
/** @} */

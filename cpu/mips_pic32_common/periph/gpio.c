/*
 * Copyright(C) 2017 Imagination Technologies Limited and/or its
 *              affiliated group companies.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

 /**
  * @ingroup     cpu_mips_pic32_common
  * @ingroup     drivers_periph_gpio
  * @{
  *
  * @file
  * @brief       Low-level GPIO driver implementation
  *
  * @}
  */

#include "cpu.h"
#include "periph/gpio.h"
#include "periph_conf.h"

#define LATx(P)         ((P)[0x30/0x4])
#define LATxCLR(P)      ((P)[0x34/0x4])
#define LATxSET(P)      ((P)[0x38/0x4])
#define LATxINV(P)      ((P)[0x3C/0x4])
#define PORTx(P)        ((P)[0x20/0x4])
#define CNPUxCLR(P)     ((P)[0x54/0x4])
#define CNPUxSET(P)     ((P)[0x58/0x4])
#define CNPDxCLR(P)     ((P)[0x64/0x4])
#define CNPDxSET(P)     ((P)[0x68/0x4])
#define ODCxCLR(P)      ((P)[0x44/0x4])
#define ODCxSET(P)      ((P)[0x48/0x4])
#define ANSELxCLR(P)    ((P)[0x04/0x4])
#define TRISxCLR(P)     ((P)[0x14/0x4])
#define TRISxSET(P)     ((P)[0x18/0x4])

/**
 * @brief   Extract the port base address from the given pin identifier
 */
static inline volatile unsigned int * _port(gpio_t pin)
{
    return (volatile unsigned int *)(pin & ~(0xff));
}

/**
 * @brief   Extract the port number form the given identifier
 *
 * The port number is extracted by looking at bits 8, 9, 10, 11 of the base
 * register addresses.
 */
static inline int _port_num(gpio_t pin)
{
    return ((pin >> 8) & 0x0f);
}

/**
 * @brief   Extract the pin number from the last 4 bit of the pin identifier
 */
static inline int _pin_num(gpio_t pin)
{
    return (pin & 0x0f);
}

int gpio_init(gpio_t pin, gpio_mode_t mode)
{
    volatile unsigned int * port = _port(pin);
    int pin_num = _pin_num(pin);
    uint8_t output = 0, pu = 0, pd = 0, od = 0;

    switch (mode) {
        case GPIO_IN:
            break;
        case GPIO_IN_PD:
            pd = 1;
            break;
        case GPIO_IN_PU:
            pu = 1;
            break;
        case GPIO_OD_PU:
            pu = 1;
        case GPIO_OD:
            od = 1;
        case GPIO_OUT:
            output = 1;
            break;
    }

    ANSELxCLR(port) = 1U << pin_num;       /* Configure GPIO as digital */

    if (pu)
        CNPUxSET(port) = 1U << pin_num;
    else
        CNPUxCLR(port) = 1U << pin_num;

    if (pd)
        CNPDxSET(port) = 1U << pin_num;
    else
        CNPDxCLR(port) = 1U << pin_num;

    if (od)
        ODCxSET(port) = 1U << pin_num;
    else
        ODCxCLR(port) = 1U << pin_num;

    if (output)
        TRISxCLR(port) = 1U << pin_num;
    else
        TRISxSET(port) = 1U << pin_num;

    return 0;
}

int gpio_read(gpio_t pin)
{
    return PORTx(_port(pin)) & (1U << _pin_num(pin));
}

void gpio_set(gpio_t pin)
{
    LATxSET(_port(pin)) = 1U << _pin_num(pin);
}

void gpio_clear(gpio_t pin)
{
    LATxCLR(_port(pin)) = 1U << _pin_num(pin);
}

void gpio_toggle(gpio_t pin)
{
    LATxINV(_port(pin)) = 1U << _pin_num(pin);
}

void gpio_write(gpio_t pin, int value)
{
    if (value)
        gpio_set(pin);
    else
        gpio_clear(pin);
}

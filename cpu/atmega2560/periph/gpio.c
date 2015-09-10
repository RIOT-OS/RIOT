/*
 * Copyright (C) 2015 HAW Hamburg

 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_periph
 * @{
 *
 * @file
 * @brief       Low-level GPIO driver implementation for ATmega2560
 *
 * @author      René Herthel <rene-herthel@outlook.de>
 *
 * @}
 */


#include <stdio.h>

#include <avr/interrupt.h>

#include "cpu.h"
#include "periph/gpio.h"
#include "periph_conf.h"

#define GPIO_BASE_PORT_A        (0x20)
#define GPIO_OFFSET_PORT_H      (0xCB)
#define GPIO_OFFSET_PIN_PORT    (0x02)
#define GPIO_OFFSET_PIN_PIN     (0x03)
#define GPIO_EXT_INT_NUMOF      (7U)

typedef struct {
    gpio_cb_t cb;
    void *arg;
} gpio_state_t;

static gpio_state_t config[GPIO_EXT_INT_NUMOF];

/**
 * @brief     Extract the pin number of the given pin
 */
static inline uint8_t _pin_num(gpio_t pin)
{
    return (pin & 0x0f);
}

/**
 * @brief     Extract the port number of the given pin
 */
static inline uint8_t _port_num(gpio_t pin)
{
    return (pin >> 4) & 0x0f;
}

/**
 * @brief     Generate the PORTx address of the give pin.
 */
static inline uint16_t _port_addr(gpio_t pin)
{
    uint8_t port_num = _port_num(pin);
    uint16_t port_addr = port_num * GPIO_OFFSET_PIN_PIN;

    port_addr += GPIO_BASE_PORT_A;
    port_addr += GPIO_OFFSET_PIN_PORT;

    if (port_num > PORT_G) {
        port_addr += GPIO_OFFSET_PORT_H;
    }

    return port_addr;
}

/**
 * @brief     Generate the DDRx address of the given pin
 */
static inline uint8_t _ddr_addr(gpio_t pin)
{
    return (_port_addr(pin) - 0x01);
}

/**
 * @brief     Generate the PINx address of the given pin.
 */
static inline uint8_t _pin_addr(gpio_t pin)
{
    return (_port_addr(pin) - 0x02);
}

int gpio_init(gpio_t pin, gpio_dir_t dir, gpio_pp_t pullup)
{
    int res;

    if (dir == GPIO_DIR_OUT) {
        _SFR_MEM8(_ddr_addr(pin)) |= (1 << _pin_num(pin));
        res = bit_is_set(_SFR_MEM8(_ddr_addr(pin)), _pin_num(pin));
    }
    else {
        _SFR_MEM8(_ddr_addr(pin)) &= ~(1 << _pin_num(pin));
        res = bit_is_clear(_SFR_MEM8(_ddr_addr(pin)), _pin_num(pin));
    }

    return (res == 0) ? -1 : 0;
}

int gpio_init_int(gpio_t pin, gpio_pp_t pullup, gpio_flank_t flank,
                  gpio_cb_t cb, void *arg)
{
    uint8_t pin_num = _pin_num(pin);

    if ((_port_num(pin) == PORT_D && pin_num > 3)
         || (_port_num(pin) == PORT_E && pin_num < 4)) {
        return -1;
    }

    if (gpio_init(pin, GPIO_DIR_IN, pullup) < 0) {
        return -1;
    }

    gpio_set(pin);

    /* clear global interrupt flag */
    cli();

    EIMSK |= (1 << pin_num);

    /* configure the flank */
    switch (flank) {
        case GPIO_RISING:
            if (pin_num < 4) {
                EICRA |= (3 << pin_num * 2);
            }
            else {
                EICRB |= (3 << (pin_num * 2) % 4);
            }
            break;
        case GPIO_FALLING:
            if (pin_num < 4) {
                EICRA |= (2 << pin_num * 2);
            }
            else {
                EICRB |= (2 << (pin_num * 2) % 4);
            }
            break;
        case GPIO_BOTH:
            if (pin_num < 4) {
                EICRA |= (1 << pin_num * 2);
            }
            else {
                EICRB |= (1 << (pin_num * 2) % 4);
            }
            break;
        default:
            return -1;
    };

    /* set callback */
    config[pin_num].cb = cb;
    config[pin_num].arg = arg;

    /* set global interrupt flag */
    sei();

    return 0;
}

void gpio_irq_enable(gpio_t pin)
{
    EIMSK |= (1 << _pin_num(pin));
}

void gpio_irq_disable(gpio_t pin)
{
    EIMSK &= ~(1 << _pin_num(pin));
}

int gpio_read(gpio_t pin)
{
    return (_SFR_MEM8(_pin_addr(pin)) & (1 << _pin_num(pin)));
}

void gpio_set(gpio_t pin)
{
    _SFR_MEM8(_port_addr(pin)) |= (1 << _pin_num(pin));
}

void gpio_clear(gpio_t pin)
{
    _SFR_MEM8(_port_addr(pin)) &= ~(1 << _pin_num(pin));
}

void gpio_toggle(gpio_t pin)
{
    if (gpio_read(pin)) {
        gpio_clear(pin);
    }
    else {
        gpio_set(pin);
    }
}

void gpio_write(gpio_t pin, int value)
{
    if (value) {
        gpio_set(pin);
    }
    else {
        gpio_clear(pin);
    }
}

static inline void irq_handler(uint8_t pin_num)
{
    config[pin_num].cb(config[pin_num].arg);
}

ISR(INT0_vect, ISR_BLOCK)
{
    irq_handler(0); /**< predefined interrupt pin */
}

ISR(INT1_vect, ISR_BLOCK)
{
    irq_handler(1); /**< predefined interrupt pin */
}

ISR(INT2_vect, ISR_BLOCK)
{
    irq_handler(2); /**< predefined interrupt pin */
}

ISR(INT3_vect, ISR_BLOCK)
{
    irq_handler(3); /**< predefined interrupt pin */
}

ISR(INT4_vect, ISR_BLOCK)
{
    irq_handler(4); /**< predefined interrupt pin */
}

ISR(INT5_vect, ISR_BLOCK)
{
    irq_handler(5); /**< predefined interrupt pin */
}

ISR(INT6_vect, ISR_BLOCK)
{
    irq_handler(6); /**< predefined interrupt pin */
}

ISR(INT7_vect, ISR_BLOCK)
{
    irq_handler(7); /**< predefined interrupt pin */
}

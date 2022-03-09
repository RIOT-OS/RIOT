/*
 * Copyright (C) 2021 Gerson Fernando Budke <nandojve@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atxmega
 * @ingroup     cpu_atxmega_periph
 * @{
 *
 * @file
 * @brief       Low-level GPIO driver implementation
 *
 * @author      Gerson Fernando Budke <nandojve@gmail.com>
 *
 * @}
 */

#include <stdio.h>
#include <avr/interrupt.h>

#include "cpu.h"
#include "periph_conf.h"
#include "periph/gpio.h"
#include "bitarithm.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief     GPIO port base
 *
 * GPIO_PORT_BASE resides in the IO address space and must be 16 bits.
 */
#define GPIO_PORT_BASE      ((uint16_t)&PORTA)

/**
 * @brief     GPIO port structure offset
 *
 * The PORT_t struct it is not complete filled and it is necessary define the
 * address offset manually.
 */
#define GPIO_PORT_OFFSET    (0x20)

static gpio_isr_ctx_t config_ctx[GPIO_EXT_INT_NUMOF];
static uint8_t config_irq[GPIO_EXT_INT_NUMOF];

/**
 * @brief     Extract the pin number of the given pin
 */
static inline uint8_t _pin_mask(gpio_t pin)
{
    return (pin & 0xff);
}

/**
 * @brief     Extract the port number of the given pin
 */
static inline uint8_t _port_num(gpio_t pin)
{
    return (pin >> 8) & 0x0f;
}

/**
 * @brief     Generate the PORTx address of the give pin in the IO address space
 */
static inline PORT_t *_port_addr(gpio_t pin)
{
    uint8_t port_num = _port_num(pin);
    uint16_t port_addr = GPIO_PORT_BASE + (port_num * GPIO_PORT_OFFSET);

    return (PORT_t *) port_addr;
}

static inline void _print_config(gpio_t pin)
{
    PORT_t *port = _port_addr(pin);
    uint8_t pin_mask = _pin_mask(pin);
    volatile uint8_t *pin_ctrl = &port->PIN0CTRL;

    DEBUG("PORT: 0x%04x, PIN: 0x%02x\n", (uint16_t)port, pin_mask);
    DEBUG("DIR: 0x%02x, IN: 0x%02x, OUT: 0x%02x\n", port->DIR, port->IN, port->OUT);
    DEBUG("INTCTRL: 0x%02x\nINTFLAGS: 0x%02x\n", port->INTCTRL, port->INTFLAGS);
    DEBUG("INT0MASK: 0x%02x\nINT1MASK: 0x%02x\n", port->INT0MASK, port->INT1MASK);

    for (uint8_t p = 0; p < 8; p++) {
        DEBUG("PIN%dCTRL: 0x%02x\n", p, pin_ctrl[p]);
    }
}

static inline void _gpio_pinctrl_set(gpio_t pin, gpio_mode_t mode,
                                     gpio_flank_t flank, gpio_cb_t cb,
                                     void *arg)
{
    uint8_t pin_mask = _pin_mask(pin);
    uint8_t port_num = _port_num(pin);
    PORT_t *port = _port_addr(pin);
    volatile uint8_t *pin_ctrl = &port->PIN0CTRL;
    uint8_t irq_state;
    uint8_t in_sense_cfg;
    uint8_t pins;
    uint8_t pin_idx;

    in_sense_cfg  = (mode & ~PORT_ISC_gm);
    in_sense_cfg |= (mode & GPIO_ANALOG)
                 ?  PORT_ISC_INPUT_DISABLE_gc
                 :  ((flank >> 4) & PORT_ISC_gm);

    pins = pin_mask;
    pin_idx = 0;

    if (mode & GPIO_OUT) {
        port->DIRSET = pin_mask;
    }
    else {
        port->DIRCLR = pin_mask;
    }

    irq_state = irq_disable();

    while (pins) {
        pins = bitarithm_test_and_clear(pins, &pin_idx);
        pin_ctrl[pin_idx] = in_sense_cfg;
    }

    if (flank & GPIO_INT_DISABLED_ALL) {
        config_ctx[port_num + PORT_MAX].cb = NULL;
        config_ctx[port_num + PORT_MAX].arg = NULL;
        config_irq[port_num + PORT_MAX] = 0;
        config_ctx[port_num].cb = NULL;
        config_ctx[port_num].arg = NULL;
        config_irq[port_num] = 0;

        port->INTCTRL = 0;
        port->INT1MASK = 0;
        port->INT0MASK = 0;
        port->INTFLAGS = PORT_INT1IF_bm | PORT_INT0IF_bm;
    }
    else if (flank & GPIO_INT1_VCT) {
        config_ctx[port_num + PORT_MAX].cb = cb;
        config_ctx[port_num + PORT_MAX].arg = arg;
        config_irq[port_num + PORT_MAX] = (flank & GPIO_LVL_HIGH)
                                          << PORT_INT1LVL_gp;

        if ((flank & GPIO_LVL_HIGH) == GPIO_LVL_OFF) {
            port->INT1MASK &= ~pin_mask;
        }
        else {
            port->INT1MASK |= pin_mask;
        }

        port->INTFLAGS = PORT_INT1IF_bm;

        /* Get mask from INT 0 and apply new INT 1 mask */
        port->INTCTRL = (port->INTCTRL & PORT_INT0LVL_gm)
                        | config_irq[port_num + PORT_MAX];
    }
    else {
        config_ctx[port_num].cb = cb;
        config_ctx[port_num].arg = arg;
        config_irq[port_num] = (flank & GPIO_LVL_HIGH)
                               << PORT_INT0LVL_gp;

        if ((flank & GPIO_LVL_HIGH) == GPIO_LVL_OFF) {
            port->INT0MASK &= ~pin_mask;
        }
        else {
            port->INT0MASK |= pin_mask;
        }

        port->INTFLAGS = PORT_INT0IF_bm;

        /* Get mask from INT 1 and apply new INT 0 mask */
        port->INTCTRL = (port->INTCTRL & PORT_INT1LVL_gm)
                        | config_irq[port_num];
    }

    irq_restore(irq_state);
}

int gpio_init(gpio_t pin, gpio_mode_t mode)
{
    DEBUG("gpio_init pin = 0x%02x mode = 0x%02x\n", pin, mode);

    _gpio_pinctrl_set(pin, mode, GPIO_INT_DISABLED_ALL, NULL, NULL);

    return 0;
}

int gpio_init_int(gpio_t pin, gpio_mode_t mode, gpio_flank_t flank,
                  gpio_cb_t cb, void *arg)
{
    DEBUG("gpio_init_int pin = 0x%02x mode = 0x%02x flank = 0x%02x\n", pin,
          mode, flank);

    if (mode & GPIO_ANALOG) {
        DEBUG("Pin can't be ANALOG (input buffer disabled) and INT at same time.\n");
        return -1;
    }

    _gpio_pinctrl_set(pin, mode, flank, cb, arg);

    if (IS_ACTIVE(ENABLE_DEBUG)) {
        _print_config(pin);
    }

    return 0;
}

void gpio_irq_enable(gpio_t pin)
{
    DEBUG("gpio_irq_enable pin = 0x%04x \n", pin);

    uint8_t pin_mask = _pin_mask(pin);
    uint8_t port_num = _port_num(pin);
    PORT_t *port = _port_addr(pin);

    if (port->INT1MASK & pin_mask) {
        port->INTFLAGS = PORT_INT1IF_bm;
        port->INTCTRL |= config_irq[port_num + PORT_MAX];
    }
    if (port->INT0MASK & pin_mask) {
        port->INTFLAGS = PORT_INT0IF_bm;
        port->INTCTRL |= config_irq[port_num];
    }

    if (IS_ACTIVE(ENABLE_DEBUG)) {
        _print_config(pin);
    }
}

void gpio_irq_disable(gpio_t pin)
{
    DEBUG("gpio_irq_disable pin = 0x%04x \n", pin);

    uint8_t pin_mask = _pin_mask(pin);
    PORT_t *port = _port_addr(pin);

    if (port->INT1MASK & pin_mask) {
        port->INTCTRL &= ~PORT_INT1LVL_gm;
    }
    if (port->INT0MASK & pin_mask) {
        port->INTCTRL &= ~PORT_INT0LVL_gm;
    }

    if (IS_ACTIVE(ENABLE_DEBUG)) {
        _print_config(pin);
    }
}

int gpio_read(gpio_t pin)
{
    PORT_t *port = _port_addr(pin);
    uint8_t pin_mask = _pin_mask(pin);

    if (IS_ACTIVE(ENABLE_DEBUG)) {
        _print_config(pin);
    }

    return port->IN & pin_mask;
}

void gpio_set(gpio_t pin)
{
    DEBUG("gpio_set pin = 0x%04x \n", pin);

    PORT_t *port = _port_addr(pin);
    uint8_t pin_mask = _pin_mask(pin);

    port->OUTSET = pin_mask;

    if (IS_ACTIVE(ENABLE_DEBUG)) {
        _print_config(pin);
    }
}

void gpio_clear(gpio_t pin)
{
    DEBUG("gpio_clear pin = 0x%04x \n", pin);

    PORT_t *port = _port_addr(pin);
    uint8_t pin_mask = _pin_mask(pin);

    port->OUTCLR = pin_mask;

    if (IS_ACTIVE(ENABLE_DEBUG)) {
        _print_config(pin);
    }
}

void gpio_toggle(gpio_t pin)
{
    DEBUG("gpio_toggle pin = 0x%04x \n", pin);

    PORT_t *port = _port_addr(pin);
    uint8_t pin_mask = _pin_mask(pin);

    port->OUTTGL = pin_mask;

    if (IS_ACTIVE(ENABLE_DEBUG)) {
        _print_config(pin);
    }
}

void gpio_write(gpio_t pin, int value)
{
    DEBUG("gpio_write pin = 0x%04x, value = 0x%02x \n", pin, value);

    if (value) {
        gpio_set(pin);
    }
    else {
        gpio_clear(pin);
    }
}

static inline void irq_handler(uint8_t port_num, uint8_t isr_vct_num)
{
    avr8_enter_isr();

    DEBUG("irq_handler port = 0x%02x, vct_num = %d \n", port_num, isr_vct_num);

    if (isr_vct_num) {
        port_num += PORT_MAX;
    }

    if (config_ctx[port_num].cb) {
        config_ctx[port_num].cb(config_ctx[port_num].arg);
    }
    else {
        DEBUG("WARNING! irq_handler without callback\n");
    }

    avr8_exit_isr();
}

#if defined(PORTA_INT0_vect)
ISR(PORTA_INT0_vect, ISR_BLOCK)
{
    irq_handler(PORT_A, 0);
}
#endif
#if defined(PORTA_INT1_vect)
ISR(PORTA_INT1_vect, ISR_BLOCK)
{
    irq_handler(PORT_A, 1);
}
#endif

#if defined(PORTB_INT0_vect)
ISR(PORTB_INT0_vect, ISR_BLOCK)
{
    irq_handler(PORT_B, 0);
}
#endif
#if defined(PORTB_INT1_vect)
ISR(PORTB_INT1_vect, ISR_BLOCK)
{
    irq_handler(PORT_B, 1);
}
#endif

#if defined(PORTC_INT0_vect)
ISR(PORTC_INT0_vect, ISR_BLOCK)
{
    irq_handler(PORT_C, 0);
}
#endif
#if defined(PORTC_INT1_vect)
ISR(PORTC_INT1_vect, ISR_BLOCK)
{
    irq_handler(PORT_C, 1);
}
#endif

#if defined(PORTD_INT0_vect)
ISR(PORTD_INT0_vect, ISR_BLOCK)
{
    irq_handler(PORT_D, 0);
}
#endif
#if defined(PORTD_INT1_vect)
ISR(PORTD_INT1_vect, ISR_BLOCK)
{
    irq_handler(PORT_D, 1);
}
#endif

#if defined(PORTE_INT0_vect)
ISR(PORTE_INT0_vect, ISR_BLOCK)
{
    irq_handler(PORT_E, 0);
}
#endif
#if defined(PORTE_INT1_vect)
ISR(PORTE_INT1_vect, ISR_BLOCK)
{
    irq_handler(PORT_E, 1);
}
#endif

#if defined(PORTF_INT0_vect)
ISR(PORTF_INT0_vect, ISR_BLOCK)
{
    irq_handler(PORT_F, 0);
}
#endif
#if defined(PORTF_INT1_vect)
ISR(PORTF_INT1_vect, ISR_BLOCK)
{
    irq_handler(PORT_F, 1);
}
#endif

#if defined(PORTG_INT0_vect)
ISR(PORTG_INT0_vect, ISR_BLOCK)
{
    irq_handler(PORT_G, 0);
}
#endif
#if defined(PORTG_INT1_vect)
ISR(PORTG_INT1_vect, ISR_BLOCK)
{
    irq_handler(PORT_G, 1);
}
#endif

#if defined(PORTH_INT0_vect)
ISR(PORTH_INT0_vect, ISR_BLOCK)
{
    irq_handler(PORT_H, 0);
}
#endif
#if defined(PORTH_INT1_vect)
ISR(PORTH_INT1_vect, ISR_BLOCK)
{
    irq_handler(PORT_H, 1);
}
#endif

#if defined(PORTJ_INT0_vect)
ISR(PORTJ_INT0_vect, ISR_BLOCK)
{
    irq_handler(PORT_J, 0);
}
#endif
#if defined(PORTJ_INT1_vect)
ISR(PORTJ_INT1_vect, ISR_BLOCK)
{
    irq_handler(PORT_J, 1);
}
#endif

#if defined(PORTK_INT0_vect)
ISR(PORTK_INT0_vect, ISR_BLOCK)
{
    irq_handler(PORT_K, 0);
}
#endif
#if defined(PORTK_INT1_vect)
ISR(PORTK_INT1_vect, ISR_BLOCK)
{
    irq_handler(PORT_K, 1);
}
#endif

#if defined(PORTL_INT0_vect)
ISR(PORTL_INT0_vect, ISR_BLOCK)
{
    irq_handler(PORT_L, 0);
}
#endif
#if defined(PORTL_INT1_vect)
ISR(PORTL_INT1_vect, ISR_BLOCK)
{
    irq_handler(PORT_L, 1);
}
#endif

#if defined(PORTM_INT0_vect)
ISR(PORTM_INT0_vect, ISR_BLOCK)
{
    irq_handler(PORT_M, 0);
}
#endif
#if defined(PORTM_INT1_vect)
ISR(PORTM_INT1_vect, ISR_BLOCK)
{
    irq_handler(PORT_M, 1);
}
#endif

#if defined(PORTN_INT0_vect)
ISR(PORTN_INT0_vect, ISR_BLOCK)
{
    irq_handler(PORT_N, 0);
}
#endif
#if defined(PORTN_INT1_vect)
ISR(PORTN_INT1_vect, ISR_BLOCK)
{
    irq_handler(PORT_N, 1);
}
#endif

#if defined(PORTP_INT0_vect)
ISR(PORTP_INT0_vect, ISR_BLOCK)
{
    irq_handler(PORT_P, 0);
}
#endif
#if defined(PORTP_INT1_vect)
ISR(PORTP_INT1_vect, ISR_BLOCK)
{
    irq_handler(PORT_P, 1);
}
#endif

#if defined(PORTQ_INT0_vect)
ISR(PORTQ_INT0_vect, ISR_BLOCK)
{
    irq_handler(PORT_Q, 0);
}
#endif
#if defined(PORTQ_INT1_vect)
ISR(PORTQ_INT1_vect, ISR_BLOCK)
{
    irq_handler(PORT_Q, 1);
}
#endif

#if defined(PORTR_INT0_vect)
ISR(PORTR_INT0_vect, ISR_BLOCK)
{
    irq_handler(PORT_R, 0);
}
#endif
#if defined(PORTR_INT1_vect)
ISR(PORTR_INT1_vect, ISR_BLOCK)
{
    irq_handler(PORT_R, 1);
}
#endif

/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_ezr32wg
 * @{
 *
 * @file
 * @brief       Low-level GPIO driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "sched.h"
#include "thread.h"
#include "periph/gpio.h"
#include "periph_conf.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief   Number of external interrupt lines
 */
#define NUMOF_IRQS                  (16U)
/**
 * @brief   Hold one interrupt context per interrupt line
 */
static gpio_isr_ctx_t isr_ctx[NUMOF_IRQS];

static inline int _port_num(gpio_t pin)
{
    return (pin & 0xf0) >> 4;
}

static inline GPIO_P_TypeDef *_port(gpio_t pin)
{
    return (GPIO_P_TypeDef *)(&GPIO->P[_port_num(pin)]);
}

static inline int _pin_pos(gpio_t pin)
{
    return (pin & 0x0f);
}

static inline int _pin_mask(gpio_t pin)
{
    return (1 << _pin_pos(pin));
}

int gpio_init(gpio_t pin, gpio_mode_t mode)
{
    GPIO_P_TypeDef *port = _port(pin);
    uint32_t pin_pos = _pin_pos(pin);

    /* enable power for the GPIO module */
    CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_GPIO;

    /* configure the mode */
    port->MODE[pin_pos >> 3] &= ~(0xf << ((pin_pos & 0x7) * 4));
    port->MODE[pin_pos >> 3] |= (mode << ((pin_pos & 0x7) * 4));
    /* reset output register */
    port->DOUTCLR = (1 << pin_pos);
    /* if input with pull-up, set the data out register */
    if (mode == GPIO_IN_PU) {
        port->DOUTSET = (1 << pin_pos);
    }

    return 0;
}

int gpio_init_int(gpio_t pin, gpio_mode_t mode, gpio_flank_t flank,
                    gpio_cb_t cb, void *arg)
{
    uint32_t pin_pos = _pin_pos(pin);

    /* configure as input */
    gpio_init(pin, mode);

    /* just in case, disable interrupt for this channel */
    GPIO->IEN &= ~(1 << pin_pos);
    /* save callback */
    isr_ctx[pin_pos].cb = cb;
    isr_ctx[pin_pos].arg = arg;
    /* configure interrupt */
    GPIO->EXTIPSEL[pin_pos >> 3] &= (0x7 << ((pin_pos & 0x7) * 4));
    GPIO->EXTIPSEL[pin_pos >> 3] |= (_port_num(pin) << ((pin_pos & 0x7) * 4));
    GPIO->EXTIRISE &= ~(1 << pin_pos);
    GPIO->EXTIRISE |= ((flank & 0x1) << pin_pos);
    GPIO->EXTIFALL &= ~(1 << pin_pos);
    GPIO->EXTIFALL &= (((flank & 0x2) >> 1) << pin_pos);
    /* enable global GPIO IRQ */
    NVIC_EnableIRQ(GPIO_EVEN_IRQn);
    /* enable the interrupt channel */
    GPIO->IEN |= (1 << pin_pos);
    return 0;
}

void gpio_irq_enable(gpio_t pin)
{
    GPIO->IEN |= _pin_mask(pin);
}

void gpio_irq_disable(gpio_t pin)
{
    GPIO->IEN &= ~(_pin_mask(pin));
}

int gpio_read(gpio_t pin)
{
    return _port(pin)->DIN & _pin_mask(pin);
}

void gpio_set(gpio_t pin)
{
    _port(pin)->DOUTSET = _pin_mask(pin);
}

void gpio_clear(gpio_t pin)
{
    _port(pin)->DOUTCLR = _pin_mask(pin);
}

void gpio_toggle(gpio_t pin)
{
    _port(pin)->DOUTTGL = _pin_mask(pin);
}

void gpio_write(gpio_t pin, int value)
{
    if (value) {
        _port(pin)->DOUTSET = _pin_mask(pin);
    } else {
        _port(pin)->DOUTCLR = _pin_mask(pin);
    }
}

/**
 * @brief   External interrupt handler
 */
void isr_gpio_even(void)
{
    for (int i = 0; i < NUMOF_IRQS; i++) {
        if (GPIO->IF & (1 << i)) {
            isr_ctx[i].cb(isr_ctx[i].arg);
            GPIO->IFC = (1 << i);
        }
    }
    if (sched_context_switch_request) {
        thread_yield();
    }
}

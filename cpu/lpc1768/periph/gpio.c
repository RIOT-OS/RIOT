/*
 * Copyright (C) 2017 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_lpc1768
 * @{
 *
 * @file
 * @brief       Low-level GPIO driver implementation
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include <assert.h>

#include "cpu.h"
#include "periph/gpio.h"

#ifdef MODULE_PERIPH_GPIO_IRQ
/**
 * @brief   Number of external interrupt lines.
 */
#define NUMOF_IRQS         (32)

/**
 * @brief   Hold one interrupt context per interrupt line
 */
static gpio_isr_ctx_t isr_ctx[NUMOF_IRQS];

static gpio_flank_t isr_state[2][32];
#endif /* MODULE_PERIPH_GPIO_IRQ */

#define PIN_MASK        (0x1f)
#define PORT_SHIFT      (5U)

static inline int _pin(gpio_t pin)
{
    return (pin & PIN_MASK);
}

static inline int _port(gpio_t pin)
{
    return (pin >> PORT_SHIFT);
}

static inline LPC_GPIO_TypeDef *_base(gpio_t pin)
{
    return (LPC_GPIO_TypeDef *) (LPC_GPIO_BASE + (_port(pin) * 0x20));
}

int gpio_init(gpio_t pin, gpio_mode_t mode)
{
    /* check for valid pin */
    if (pin == GPIO_UNDEF) {
        return -1;
    }

    if (_port(pin) > 4 || _pin(pin) > 32) {
        return -1;
    }

    /* enable gpio peripheral */
    LPC_SC->PCONP |= (1 << 15);

    /* pin as output or input */
    LPC_GPIO_TypeDef *base = _base(pin);

    base->FIODIR &= ~(1 << _pin(pin));
    base->FIODIR |= ((mode & 0x01) << _pin(pin));

    /* configure pin function */
    int reg = 2 * _port(pin) + (_pin(pin) / 16);
    int bit = (pin % 16) * 2;

    ((uint32_t *) &LPC_PINCON->PINSEL0)[reg] &= ~(0x03 << bit);

    /* configure pull up/down */
    ((uint32_t *) &LPC_PINCON->PINMODE0)[reg] &= ~(0x03 << bit);
    ((uint32_t *) &LPC_PINCON->PINMODE0)[reg] |= (((mode >> 1) & 0x03) << bit);

    /* configure open drain */
    ((uint32_t *) &LPC_PINCON->PINMODE_OD0)[_port(pin)] &= ~(1 << _pin(pin));
    ((uint32_t *) &LPC_PINCON->PINMODE_OD0)[_port(pin)] |= (((mode >> 3) & 0x01) << _pin(pin));

    return 0;
}

int gpio_read(gpio_t pin)
{
    LPC_GPIO_TypeDef *base = _base(pin);

    return (base->FIOPIN & (1 << _pin(pin))) ? 1 : 0;
}

void gpio_set(gpio_t pin)
{
    LPC_GPIO_TypeDef *base = _base(pin);

    base->FIOSET = (1 << _pin(pin));
}

void gpio_clear(gpio_t pin)
{
    LPC_GPIO_TypeDef *base = _base(pin);

    base->FIOCLR = (1 << _pin(pin));
}

void gpio_toggle(gpio_t pin)
{
    LPC_GPIO_TypeDef *base = _base(pin);

    base->FIOPIN ^= (1 << _pin(pin));
}

void gpio_write(gpio_t pin, bool value)
{
    LPC_GPIO_TypeDef *base = _base(pin);

    if (value) {
        base->FIOSET = (1 << _pin(pin));
    }
    else {
        base->FIOCLR = (1 << _pin(pin));
    }
}

#ifdef MODULE_PERIPH_GPIO_IRQ
static inline void _configure_flank(gpio_t pin, gpio_flank_t flank)
{
    switch (flank) {
        case GPIO_RISING:
            if (_port(pin) == 0) {
                LPC_GPIOINT->IO0IntEnF &= ~(1 << _pin(pin));
                LPC_GPIOINT->IO0IntEnR |= (1 << _pin(pin));
            }
            else {
                LPC_GPIOINT->IO2IntEnF &= ~(1 << _pin(pin));
                LPC_GPIOINT->IO2IntEnR |= (1 << _pin(pin));
            }

            break;
        case GPIO_FALLING:
            if (_port(pin) == 0) {
                LPC_GPIOINT->IO0IntEnF |= (1 << _pin(pin));
                LPC_GPIOINT->IO0IntEnR &= ~(1 << _pin(pin));
            }
            else {
                LPC_GPIOINT->IO2IntEnF |= (1 << _pin(pin));
                LPC_GPIOINT->IO2IntEnR &= ~(1 << _pin(pin));
            }

            break;
        case GPIO_BOTH:
            if (_port(pin) == 0) {
                LPC_GPIOINT->IO0IntEnF |= 1 << _pin(pin);
                LPC_GPIOINT->IO0IntEnR |= 1 << _pin(pin);
            }
            else {
                LPC_GPIOINT->IO2IntEnF |= 1 << _pin(pin);
                LPC_GPIOINT->IO2IntEnR |= 1 << _pin(pin);
            }

            break;
    }
}

int gpio_init_int(gpio_t pin, gpio_mode_t mode, gpio_flank_t flank,
                  gpio_cb_t cb, void *arg)
{
    /* only certain pins can be used as interrupt pins */
    if (_port(pin) != 0 && _port(pin) != 2) {
        return -1;
    }

    /* initialize the pin */
    int result = gpio_init(pin, mode);

    if (result != 0) {
        return result;
    }

    /* store interrupt callback */
    isr_ctx[_pin(pin)].cb = cb;
    isr_ctx[_pin(pin)].arg = arg;

    /* need to store flank configuration for (re)enable irq */
    isr_state[_port(pin) >> 1][_pin(pin)] = flank;

    /* set flank configuration */
    _configure_flank(pin, flank);

    /* clear any pending requests and enable the interrupt */
    NVIC_ClearPendingIRQ(EINT3_IRQn);
    NVIC_EnableIRQ(EINT3_IRQn);

    return 0;
}

void gpio_irq_enable(gpio_t pin)
{
    assert(_port(pin) == 0 || _port(pin) == 2);

    _configure_flank(pin, isr_state[_port(pin) >> 1][_pin(pin)]);
}

void gpio_irq_disable(gpio_t pin)
{
    assert(_port(pin) == 0 || _port(pin) == 2);

    if (_port(pin) == 0) {
        LPC_GPIOINT->IO0IntEnF &= ~(1 << _pin(pin));
        LPC_GPIOINT->IO0IntEnR &= ~(1 << _pin(pin));
    }
    else {
        LPC_GPIOINT->IO2IntEnF &= ~(1 << _pin(pin));
        LPC_GPIOINT->IO2IntEnR &= ~(1 << _pin(pin));
    }
}

void isr_eint3(void)
{
    /* combine all interrupts */
    uint32_t status = LPC_GPIOINT->IO0IntStatF | LPC_GPIOINT->IO0IntStatR |
                      LPC_GPIOINT->IO2IntStatF | LPC_GPIOINT->IO2IntStatR;

    /* invoke all handlers */
    for (int i = 0; i < NUMOF_IRQS; i++) {
        if (status & ((uint32_t)1 << i)) {
            isr_ctx[i].cb(isr_ctx[i].arg);

            LPC_GPIOINT->IO0IntClr |= (1 << i);
            LPC_GPIOINT->IO2IntClr |= (1 << i);
        }
    }

    cortexm_isr_end();
}
#endif /* MODULE_PERIPH_GPIO_IRQ */

/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_lpc11u34
 * @{
 *
 * @file
 * @brief       Low-level GPIO driver implementation
 *
 * @author      Paul RATHGEB <paul.rathgeb@skynet.be>
 *
 * @}
 */

#include "cpu.h"
#include "periph/gpio.h"

#define PIN_MASK        (0x00ff)
#define PORT_SHIFT      (8U)

#define MODE_DIR_MASK   (0x0001)
#define MODE_IOCON_MASK (0xfff8)

#define ISR_NUMOF       (8U)

#define SYSCTL_PINTMASK (1 << 19)

/* for efficiency reasons, we define our own custom IOCON_Type */
typedef struct {
    __IO uint32_t PIO[56];      /* 24 of P0 + 32 of P1 */
} iocon_t;

#define IOCON           ((iocon_t *)LPC_IOCON_BASE)

static gpio_t isrmap[] = { GPIO_UNDEF, GPIO_UNDEF, GPIO_UNDEF, GPIO_UNDEF,
                           GPIO_UNDEF, GPIO_UNDEF, GPIO_UNDEF, GPIO_UNDEF };

static gpio_isr_ctx_t isrctx[ISR_NUMOF];

static inline int port(gpio_t pin)
{
    return (int)(pin >> PORT_SHIFT);
}

static inline int num(gpio_t pin)
{
    return (pin & PIN_MASK);
}

static inline uint32_t mask(gpio_t pin)
{
    return (1 << num(pin));
}

static inline int ionum(gpio_t pin)
{
    return (port(pin) * 24) + num(pin);
}

int gpio_init(gpio_t pin, gpio_mode_t mode)
{
    /* reset any eventually set interrupt configuration */
    for (int i = 0; i < ISR_NUMOF; i++) {
        if (isrmap[i] == pin) {
            NVIC_DisableIRQ(FLEX_INT0_IRQn + i);
            isrmap[i] = GPIO_UNDEF;
        }
    }

    LPC_GPIO->DIR[port(pin)] &= ~mask(pin);
    LPC_GPIO->DIR[port(pin)] |= ((mode & MODE_DIR_MASK) << num(pin));
    IOCON->PIO[ionum(pin)] = (mode & MODE_IOCON_MASK);

    return 0;
}

int gpio_init_int(gpio_t pin, gpio_mode_t mode, gpio_flank_t flank,
                  gpio_cb_t cb, void *arg)
{
    /* make sure we have an interrupt channel available */
    int i = 0;
    while ((i < ISR_NUMOF) && (isrmap[i] != GPIO_UNDEF) && (isrmap[i] != pin)) {
        i++;
    }
    if (i == ISR_NUMOF) {
        return -1;
    }

    /* do basic pin configuration */
    if (gpio_init(pin, mode) != 0) {
        return -1;
    }

    /* enable power for GPIO pin interrupt interface */
    LPC_SYSCON->SYSAHBCLKCTRL |= SYSCTL_PINTMASK;

    /* save ISR context */
    isrctx[i].cb = cb;
    isrctx[i].arg = arg;
    isrmap[i] = pin;

    /* Register pin as flex interrupt */
    LPC_SYSCON->PINTSEL[i] = ionum(pin);

    /* set active flank configuration */
    LPC_GPIO_PIN_INT->ISEL = 0;
    switch (flank) {
        case GPIO_RISING:
            LPC_GPIO_PIN_INT->IENR |=  (1 << i);
            LPC_GPIO_PIN_INT->IENF &= ~(1 << i);
            break;
        case GPIO_FALLING:
            LPC_GPIO_PIN_INT->IENR &= ~(1 << i);
            LPC_GPIO_PIN_INT->IENF |=  (1 << i);
            break;
        case GPIO_BOTH:
            LPC_GPIO_PIN_INT->IENR |=  (1 << i);
            LPC_GPIO_PIN_INT->IENF |=  (1 << i);
        default:
            return -1;
    }

    /* clear any pending requests and enable the pin's interrupt */
    LPC_GPIO_PIN_INT->IST = (1 << i);
    NVIC_EnableIRQ(FLEX_INT0_IRQn + i);

    return 0;
}

void gpio_irq_enable(gpio_t pin)
{
    for (int i = 0; i < ISR_NUMOF; i++) {
        if (isrmap[i] == pin) {
            NVIC_EnableIRQ(FLEX_INT0_IRQn + i);
        }
    }
}

void gpio_irq_disable(gpio_t pin)
{
    for (int i = 0; i < ISR_NUMOF; i++) {
        if (isrmap[i] == pin) {
            NVIC_DisableIRQ(FLEX_INT0_IRQn + i);
        }
    }
}

int gpio_read(gpio_t pin)
{
    return (LPC_GPIO->PIN[port(pin)] & mask(pin)) ? 1 : 0;
}

void gpio_set(gpio_t pin)
{
    LPC_GPIO->SET[port(pin)] = mask(pin);
}

void gpio_clear(gpio_t pin)
{
    LPC_GPIO->CLR[port(pin)] = mask(pin);
}

void gpio_toggle(gpio_t pin)
{
    LPC_GPIO->NOT[port(pin)] = mask(pin);
}

void gpio_write(gpio_t pin, int value)
{
    if (value) {
        LPC_GPIO->SET[port(pin)] = mask(pin);
    } else {
        LPC_GPIO->CLR[port(pin)] = mask(pin);
    }
}

static inline void isr_common(uint8_t int_id) {

    LPC_GPIO_PIN_INT->IST |= (1 << int_id);
    isrctx[int_id].cb(isrctx[int_id].arg);

    cortexm_isr_end();
}

void isr_pinint0(void)
{
    isr_common(0);
}
void isr_pinint1(void)
{
    isr_common(1);
}
void isr_pinint2(void)
{
    isr_common(2);
}
void isr_pinint3(void)
{
    isr_common(3);
}
void isr_pinint4(void)
{
    isr_common(4);
}
void isr_pinint5(void)
{
    isr_common(5);
}
void isr_pinint6(void)
{
    isr_common(6);
}
void isr_pinint7(void)
{
    isr_common(7);
}

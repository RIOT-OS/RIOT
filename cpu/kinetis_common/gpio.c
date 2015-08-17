/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 * Copyright (C) 2014 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_kinetis_common_gpio
 *
 * @{
 *
 * @file
 * @brief       Low-level GPIO driver implementation
 *
 * @author      Hauke Petersen <mail@haukepetersen.de>
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Jonas Remmert <j.remmert@phytec.de>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include "cpu.h"
#include "sched.h"
#include "thread.h"
#include "utlist.h"
#include "mutex.h"
#include "periph/gpio.h"
#include "periph_conf.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#ifndef PIN_MUX_FUNCTION_ANALOG
#define PIN_MUX_FUNCTION_ANALOG     0
#endif

#ifndef PIN_MUX_FUNCTION_GPIO
#define PIN_MUX_FUNCTION_GPIO       1
#endif

/**
 * @brief Linked list entry for interrupt configurations.
 */
typedef struct gpio_int_config_entry {
    struct gpio_int_config_entry* next; /**< pointer to next entry */
    gpio_cb_t cb;    /**< callback called from GPIO interrupt */
    void *arg;       /**< argument passed to the callback */
    uint32_t irqc;   /**< remember interrupt configuration between disable/enable */
    uint8_t pin;     /**< pin number within the port */
} gpio_int_config_entry_t;

/* Linked list of interrupt handlers for each port.
 * Using a linked list saves memory when less than 80% of all GPIO pins on the
 * CPU are configured for interrupts, which is true for (almost) all real world
 * applications. */
static gpio_int_config_entry_t* gpio_interrupts[PORT_NUMOF];

static mutex_t int_config_lock = MUTEX_INIT;

/* Maximum number of simultaneously enabled GPIO interrupts. Each pool entry
 * uses 20 bytes of RAM.
 */
#ifndef GPIO_INT_POOL_SIZE
#define GPIO_INT_POOL_SIZE 16
#endif

/* Pool of linked list entries which can be used by any port configuration.
 * Rationale: Avoid dynamic memory inside low level periph drivers. */
gpio_int_config_entry_t config_pool[GPIO_INT_POOL_SIZE];

static PORT_Type * const _port_ptrs[] = PORT_BASE_PTRS;
static GPIO_Type * const _gpio_ptrs[] = GPIO_BASE_PTRS;

static inline uint32_t _port_num(gpio_t dev) {
    return (uint32_t)((dev & GPIO_PORT_MASK) >> GPIO_PORT_SHIFT);
}

static inline uint8_t _pin_num(gpio_t dev) {
    return (uint8_t)((dev & GPIO_PIN_MASK) >> GPIO_PIN_SHIFT);
}

static inline PORT_Type *_port(gpio_t dev) {
    return _port_ptrs[_port_num(dev)];
}

static inline GPIO_Type *_gpio(gpio_t dev) {
    return _gpio_ptrs[_port_num(dev)];
}

static void _clear_interrupt_config(gpio_t dev) {
    gpio_int_config_entry_t* entry = NULL;
    uint8_t pin_number = _pin_num(dev);
    mutex_lock(&int_config_lock);
    /* Search for the given pin in the port's interrupt configuration */
    LL_SEARCH_SCALAR(gpio_interrupts[_port_num(dev)], entry, pin, pin_number);
    if (entry != NULL) {
        LL_DELETE(gpio_interrupts[_port_num(dev)], entry);
        /* pin == 0 means the entry is available */
        entry->pin = 0;
    }
    mutex_unlock(&int_config_lock);
}

static gpio_int_config_entry_t* _allocate_interrupt_config(uint8_t port) {
    gpio_int_config_entry_t* ret = NULL;

    mutex_lock(&int_config_lock);
    for (uint8_t i = 0; i < GPIO_INT_POOL_SIZE; ++i) {
        if (config_pool[i].pin == 0) {
            /* temporarily set pin to something non-zero until the proper pin
             * number is set by the init code */
            config_pool[i].pin = 200;
            ret = &config_pool[i];
            LL_PREPEND(gpio_interrupts[port], ret);
            break;
        }
    }
    mutex_unlock(&int_config_lock);

    return ret;
}

int gpio_init(gpio_t dev, gpio_dir_t dir, gpio_pp_t pushpull)
{
    switch (_port_num(dev)) {
#ifdef PORTA_BASE
        case PORT_A:
            PORTA_CLOCK_GATE = 1;
            break;
#endif

#ifdef PORTB_BASE
        case PORT_B:
            PORTB_CLOCK_GATE = 1;
            break;
#endif

#ifdef PORTC_BASE
        case PORT_C:
            PORTC_CLOCK_GATE = 1;
            break;
#endif

#ifdef PORTD_BASE
        case PORT_D:
            PORTD_CLOCK_GATE = 1;
            break;
#endif

#ifdef PORTE_BASE
        case PORT_E:
            PORTE_CLOCK_GATE = 1;
            break;
#endif

#ifdef PORTF_BASE
        case PORT_F:
            PORTF_CLOCK_GATE = 1;
            break;
#endif

#ifdef PORTG_BASE
        case PORT_G:
            PORTG_CLOCK_GATE = 1;
            break;
#endif

        default:
            return -1;
    }

    uint8_t pin = _pin_num(dev);
    PORT_Type *port = _port(dev);
    GPIO_Type *gpio = _gpio(dev);

    _clear_interrupt_config(dev);

    /* Reset all pin control settings for the pin */
    /* Switch to analog input function while fiddling with the settings, to be safe. */
    port->PCR[pin] = PORT_PCR_MUX(PIN_MUX_FUNCTION_ANALOG);

    /* set to push-pull configuration */
    switch (pushpull) {
        case GPIO_PULLUP:
            port->PCR[pin] |= PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; /* Pull enable, pull up */
            break;

        case GPIO_PULLDOWN:
            port->PCR[pin] |= PORT_PCR_PE_MASK; /* Pull enable, !pull up */
            break;

        default:
            break;
    }

    if (dir == GPIO_DIR_OUT) {
        BITBAND_REG32(gpio->PDDR, pin) = 1;    /* set pin to output mode */
        gpio->PCOR = GPIO_PCOR_PTCO(1 << pin); /* set output to low */
    }
    else {
        BITBAND_REG32(gpio->PDDR, pin) = 0;    /* set pin to input mode */
    }

    /* Select GPIO function for the pin */
    port->PCR[pin] |= PORT_PCR_MUX(PIN_MUX_FUNCTION_GPIO);

    return 0;
}

int gpio_init_int(gpio_t dev, gpio_pp_t pushpull, gpio_flank_t flank, gpio_cb_t cb, void *arg)
{
    int res;
    IRQn_Type irqn;

    res = gpio_init(dev, GPIO_DIR_IN, pushpull);

    if (res < 0) {
        return res;
    }

    switch (_port_num(dev)) {
#ifdef PORTA_BASE
        case PORT_A:
            irqn = PORTA_IRQn;
            break;
#endif

#ifdef PORTB_BASE
        case PORT_B:
            irqn = PORTB_IRQn;
            break;
#endif

#ifdef PORTC_BASE
        case PORT_C:
            irqn = PORTC_IRQn;
            break;
#endif

#ifdef PORTD_BASE
        case PORT_D:
            irqn = PORTD_IRQn;
            break;
#endif

#ifdef PORTE_BASE
        case PORT_E:
            irqn = PORTE_IRQn;
            break;
#endif

#ifdef PORTF_BASE
        case PORT_F:
            irqn = PORTF_IRQn;
            break;
#endif

#ifdef PORTG_BASE
        case PORT_G:
            irqn = PORTG_IRQn;
            break;
#endif

        default:
            return -1;
    }

    uint32_t irqc;
    /* configure the active edges */
    switch (flank) {
        case GPIO_RISING:
            irqc = PORT_PCR_IRQC(PIN_INTERRUPT_RISING);
            break;

        case GPIO_FALLING:
            irqc = PORT_PCR_IRQC(PIN_INTERRUPT_FALLING);
            break;

        case GPIO_BOTH:
            irqc = PORT_PCR_IRQC(PIN_INTERRUPT_EDGE);
            break;

        default:
            /* Unknown setting */
            return -1;
    }

    gpio_int_config_entry_t* config = _allocate_interrupt_config(_port_num(dev));
    if (config == NULL) {
        /* No free interrupt config entries */
        return -1;
    }

    /* Enable port interrupts in the NVIC */
    NVIC_SetPriority(irqn, GPIO_IRQ_PRIO);
    NVIC_EnableIRQ(irqn);

    uint8_t pin = _pin_num(dev);
    PORT_Type *port = _port(dev);

    config->cb = cb;
    config->arg = arg;
    config->irqc = irqc;
    /* Allow the callback to be found by the IRQ handler by setting the proper
     * pin number */
    config->pin = pin;

    port->PCR[pin] &= ~(PORT_PCR_IRQC_MASK); /* Disable interrupt */
    BITBAND_REG32(port->PCR[pin], PORT_PCR_ISF_SHIFT) = 1; /* Clear interrupt flag */
    port->PCR[pin] |= config->irqc; /* Enable interrupt */

    return 0;
}

void gpio_irq_enable(gpio_t dev)
{
    /* Restore saved state */
    PORT_Type *port = _port(dev);
    gpio_int_config_entry_t* entry = NULL;
    uint8_t pin_number = _pin_num(dev);
    mutex_lock(&int_config_lock);
    /* Search for the given pin in the port's interrupt configuration */
    LL_SEARCH_SCALAR(gpio_interrupts[_port_num(dev)], entry, pin, pin_number);
    uint32_t irqc = entry->irqc;
    mutex_unlock(&int_config_lock);
    if (entry == NULL) {
        /* Pin has not been configured for interrupts */
        return;
    }
    port->PCR[pin_number] &= ~(PORT_PCR_IRQC_MASK);
    port->PCR[pin_number] |= irqc;
}

void gpio_irq_disable(gpio_t dev)
{
    /* Save irqc state before disabling to allow enabling with the same trigger
     * settings later. */
    PORT_Type *port = _port(dev);
    uint8_t pin_number = _pin_num(dev);
    uint32_t irqc = PORT_PCR_IRQC_MASK & port->PCR[pin_number];
    gpio_int_config_entry_t* entry = NULL;
    mutex_lock(&int_config_lock);
    /* Search for the given pin in the port's interrupt configuration */
    LL_SEARCH_SCALAR(gpio_interrupts[_port_num(dev)], entry, pin, pin_number);
    if (entry == NULL) {
        /* Pin has not been configured for interrupts */
        mutex_unlock(&int_config_lock);
        return;
    }
    entry->irqc = irqc;
    mutex_unlock(&int_config_lock);
    port->PCR[pin_number] &= ~(PORT_PCR_IRQC_MASK);
}

int gpio_read(gpio_t dev)
{
    return ((_gpio(dev)->PDIR & GPIO_PDIR_PDI(1 << _pin_num(dev))) ? 1 : 0);
}

void gpio_set(gpio_t dev)
{
    _gpio(dev)->PSOR = (1 << _pin_num(dev));
}

void gpio_clear(gpio_t dev)
{
    _gpio(dev)->PCOR = (1 << _pin_num(dev));
}

void gpio_toggle(gpio_t dev)
{
    _gpio(dev)->PTOR = (1 << _pin_num(dev));
}

void gpio_write(gpio_t dev, int value)
{
    if (value) {
        _gpio(dev)->PSOR = (1 << _pin_num(dev));
    }
    else {
        _gpio(dev)->PCOR = (1 << _pin_num(dev));
    }
}

static inline void irq_handler(uint8_t port_num)
{
    gpio_int_config_entry_t *entry;
    PORT_Type *port = _port_ptrs[port_num];
    uint32_t isf = port->ISFR; /* Interrupt status flags */
    LL_FOREACH(gpio_interrupts[port_num], entry) {
        if (isf & (1 << entry->pin)) {
            if (entry->cb != NULL) {
                entry->cb(entry->arg);
            }
        }
    }
    /* Clear interrupt flags */
    port->ISFR = isf;

    if (sched_context_switch_request) {
        thread_yield();
    }
}

#ifdef PORTA_BASE
void isr_porta(void)
{
    irq_handler(PORT_A);
}
#endif /* PORTA_BASE */

#ifdef PORTB_BASE
void isr_portb(void)
{
    irq_handler(PORT_B);
}
#endif /* ISR_PORT_B */

#ifdef PORTC_BASE
void isr_portc(void)
{
    irq_handler(PORT_C);
}
#endif /* ISR_PORT_C */

#ifdef PORTD_BASE
void isr_portd(void)
{
    irq_handler(PORT_D);
}
#endif /* ISR_PORT_D */

#ifdef PORTE_BASE
void isr_porte(void)
{
    irq_handler(PORT_E);
}
#endif /* ISR_PORT_E */

#ifdef PORTF_BASE
void isr_portf(void)
{
    irq_handler(PORT_F);
}
#endif /* ISR_PORT_F */

#ifdef PORTG_BASE
void isr_portg(void)
{
    irq_handler(PORT_G);
}
#endif /* ISR_PORT_G */

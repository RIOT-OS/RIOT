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
 * @ingroup     cpu_kinetis
 * @ingroup     drivers_periph_gpio
 *
 * @{
 *
 * @file
 * @brief       Low-level GPIO driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Jonas Remmert <j.remmert@phytec.de>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include <stddef.h>
#include <stdint.h>
#include "cpu.h"
#include "bitarithm.h"
#include "bit.h"
#include "periph/gpio.h"

/* Single-port MCU*/
#if !defined(PORTA_BASE) && defined(PORT_BASE)
#  define PORTA_BASE PORT_BASE
#  define PORTA PORT
#endif

#ifndef PORT_PCR_ODE_MASK
/* For compatibility with Kinetis CPUs without open drain GPIOs (e.g. KW41Z) */
#define PORT_PCR_ODE_MASK 0
#endif

/**
 * @brief   Get the OCR reg value from the gpio_mode_t value
 */
#define MODE_PCR_MASK       (PORT_PCR_ODE_MASK | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK)

/**
 * @brief   This bit in the mode is set to 1 for output configuration
 */
#define MODE_OUT            (0x80)

/**
 * @brief   Shifting a gpio_t value by this number of bit we can extract the
 *          port number from the GPIO base address
 */
#define GPIO_SHIFT          (6)

/**
 * @brief   Mask used to extract the PORT base address from the gpio_t value
 */
#define PORT_ADDR_MASK      (0x00007000)

/**
 * @brief   Mask used to extract the GPIO base address from the gpio_t value
 */
#define GPIO_ADDR_MASK      (0x000001c0)

/**
 * @brief   Cleaned up PORT base address
 */
#define PORT_ADDR_BASE      (PORTA_BASE & ~(PORT_ADDR_MASK))

/**
 * @brief   Cleaned up GPIO base address
 */
#define GPIO_ADDR_BASE      (GPIOA_BASE & ~(GPIO_ADDR_MASK))

/**
 * @brief   Kinetis CPUs have 32 pins per port
 */
#define PINS_PER_PORT       (32)

#ifdef MODULE_PERIPH_GPIO_IRQ
/**
 * @brief   Calculate the needed memory (in byte) needed to save 4 bits per MCU
 *          pin
 */
#define ISR_MAP_SIZE        (GPIO_PORTS_NUMOF * PINS_PER_PORT * 4 / 8)

/**
 * @brief   Define the number of simultaneously configurable interrupt channels
 *
 * We have configured 4-bits per pin, so we can go up to 16 simultaneous active
 * extern interrupt sources.
 */
#define CTX_NUMOF           (8U)

/**
 * @brief   Interrupt context data
 */
typedef struct {
    gpio_cb_t cb;
    void *arg;
    uint32_t state;
} isr_ctx_t;

/**
 * @brief   Allocation of memory for each independent interrupt slot
 *
 * We trust the start-up code here to initialize all bytes of this array to
 * zero.
 */
static isr_ctx_t isr_ctx[CTX_NUMOF];

/**
 * @brief   Allocation of 4 bit per pin to map a pin to an interrupt context
 */
static uint32_t isr_map[ISR_MAP_SIZE];

static const uint8_t port_irqs[] = PORT_IRQS;
#endif /* MODULE_PERIPH_GPIO_IRQ */

static inline PORT_Type *port(gpio_t pin)
{
    return (PORT_Type *)(PORT_ADDR_BASE | (pin & PORT_ADDR_MASK));
}

static inline GPIO_Type *gpio(gpio_t pin)
{
    return (GPIO_Type *)(GPIO_ADDR_BASE | (pin & GPIO_ADDR_MASK));
}

static inline int port_num(gpio_t pin)
{
    return (int)((pin >> GPIO_SHIFT) & 0x7);
}

static inline int pin_num(gpio_t pin)
{
    return (int)(pin & 0x3f);
}

#ifdef MODULE_PERIPH_GPIO_IRQ

/**
 * @brief   Get context for a specific pin
 */
static inline int get_ctx(int port, int pin)
{
    return (isr_map[(port * 4) + (pin >> 3)] >> ((pin & 0x7) * 4)) & 0xf;
}

/**
 * @brief   Find a free spot in the array containing the interrupt contexts
 */
static int get_free_ctx(void)
{
    for (unsigned int i = 0; i < CTX_NUMOF; i++) {
        if (isr_ctx[i].cb == NULL) {
            return i;
        }
    }
    return -1;
}

/**
 * @brief   Write an entry to the context map array
 */
static void write_map(int port, int pin, int ctx)
{
    isr_map[(port * 4) + (pin >> 3)] &= ~(0xf << ((pin & 0x7) * 4));
    isr_map[(port * 4) + (pin >> 3)] |=  (ctx << ((pin & 0x7) * 4));
}

/**
 * @brief   Clear the context for the given pin
 */
static void ctx_clear(int port, int pin)
{
    int ctx = get_ctx(port, pin);

    write_map(port, pin, ctx);
}
#endif /* MODULE_PERIPH_GPIO_IRQ */

static inline void clk_en(gpio_t pin)
{
#if defined(SIM_SCGC5_PORTA_SHIFT)
    bit_set32(&SIM->SCGC5, SIM_SCGC5_PORTA_SHIFT + port_num(pin));
#else
    /* In some cases GPIO is always clocked */
    (void) pin;
#endif

}

int gpio_init(gpio_t pin, gpio_mode_t mode)
{
#ifdef KINETIS_HAVE_PCR
    /* set pin to analog mode while configuring it */
    gpio_init_port(pin, GPIO_AF_ANALOG);
#endif

    /* set pin direction */
    if (mode & MODE_OUT) {
        gpio(pin)->PDDR |=  (1 << pin_num(pin));
    }
    else {
        gpio(pin)->PDDR &= ~(1 << pin_num(pin));
    }

#ifdef KINETIS_HAVE_PCR
    /* enable GPIO function */
    port(pin)->PCR[pin_num(pin)] = (GPIO_AF_GPIO | (mode & MODE_PCR_MASK));
#endif
    return 0;
}

void gpio_init_port(gpio_t pin, uint32_t pcr)
{
    /* enable PORT clock in case it was not active before */
    clk_en(pin);

#ifdef KINETIS_HAVE_PCR
#ifdef MODULE_PERIPH_GPIO_IRQ
    /* if the given interrupt was previously configured as interrupt source, we
     * need to free its interrupt context. We to this only after we
     * re-configured the pin in case an event is happening just in between... */
    uint32_t isr_state = port(pin)->PCR[pin_num(pin)];
#endif /* MODULE_PERIPH_GPIO_IRQ */

    /* set new PCR value */
    port(pin)->PCR[pin_num(pin)] = pcr;

#ifdef MODULE_PERIPH_GPIO_IRQ
    /* and clear the interrupt context if needed */
    if (isr_state & PORT_PCR_IRQC_MASK) {
        ctx_clear(port_num(pin), pin_num(pin));
    }
#endif /* MODULE_PERIPH_GPIO_IRQ */
#else
    (void) pcr;
#endif /* KINETIS_HAVE_PCR */
}

int gpio_read(gpio_t pin)
{
    if (gpio(pin)->PDDR & (1 << pin_num(pin))) {
        return (gpio(pin)->PDOR & (1 << pin_num(pin))) ? 1 : 0;
    }
    else {
        return (gpio(pin)->PDIR & (1 << pin_num(pin))) ? 1 : 0;
    }
}

void gpio_set(gpio_t pin)
{
    gpio(pin)->PSOR = (1 << pin_num(pin));
}

void gpio_clear(gpio_t pin)
{
    gpio(pin)->PCOR = (1 << pin_num(pin));
}

void gpio_toggle(gpio_t pin)
{
    gpio(pin)->PTOR = (1 << pin_num(pin));
}

void gpio_write(gpio_t pin, bool value)
{
    if (value) {
        gpio(pin)->PSOR = (1 << pin_num(pin));
    }
    else {
        gpio(pin)->PCOR = (1 << pin_num(pin));
    }
}

#ifdef MODULE_PERIPH_GPIO_IRQ
int gpio_init_int(gpio_t pin, gpio_mode_t mode, gpio_flank_t flank,
                  gpio_cb_t cb, void *arg)
{
    if (gpio_init(pin, mode) < 0) {
        return -1;
    }

    /* try go grab a free spot in the context array */
    int ctx_num = get_free_ctx();
    if (ctx_num < 0) {
        return -1;
    }

    /* save interrupt context */
    isr_ctx[ctx_num].cb = cb;
    isr_ctx[ctx_num].arg = arg;
    isr_ctx[ctx_num].state = flank;
    write_map(port_num(pin), pin_num(pin), ctx_num);

    /* clear interrupt flags */
    port(pin)->ISFR &= ~(1 << pin_num(pin));

    /* enable global port interrupts in the NVIC */
    NVIC_EnableIRQ(port_irqs[port_num(pin)]);

    /* finally, enable the interrupt for the selected pin */
    port(pin)->PCR[pin_num(pin)] |= flank;
    return 0;
}

void gpio_irq_enable(gpio_t pin)
{
    int ctx = get_ctx(port_num(pin), pin_num(pin));
    port(pin)->PCR[pin_num(pin)] |= isr_ctx[ctx].state;
}

void gpio_irq_disable(gpio_t pin)
{
    int ctx = get_ctx(port_num(pin), pin_num(pin));
    isr_ctx[ctx].state = port(pin)->PCR[pin_num(pin)] & PORT_PCR_IRQC_MASK;
    port(pin)->PCR[pin_num(pin)] &= ~(PORT_PCR_IRQC_MASK);
}

static inline void irq_handler(PORT_Type *port, int port_num)
{
    /* take interrupt flags only from pins which interrupt is enabled */
    uint32_t status = port->ISFR;

    while (status) {
        /* get position of first bit set in status */
        unsigned pin = bitarithm_lsb(status);
        /* clear it */
        status &= ~(1 << pin);
        if (port->PCR[pin] & PORT_PCR_IRQC_MASK) {
            port->ISFR = (1u << pin);
            int ctx = get_ctx(port_num, pin);
            gpio_cb_t cb = isr_ctx[ctx].cb;
            if (cb) {
                cb(isr_ctx[ctx].arg);
            }
        }
    }
}

#ifdef PORTA_BASE
void isr_porta(void)
{
    irq_handler(PORTA, 0);
    cortexm_isr_end();
}
#endif /* PORTA_BASE */

#ifdef PORTB_BASE
void isr_portb(void)
{
    irq_handler(PORTB, 1);
    cortexm_isr_end();
}
#endif /* ISR_PORT_B */

#ifdef PORTC_BASE
void isr_portc(void)
{
    irq_handler(PORTC, 2);
    cortexm_isr_end();
}
#endif /* ISR_PORT_C */

#ifdef PORTD_BASE
void isr_portd(void)
{
    irq_handler(PORTD, 3);
    cortexm_isr_end();
}
#endif /* ISR_PORT_D */

#ifdef PORTE_BASE
void isr_porte(void)
{
    irq_handler(PORTE, 4);
    cortexm_isr_end();
}
#endif /* ISR_PORT_E */

#ifdef PORTF_BASE
void isr_portf(void)
{
    irq_handler(PORTF, 5);
    cortexm_isr_end();
}
#endif /* ISR_PORT_F */

#ifdef PORTG_BASE
void isr_portg(void)
{
    irq_handler(PORTG, 6);
    cortexm_isr_end();
}
#endif /* ISR_PORT_G */

#if defined(PORTB_BASE) && defined(PORTC_BASE)
/* Combined ISR used in certain KL devices */
void isr_portb_portc(void)
{
    irq_handler(PORTB, 1);
    irq_handler(PORTC, 2);
    cortexm_isr_end();
}
#endif
#if defined(PORTC_BASE) && defined(PORTD_BASE)
/* Combined ISR used in certain KL devices */
void isr_portc_portd(void)
{
    irq_handler(PORTC, 2);
    irq_handler(PORTD, 3);
}
#endif
#endif /* MODULE_PERIPH_GPIO_IRQ */

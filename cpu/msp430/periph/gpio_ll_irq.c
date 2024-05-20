/*
 * Copyright (C) 2024 Marian Buschsieweke
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_msp430
 * @ingroup     drivers_periph_gpio_ll_irq
 * @{
 *
 * @file
 * @brief       IRQ implementation of the GPIO Low-Level API for MSP430
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>
 *
 * @}
 */

#include <errno.h>
#include <stdbool.h>

#include "compiler_hints.h"
#include "container.h"
#include "periph/gpio_ll.h"
#include "periph/gpio_ll_irq.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define ISR_PORTS_NUMOF     2U
#define PINS_PER_PORT       8U
#define ISR_NUMOF           (ISR_PORTS_NUMOF * PINS_PER_PORT)

struct isr_ctx {
    gpio_ll_cb_t cb;
    void *arg;
};

static struct isr_ctx _isr_ctx[ISR_NUMOF];

static unsigned _idx(uword_t port_num, uint8_t pin)
{
    if (port_num == 2) {
        return pin + 8;
    }

    return pin;
}

int gpio_ll_irq(gpio_port_t _port, uint8_t pin, gpio_irq_trig_t trig,
                gpio_ll_cb_t cb, void *arg)
{
    uword_t port_num = gpio_port_num(_port);
    if (unlikely((port_num > 2) || (port_num == 0))) {
        DEBUG_PUTS("[gpio_ll_irq] GPIO port without IRQ support used");
        return -ENOTSUP;
    }

    switch (trig) {
    case GPIO_TRIGGER_EDGE_RISING:
    case GPIO_TRIGGER_EDGE_FALLING:
        break;
    default:
        DEBUG_PUTS("[gpio_ll_irq] unsupported trigger used");
        return -ENOTSUP;
    }

    msp430_port_p1_p2_t *port = container_of((msp430_port_t *)_port, msp430_port_p1_p2_t, base);

    unsigned idx = _idx(port_num, pin);
    uword_t mask = 1U << pin;

    /* Disable IRQs to avoid race when updating CTX. Relying on compiler to
     * use BIC.B instruction to implement this, which is inherently atomic. */
    port->IE &= ~mask;
    _isr_ctx[idx].cb = cb;
    _isr_ctx[idx].arg = arg;

    /* Configuring interrupt edge select. Relying on atomic BIS.B and BIC.B
     * instructions used by the compiler */
    port->IES &= ~mask;
    if (trig == GPIO_TRIGGER_EDGE_FALLING) {
        port->IES |= mask;
    }

    /* Clear pending IRQs, relying on atomic BIC.B instruction used by the
     * compiler. */
    port->IFG &= ~mask;

    /* Finally, enabling the IRQ, relying on atomic BIS.B instruction used by
     * the compiler */
    port->IE |= mask;

    return 0;
}

/* It appears that disabling the interrupt in IE does not only mask IRQs,
 * but disables the IRQ edge detector hardware completely. Hence, IRQs
 * that came in while masked will not trigger after unmasking, as the
 * API would expect. For this reason, we don't provide gpio_ll_irq_unmask() but
 * only gpio_ll_irq_unmask_and_clear().
 */
void gpio_ll_irq_mask(gpio_port_t _port, uint8_t pin)
{
    msp430_port_p1_p2_t *port = container_of((msp430_port_t *)_port, msp430_port_p1_p2_t, base);
    port->IE &= ~(1U << pin);
}

void gpio_ll_irq_unmask_and_clear(gpio_port_t _port, uint8_t pin)
{
    msp430_port_p1_p2_t *port = container_of((msp430_port_t *)_port, msp430_port_p1_p2_t, base);
    uword_t mask = 1U << pin;
    /* We clear IFG anyway despite bits in IFG not getting set without IE, a
     * call may rely on this function clearing IRQs while already unmasked. */
    port->IFG &= ~mask;
    port->IE |= mask;
}

static void _isr_handler(msp430_port_p1_p2_t *port, int ctx)
{
    for (unsigned i = 0; i < PINS_PER_PORT; i++) {
        unsigned mask = 1U << i;
        if ((port->IE & mask) && (port->IFG & mask)) {
            port->IFG &= ~mask;
            _isr_ctx[i + ctx].cb(_isr_ctx[i + ctx].arg);
        }
    }
}

ISR(PORT1_VECTOR, isr_port1)
{
    __enter_isr();
    _isr_handler(&PORT_1, 0);
    __exit_isr();
}

ISR(PORT2_VECTOR, isr_port2)
{
    __enter_isr();
    _isr_handler(&PORT_2, 8);
    __exit_isr();
}

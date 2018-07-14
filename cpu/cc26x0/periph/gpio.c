/*
 * Copyright (C) 2016 Leon George
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cc26x0
 * @ingroup     drivers_periph_gpio
 * @{
 *
 * @file
 * @brief       Low-level GPIO driver implementation
 *
 * @author      Leon M. George <leon@georgemail.eu>
 *
 * @}
 */

#include "cpu.h"
#include "periph/gpio.h"

#define GPIO_ISR_CHAN_NUMOF     (32)

#define DOE_SHIFT               (29U)

/**
 * @brief   static callback memory
 */
static gpio_isr_ctx_t gpio_chan[GPIO_ISR_CHAN_NUMOF];

int gpio_init_ll(gpio_t pin, gpio_mode_t mode)
{
    if ((unsigned int)pin > 31)
        return -1;

    /* enable GPIO clock */
    PRCM->PDCTL0 |= PDCTL0_PERIPH_ON;
    while(!(PRCM->PDSTAT0 & PDSTAT0_PERIPH_ON)) ;
    PRCM->GPIOCLKGR |= 1;
    PRCM->CLKLOADCTL |= CLKLOADCTL_LOAD;
    while (!(PRCM->CLKLOADCTL & CLKLOADCTL_LOADDONE)) ;

    /* configure the GPIO mode */
    IOC->CFG[pin] = mode;
    GPIO->DOE &= ~(1 << pin);
    GPIO->DOE |= ((~(mode >> DOE_SHIFT) & 0x1) << pin);

    return 0;
}

int gpio_init_int_ll(gpio_t pin, gpio_mode_t mode, gpio_flank_t flank,
                     gpio_cb_t cb, void *arg)
{
    int init = gpio_init_ll(pin, mode);
    if (init != 0)
        return init;

    NVIC_EnableIRQ(EDGE_DETECT_IRQN);

    IOC->CFG[pin] |= flank;

    gpio_chan[pin].cb = cb;
    gpio_chan[pin].arg = arg;

    /* clears the interrupt flag */
    GPIO->EVFLAGS |= (1 << pin);

    gpio_irq_enable_ll(pin);

    return 0;
}

void gpio_irq_enable_ll(gpio_t pin)
{
    IOC->CFG[pin] |= IOCFG_EDGEIRQ_ENABLE;
}

void gpio_irq_disable_ll(gpio_t pin)
{
    IOC->CFG[pin] &= ~IOCFG_EDGEIRQ_ENABLE;
}

int gpio_read_ll(gpio_t pin)
{
    if (GPIO->DOE & (1 << pin)) {
        return (GPIO->DOUT >> pin) & 1;
    }
    else {
        return (GPIO->DIN >> pin) & 1;
    }
}

void gpio_set_ll(gpio_t pin)
{
    GPIO->DOUTSET = (1 << pin);
}

void gpio_clear_ll(gpio_t pin)
{
    GPIO->DOUTCLR = (1 << pin);
}

void gpio_toggle_ll(gpio_t pin)
{
    GPIO->DOUTTGL = (1 << pin);
}

void gpio_write_ll(gpio_t pin, int value)
{
    if (value) {
        GPIO->DOUTSET = (1 << pin);
    } else {
        GPIO->DOUTCLR = (1 << pin);
    }
}

void isr_edge(void)
{
    for (unsigned pin = 0; pin < GPIO_ISR_CHAN_NUMOF; pin++) {
        /* doc claims EVFLAGS will only be set for pins that have edge detection enabled */
        if (GPIO->EVFLAGS & (1 << pin)) {
            GPIO->EVFLAGS |= (1 << pin);
            gpio_chan[pin].cb(gpio_chan[pin].arg);
        }
    }
    cortexm_isr_end();
}

/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_nrf51822
 * @{
 *
 * @file
 * @brief       Low-level GPIO driver implementation
 *
 * NOTE: this GPIO driver implementation supports due to hardware limitations
 *       only one pin configured as external interrupt source at a time!
 *
 * @author      Christian Kühling <kuehling@zedat.fu-berlin.de>
 * @author      Timo Ziegler <timo.ziegler@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "sched.h"
#include "thread.h"
#include "periph/gpio.h"
#include "periph_conf.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

static uint8_t index = 0;

/**
 * @brief   Place to store the interrupt context
 */
static gpio_isr_ctx_t exti_chan[4];

int gpio_init(gpio_t pin, gpio_dir_t dir, gpio_pp_t pullup)
{
    /* configure pin direction, input buffer and pull resistor state */
    NRF_GPIO->PIN_CNF[pin] = ((dir << GPIO_PIN_CNF_DIR_Pos) |
                              (dir << GPIO_PIN_CNF_INPUT_Pos) |
                              (pullup << GPIO_PIN_CNF_PULL_Pos));
    return 0;
}

int gpio_init_int(gpio_t pin, gpio_pp_t pullup, gpio_flank_t flank,
                    gpio_cb_t cb, void *arg)
{
    /* disable external interrupt in case one is active */
    //NRF_GPIOTE->INTENSET &= ~(GPIOTE_INTENSET_IN0_Msk);
    NRF_GPIOTE->INTENSET &= ~(0x1UL << index);

    /* save callback */
    exti_chan[index].cb = cb;
    exti_chan[index].arg = arg;


    /* configure pin as input */
    gpio_init(pin, GPIO_DIR_IN, pullup);
    /* set interrupt priority and enable global GPIOTE interrupt */
    NVIC_EnableIRQ(GPIOTE_IRQn);
    /* configure the GPIOTE channel: set even mode, pin and active flank */
    NRF_GPIOTE->CONFIG[index] = (GPIOTE_CONFIG_MODE_Event |
                             (pin << GPIOTE_CONFIG_PSEL_Pos) |
                             (flank << GPIOTE_CONFIG_POLARITY_Pos));
    /* enable external interrupt */
    //NRF_GPIOTE->INTENSET |= GPIOTE_INTENSET_IN0_Msk;
    NRF_GPIOTE->INTENSET |= (0x1UL << index);

    // point to the next free isr context slot
    index++;

    return 0;
}

/*
 * the gpio_init_mux function is not defined as it is not needed for this CPU
 */

void gpio_irq_enable(gpio_t pin)
{
    (void) pin;
    NRF_GPIOTE->INTENSET |= GPIOTE_INTENSET_IN0_Msk|GPIOTE_INTENSET_IN1_Msk;
}

void gpio_irq_disable(gpio_t dev)
{
    (void) dev;
    NRF_GPIOTE->INTENCLR |= GPIOTE_INTENSET_IN0_Msk|GPIOTE_INTENSET_IN1_Msk;
}

int gpio_read(gpio_t pin)
{
    if (NRF_GPIO->DIR & (1 << pin)) {
        return (NRF_GPIO->OUT & (1 << pin)) ? 1 : 0;
    }
    else {
        return (NRF_GPIO->IN & (1 << pin)) ? 1 : 0;
    }
}

void gpio_set(gpio_t pin)
{
    NRF_GPIO->OUTSET = (1 << pin);
}

void gpio_clear(gpio_t pin)
{
    NRF_GPIO->OUTCLR = (1 << pin);
}

void gpio_toggle(gpio_t pin)
{
    NRF_GPIO->OUT ^= (1 << pin);
}

void gpio_write(gpio_t pin, int value)
{
    if (value) {
        NRF_GPIO->OUTSET = (1 << pin);
    } else {
        NRF_GPIO->OUTCLR = (1 << pin);
    }
}

void isr_gpiote(void)
{
	for(int i=0; i<4;i++) {
	    if (NRF_GPIOTE->EVENTS_IN[i] == 1)
	    {
	        NRF_GPIOTE->EVENTS_IN[i] = 0;
	        exti_chan[i].cb(exti_chan[i].arg);
	    }
	}
    if (sched_context_switch_request) {
        thread_yield();
    }
}

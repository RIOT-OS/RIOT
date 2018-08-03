/*
 * Copyright (C) 2015 Jan Wagner <mail@jwagner.eu>
 *               2015-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_nrf5x_common
 * @ingroup     drivers_periph_gpio
 * @{
 *
 * @file
 * @brief       Low-level GPIO driver implementation
 *
 * @note        This GPIO driver implementation supports only one pin to be
 *              defined as external interrupt.
 *
 * @author      Christian Kühling <kuehling@zedat.fu-berlin.de>
 * @author      Timo Ziegler <timo.ziegler@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Jan Wagner <mail@jwagner.eu>
 *
 * @}
 */

#include "cpu.h"
#include "periph/gpio.h"
#include "periph_cpu.h"
#include "periph_conf.h"

#define PORT_BIT            (1 << 5)
#define PIN_MASK            (0x1f)

/**
 * @brief   Place to store the interrupt context
 */
static gpio_isr_ctx_t exti_chan;

/**
 * @brief   Get the port's base address
 */
static inline NRF_GPIO_Type* port(gpio_t pin)
{
#if (CPU_FAM_NRF51)
    (void) pin;
    return NRF_GPIO;
#elif defined(CPU_MODEL_NRF52832XXAA)
    (void) pin;
    return NRF_P0;
#else
    return (pin & PORT_BIT) ? NRF_P1 : NRF_P0;
#endif
}

int gpio_init(gpio_t pin, gpio_mode_t mode)
{
    switch (mode) {
        case GPIO_IN:
        case GPIO_IN_PD:
        case GPIO_IN_PU:
        case GPIO_OUT:
            /* configure pin direction, input buffer and pull resistor state */
            port(pin)->PIN_CNF[pin] = mode;
            break;
        default:
            return -1;
    }

    return 0;
}

int gpio_init_int(gpio_t pin, gpio_mode_t mode, gpio_flank_t flank,
                  gpio_cb_t cb, void *arg)
{
    /* disable external interrupt in case one is active */
    NRF_GPIOTE->INTENSET &= ~(GPIOTE_INTENSET_IN0_Msk);
    /* save callback */
    exti_chan.cb = cb;
    exti_chan.arg = arg;
    /* configure pin as input */
    gpio_init(pin, mode);
    /* set interrupt priority and enable global GPIOTE interrupt */
    NVIC_EnableIRQ(GPIOTE_IRQn);
    /* configure the GPIOTE channel: set even mode, pin and active flank */
    NRF_GPIOTE->CONFIG[0] = (GPIOTE_CONFIG_MODE_Event |
                             (pin << GPIOTE_CONFIG_PSEL_Pos) |
#ifdef CPU_MODEL_NRF52840XXAA
                             ((pin & PORT_BIT) << 8) |
#endif
                             (flank << GPIOTE_CONFIG_POLARITY_Pos));
    /* enable external interrupt */
    NRF_GPIOTE->INTENSET |= GPIOTE_INTENSET_IN0_Msk;
    return 0;
}

void gpio_irq_enable(gpio_t pin)
{
    (void) pin;
    NRF_GPIOTE->INTENSET |= GPIOTE_INTENSET_IN0_Msk;
}

void gpio_irq_disable(gpio_t pin)
{
    (void) pin;
    NRF_GPIOTE->INTENCLR |= GPIOTE_INTENSET_IN0_Msk;
}

int gpio_read(gpio_t pin)
{
    if (port(pin)->DIR & (1 << pin)) {
        return (port(pin)->OUT & (1 << pin)) ? 1 : 0;
    }
    else {
        return (port(pin)->IN & (1 << pin)) ? 1 : 0;
    }
}

void gpio_set(gpio_t pin)
{
    port(pin)->OUTSET = (1 << pin);
}

void gpio_clear(gpio_t pin)
{
    port(pin)->OUTCLR = (1 << pin);
}

void gpio_toggle(gpio_t pin)
{
    port(pin)->OUT ^= (1 << pin);
}

void gpio_write(gpio_t pin, int value)
{
    if (value) {
        port(pin)->OUTSET = (1 << pin);
    } else {
        port(pin)->OUTCLR = (1 << pin);
    }
}

void isr_gpiote(void)
{
    if (NRF_GPIOTE->EVENTS_IN[0] == 1) {
        NRF_GPIOTE->EVENTS_IN[0] = 0;
        exti_chan.cb(exti_chan.arg);
    }
    cortexm_isr_end();
}

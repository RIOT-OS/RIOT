/*
 * Copyright (C) 2015 Jan Wagner <mail@jwagner.eu>
 *               2015-2016 Freie Universität Berlin
 *               2019 Inria
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
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include "cpu.h"
#include "periph/gpio.h"
#include "periph_cpu.h"
#include "periph_conf.h"

#define PORT_BIT            (1 << 5)
#define PIN_MASK            (0x1f)

#ifdef MODULE_PERIPH_GPIO_IRQ

#if CPU_FAM_NRF51
#define GPIOTE_CHAN_NUMOF     (4U)
#else
#define GPIOTE_CHAN_NUMOF     (8U)
#endif

/**
 * @brief   Index of next interrupt in GPIOTE channel list.
 *
 * The index is incremented at the end of each call to gpio_init_int.
 * The index cannot be greater or equal than GPIOTE_CHAN_NUMOF.
 */
static uint8_t _gpiote_next_index = 0;

/**
 * @brief   Array containing a mapping between GPIOTE channel and pin
 */
static gpio_t _exti_pins[GPIOTE_CHAN_NUMOF];

/**
 * @brief   Place to store the interrupt context
 */
static gpio_isr_ctx_t exti_chan[GPIOTE_CHAN_NUMOF];
#endif /* MODULE_PERIPH_GPIO_IRQ */

/**
 * @brief   Get the port's base address
 */
static inline NRF_GPIO_Type *port(gpio_t pin)
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

/**
 * @brief   Get a pin's offset
 */
static inline int pin_num(gpio_t pin)
{
#ifdef CPU_MODEL_NRF52840XXAA
    return (pin & PIN_MASK);
#else
    return (int)pin;
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
            port(pin)->PIN_CNF[pin_num(pin)] = mode;
            break;
        default:
            return -1;
    }

    return 0;
}

int gpio_read(gpio_t pin)
{
    if (port(pin)->DIR & (1 << pin_num(pin))) {
        return (port(pin)->OUT & (1 << pin_num(pin))) ? 1 : 0;
    }
    else {
        return (port(pin)->IN & (1 << pin_num(pin))) ? 1 : 0;
    }
}

void gpio_set(gpio_t pin)
{
    port(pin)->OUTSET = (1 << pin_num(pin));
}

void gpio_clear(gpio_t pin)
{
    port(pin)->OUTCLR = (1 << pin_num(pin));
}

void gpio_toggle(gpio_t pin)
{
    port(pin)->OUT ^= (1 << pin_num(pin));
}

void gpio_write(gpio_t pin, int value)
{
    if (value) {
        port(pin)->OUTSET = (1 << pin_num(pin));
    }
    else {
        port(pin)->OUTCLR = (1 << pin_num(pin));
    }
}

#ifdef MODULE_PERIPH_GPIO_IRQ
int gpio_init_int(gpio_t pin, gpio_mode_t mode, gpio_flank_t flank,
                  gpio_cb_t cb, void *arg)
{
    uint8_t _pin_index = 0xff;
    /* Looking for already known pin in exti table */
    for (unsigned int i = 0; i < _gpiote_next_index; i++) {
        if (_exti_pins[i] == pin) {
            _pin_index = i;
            break;
        }
    }

    /* New pin */
    if (_pin_index == 0xff) {
        assert(_gpiote_next_index < GPIOTE_CHAN_NUMOF);
        _pin_index = _gpiote_next_index;
        /* associate the current pin with channel index */
        _exti_pins[_pin_index] = pin;
        /* increase next index for next pin initialization */
        _gpiote_next_index++;
    }

    /* save callback */
    exti_chan[_pin_index].cb = cb;
    exti_chan[_pin_index].arg = arg;
    /* configure pin as input */
    gpio_init(pin, mode);
    /* set interrupt priority and enable global GPIOTE interrupt */
    NVIC_EnableIRQ(GPIOTE_IRQn);
    /* configure the GPIOTE channel: set even mode, pin and active flank */
    NRF_GPIOTE->CONFIG[_pin_index] = (GPIOTE_CONFIG_MODE_Event |
                             (pin_num(pin) << GPIOTE_CONFIG_PSEL_Pos) |
#ifdef CPU_MODEL_NRF52840XXAA
                             ((pin & PORT_BIT) << 8) |
#endif
                             (flank << GPIOTE_CONFIG_POLARITY_Pos));
    /* enable external interrupt */
    NRF_GPIOTE->INTENSET |= (GPIOTE_INTENSET_IN0_Msk << _pin_index);

    return 0;
}

void gpio_irq_enable(gpio_t pin)
{
    for (unsigned int i = 0; i < _gpiote_next_index; i++) {
        if (_exti_pins[i] == pin) {
            NRF_GPIOTE->CONFIG[i] |= GPIOTE_CONFIG_MODE_Event;
            NRF_GPIOTE->INTENSET |= (GPIOTE_INTENSET_IN0_Msk << i);
            break;
        }
    }
}

void gpio_irq_disable(gpio_t pin)
{
    for (unsigned int i = 0; i < _gpiote_next_index; i++) {
        if (_exti_pins[i] == pin) {
            /* Clear mode configuration: 00 = Disabled */
            NRF_GPIOTE->CONFIG[i] &= ~(GPIOTE_CONFIG_MODE_Msk);
            NRF_GPIOTE->INTENCLR = (GPIOTE_INTENCLR_IN0_Msk << i);
            break;
        }
    }
}

void isr_gpiote(void)
{
    for (unsigned int i = 0; i < _gpiote_next_index; ++i) {
        if (NRF_GPIOTE->EVENTS_IN[i] == 1) {
            NRF_GPIOTE->EVENTS_IN[i] = 0;
            exti_chan[i].cb(exti_chan[i].arg);
            break;
        }
    }
    cortexm_isr_end();
}
#endif /* MODULE_PERIPH_GPIO_IRQ */

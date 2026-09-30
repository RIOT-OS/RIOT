/*
 * SPDX-FileCopyrightText: 2026 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_nrf54l
 * @ingroup     drivers_periph_gpio
 * @{
 *
 * @file
 * @brief       Low-level GPIO driver implementation for the nRF54L family
 *
 * The nRF54L family provides three GPIO ports located in different power
 * domains, hence with non-contiguous base addresses. External interrupts are
 * provided by two GPIOTE instances: GPIOTE20 serves port P1 (8 channels) and
 * GPIOTE30 serves port P0 (4 channels). Port P2 has no GPIOTE instance, so
 * pins on this port cannot be configured as external interrupts.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <assert.h>

#include "cpu.h"
#include "periph/gpio.h"
#include "periph_cpu.h"
#include "periph_conf.h"

#define PORT_POS            (5U)
#define PIN_MASK            (0x1f)

/**
 * @brief   GPIO port base addresses, indexed by port number
 */
static NRF_GPIO_Type *const _port[GPIO_COUNT] = {
    NRF_P0,
    NRF_P1,
    NRF_P2,
};

/**
 * @brief   Get the port's base address
 */
static inline NRF_GPIO_Type *port(gpio_t pin)
{
    return _port[pin >> PORT_POS];
}

/**
 * @brief   Get a pin's offset
 */
static inline int pin_num(gpio_t pin)
{
    return (pin & PIN_MASK);
}

int gpio_init(gpio_t pin, gpio_mode_t mode)
{
    switch (mode) {
        case GPIO_IN:
        case GPIO_IN_PD:
        case GPIO_IN_PU:
        case GPIO_IN_OD_PU:
        case GPIO_OUT:
            /* configure pin direction, input buffer, pull resistor state
             * and drive configuration */
            port(pin)->PIN_CNF[pin_num(pin)] = mode;
            break;
        default:
            return -1;
    }

    return 0;
}

bool gpio_read(gpio_t pin)
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

void gpio_write(gpio_t pin, bool value)
{
    if (value) {
        port(pin)->OUTSET = (1 << pin_num(pin));
    }
    else {
        port(pin)->OUTCLR = (1 << pin_num(pin));
    }
}

#ifdef MODULE_PERIPH_GPIO_IRQ

#  define GPIOTE20_CHAN_NUMOF (8U)
#  define GPIOTE30_CHAN_NUMOF (4U)

/**
 * @brief   Per GPIOTE instance interrupt state
 */
typedef struct {
    NRF_GPIOTE_Type *dev;                       /**< GPIOTE instance */
    IRQn_Type irqn;                             /**< IRQ line 1 of the instance */
    uint8_t chan_numof;                         /**< available channels */
    uint8_t next_index;                         /**< next free channel */
    gpio_t pins[GPIOTE20_CHAN_NUMOF];           /**< channel to pin mapping */
    gpio_isr_ctx_t ctx[GPIOTE20_CHAN_NUMOF];    /**< interrupt contexts */
} gpiote_state_t;

static gpiote_state_t _gpiote[] = {
    {
        .dev = NRF_GPIOTE30,
        .irqn = GPIOTE30_1_IRQn,
        .chan_numof = GPIOTE30_CHAN_NUMOF,
    },
    {
        .dev = NRF_GPIOTE20,
        .irqn = GPIOTE20_1_IRQn,
        .chan_numof = GPIOTE20_CHAN_NUMOF,
    },
};

/**
 * @brief   Get the GPIOTE instance state serving a given pin
 *
 * @retval  NULL if the pin's port has no GPIOTE instance (port P2)
 */
static gpiote_state_t *gpiote(gpio_t pin)
{
    switch (pin >> PORT_POS) {
        case 0:
            return &_gpiote[0];
        case 1:
            return &_gpiote[1];
        default:
            return NULL;
    }
}

static uint8_t _get_channel(gpiote_state_t *state, gpio_t pin)
{
    for (unsigned i = 0; i < state->next_index; i++) {
        if (state->pins[i] == pin) {
            return i;
        }
    }
    return 0xff;
}

uint8_t gpio_int_get_exti(gpio_t pin)
{
    gpiote_state_t *state = gpiote(pin);
    if (state == NULL) {
        return 0xff;
    }
    return _get_channel(state, pin);
}

int gpio_init_int(gpio_t pin, gpio_mode_t mode, gpio_flank_t flank,
                  gpio_cb_t cb, void *arg)
{
    gpiote_state_t *state = gpiote(pin);

    /* port P2 has no GPIOTE instance */
    if (state == NULL) {
        return -1;
    }

    uint8_t chan = _get_channel(state, pin);

    /* new pin */
    if (chan == 0xff) {
        assert(state->next_index < state->chan_numof);
        chan = state->next_index;
        /* associate the current pin with the channel */
        state->pins[chan] = pin;
        /* increase next index for next pin initialization */
        state->next_index++;
    }

    /* save callback */
    state->ctx[chan].cb = cb;
    state->ctx[chan].arg = arg;
    /* configure pin as input */
    gpio_init(pin, mode);
    /* set interrupt priority and enable global GPIOTE interrupt */
    NVIC_EnableIRQ(state->irqn);
    /* configure the GPIOTE channel: set event mode, pin, port and active
     * flank; the PSEL and PORT fields are adjacent, so the gpio_t encoding
     * (port << 5 | pin) can be written in one go */
    state->dev->CONFIG[chan] = ((GPIOTE_CONFIG_MODE_Event << GPIOTE_CONFIG_MODE_Pos) |
                                (pin << GPIOTE_CONFIG_PSEL_Pos) |
                                (flank << GPIOTE_CONFIG_POLARITY_Pos));
    /* enable external interrupt on IRQ line 1, the line assigned to the
     * secure application core (see GPIOTE_IRQ_GROUP in the nRF54L15 interim
     * vendor header) */
    state->dev->INTENSET1 = (GPIOTE_INTENSET1_IN0_Msk << chan);

    return 0;
}

void gpio_irq_enable(gpio_t pin)
{
    gpiote_state_t *state = gpiote(pin);
    if (state == NULL) {
        return;
    }

    uint8_t chan = _get_channel(state, pin);
    if (chan != 0xff) {
        state->dev->CONFIG[chan] |= (GPIOTE_CONFIG_MODE_Event << GPIOTE_CONFIG_MODE_Pos);
        state->dev->INTENSET1 = (GPIOTE_INTENSET1_IN0_Msk << chan);
    }
}

void gpio_irq_disable(gpio_t pin)
{
    gpiote_state_t *state = gpiote(pin);
    if (state == NULL) {
        return;
    }

    uint8_t chan = _get_channel(state, pin);
    if (chan != 0xff) {
        /* clear mode configuration: 00 = Disabled */
        state->dev->CONFIG[chan] &= ~(GPIOTE_CONFIG_MODE_Msk);
        state->dev->INTENCLR1 = (GPIOTE_INTENSET1_IN0_Msk << chan);
    }
}

static void _irq_handler(gpiote_state_t *state)
{
    for (unsigned i = 0; i < state->next_index; i++) {
        if (state->dev->EVENTS_IN[i] == 1) {
            state->dev->EVENTS_IN[i] = 0;
            state->ctx[i].cb(state->ctx[i].arg);
            break;
        }
    }
    cortexm_isr_end();
}

void isr_gpiote30_1(void)
{
    _irq_handler(&_gpiote[0]);
}

void isr_gpiote20_1(void)
{
    _irq_handler(&_gpiote[1]);
}
#endif /* MODULE_PERIPH_GPIO_IRQ */

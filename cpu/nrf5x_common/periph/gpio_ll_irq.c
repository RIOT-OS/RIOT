/*
 * Copyright (C) 2015 Jan Wagner <mail@jwagner.eu>
 *               2015-2016 Freie Universität Berlin
 *               2019 Inria
 *               2021 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_nrf5x_common
 * @ingroup     drivers_periph_gpio_ll_irq
 * @{
 *
 * @file
 * @brief       IRQ implementation of the GPIO Low-Level API for  the nRF5x MCU family
 *
 * @note        This GPIO driver implementation supports only one pin to be
 *              defined as external interrupt.
 *
 * @author      Christian Kühling <kuehling@zedat.fu-berlin.de>
 * @author      Timo Ziegler <timo.ziegler@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Jan Wagner <mail@jwagner.eu>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <assert.h>
#include <errno.h>

#include "cpu.h"
#include "periph/gpio_ll.h"
#include "periph/gpio_ll_irq.h"
#include "periph_conf.h"
#include "periph_cpu.h"

#ifdef NRF_GPIOTE0_S
#define NRF_GPIOTE NRF_GPIOTE0_S
#define GPIOTE_IRQn GPIOTE0_IRQn
#endif

#if CPU_FAM_NRF51
#define GPIOTE_CHAN_NUMOF     (4U)
#else
#define GPIOTE_CHAN_NUMOF     (8U)
#endif

/**
 * @brief   Place to store the interrupt context
 */
struct isr_ctx {
    gpio_ll_cb_t cb;
    void *arg;
};
static struct isr_ctx isr_ctx[GPIOTE_CHAN_NUMOF];

static uint8_t get_portsel(uint32_t conf)
{
#ifdef GPIOTE_CONFIG_PORT_Msk
    return (conf & GPIOTE_CONFIG_PORT_Msk) >> GPIOTE_CONFIG_PORT_Pos;
#else
    (void)conf;
    return 0;
#endif
}

/**
 * @brief   get the GPIOTE channel used to monitor the given pin
 *
 * @return  the GPIOTE channel monitoring the specified pin
 * @retval  GPIOTE_CHAN_NUMOF   no GPIOTE channel is monitoring the given pin
 */
static unsigned get_channel_of_pin(uint8_t port_num, uint8_t pin)
{
    /* port_num unused for nrf51 */
    (void)port_num;
    for (unsigned i = 0; i < GPIOTE_CHAN_NUMOF; i++) {
        uint32_t conf = NRF_GPIOTE->CONFIG[i];
        uint32_t mode = (conf & GPIOTE_CONFIG_MODE_Msk) >> GPIOTE_CONFIG_MODE_Pos;
        if (mode == GPIOTE_CONFIG_MODE_Event) {
            uint8_t pinsel = (conf & GPIOTE_CONFIG_PSEL_Msk) >> GPIOTE_CONFIG_PSEL_Pos;
            uint8_t portsel = get_portsel(conf);
            if ((pinsel == pin) && (portsel == port_num)) {
                return i;
            }
        }
    }

    return GPIOTE_CHAN_NUMOF;
}

/**
 * @brief   select a GPIOTE channel suitable for managing the irq for the given
 *          pin
 *
 * @return  if one channel is already used for the given pin, return that.
 *          Otherwise return a free channel
 * @retval  GPIOTE_CHAN_NUMOF   all GPIOTE channels occupied by pins different
 *                              to the selected one
 */
static unsigned get_channel_for_pin(uint8_t port_num, uint8_t pin)
{
    unsigned result = get_channel_of_pin(port_num, pin);
    if (result != GPIOTE_CHAN_NUMOF) {
        return result;
    }

    /* no channel devoted to the pin yet, return first free channel instead */
    for (unsigned i = 0; i < GPIOTE_CHAN_NUMOF; i++) {
        uint32_t conf = NRF_GPIOTE->CONFIG[i];
        uint32_t mode = (conf & GPIOTE_CONFIG_MODE_Msk) >> GPIOTE_CONFIG_MODE_Pos;
        if (mode != GPIOTE_CONFIG_MODE_Event) {
            /* free channel found */
            return i;
        }
    }

    return GPIOTE_CHAN_NUMOF;
}

int gpio_ll_irq(gpio_port_t port, uint8_t pin,
                gpio_irq_trig_t trig, gpio_ll_cb_t cb, void *arg)
{
    /* param port is not used on nRF5x variants with only one GPIO port */
    (void)port;
    uint8_t port_num = gpio_port_num(port);
    uint8_t channel = get_channel_for_pin(port_num, pin);
    assert((trig != GPIO_TRIGGER_LEVEL_HIGH) && (trig != GPIO_TRIGGER_LEVEL_LOW));

    if (channel == GPIOTE_CHAN_NUMOF) {
        return -EBUSY;
    }

    /* mask IRQ */
    NRF_GPIOTE->INTENCLR = GPIOTE_INTENSET_IN0_Msk << channel;

    isr_ctx[channel].cb = cb;
    isr_ctx[channel].arg = arg;

    /* use event mode */
    uint32_t config = GPIOTE_CONFIG_MODE_Event << GPIOTE_CONFIG_MODE_Pos;
    /* set pin and (nRF52 only) port */
    config |= (uint32_t)pin << GPIOTE_CONFIG_PSEL_Pos;
#ifdef GPIOTE_CONFIG_PORT_Pos
    config |= (uint32_t)port_num << GPIOTE_CONFIG_PORT_Pos;
#endif
    /* set trigger */
    config |= (uint32_t)trig & GPIOTE_CONFIG_POLARITY_Msk;
    /* apply config */
    NRF_GPIOTE->CONFIG[channel] = config;
    /* enable IRQ */
    NVIC_EnableIRQ(GPIOTE_IRQn);
    /* clear any spurious IRQ still present */
    NRF_GPIOTE->EVENTS_IN[channel] = 0;
    /* unmask IRQ */
    NRF_GPIOTE->INTENSET = GPIOTE_INTENSET_IN0_Msk << channel;

    return 0;
}

void gpio_ll_irq_mask(gpio_port_t port, uint8_t pin)
{
    /* param port is not used on nRF5x variants with only one GPIO port */
    (void)port;
    uint8_t port_num = gpio_port_num(port);
    unsigned channel = get_channel_of_pin(port_num, pin);
    assert(channel != GPIOTE_CHAN_NUMOF);
    if (channel != GPIOTE_CHAN_NUMOF) {
        NRF_GPIOTE->INTENCLR = GPIOTE_INTENCLR_IN0_Msk << channel;
    }
}

void gpio_ll_irq_unmask(gpio_port_t port, uint8_t pin)
{
    /* param port is not used on nRF5x variants with only one GPIO port */
    (void)port;
    uint8_t port_num = gpio_port_num(port);
    unsigned channel = get_channel_of_pin(port_num, pin);
    assert(channel != GPIOTE_CHAN_NUMOF);
    if (channel != GPIOTE_CHAN_NUMOF) {
        NRF_GPIOTE->INTENSET = GPIOTE_INTENCLR_IN0_Msk << channel;
    }
}

void gpio_ll_irq_unmask_and_clear(gpio_port_t port, uint8_t pin)
{
    /* param port is not used on nRF5x variants with only one GPIO port */
    (void)port;
    uint8_t port_num = gpio_port_num(port);
    unsigned channel = get_channel_of_pin(port_num, pin);
    assert(channel != GPIOTE_CHAN_NUMOF);
    if (channel != GPIOTE_CHAN_NUMOF) {
        NRF_GPIOTE->EVENTS_IN[channel] = 0;
        NRF_GPIOTE->INTENSET = GPIOTE_INTENCLR_IN0_Msk << channel;
    }
}

void gpio_ll_irq_off(gpio_port_t port, uint8_t pin)
{
    /* param port is not used on nRF5x variants with only one GPIO port */
    (void)port;
    uint8_t port_num = gpio_port_num(port);
    unsigned channel = get_channel_of_pin(port_num, pin);
    assert(channel != GPIOTE_CHAN_NUMOF);
    if (channel != GPIOTE_CHAN_NUMOF) {
        NRF_GPIOTE->INTENSET = GPIOTE_INTENCLR_IN0_Msk << channel;
        NRF_GPIOTE->CONFIG[channel] = 0;
    }
}

void isr_gpiote(void)
{
    for (unsigned int i = 0; i < GPIOTE_CHAN_NUMOF; ++i) {
        if (NRF_GPIOTE->EVENTS_IN[i] == 1) {
            NRF_GPIOTE->EVENTS_IN[i] = 0;
            isr_ctx[i].cb(isr_ctx[i].arg);
        }
    }
    cortexm_isr_end();
}

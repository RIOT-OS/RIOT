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
 * @ingroup     drivers_periph_gpio_ll
 * @{
 *
 * @file
 * @brief       Peripheral GPIO Low-Level API implementation for the nRF5x MCU family
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
#include <string.h>

#include "cpu.h"
#include "periph/gpio_ll.h"
#include "periph_cpu.h"
#include "periph_conf.h"

int gpio_ll_init(gpio_port_t port, uint8_t pin, const gpio_conf_t *conf)
{
    if (conf->pull == GPIO_PULL_KEEP) {
        return -ENOTSUP;
    }

    uint32_t pin_cnf = conf->pull;
    switch (conf->state) {
    case GPIO_OUTPUT_PUSH_PULL:
        /* INPUT bit needs to be *CLEARED* in input mode, so set to disconnect input buffer */
        pin_cnf |= GPIO_PIN_CNF_DIR_Msk | GPIO_PIN_CNF_INPUT_Msk;
        if (conf->drive_strength) {
            pin_cnf |= GPIO_PIN_CNF_DRIVE_H0H1 << GPIO_PIN_CNF_DRIVE_Pos;
        }
        break;
    case GPIO_OUTPUT_OPEN_DRAIN:
        pin_cnf |= GPIO_PIN_CNF_DIR_Msk;
        if (conf->drive_strength) {
            pin_cnf |= GPIO_PIN_CNF_DRIVE_H0D1 << GPIO_PIN_CNF_DRIVE_Pos;
        }
        else {
            pin_cnf |= GPIO_PIN_CNF_DRIVE_S0D1 << GPIO_PIN_CNF_DRIVE_Pos;
        }
        break;
    case GPIO_OUTPUT_OPEN_SOURCE:
        pin_cnf |= GPIO_PIN_CNF_DIR_Msk;
        if (conf->drive_strength) {
            pin_cnf |= GPIO_PIN_CNF_DRIVE_D0H1 << GPIO_PIN_CNF_DRIVE_Pos;
        }
        else {
            pin_cnf |= GPIO_PIN_CNF_DRIVE_D0S1 << GPIO_PIN_CNF_DRIVE_Pos;
        }
        break;
    case GPIO_INPUT:
        break;
    case GPIO_DISCONNECT:
    default:
        /* INPUT bit needs to be *CLEARED* in input mode, so set to disconnect input buffer */
        pin_cnf |= GPIO_PIN_CNF_INPUT_Msk;
        break;
    }

    if (conf->state != GPIO_OUTPUT_PUSH_PULL) {
        switch (conf->pull) {
        default:
        case GPIO_FLOATING:
            break;
        case GPIO_PULL_UP:
            pin_cnf |= GPIO_PIN_CNF_PULL_Pullup << GPIO_PIN_CNF_PULL_Pos;
            break;
        case GPIO_PULL_DOWN:
            pin_cnf |= GPIO_PIN_CNF_PULL_Pulldown << GPIO_PIN_CNF_PULL_Pos;
            break;
        }
    }

    NRF_GPIO_Type *p = (NRF_GPIO_Type *)port;
    if (conf->initial_value) {
        p->OUTSET = 1UL << pin;
    }
    else {
        p->OUTCLR = 1UL << pin;
    }
    p->PIN_CNF[pin] = pin_cnf;

    return 0;
}

void gpio_ll_query_conf(gpio_conf_t *dest, gpio_port_t port, uint8_t pin)
{
    assert((dest != NULL)
            && (NULL == gpio_port_unpack_addr(port))
            && (pin < 32));
    memset(dest, 0, sizeof(*dest));
    /* Searching "Schmitt" in
     * https://infocenter.nordicsemi.com/pdf/nRF52840_OPS_v0.5.pdf yields
     * no matches. Assuming Schmitt trigger cannot be disabled for the
     * nRF5x MCU.
     */
    dest->schmitt_trigger = true;
    dest->state = GPIO_INPUT;

    NRF_GPIO_Type *p = (NRF_GPIO_Type *)port;
    uint32_t cnf = p->PIN_CNF[pin];

    if (cnf & GPIO_PIN_CNF_DIR_Msk) {
        /* some kind of output, determine which: */
        switch ((cnf >> GPIO_PIN_CNF_DRIVE_Pos) & 0xf) {
        default:
            /* push-pull with mix of high drive and standard drive (e.g. high
             * drive for 0, standard drive for 1) is also possible
             * hardware-wise, but not supported via the API. Anyways, if someone
             * bypassed the API to set this, get at least the closest match to
             * the configuration.
             */
        case GPIO_PIN_CNF_DRIVE_S0S1:
            /* standard drive 0, standard drive 1
             * --> push pull with weak drive */
            dest->state = GPIO_OUTPUT_PUSH_PULL;
            dest->drive_strength = GPIO_DRIVE_WEAK;
            break;
        case GPIO_PIN_CNF_DRIVE_H0H1:
            /* high drive 0, high drive 1
             * --> push pull with high drive */
            dest->state = GPIO_OUTPUT_PUSH_PULL;
            dest->drive_strength = GPIO_DRIVE_STRONG;
            break;
        case GPIO_PIN_CNF_DRIVE_S0D1:
            /* standard drive 0, disconnect at 1
             * --> open drain with weak drive */
            dest->state = GPIO_OUTPUT_OPEN_DRAIN;
            dest->drive_strength = GPIO_DRIVE_WEAK;
            break;
        case GPIO_PIN_CNF_DRIVE_H0D1:
            /* high drive 0, disconnect at 1
             * --> open drain with strong drive */
            dest->state = GPIO_OUTPUT_OPEN_DRAIN;
            dest->drive_strength = GPIO_DRIVE_STRONG;
            break;
        case GPIO_PIN_CNF_DRIVE_D0S1:
            /* disconnect at 0, standard drive 1
             * --> open emitter with weak drive */
            dest->state = GPIO_OUTPUT_OPEN_SOURCE;
            dest->drive_strength = GPIO_DRIVE_WEAK;
            break;
        case GPIO_PIN_CNF_DRIVE_D0H1:
            /* disconnect at 0, high drive 1
             * --> open emitter with strong drive */
            dest->state = GPIO_OUTPUT_OPEN_SOURCE;
            dest->drive_strength = GPIO_DRIVE_STRONG;
            break;
        }
    }
    else {
        if (cnf & GPIO_PIN_CNF_INPUT_Msk) {
            /* input buffer is disconnected and pin is not in output mode
             * --> GPIO pin is off
             */
            dest->state = GPIO_DISCONNECT;
        }
    }

    switch ((cnf & GPIO_PIN_CNF_PULL_Msk) >> GPIO_PIN_CNF_PULL_Pos) {
    case GPIO_PIN_CNF_PULL_Pullup:
        dest->pull = GPIO_PULL_UP;
        break;
    case GPIO_PIN_CNF_PULL_Pulldown:
        dest->pull = GPIO_PULL_DOWN;
        break;
    default:
        dest->pull = GPIO_FLOATING;
        break;
    }

    if (dest->state == GPIO_INPUT) {
        dest->initial_value = (gpio_ll_read(port) >> pin) & 1UL;
    }
    else {
        dest->initial_value = (gpio_ll_read_output(port) >> pin) & 1UL;
    }
}

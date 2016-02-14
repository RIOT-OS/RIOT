/*
 * Copyright (C) 2016 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_kinetis_common_port
 *
 * @{
 *
 * @file
 * @brief       Low-level PORT driver implementation
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include <stdint.h>
#include "cpu.h"
#include "port.h"
#include "periph/gpio.h"

PORT_Type * const kinetis_port_ptrs[] = PORT_BASE_PTRS;

int port_init(gpio_t dev, gpio_pp_t pushpull, port_af_t af)
{
    switch (gpio_port_num(dev)) {
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

    uint8_t pin = gpio_pin_num(dev);
    PORT_Type *port = gpio_port(dev);

    /* Reset all pin control settings for the pin */
    /* Switch to analog input function while fiddling with the settings, to be safe. */
    port->PCR[pin] = PORT_PCR_MUX(PORT_AF_ANALOG);

    /* set to push-pull configuration */
    port->PCR[pin] |= pushpull;

    /* Set alternate function */
    port->PCR[pin] &= ~(PORT_PCR_MUX_MASK);
    port->PCR[pin] |= PORT_PCR_MUX(af);

    return 0;
}

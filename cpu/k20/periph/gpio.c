/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_k20
 * @{
 *
 * @file
 * @brief       GPIO driver implementation
 *
 * @author      Finn Wilke <finn.wilke@fu-berlin.de>
 *
 * @}
 */


#include "cpu.h"
#include "sched.h"
#include "thread.h"
#include "periph/gpio.h"
#include "periph_conf.h"
#include "kinetis_sim.h"
#include "MK20DZ10.h"


/**
 * @brief Reconfigures a GPIO as input/output
 *
 * @see k20_gpio_enable_output(GPIO_MemMapPtr, uint8_t, bool)
 * @see k20_gpio_enable_input(GPIO_MemMapPtr, uint8_t)
 *
 * @param[in]  gpio         A pointer to the GPIO. Obtain with the macro PORTx_BASE_PTR
 * @param[in]  pinid        The number of the pin to be set. Must be between 0 and 31
 * @param[in]  output       Evaluates to true if reconfiguring as output, false if
 *                          reconfiguring as input
 *
 * @return 0 if setting the GPIO was successful, -1 otherwise.
 * @{
 */
int k20_gpio_direction(GPIO_MemMapPtr gpio, uint8_t pinid, bool output)
{
    if (pinid >= 32) {
        return -1;
    }

    if (gpio) {
        if (output) {
            gpio->PDDR |= (1 << pinid);
        } else {
            gpio->PDDR &= ~(1 << pinid);
        }

        return 0;
    }

    return -1;
}
/** @{ */

static int _gpio_pin_activate(GPIO_MemMapPtr gpio, uint8_t pinid, bool high_power)
{
    if (pinid >= 32) {
        return -1;
    }

    PORT_MemMapPtr port;

    /* Enable the clock gate for the corresponding port first */
    if (gpio == PTA_BASE_PTR) {
        kinetis_clock_gate_enable(K20_CGATE_PORTA);
        port = PORTA_BASE_PTR;
    }
    else if (gpio == PTB_BASE_PTR) {
        kinetis_clock_gate_enable(K20_CGATE_PORTB);
        port = PORTB_BASE_PTR;
    }
    else if (gpio == PTC_BASE_PTR) {
        kinetis_clock_gate_enable(K20_CGATE_PORTC);
        port = PORTC_BASE_PTR;
    }
    else if (gpio == PTD_BASE_PTR) {
        kinetis_clock_gate_enable(K20_CGATE_PORTD);
        port = PORTD_BASE_PTR;
    }
    else if (gpio == PTE_BASE_PTR) {
        kinetis_clock_gate_enable(K20_CGATE_PORTE);
        port = PORTE_BASE_PTR;
    }
    else {
        return -1;
    }

    port->PCR[pinid] |= ((uint32_t)high_power << PORT_PCR_DSE_SHIFT)
                     |  (001 << PORT_PCR_MUX_SHIFT);

    return 0;
}

static int _gpio_enable(GPIO_MemMapPtr gpio, uint8_t pinid, bool output, bool high_power)
{
    if(_gpio_pin_activate(gpio, pinid, high_power) != 0) {
        return -1;
    }

    return k20_gpio_direction(gpio, pinid, output);
}

/**
 * @brief Enables a GPIO as output
 *
 * @see k20_gpio_enable_input(GPIO_MemMapPtr, uint8_t)
 * @see k20_gpio_direction(GPIO_MemMapPtr, uint8_t, bool)
 *
 * @param[in]  gpio         A pointer to the GPIO. Obtain with the macro PORTx_BASE_PTR
 * @param[in]  pinid        The number of the pin to be set. Must be between 0 and 31
 * @param[in]  high_power   Use high power mode (for driving LEDs and similar peripherals)
 *
 * @return 0 if setting the GPIO was successful, -1 otherwise.
 * @{
 */
int k20_gpio_enable_output(GPIO_MemMapPtr gpio, uint8_t pinid, bool high_power)
{
    return _gpio_enable(gpio, pinid, true, high_power);
}
/** @} */

/**
 * @brief Enables a GPIO as input
 *
 * @see k20_gpio_enable_output(GPIO_MemMapPtr, uint8_t, bool)
 * @see k20_gpio_direction(GPIO_MemMapPtr, uint8_t, bool)
 *
 * @param[in]  gpio         A pointer to the GPIO. Obtain with the macro PORTx_BASE_PTR
 * @param[in]  pinid        The number of the pin to be set. Must be between 0 and 31
 *
 * @return 0 if setting the GPIO was successful, -1 otherwise.
 * @{
 */
int k20_gpio_enable_input(GPIO_MemMapPtr gpio, uint8_t pinid)
{
    return _gpio_enable(gpio, pinid, false, false);
}
/** @} */
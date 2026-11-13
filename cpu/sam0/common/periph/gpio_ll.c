/*
 * SPDX-FileCopyrightText: 2023 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_sam0_common
 * @ingroup     drivers_periph_gpio
 * @{
 *
 * @file
 * @brief       GPIO Low-level API implementation for the SAM0 GPIO peripheral
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>
 *
 * This implementation uses the IOBUS for single-cycle I/O for writes in any
 * case. Reading via the IOBUS requires however for continuous sampling to
 * be enabled, as reads on the IOBUS cannot stall the CPU to wait for the
 * on-demand sampling result to be available. Therefore, reads are done by
 * default via the slower APB bus.
 *
 * To also enable reading via the IOBUS, add the following snipped to your
 * `Makefile`:
 *
 * ```
 * FEATURES_OPTIONAL += periph_gpio_fast_read
 * ```
 *
 * This enables continuous sampling on any pin configured as input, so that
 * the IOBUS can safely be used for reads as well. Consequently, it will now
 * consistently use the IOBUS for I/O.
 *
 * @}
 */

#include <errno.h>
#include <string.h>

#include "compiler_hints.h"
#include "cpu.h"
#include "irq.h"
#include "periph/gpio_ll.h"

#ifdef MODULE_FMT
#  include "fmt.h"
#else
#  include <stdio.h>
static inline void print_str(const char *str)
{
    fputs(str, stdout);
}
#endif

void gpio_ll_mux(gpio_port_t port, uint8_t pin, gpio_mux_t mux)
{
    assume(pin < 32);
    assume(gpio_port_unpack_addr(port) == NULL);
    PortGroup *iobus = (PortGroup *)port;
    PortGroup *apb = sam0_gpio_iobus2ap(iobus);

    unsigned irq_state = irq_disable();
    if (mux == GPIO_MUX_DISABLED) {
        apb->PINCFG[pin].reg &= ~PORT_PINCFG_PMUXEN;
    }
    else {
        unsigned pmux_reg = pin >> 1;
        unsigned pmux_pos = (pin & 0x01) << 2;
        apb->PINCFG[pin].reg |= PORT_PINCFG_PMUXEN;
        unsigned pmux = apb->PMUX[pmux_reg].reg;
        pmux &= ~(PORT_PMUX_PMUXE_Msk << pmux_pos);
        pmux |= (unsigned)mux << pmux_pos;
        apb->PMUX[pmux_reg].reg = pmux;
    }
    irq_restore(irq_state);
}

int gpio_ll_init(gpio_port_t port, uint8_t pin, gpio_conf_t conf)
{
    assume(pin < 32);
    assume(gpio_port_unpack_addr(port) == NULL);
    PortGroup *iobus = (PortGroup *)port;
    PortGroup *apb = sam0_gpio_iobus2ap(iobus);
    uint32_t pin_mask = 1U << pin;
    uint8_t pin_cfg = 0;
    bool initial_value = false;
    bool output_enable = false;

    initial_value = conf.initial_value;

    switch (conf.state) {
    case GPIO_INPUT:
        pin_cfg |= PORT_PINCFG_INEN;
        break;
    case GPIO_OUTPUT_PUSH_PULL:
        output_enable = true;
        break;
    case GPIO_USED_BY_PERIPHERAL:
        pin_cfg |= PORT_PINCFG_PMUXEN;
        break;
    case GPIO_DISCONNECT:
        break;
    case GPIO_OUTPUT_OPEN_DRAIN:
    case GPIO_OUTPUT_OPEN_SOURCE:
    default:
        return -ENOTSUP;
    }

    switch (conf.pull) {
    case GPIO_PULL_UP:
        pin_cfg |= PORT_PINCFG_PULLEN;
        initial_value = true;
        break;
    case GPIO_PULL_DOWN:
        pin_cfg |= PORT_PINCFG_PULLEN;
        initial_value = false;
        break;
    case GPIO_FLOATING:
        break;
    default:
        return -ENOTSUP;
    }

    if (conf.drive_strength == GPIO_DRIVE_STRONG) {
        pin_cfg |= PORT_PINCFG_DRVSTR;
    }

    if (IS_USED(MODULE_PERIPH_GPIO_FAST_READ)) {
        /* This read-modify-write needs to be made atomic to avoid
         * corrupting the control register. */
        unsigned state = irq_disable();
        if (conf.state == GPIO_INPUT) {
            apb->CTRL.reg |= pin_mask;
        }
        else {
            apb->CTRL.reg &= ~pin_mask;
        }
        irq_restore(state);
    }

    /* Writing the settings now in careful order. All accesses are done via
     * the clear / set special registers that are naturally atomic, except
     * for the PINCFG register. But that is not shared with other pins, so
     * no need to sync that. (The API says concurrent configurations of the
     * exact same GPIO pin are forbidden.) */
    if (initial_value) {
        iobus->OUTSET.reg = pin_mask;
    }
    else {
        iobus->OUTCLR.reg = pin_mask;
    }

    apb->PINCFG[pin].reg = pin_cfg;

    if (output_enable) {
        iobus->DIRSET.reg = pin_mask;
    }
    else {
        iobus->DIRCLR.reg = pin_mask;
    }

    return 0;
}

gpio_conf_t gpio_ll_query_conf(gpio_port_t port, uint8_t pin)
{
    gpio_conf_t result = { 0 };
    assume(pin < 32);
    assume(gpio_port_unpack_addr(port) == NULL);
    PortGroup *iobus = (PortGroup *)port;
    PortGroup *apb = sam0_gpio_iobus2ap(iobus);

    uint32_t pin_mask = 1U << pin;
    uint8_t pin_cfg = apb->PINCFG[pin].reg;

    if (pin_cfg & PORT_PINCFG_DRVSTR) {
        result.drive_strength = GPIO_DRIVE_STRONG;
    }

    if (pin_cfg & PORT_PINCFG_PULLEN) {
        if (iobus->OUT.reg & pin_mask) {
            result.pull = GPIO_PULL_UP;
        }
        else {
            result.pull = GPIO_PULL_DOWN;
        }
    }

    if (pin_cfg & PORT_PINCFG_PMUXEN) {
        result.state = GPIO_USED_BY_PERIPHERAL;
    }
    else {
        if (iobus->DIR.reg & pin_mask) {
            result.state = GPIO_OUTPUT_PUSH_PULL;
        }
        else {
            if (pin_cfg & PORT_PINCFG_INEN) {
                result.state = GPIO_INPUT;
            }
            else {
                result.state = GPIO_DISCONNECT;
            }
        }
    }

    if (result.state == GPIO_INPUT) {
        result.initial_value = (gpio_ll_read(port) >> pin) & 1UL;
    }
    else {
        result.initial_value = (gpio_ll_read_output(port) >> pin) & 1UL;
    }

    return result;
}

void gpio_ll_print_conf(gpio_conf_t conf)
{
    static const char *drive_strs[] = {
        [GPIO_DRIVE_WEAK] = "weak",
        [GPIO_DRIVE_STRONG] = "strong",
    };

    gpio_ll_print_conf_common(conf);
    print_str(", drive: ");
    print_str(drive_strs[conf.drive_strength]);
}

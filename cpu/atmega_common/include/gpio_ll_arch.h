/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     cpu_atmega_common
 * @ingroup     drivers_periph_gpio_ll
 * @{
 *
 * @file
 * @brief       CPU specific part of the Peripheral GPIO Low-Level API
 *
 * @note        This GPIO driver implementation supports only one pin to be
 *              defined as external interrupt.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */


#include <assert.h>

#include "cpu.h"
#include "irq.h"
#include "kernel_defines.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DOXYGEN /* hide implementation specific details from Doxygen */

/* AVR generally requires two CPU cycles for loads from and stores to memory.
 * However, there are special single cycle instructions, which however require
 * the target address to be given as an 5 bit immediate. So only a 64 byte sized
 * part of the data address space can be accessed this way, which is called
 * the I/O memory (as (almost) only memory mapped I/O is mapped there).
 *
 * GPIO ports up to G are part of the I/O mapped area, but starting from port H
 * the GPIO registers are only accessible via regular load and store operations
 * and mapped slightly different in the data address space. For some reason,
 * there is a gap between the GPIO memory regions for port A to G and H and
 * above that results in special handling for GPIO ports H and above.
 *
 * Note that this implementation always uses the data address way to access GPIO
 * registers and never the single cycle instructions. However, GCC converts the
 * instructions into semantically equivalent single CPU cycle instructions
 * whenever the target address is known at compile time (so can be expressed as
 * immediate) and also mapped into the I/O address space. We rely on this
 * optimization to claim single cycle GPIO accesses for GPIO ports below H,
 * whenever the port number is known at compile time.
 */

#define GPIO_PORT_NUMBERING_ALPHABETIC  1

#ifdef PINA
/* We sadly cannot use PINA, as PINA is technically (in terms of C spec lingo)
 * not constant and would trigger:
 *      initializer element is not constant
 * Hence, the defines are a bit more involved to yield proper constants
 * suitable for initializers.
 */
#  define GPIO_PORT_0   (ATMEGA_GPIO_BASE_A)
#endif

#ifdef PINB
#  define GPIO_PORT_1   (ATMEGA_GPIO_BASE_A + 1 * ATMEGA_GPIO_SIZE)
#endif

#ifdef PINC
#  define GPIO_PORT_2   (ATMEGA_GPIO_BASE_A + 2 * ATMEGA_GPIO_SIZE)
#endif

#ifdef PIND
#  define GPIO_PORT_3   (ATMEGA_GPIO_BASE_A + 3 * ATMEGA_GPIO_SIZE)
#endif

#ifdef PINE
#  define GPIO_PORT_4   (ATMEGA_GPIO_BASE_A + 4 * ATMEGA_GPIO_SIZE)
#endif

#ifdef PINF
#  define GPIO_PORT_5   (ATMEGA_GPIO_BASE_A + 5 * ATMEGA_GPIO_SIZE)
#endif

#ifdef PING
#  define GPIO_PORT_6   (ATMEGA_GPIO_BASE_A + 6 * ATMEGA_GPIO_SIZE)
#endif

/* There is a larger gap between PING and PINH to allow other peripherals to
 * also be mapped into the fast I/O memory area. */
#ifdef PINH
#  define GPIO_PORT_7   (ATMEGA_GPIO_BASE_H)
#endif

#ifdef PINI
#  define GPIO_PORT_8   (ATMEGA_GPIO_BASE_H + 1 * ATMEGA_GPIO_SIZE)
#endif

#ifdef PINJ
#  define GPIO_PORT_9   (ATMEGA_GPIO_BASE_H + 2 * ATMEGA_GPIO_SIZE)
#endif

#ifdef PINK
#  define GPIO_PORT_10  (ATMEGA_GPIO_BASE_H + 3 * ATMEGA_GPIO_SIZE)
#endif

#ifdef PINL
#  define GPIO_PORT_11  (ATMEGA_GPIO_BASE_H + 4 * ATMEGA_GPIO_SIZE)
#endif

static inline gpio_port_t gpio_port(uword_t num)
{
#ifdef PINH
    if (num >= PORT_H) {
        return ATMEGA_GPIO_BASE_H + ((num - PORT_H) * ATMEGA_GPIO_SIZE);
    }
#endif

    return ATMEGA_GPIO_BASE_A + (num * ATMEGA_GPIO_SIZE);
}

static inline uword_t gpio_port_num(gpio_port_t port)
{
#ifdef PINH
    if ((port) >= ATMEGA_GPIO_BASE_H) {
        return (port - ATMEGA_GPIO_BASE_H) / ATMEGA_GPIO_SIZE + PORT_H;
    }
#endif

     return (port - ATMEGA_GPIO_BASE_A) / ATMEGA_GPIO_SIZE;
}

static inline uword_t gpio_ll_read(gpio_port_t port)
{
    atmega_gpio_port_t *p = (void *)port;
    return p->pin;
}

static inline uword_t gpio_ll_read_output(gpio_port_t port)
{
    atmega_gpio_port_t *p = (void *)port;
    return p->port;
}

/**
 * @brief   Check if a set/clear operation with the on the given port with the
 *          given mask can be done in a single-cycle bit-set / bit-clear
 *          instruction.
 *
 * @param   port    GPIO port to set/clear bits on
 * @param   mask    Bitmask specifying the bits to set/clear
 *
 * The compiler can optimize a clear/set operation if all of the following is
 * true:
 *
 * 1. The GPIO port number is known at compile time, so that it can be used as
 *    immediate
 * 2. The GPIO port is alphabetically smaller than H, otherwise its registers
 *    are not within the I/O space (and cannot fit the 5 bit immediate field)
 * 3. The bitmask is known at compile time
 * 4. Exactly one bit is set in the bitmask
 */
static inline bool _can_bitwise_access(gpio_port_t port, uword_t mask)
{
    if (IS_CT_CONSTANT(port)
#ifdef ATMEGA_GPIO_BASE_H
        && IS_CT_CONSTANT(port < ATMEGA_GPIO_BASE_H)
        && (port < ATMEGA_GPIO_BASE_H)
#endif
        && IS_CT_CONSTANT(mask)
        && IS_CT_CONSTANT(__builtin_popcount(mask) == 1)) {
        return __builtin_popcount(mask) == 1;
    }

    return 0;
}

static inline void gpio_ll_set(gpio_port_t port, uword_t mask)
{
    atmega_gpio_port_t *p = (void *)port;
    if (_can_bitwise_access(port, mask)) {
            p->port |= mask;
    }
    else {
        unsigned state = irq_disable();
        p->port |= mask;
        irq_restore(state);
    }
}

static inline void gpio_ll_clear(gpio_port_t port, uword_t mask)
{
    atmega_gpio_port_t *p = (void *)port;
    if (_can_bitwise_access(port, mask)) {
        p->port &= ~mask;
    }
    else {
        unsigned state = irq_disable();
        p->port &= ~mask;
        irq_restore(state);
    }
}

static inline void gpio_ll_toggle(gpio_port_t port, uword_t mask)
{
    atmega_gpio_port_t *p = (void *)port;
    /* this is equivalent to `p->port ^= mask`, but faster and inherently
     * atomically */
    p->pin = mask;
}

static inline void gpio_ll_write(gpio_port_t port, uword_t value)
{
    atmega_gpio_port_t *p = (void *)port;
    p->port = value;
}

static inline gpio_port_t gpio_get_port(gpio_t pin)
{
    return gpio_port(pin >> 4);
}

static inline uint8_t gpio_get_pin_num(gpio_t pin)
{
    return pin & 0x0f;
}

static inline uword_t gpio_ll_prepare_write_all_outputs(gpio_port_t port,
                                                        uword_t value)
{
    atmega_gpio_port_t *p = (void *)port;
    uword_t result = (gpio_ll_read_output(port) & (~p->ddr)) | value;
    return result;
}

static inline uword_t gpio_ll_prepare_write(gpio_port_t port, uword_t mask,
                                            uword_t value)
{
    atmega_gpio_port_t *p = (void *)port;
    uword_t result = gpio_ll_read_output(port);
    result &= (~p->ddr) | (~mask);
    result |= value;
    return result;
}

static inline void gpio_ll_switch_dir_output(gpio_port_t port, uword_t outputs)
{
    unsigned irq_state = irq_disable();
    atmega_gpio_port_t *p = (void *)port;
    p->ddr |= outputs;
    irq_restore(irq_state);
}

static inline void gpio_ll_switch_dir_input(gpio_port_t port, uword_t inputs)
{
    unsigned irq_state = irq_disable();
    atmega_gpio_port_t *p = (void *)port;
    p->ddr &= ~(inputs);
    irq_restore(irq_state);
}

static inline gpio_port_t gpio_port_pack_addr(void *addr)
{
    return (gpio_port_t)addr;
}

static inline void * gpio_port_unpack_addr(gpio_port_t port)
{
    if (port < RAMSTART) {
        return NULL;
    }

    return (void *)port;
}

static inline bool is_gpio_port_num_valid(uint_fast8_t num)
{
    switch (num) {
    default:
        return false;
#ifdef DDRA
    case 0:
#endif
#ifdef DDRB
    case 1:
#endif
#ifdef DDRC
    case 2:
#endif
#ifdef DDRD
    case 3:
#endif
#ifdef DDRE
    case 4:
#endif
#ifdef DDRF
    case 5:
#endif
#ifdef DDRG
    case 6:
#endif
#ifdef DDRH
    case 7:
#endif
#ifdef DDRI
    case 8:
#endif
#ifdef DDRJ
    case 9:
#endif
#ifdef DDRK
    case 10:
#endif
#ifdef DDRL
    case 11:
#endif
#ifdef DDRM
    case 12:
#endif
#ifdef DDRN
    case 13:
#endif
#ifdef DDRO
    case 14:
#endif
#ifdef DDRP
    case 15:
#endif
#ifdef DDRQ
    case 16:
#endif
#ifdef DDRR
    case 17:
#endif
#ifdef DDRS
    case 18:
#endif
#ifdef DDRT
    case 19:
#endif
#ifdef DDRU
    case 20:
#endif
#ifdef DDRV
    case 21:
#endif
#ifdef DDRW
    case 22:
#endif
#ifdef DDRX
    case 23:
#endif
#ifdef DDRY
    case 24:
#endif
#ifdef DDRZ
    case 25:
#endif
        return true;
    }
}

#endif /* DOXYGEN */
#ifdef __cplusplus
}
#endif

/** @} */

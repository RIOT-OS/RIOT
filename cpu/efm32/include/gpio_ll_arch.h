/*
 * Copyright (C) 2022 Christian Amsüss
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_efm32
 * @ingroup     drivers_periph_gpio_ll
 * @{
 *
 * @file
 * @brief       CPU specific part of the Peripheral GPIO Low-Level API
 *
 * Noteworthy aspects of this implementation:
 *
 * * The platform has no "GPIO_USED_BY_PERIPHERAL" -- instead, the pin needs to
 *   be configured as by the using peripheral's properties (eg. push-pull for
 *   UART, disabled for ADC to minimize GPIO influence, etc.).
 *
 * * Alternative drive strengths are supported by the hardware, but not
 *   implemented in the driver. Adding them would be possible, but is tedious
 *   to implement: As each port can only have one alternative drive strength,
 *   changing that would require iterating over all pins, decide whether they
 *   are using an alternative drive strength, and refuse changing it if any are
 *   found.
 *
 * * There is an optional glitch suppression filter after the Schmitt trigger;
 *   no custom API (which would be a local extension to @ref gpio_conf_t) is
 *   implemented yet to enable the filters.
 *
 * @author      Christian Amsüss <chrysn@fsfe.org>
 */

#ifndef GPIO_LL_ARCH_H
#define GPIO_LL_ARCH_H

#include "cpu.h"
#include "periph_cpu.h"

#include "em_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DOXYGEN /* hide implementation specific details from Doxygen */

/* We could do
 *
#define GPIO_PORT(num) (GPIO->P[num])
#define GPIO_PORT_NUM(port) ((port - &GPIO->P))
 *
 * which forks for some operations, but at latest when _ll_set needs to fan out
 * for some EFM32 families to
 *
#if defined(_GPIO_P_DOUTSET_MASK)
  GPIO->P[port].DOUTSET = pins;
#elif defined(GPIO_HAS_SET_CLEAR)
  GPIO->P_SET[port].DOUT = pins;
#else
  (some bit-banding-style interaction on P)
#endif
 *
 * that approach becomes an unbearable burden, because P_SET is not necessarily
 * as large as P, and getting from a P pointer to a P_SET pointer would involve
 * division and multiplication. Instead, falling back to addressing ports by
 * their index number, which does require an additional multiplication for most
 * accesses, but at least does that consistently.
 *
 * (It also makes things easier because it allows going through the helper
 * functions).
 *
 * There appears to be one truly viable alternative: implementing gpio_ll only
 * for those EFM32 that do have DOUTSET etc. in P, with no way of having such
 * an implementation for other EFM32 families. For the time being, the
 * suboptimal-but-works-for-all version is the best we have.
 */

#define GPIO_PORT(num) (num)
#define GPIO_PORT_NUM(port) (port)

static inline uword_t gpio_ll_read(gpio_port_t port)
{
    return GPIO_PortInGet(port);
}

static inline uword_t gpio_ll_read_output(gpio_port_t port)
{
    return GPIO_PortOutGet(port);
}

static inline void gpio_ll_set(gpio_port_t port, uword_t mask)
{
    GPIO_PortOutSet(port, mask);
}

static inline void gpio_ll_clear(gpio_port_t port, uword_t mask)
{
    GPIO_PortOutClear(port, mask);
}

static inline void gpio_ll_toggle(gpio_port_t port, uword_t mask)
{
    GPIO_PortOutToggle(port, mask);
}

static inline void gpio_ll_write(gpio_port_t port, uword_t value)
{
    GPIO->P[port].DOUT = value;
}

static inline gpio_port_t gpio_get_port(gpio_t pin)
{
    return (pin >> 4);
}

static inline uint8_t gpio_get_pin_num(gpio_t pin)
{
    return (pin & 0x0f);
}

static inline gpio_port_t gpio_port_pack_addr(void *addr)
{
    return (gpio_port_t)addr;
}

static inline bool is_gpio_port_num_valid(uint_fast8_t num)
{
    return GPIO_PORT_VALID(num);
}

static inline void * gpio_port_unpack_addr(gpio_port_t port)
{
    if ((port & ~0xff) == 0 && is_gpio_port_num_valid(port)) {
        return NULL;
    }

    return (void *)port;
}

#endif /* DOXYGEN */
#ifdef __cplusplus
}
#endif

#endif /* GPIO_LL_ARCH_H */
/** @} */

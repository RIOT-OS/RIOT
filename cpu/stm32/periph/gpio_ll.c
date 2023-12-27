/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *               2015 Hamburg University of Applied Sciences
 *               2017-2020 Inria
 *               2017 OTA keys S.A.
 *               2021 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32
 * @ingroup     drivers_periph_gpio_ll
 * @{
 *
 * @file
 * @brief       GPIO Low-level API implementation for the STM32 GPIO peripheral
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Katja Kirstein <katja.kirstein@haw-hamburg.de>
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <errno.h>
#include <string.h>

#include "cpu.h"
#include "bitarithm.h"
#include "periph/gpio_ll.h"

#ifdef MODULE_FMT
#include "fmt.h"
#else
static inline void print_str(const char *str)
{
    fputs(str, stdout);
}
#endif

#ifdef RCC_AHBENR_GPIOAEN
#  define GPIO_BUS      AHB
#  define GPIOAEN       RCC_AHBENR_GPIOAEN
#endif

#ifdef RCC_AHB1ENR_GPIOAEN
#  define GPIO_BUS      AHB1
#  define GPIOAEN       RCC_AHB1ENR_GPIOAEN
#endif

#ifdef RCC_AHB2ENR1_GPIOAEN
#  define GPIO_BUS      AHB2
#  define GPIOAEN       RCC_AHB2ENR1_GPIOAEN
#endif

#ifdef RCC_AHB2ENR_GPIOAEN
#  define GPIO_BUS      AHB2
#  define GPIOAEN       RCC_AHB2ENR_GPIOAEN
#endif

#ifdef RCC_MC_AHB4ENSETR_GPIOAEN
#  define GPIO_BUS      AHB4
#  define GPIOAEN       RCC_MC_AHB4ENSETR_GPIOAEN
#endif

#ifdef RCC_IOPENR_GPIOAEN
#  define GPIO_BUS      IOP
#  define GPIOAEN       RCC_IOPENR_GPIOAEN
#endif

#ifdef RCC_APB2ENR_IOPAEN
#  define GPIO_BUS      APB2
#  define GPIOAEN       RCC_APB2ENR_IOPAEN
#endif

static void _init_clock(gpio_port_t port)
{
    periph_clk_en(GPIO_BUS, (GPIOAEN << GPIO_PORT_NUM(port)));
#ifdef PORTG_REQUIRES_EXTERNAL_POWER
    if (port == (uintptr_t)GPIOG) {
        /* Port G requires external power supply */
        periph_clk_en(APB1, RCC_APB1ENR1_PWREN);
        PWR->CR2 |= PWR_CR2_IOSV;
    }
#endif
}

#if defined(GPIO_MODER_MODER0) || defined(GPIO_MODER_MODE0)
static void _set_dir(gpio_port_t port, uint8_t pin, bool output)
{
    GPIO_TypeDef *p = (void *)port;
    uint32_t tmp = p->MODER;
    tmp &= ~(0x3 << (2 * pin));
    if (output) {
        tmp |= 1UL << (2 * pin);
    }
    p->MODER = tmp;
}
#endif

#if (defined(GPIO_MODER_MODER0) || defined(GPIO_MODER_MODE0)) \
    && (defined(GPIO_OTYPER_OT0) || defined(GPIO_OTYPER_OT_0))
static gpio_state_t _get_state(gpio_port_t port, uint8_t pin)
{
    GPIO_TypeDef *p = (void *)port;
    uint32_t moder = (p->MODER >> (2 * pin)) & 0x3UL;
    switch (moder) {
    case 0:
        return GPIO_INPUT;
    case 1:
        return ((p->OTYPER >> pin) & 0x1UL) ? GPIO_OUTPUT_OPEN_DRAIN
                                            : GPIO_OUTPUT_PUSH_PULL;
    }
    return GPIO_USED_BY_PERIPHERAL;
}
#endif

#if defined(GPIO_OTYPER_OT0) || defined(GPIO_OTYPER_OT_0)
static void _set_output_type(gpio_port_t port, uint8_t pin, bool open_drain)
{
    GPIO_TypeDef *p = (void *)port;
    if (open_drain) {
        p->OTYPER |= 1UL << pin;
    }
    else {
        p->OTYPER &= ~(1UL << pin);
    }
}
#endif

#if defined(GPIO_PUPDR_PUPDR0) || defined(GPIO_PUPDR_PUPD0)
static void _set_pull_config(gpio_port_t port, uint8_t pin, gpio_pull_t pull)
{
    GPIO_TypeDef *p = (void *)port;
    /* being more verbose here so that compiler doesn't generate two loads and stores when accessing
     * volatile variable */
    uint32_t pupdr = p->PUPDR;
    pupdr &= ~(0x3UL << (2 * pin));
    pupdr |= (uint32_t)pull << (2 * pin);
    p->PUPDR = pupdr;
}

static gpio_pull_t _get_pull_config(gpio_port_t port, uint8_t pin)
{
    GPIO_TypeDef *p = (void *)port;
    uint32_t pupdr = (p->PUPDR >> (2 * pin)) & 0x3UL;
    return (gpio_pull_t)pupdr;
}
#endif

#if defined(GPIO_OSPEEDR_OSPEED0) || defined(GPIO_OSPEEDER_OSPEEDR0) \
    || defined(GPIO_OSPEEDER_OSPEED0) || defined(GPIO_OSPEEDR_OSPEEDR0)
static void _set_slew_rate(gpio_port_t port, uint8_t pin, gpio_slew_t slew_rate)
{
    GPIO_TypeDef *p = (void *)port;
    /* being more verbose here so that compiler doesn't generate two loads and
     * stores when accessing volatile variable */
    uint32_t ospeedr = p->OSPEEDR;
    ospeedr &= ~(3UL << (2 * pin));
    ospeedr |= (uint32_t)slew_rate << (2 * pin);
    p->OSPEEDR = ospeedr;
}

static gpio_slew_t _get_slew_rate(gpio_port_t port, uint8_t pin)
{
    GPIO_TypeDef *p = (void *)port;
    uint32_t ospeedr = (p->OSPEEDR >> (2 * pin)) & 0x3UL;
    return (gpio_slew_t)ospeedr;
}
#endif

#ifdef GPIO_CRL_MODE
static void _set_legacy_f1_config(gpio_port_t port, uint8_t pin,
                                  const gpio_conf_t *conf)
{
    /* STM32F1 style config register mix output mode and slew rate into the
     * same field. This look up table can be used to look up the correct
     * output mode by slew rate */
    static const uint8_t output_mode_by_slew_rate[] = {
        [GPIO_SLEW_SLOWEST] = GPIO_CRL_MODE0_OUTPUT_2MHZ,
        [GPIO_SLEW_FAST] = GPIO_CRL_MODE0_OUTPUT_10MHZ,
        [GPIO_SLEW_FASTEST] = GPIO_CRL_MODE0_OUTPUT_50MHZ,
    };

    GPIO_TypeDef *p = (void *)port;
    /* There is low control register (CRL) for pins 0-7, and a high control
     * register (CRH) for pins 8-15. `offset` is the offset within the
     * registers, `high_reg` is true if CRH is to be used */
    unsigned offset = (pin & 0x7U) << 2;
    bool high_reg = pin > 7;
    uint32_t control = high_reg ? p->CRH : p -> CRL;

    assert((unsigned)conf->slew_rate < ARRAY_SIZE(output_mode_by_slew_rate));

    /* prepare bis in cnf and mode fields for given pin */
    uint32_t cnf_mode = 0;
    switch (conf->state) {
    default:
    case GPIO_DISCONNECT:
        /* Keeping GPIO in analog mode is said to reduce power consumption.
         * This is plausible, as the Schmitt trigger and the input buffer could
         * be disabled. */
        cnf_mode = GPIO_CRL_MODE0_INPUT | GPIO_CRL_CNF0_INPUT_ANALOG;
        break;
    case GPIO_INPUT:
        switch (conf->pull) {
        default:
        case GPIO_FLOATING:
            cnf_mode = GPIO_CRL_MODE0_INPUT | GPIO_CRL_CNF0_INPUT_FLOATING;
            break;
        case GPIO_PULL_UP:
            cnf_mode = GPIO_CRL_MODE0_INPUT | GPIO_CRL_CNF0_INPUT_PULL;
            /* ODR controls pull resistor in input mode. We access ODR via
             * BSRR to atomically set the bit (mostly to safe ROM and CPU
             * cycles, IRQs are disabled anyway) */
            p->BSRR = 1U << pin;
            break;
        case GPIO_PULL_DOWN:
            cnf_mode = GPIO_CRL_MODE0_INPUT | GPIO_CRL_CNF0_INPUT_PULL;
            /* ODR controls pull resistor in input mode. We access ODR via
             * BSRR to atomically clear the bit (mostly to safe ROM and CPU
             * cycles, IRQs are disabled anyway) */
            p->BSRR = 1U << (pin | 0x10);
        }
        break;
    case GPIO_OUTPUT_PUSH_PULL:
        cnf_mode = GPIO_CRL_CNF0_OUTPUT_PUSH_PULL
                 | output_mode_by_slew_rate[conf->slew_rate];
        break;
    case GPIO_OUTPUT_OPEN_DRAIN:
        cnf_mode = GPIO_CRL_CNF0_OUTPUT_OPEN_DRAIN
                 | output_mode_by_slew_rate[conf->slew_rate];
    }

    /* clear old values of cnf and mode fields in config reg */
    control &= ~(0xFU << offset);
    /* apply new values of cnf and mode fields in config reg */
    control |= cnf_mode << offset;

    if (high_reg) {
        p->CRH = control;
    }
    else {
        p->CRL = control;
    }
}
static void _get_legacy_f1_config(gpio_conf_t *dest, gpio_port_t port,
                                  uint8_t pin)
{
    GPIO_TypeDef *p = (void *)port;
    unsigned offset = (pin & 0x7U) << 2;
    bool high_reg = pin > 7;
    uint32_t control = high_reg ? p->CRH : p ->CRL;
    uint32_t cnf_mode = control >> offset;
    uint32_t cnf = cnf_mode & GPIO_CRL_CNF0_Msk;
    uint32_t mode = cnf_mode & GPIO_CRL_MODE0_Msk;

    switch (mode) {
    default:
    case GPIO_CRL_MODE0_INPUT:
        switch (cnf) {
        default:
        case GPIO_CRL_CNF0_INPUT_ANALOG:
            dest->state = GPIO_DISCONNECT;
            break;
        case GPIO_CRL_CNF0_INPUT_FLOATING:
            dest->state = GPIO_INPUT;
            break;
        case GPIO_CRL_CNF0_INPUT_PULL:
            dest->state = GPIO_INPUT;
            dest->pull = GPIO_PULL_DOWN;
            if (p->ODR & (1U << pin)) {
                dest->pull = GPIO_PULL_UP;
            }
        }
        return;
    case GPIO_CRL_MODE0_OUTPUT_2MHZ:
        dest->slew_rate = GPIO_SLEW_SLOWEST;
        break;
    case GPIO_CRL_MODE0_OUTPUT_10MHZ:
        dest->slew_rate = GPIO_SLEW_FAST;
        break;
    case GPIO_CRL_MODE0_OUTPUT_50MHZ:
        dest->slew_rate = GPIO_SLEW_FASTEST;
        break;
    }

    switch (cnf) {
    case GPIO_CRL_CNF0_OUTPUT_PUSH_PULL:
        dest->state = GPIO_OUTPUT_PUSH_PULL;
        break;
    case GPIO_CRL_CNF0_OUTPUT_OPEN_DRAIN:
        dest->state = GPIO_OUTPUT_OPEN_DRAIN;
        break;
    default:
    case GPIO_CRL_CNF0_AF_PUSH_PULL:
    case GPIO_CRL_CNF0_AF_OPEN_DRAIN:
        dest->state = GPIO_USED_BY_PERIPHERAL;
    }
}
#endif

int gpio_ll_init(gpio_port_t port, uint8_t pin, const gpio_conf_t *conf)
{
    if ((conf->pull == GPIO_PULL_KEEP) || (conf->state == GPIO_OUTPUT_OPEN_SOURCE)) {
        return -ENOTSUP;
    }

#ifndef GPIO_PUPDR_PUPDR0
    /* without dedicated pull up / pull down register, pull resistors can only
     * be used with input pins */
    if ((conf->state == GPIO_OUTPUT_OPEN_DRAIN) && (conf->pull != GPIO_FLOATING)) {
        return -ENOTSUP;
    }
#endif

    unsigned state = irq_disable();
    _init_clock(port);
    if (conf->initial_value) {
        gpio_ll_set(port, 1UL << pin);
    }
    else {
        gpio_ll_clear(port, 1UL << pin);
    }
#ifdef GPIO_CRL_MODE
    /* old STM32F1 style GPIO configuration register layout */
    _set_legacy_f1_config(port, pin, conf);
#else
    /* modern STM32 style GPIO configuration register layout */
    _set_output_type(port, pin, conf->state == GPIO_OUTPUT_OPEN_DRAIN);
    _set_pull_config(port, pin, conf->pull);
    _set_slew_rate(port, pin, conf->slew_rate);
    _set_dir(port, pin, conf->state < GPIO_INPUT);
#endif
    irq_restore(state);

    return 0;
}

void gpio_ll_query_conf(gpio_conf_t *dest, gpio_port_t port, uint8_t pin)
{
    assert(dest);
    unsigned state = irq_disable();
    memset(dest, 0, sizeof(*dest));
#ifdef GPIO_CRL_MODE
    /* old STM32F1 style GPIO configuration register layout */
    _get_legacy_f1_config(dest, port, pin);
#else
    /* modern STM32 style GPIO configuration register layout */
    dest->state = _get_state(port, pin);
    dest->pull = _get_pull_config(port, pin);
    dest->slew_rate = _get_slew_rate(port, pin);
#endif
    if (dest->state == GPIO_INPUT) {
        dest->initial_value = (gpio_ll_read(port) >> pin) & 1UL;
    }
    else {
        dest->initial_value = (gpio_ll_read_output(port) >> pin) & 1UL;
    }
    irq_restore(state);
}

void gpio_ll_print_conf(const gpio_conf_t *conf)
{
    static const char *slew_strs[] = {
        [GPIO_SLEW_SLOWEST] = "slowest",
#if STM32_HAS_OSPEED
        [GPIO_SLEW_SLOW] = "slow",
#endif
        [GPIO_SLEW_FAST] = "fast",
        [GPIO_SLEW_FASTEST] = "fastest",
/* If only three slew rates are supported, a fourth value would be
 * representable with the two-bit field. Let's be rather safe than sorry */
#if !STM32_HAS_OSPEED
        "invalid"
#endif
    };

    gpio_ll_print_conf_common(conf);
    print_str(", slew: ");
    print_str(slew_strs[conf->slew_rate]);

    if (conf->schmitt_trigger_disabled) {
        print_str(", Schmitt trigger disabled");
    }
}

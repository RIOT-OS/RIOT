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
 * @brief       GPIO Low-level API implementation for the STM32 GPIO peripheral (except F1)
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

#if defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F3) || defined(CPU_FAM_STM32L1)
#  define GPIO_BUS      AHB
#  define GPIOAEN       RCC_AHBENR_GPIOAEN
#elif defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32G0)
#  define GPIO_BUS      IOP
#  define GPIOAEN       RCC_IOPENR_GPIOAEN
#elif defined(CPU_FAM_STM32L4) || defined(CPU_FAM_STM32WB) || \
      defined(CPU_FAM_STM32G4) || defined(CPU_FAM_STM32L5) || \
      defined(CPU_FAM_STM32U5) || defined(CPU_FAM_STM32WL)
#define GPIO_BUS        AHB2
#  if defined(CPU_FAM_STM32U5)
#    define GPIOAEN     RCC_AHB2ENR1_GPIOAEN
#  else
#    define GPIOAEN     RCC_AHB2ENR_GPIOAEN
#  endif
#  ifdef PWR_CR2_IOSV
#    define PORTG_REQUIRES_EXTERNAL_POWER
#  endif
#elif defined(CPU_FAM_STM32MP1)
#  define GPIO_BUS      AHB4
#  define GPIOAEN       RCC_MC_AHB4ENSETR_GPIOAEN
#else
#  define GPIO_BUS      AHB1
#  define GPIOAEN       RCC_AHB1ENR_GPIOAEN
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

int gpio_ll_init(gpio_port_t port, uint8_t pin, const gpio_conf_t *conf)
{
    if ((conf->pull == GPIO_PULL_KEEP) || (conf->state == GPIO_OUTPUT_OPEN_SOURCE)) {
        return -ENOTSUP;
    }

    unsigned state = irq_disable();
    _init_clock(port);
    if (conf->initial_value) {
        gpio_ll_set(port, 1UL << pin);
    }
    else {
        gpio_ll_clear(port, 1UL << pin);
    }
    _set_output_type(port, pin, conf->state == GPIO_OUTPUT_OPEN_DRAIN);
    _set_pull_config(port, pin, conf->pull);
    _set_slew_rate(port, pin, conf->slew_rate);
    _set_dir(port, pin, conf->state < GPIO_INPUT);
    irq_restore(state);

    return 0;
}

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

static gpio_pull_t _get_pull_config(gpio_port_t port, uint8_t pin)
{
    GPIO_TypeDef *p = (void *)port;
    uint32_t pupdr = (p->PUPDR >> (2 * pin)) & 0x3UL;
    return (gpio_pull_t)pupdr;
}

static gpio_slew_t _get_slew_rate(gpio_port_t port, uint8_t pin)
{
    GPIO_TypeDef *p = (void *)port;
    uint32_t ospeedr = (p->OSPEEDR >> (2 * pin)) & 0x3UL;
    return (gpio_slew_t)ospeedr;
}

void gpio_ll_query_conf(gpio_conf_t *dest, gpio_port_t port, uint8_t pin)
{
    assert(dest);
    unsigned state = irq_disable();
    memset(dest, 0, sizeof(*dest));
    dest->state = _get_state(port, pin);
    dest->pull = _get_pull_config(port, pin);
    dest->slew_rate = _get_slew_rate(port, pin);
    if (dest->state == GPIO_INPUT) {
        dest->initial_value = (gpio_ll_read(port) >> pin) & 1UL;
    }
    else {
        dest->initial_value = (gpio_ll_read_output(port) >> pin) & 1UL;
    }
    irq_restore(state);
}

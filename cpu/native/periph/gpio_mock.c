/*
 * Copyright (C) 2015 Takuo Yonezawa <Yonezawa-T2@mail.dnp.co.jp>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @ingroup     cpu_native
 * @ingroup     drivers_periph_gpio
 * @{
 *
 * @file
 * @brief       empty GPIO implementation
 *
 * @author      Takuo Yonezawa <Yonezawa-T2@mail.dnp.co.jp>
 */

#include "periph/gpio.h"

/**
 * @brief Mocked GPIO array
 */
gpio_mock_t gpio_mock[GPIO_PORT_MAX][GPIO_PIN_MAX];

__attribute__((weak)) int gpio_init(gpio_t pin, gpio_mode_t mode) {
    (void) pin;
    (void) mode;

    if (pin) {
        pin->mode = mode;
        pin->value = 0;
        return 0;
    }

    return -1;
}

__attribute__((weak)) int gpio_init_int(gpio_t pin, gpio_mode_t mode, gpio_flank_t flank,
                  gpio_cb_t cb, void *arg)
{
    (void) cb;
    (void) arg;

    if (pin) {
        pin->mode = mode;
        pin->flank = flank;
        pin->value = 0;
        pin->cb = cb;
        pin->arg = arg;

        return 0;
    }

    return -1;
}

__attribute__((weak)) void gpio_irq_enable(gpio_t pin)
{
    (void) pin;
}

__attribute__((weak)) void gpio_irq_disable(gpio_t pin)
{
    (void) pin;
}

__attribute__((weak)) int gpio_read(gpio_t pin) {
  if (pin) {
    return pin->value;
  }

  return -1;
}

__attribute__((weak)) void gpio_set(gpio_t pin) {
  if (pin) {
    pin->value = 1;
  }
}

__attribute__((weak)) void gpio_clear(gpio_t pin) {
  if (pin) {
    pin->value = 0;
  }
}

__attribute__((weak)) void gpio_toggle(gpio_t pin) {
  if (pin) {
    pin->value ^= 1;
  }
}

__attribute__((weak)) void gpio_write(gpio_t pin, int value) {
  if (pin) {
    pin->value = value;
  }
}

/** @} */

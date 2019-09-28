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

int gpio_cpu_init(gpio_t pin, gpio_mode_t mode) {
  (void) pin;
  (void) mode;

  if (mode >= GPIO_OUT)
    return 0;
  else
    return -1;
}

int gpio_cpu_read(gpio_t pin) {
  (void) pin;

  return 0;
}

void gpio_cpu_set(gpio_t pin) {
  (void) pin;
}

void gpio_cpu_clear(gpio_t pin) {
  (void) pin;
}

void gpio_cpu_toggle(gpio_t pin) {
  (void) pin;
}

void gpio_cpu_write(gpio_t pin, int value) {
  (void) pin;
  (void) value;
}
/** @} */

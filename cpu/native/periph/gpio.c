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

int gpio_init_ll(gpio_t pin, gpio_mode_t mode) {
  (void) pin;
  (void) mode;

  return -1;
}

int gpio_init_int_ll(gpio_t pin, gpio_mode_t mode, gpio_flank_t flank,
                     gpio_cb_t cb, void *arg){
  (void) pin;
  (void) mode;
  (void) flank;
  (void) cb;
  (void) arg;

  return -1;
}

void gpio_irq_enable_ll(gpio_t pin) {
  (void) pin;
}

void gpio_irq_disable_ll(gpio_t pin) {
  (void) pin;
}

int gpio_read_ll(gpio_t pin) {
  (void) pin;

  return 0;
}

void gpio_set_ll(gpio_t pin) {
  (void) pin;
}

void gpio_clear_ll(gpio_t pin) {
  (void) pin;
}

void gpio_toggle_ll(gpio_t pin) {
  (void) pin;
}

void gpio_write_ll(gpio_t pin, int value) {
  (void) pin;
  (void) value;
}
/** @} */

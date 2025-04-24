/*
 * SPDX-FileCopyrightText: 2025 Tom Hert <git@annsann.eu>
 * SPDX-FileCopyrightText: 2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include "board.h"
#include "periph/gpio.h"
#include "ztimer.h"

/* Define the LED0 pin and mode */
gpio_t led0 = GPIO_PIN(1, 9);
gpio_mode_t led0_mode = GPIO_OUT;

/* Define the LED1 pin and mode */
gpio_t led1 = GPIO_PIN(1, 10);
gpio_mode_t led1_mode = GPIO_OUT;

/* Define the button pin */
gpio_t button = GPIO_PIN(1, 2);

/* This callback function will be called when the button state changes */
void button_callback(void *arg) {
  /* the argument is not used */
  (void)arg;

  /* Toggle the LED1 pin based on the button state */
  if (gpio_read(button)) {
    gpio_clear(led1);
  } else {
    gpio_set(led1);
  }
}

int main(void) {
  /* Initialize the LED0 pin */
  gpio_init(led0, led0_mode);
  /* Turn off the LED0 pin */
  gpio_clear(led0);

  /* Initialize the LED1 pin */
  gpio_init(led1, led1_mode);
  /* Turn off the LED1 pin */
  gpio_clear(led1);

  /* Initialize the button pin */
  gpio_init_int(button, GPIO_IN_PU, GPIO_BOTH, button_callback, NULL);

  /* Loop forever */
  while (1) {
    /* Toggle the LED0 pin every 500 milliseconds */
    gpio_toggle(led0);
    ztimer_sleep(ZTIMER_MSEC, 500);
  }
}

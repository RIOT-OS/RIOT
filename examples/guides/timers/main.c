/*
 * SPDX-FileCopyrightText: 2025 Tom Hert <git@annsann.eu>
 * SPDX-FileCopyrightText: 2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include <stdio.h>

#include "ztimer.h"

void timer_callback(void *arg) {
  /* Cast the received pointer "arg" to a C String type */
  char *message = (char *)arg;

  /* Print the message */
  puts(message);
}

int main(void) {
  /* Create a timer */
  ztimer_t timer = {.callback = timer_callback,
                    .arg = "3 seconds have passed!"};

  /* Set the timer to fire in 3 seconds */
  ztimer_set(ZTIMER_SEC, &timer, 3);

  /* Sleep for 5 seconds */
  ztimer_sleep(ZTIMER_SEC, 5);

  puts("5 seconds have passed!");

  return 0;
}

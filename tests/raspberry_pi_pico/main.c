/*
 * Copyright (C) 2021 Ishraq Ibne Ashraf
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 * @file
 * @brief RaspberryPi Pico test application
 *
 * @author Ishraq Ibne Ashraf<ishraq.i.ashraf@gmail.com>
 * @}
 */

#include <stdio.h>
#include <stdint.h>

#include "board.h"
#include "periph/gpio.h"
#include "periph/timer.h"

void cb_timer(void *arg, int channel) {

    printf(
        "cb_timer(), CH: %d, arg: %ld\n",
        channel,
        *((uint32_t*)arg)
    );

    gpio_toggle(LED0_PIN);

    *((uint32_t*)arg) = *((uint32_t*)arg) + 1;
}

int main(void) {
    volatile uint32_t go_ahead = 0;

    printf(
        "\n[+] Initialising board LED on GPIO = %ld\n",
        LED0_PIN
    );

    gpio_init(LED0_PIN, GPIO_OUT);

    printf("\n[+] Initialising timer\n");

    timer_init(TIMER_DEV(0), 0, cb_timer, (void*)&go_ahead);

    timer_stop(TIMER_DEV(0));

    timer_clear(TIMER_DEV(0), 0);
    timer_clear(TIMER_DEV(0), 1);
    timer_clear(TIMER_DEV(0), 2);
    timer_clear(TIMER_DEV(0), 3);

    timer_set(TIMER_DEV(0), 0, 1000000);
    timer_set(TIMER_DEV(0), 1, 4000000);
    timer_set(TIMER_DEV(0), 2, 8000000);
    timer_set(TIMER_DEV(0), 3, 16000000);

    printf("\n[+] Start relative...\n");

    timer_start(TIMER_DEV(0));

    while(go_ahead != 4) {}

    printf("\n[+] Start blinky blink...\n");

    timer_set_periodic(TIMER_DEV(0), 0, 2000000, 0);
    timer_set_periodic(TIMER_DEV(0), 1, 100000, 0);
    timer_set_periodic(TIMER_DEV(0), 2, 200000, 0);
    timer_set_periodic(TIMER_DEV(0), 3, 300000, 0);

    while(1) {}

    return 0;
}

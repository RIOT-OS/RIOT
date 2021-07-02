/*
 * Copyright (C) 2021 Ishraq Ibne Ashraf
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup examples
 * @{
 * @file
 * @brief RaspberryPi Pico test application
 *
 * @author Ishraq Ibne Ashraf<ishraq.i.ashraf@gmail.com>
 * @}
 */

#include <stdio.h>

#include "board.h"
#include "periph/gpio.h"
#include "periph/timer.h"

uint32_t i = 0;

void tcb(void *arg, int channel) {
    (void)(arg);
    (void)(channel);

    //printf("tcb(), CH=%d\n", channel);

    if(i%2) {
        gpio_set(LED0_PIN);
    }
    else {
        gpio_clear(LED0_PIN);
    }

    i++;
}

int main(void) {
    timer_init(TIMER_DEV(0), 0, tcb, NULL);
    timer_set(TIMER_DEV(0), 0, 1000000);

    printf(
        "\n[+] Initialising board LED on GPIO = %ld\n",
        LED0_PIN
    );

    gpio_init(LED0_PIN, GPIO_OUT);

    printf(
        "\n[+] Turning on board LED on GPIO = %ld\n",
        LED0_PIN
    );

    gpio_set(LED0_PIN);

    printf(
        "[+] Turning off board LED on GPIO = %ld\n",
        LED0_PIN
    );

    gpio_clear(LED0_PIN);

    printf(
        "[+] Turning on board LED on GPIO = %ld\n",
        LED0_PIN
    );

    gpio_set(LED0_PIN);

    while(1) {}
    //__WFI();
    //__asm__ __volatile__("wfe");

    return 0;
}

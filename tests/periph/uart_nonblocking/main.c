/*
 * Copyright (C) 2019 Benjamin Valentin <benpicco@googlemail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Simple test application for non-blocking UART functionality
 *
 * @author      Benjamin Valentin <benpicco@googlemail.com>
 *
 * @}
 */

#include <stdio.h>
#include "ztimer.h"
#include "periph_conf.h"

#define LINE_DELAY_MS   100

static inline uint32_t puts_delay(const char* str)
{
    puts(str);
    ztimer_sleep(ZTIMER_USEC, LINE_DELAY_MS * 1000);
    return LINE_DELAY_MS * 1000;
}

static void _irq_disabled_print(void)
{
    unsigned state = irq_disable();
    /* fill the transmit buffer */
    for (uint8_t i = 0; i < UART_TXBUF_SIZE; i++) {
        printf(" ");
    }
    puts("\nputs with disabled interrupts and a full transmit buffer");
    irq_restore(state);
}

int main(void)
{
    _irq_disabled_print();

    uint32_t total_us = 0;
    uint32_t counter = ztimer_now(ZTIMER_USEC);

    /* Richard Stallman and the Free Software Foundation
       claim no copyright on this song. */
    total_us += puts_delay("");
    total_us += puts_delay("Join us now and share the software;");
    total_us += puts_delay("You'll be free, hackers, you'll be free.");
    total_us += puts_delay("Join us now and share the software;");
    total_us += puts_delay("You'll be free, hackers, you'll be free.");
    total_us += puts_delay("");
    total_us += puts_delay("Hoarders can get piles of money,");
    total_us += puts_delay("That is true, hackers, that is true.");
    total_us += puts_delay("But they cannot help their neighbors;");
    total_us += puts_delay("That's not good, hackers, that's not good.");
    total_us += puts_delay("");
    total_us += puts_delay("When we have enough free software");
    total_us += puts_delay("At our call, hackers, at our call,");
    total_us += puts_delay("We'll kick out those dirty licenses");
    total_us += puts_delay("Ever more, hackers, ever more.");
    total_us += puts_delay("");
    total_us += puts_delay("Join us now and share the software;");
    total_us += puts_delay("You'll be free, hackers, you'll be free.");
    total_us += puts_delay("Join us now and share the software;");
    total_us += puts_delay("You'll be free, hackers, you'll be free.");
    total_us += puts_delay("");

    counter = ztimer_now(ZTIMER_USEC) - counter;

    printf("== printed in %" PRIu32 "/%" PRIu32 " µs ==\n", counter, total_us);

    puts("[SUCCESS]");

    return 0;
}

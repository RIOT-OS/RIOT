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
#include <xtimer.h>

#define LINE_DELAY_MS   100

static inline uint32_t puts_delay(const char* str)
{
    puts(str);
    xtimer_usleep(LINE_DELAY_MS * 1000);
    return LINE_DELAY_MS * 1000;
}

int main(void)
{
    uint32_t total_us = 0;
    xtimer_ticks32_t counter = xtimer_now();

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

    counter.ticks32 = xtimer_now().ticks32 - counter.ticks32;

    printf("== printed in %" PRIu32 "/%" PRIu32 " µs ==\n", xtimer_usec_from_ticks(counter), total_us);

    puts("[SUCCESS]");

    return 0;
}

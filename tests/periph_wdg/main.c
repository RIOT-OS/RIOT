/*
 * Copyright (C) 2016 Unwired Devices [info@unwds.com]
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief WDG test application.
 *
 * Tested on:
 * - STM32L151
 *
 * @author EP <ep@unwds.com>
 *
 * @}
 */

#include <stdint.h>
#include <stdio.h>

#include "cpu_conf.h"
#include "periph/wdg.h"
#include "xtimer.h"

#define WDG_RELOAD (uint16_t) 0x0FFF
#define WDG_PRESCALER 0x03

int main(void)
{
    xtimer_init();

    /* Wait 5 seconds to avoid reset loop and possible bricking */
    xtimer_sleep(5);

    puts("Test for the WDG driver");
    puts("This test will setup the WDG, reload it and then hangs up in infinite loop\n");

    /* Check that previous reset was initiated by watchdog timer  */
    if (wdg_reset_occurred()) {
        puts("[!] Reset by WDG occurred");

        for (;; ) ;  /* Hang here */
    }

    puts("[!] Setting up watchdog timer...");

    /* Setup WDG */
    wdg_set_prescaler(WDG_PRESCALER);
    wdg_set_reload(WDG_RELOAD);
    wdg_reload();

	/* Enable WDG */
    wdg_enable();

    puts("[!] Watchdog enabled. Sleeping 150 ms...");

    /* Wait 150 ms */
    xtimer_usleep(1000 * 150);

    puts("[!] Reloading watchdog...");
    wdg_reload();

    /* Freeze */
    puts("[!] Watchdog reloaded. Now freezing...");

    volatile int seconds;
    for (seconds = 0;; seconds++) {
        printf("[*] Freeze: %d second(s)\n", seconds);
        xtimer_sleep(1);
    }

    return 0;
}

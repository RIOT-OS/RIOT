/*
 * Copyright (C) 2017 Inria
 *               2017 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Example of how to use javascript on RIOT
 *
 * @author      Emmanuel Baccelli <emmanuel.baccelli@inria.fr>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "js.h"

/* include headers generated from *.js */
#include "lib.js.h"
#include "main.js.h"

static event_queue_t event_queue;

int main(void)
{
    printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    printf("This board features a(n) %s MCU.\n", RIOT_MCU);

    event_queue_init(&event_queue);

    puts("Initializing jerryscript...");
    js_event_queue = &event_queue;
    js_init();

    puts("Executing lib.js...");
    js_run(lib_js, lib_js_len);

    puts("Executing main.js...");
    js_run(main_js, main_js_len);

    puts("Entering event loop...");
    event_loop(&event_queue);

    return 0;
}

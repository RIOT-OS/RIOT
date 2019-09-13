/*
 * Copyright (C) 2020 Hackerspace San Salvador
 *
 * This example demonstrates the use of the wiegand driver to
 * decode a signal generated within the same example.
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
 * @brief       Application to demonstrates the use of the Wiegand driver
 *
 * @author      Mario Gomez <mario.gomez@teubi.co>
 *
 * @}
 */
#include <stdio.h>
#include <string.h>

#include "thread.h"
#include "xtimer.h"
#include "periph/gpio.h"
#include "shell.h"
#include "wiegand.h"
#include "wiegand_params.h"
#include "msg.h"

/* BEGIN: Global parameters */

// To test connect WG_OUT_D1 with WG_IN_D1 and WG_OUT_D0 with WG_IN_D0

/*
 * Configuration for Arduino MKR1000 or Arduino Zero
 * It should support any GPIO with flank-triggered interrupts.
 *
 */
#define WG_IN_D0 GPIO_PIN(PA, 4)    // Pin A3 in MRK1000 / Arduino Zero
#define WG_IN_D1 GPIO_PIN(PA, 6)    // Pin A5 in MRK1000 / Arduino Zero
#define WG_OUT_D0 GPIO_PIN(PA, 5)   // Pin A4 in MRK1000 / Arduino Zero
#define WG_OUT_D1 GPIO_PIN(PA, 7)   // Pin A6 in MRK1000 / Arduino Zero

/* Configuration for SAMR21G18-based boards */
//#define WG_IN_D0 GPIO_PIN(PA, 6)
//#define WG_IN_D1 GPIO_PIN(PA, 5)
//#define WG_OUT_D0 GPIO_PIN(PA, 8)
//#define WG_OUT_D1 GPIO_PIN(PA, 7)

/* END: Global parameters */

/* BEGIN: Wiegand reader thread */
char wg_reader_stack[THREAD_STACKSIZE_DEFAULT];
void *wg_reader_thread(void *arg)
{
    (void)arg;
    uint32_t card;

    // Create a new device
    wiegand_t wiegand_dev;

    // Configure connection parameters
    wiegand_params_t wiegand_param = {
        .d0 = WG_IN_D0,
        .d1 = WG_IN_D1,
        .flank = GPIO_RISING
    };

    // Note: Default wiegand_params can be changed on the Makefile
    wiegand_init(&wiegand_dev, &wiegand_param);

    // Note: uncomment the following to use the default parameters
    // requires to include the file "wiegand_params.h"
    // defaults could be changed on the Makefile
    //wiegand_init(&wiegand_dev, &wiegand_params);

    while (1) {
        if (wiegand_available(&wiegand_dev)) {
            (void)puts("Card found...");
            card = wiegand_get_code(&wiegand_dev);
            (void)printf("Card high: %d\n", (uint16_t)(card >> 16));
            (void)printf("Card low:  %d\n", (uint16_t)(card & 0xFFFF));
        }
        xtimer_usleep(100000);
    }

    return NULL;
}
/* END: Wiegand reader */

/* BEGIN: Wiegand simulator */

/* Wiegand test frame */
const uint64_t wg_msg = 0b00000001110100111100000000000001;
const uint8_t wg_size = 26;

char wg_simulator_stack[THREAD_STACKSIZE_DEFAULT];
/**
 * @brief This thread simulates a Wiegand frame.
 **/
void *wg_simulator_thread(void *arg)
{
    (void)arg;
    uint8_t i = 0;

    gpio_init(WG_OUT_D1, GPIO_OUT);
    gpio_init(WG_OUT_D0, GPIO_OUT);
    gpio_clear(WG_OUT_D0);
    gpio_clear(WG_OUT_D1);

    while (1) {
        (void)puts("Sending WG frame...");

        for (i = 0; i < wg_size; i++) {
            if ((((wg_msg >> i) & 0x01)) > 0) {
                gpio_set(WG_OUT_D1);
            }
            else {
                gpio_set(WG_OUT_D0);
            }
            xtimer_usleep(100);
            gpio_clear(WG_OUT_D0);
            gpio_clear(WG_OUT_D1);
            xtimer_usleep(900);
        }

        (void)puts("WG frame sent...");
        xtimer_sleep(5);
    }

    return NULL;
}
/* END: Wiegand simulator */

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

static const shell_command_t shell_commands[] = {
    { NULL, NULL, NULL }
};

int main(void)
{
    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    (void)puts("Welcome to RIOT!");

    // Start reader first
    (void)puts("Starting Wiegand reader thread...");
    thread_create
    (
        wg_reader_stack,
        sizeof(wg_reader_stack),
        THREAD_PRIORITY_MAIN - 3,
        THREAD_CREATE_STACKTEST,
        wg_reader_thread,
        NULL,
        "simulator_thread"
    );

    // Now start to send frames
    (void)puts("Starting Wiegand simulation thread...");
    thread_create
    (
        wg_simulator_stack,
        sizeof(wg_simulator_stack),
        THREAD_PRIORITY_MAIN - 1,
        THREAD_CREATE_STACKTEST,
        wg_simulator_thread,
        NULL,
        "simulator_thread"
    );

    /* start shell */
    (void)puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];

    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}

/*
 * Copyright (C) 2015 Freie Universität Berlin
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
 * @brief       Example application for demonstrating the RIOT network stack
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "firmware.h"
#include "shell.h"
#include "msg.h"
#include "suit.h"
#include "net/ota_suit.h"
#include "firmware/manifest.h"
#include "xtimer.h"

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

int main(void)
{
    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    puts("RIOT suit OTA example application");
    printf("Running from slot %i\n", firmware_current_slot());

    firmware_manifest_run();
    ota_suit_run();

    /* start shell */
    puts("All up and running");
    while(1) {
        xtimer_sleep(100);
    }

    /* should be never reached */
    return 0;
}

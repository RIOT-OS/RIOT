/*
 * Copyright (C) 2021 Freie Universität Berlin,
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
 * @brief       testing msg_queue_print
 *
 *
 * @author      Julian Holzwarth <julian.holzwarth@fu-berlin.de>
 *
 */

#include <stdio.h>
#include <inttypes.h>

#include "msg.h"

#define QUEUE_SIZE 8

msg_t msg_queue[QUEUE_SIZE];

int main(void)
{
    msg_t messages[QUEUE_SIZE];

    msg_queue_print();
    msg_init_queue(msg_queue, QUEUE_SIZE);
    msg_queue_print();

    for (int i = 0; i < QUEUE_SIZE; i++) {
        messages[i].type = i;
        messages[i].content.value = i;
        msg_send_to_self(&messages[i]);
    }

    msg_queue_print();
    puts("DONE");
    return 0;
}

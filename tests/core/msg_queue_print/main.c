/*
 * SPDX-FileCopyrightText: 2021 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2024 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
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
 * @author      Mikolai Gütschow <mikolai.guetschow@tu-dresden.de>
 *
 */

#include <stdio.h>
#include <inttypes.h>

#include "msg.h"

#define QUEUE_SIZE 8

msg_t msg_queue[QUEUE_SIZE];

int main(void)
{
    msg_t msg;

    msg_queue_print();
    msg_init_queue(msg_queue, QUEUE_SIZE);
    msg_queue_print();

    /* fill message queue */
    for (uintptr_t i = 0; i < QUEUE_SIZE; i++) {
        msg = (msg_t) {
            .type = i,
            .content.ptr = (void *)i,
        };
        msg_send_to_self(&msg);
    }

    msg_queue_print();

    /* drain half of message queue */
    for (uintptr_t i = 0; i < QUEUE_SIZE/2; i++) {
        msg_receive(&msg);
    }

    msg_queue_print();

    /* fill up message queue again */
    for (uintptr_t i = QUEUE_SIZE; i < QUEUE_SIZE + QUEUE_SIZE/2; i++) {
        msg = (msg_t) {
            .type = i,
            .content.ptr = (void *)i,
        };
        msg_send_to_self(&msg);
    }

    msg_queue_print();

    puts("DONE");
    return 0;
}

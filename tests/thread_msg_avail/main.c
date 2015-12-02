/*
 * Copyright (C) 2015 Nick van IJzendoorn <nijzendoorn@engineering-spirit.nl>
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
 * @brief Thread test application
 *
 * @author Nick van IJzendoorn <nijzendoorn@engineering-spirit.nl>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "thread.h"
#include "msg.h"

#define MSG_QUEUE_LENGTH                (8)

msg_t msg_queue[MSG_QUEUE_LENGTH];

int main(void)
{
    msg_t msges[MSG_QUEUE_LENGTH];

    msg_init_queue(msg_queue, MSG_QUEUE_LENGTH);

    for (int idx = 0; idx < MSG_QUEUE_LENGTH; ++idx) {
        msges[idx].sender_pid = thread_getpid();
        msges[idx].type = idx;

        msg_send_to_self(msges + idx);

        printf("Add message %d\n", idx);

        while (msg_avail() != (idx) + 1)
            ; /* spin forever if we don't have the result we expect */
    }

    for (int idx = msg_avail(); idx > 0; --idx) {
        msg_t msg;
        msg_receive(&msg);

        printf("Receive message: %d\n", (MSG_QUEUE_LENGTH - idx));

        while (msg.type != (MSG_QUEUE_LENGTH - idx) || msg_avail() != idx - 1)
            ; /* spin forever if we don't have the result we expect */
    }

    puts("TEST PASSED\n");
    return 0;
}

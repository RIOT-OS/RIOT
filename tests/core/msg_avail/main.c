/*
 * SPDX-FileCopyrightText: 2015 Nick van IJzendoorn <nijzendoorn@engineering-spirit.nl>
 * SPDX-FileCopyrightText: 2017 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief Thread test application
 *
 * @author Nick van IJzendoorn <nijzendoorn@engineering-spirit.nl>
 * @author Sebastian Meiling <s@mlng.net>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "log.h"
#include "msg.h"

#define MSG_QUEUE_LENGTH                (8)

msg_t msg_queue[MSG_QUEUE_LENGTH];

int main(void)
{
    msg_t msges[MSG_QUEUE_LENGTH];

    msg_init_queue(msg_queue, MSG_QUEUE_LENGTH);

    puts("[START]");

    /* add message to own queue */
    for (unsigned idx = 0; idx < MSG_QUEUE_LENGTH; ++idx) {
        msges[idx].type = idx;
        msg_send_to_self(msges + idx);
        LOG_INFO("+ add msg: %d\n", idx);
        if (msg_avail() != (idx) + 1) {
            puts("[FAILED]");
            return 1;
        }
    }
    /* receive available messages in queue */
    for (unsigned idx = msg_avail(); idx > 0; --idx) {
        msg_t msg;
        msg_receive(&msg);
        LOG_INFO("- got msg: %d\n", (MSG_QUEUE_LENGTH - idx));
        if (msg.type != (MSG_QUEUE_LENGTH - idx)
            || msg_avail() != idx - 1) {
            puts("[FAILED]");
            return 1;
        }
    }
    puts("[SUCCESS]");
    return 0;
}

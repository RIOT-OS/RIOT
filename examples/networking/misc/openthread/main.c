/*
 * Copyright (C) 2017 Baptiste CLENET
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 * @brief       OpenThread test application
 *
 * @author      Baptiste Clenet <bapclenet@gmail.com>
 */

#include <stdio.h>

#include "ot.h"
#include "openthread/thread.h"

static void _panid_handler(event_t *event);

static event_t event_panid = {
    .handler = _panid_handler
};

static void _panid_handler(event_t *event)
{
    (void) event;

    /* It's safe here to call any OpenThread specific API, since this code runs
     * in the same thread as the OpenThread tasklet handler */

    /* Get PanID from OpenThread API */
    uint16_t panid = otLinkGetPanId(openthread_get_instance());
    printf("Current panid: 0x%x\n", panid);
}

int main(void)
{
    /* Example of how to call OpenThread stack functions */
    puts("This is an OpenThread example");

    /** OpenThread needs to synchronize otApi calls
     * with its tasklet scheduler. This
     * synchronization is handled by RIOT using an
     * event queue (accessible via openthread_get_evq())
     */
    event_post(openthread_get_evq(), &event_panid);

    return 0;
}

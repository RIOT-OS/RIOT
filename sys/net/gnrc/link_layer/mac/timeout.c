/*
 * Copyright (C) 2015 Daniel Krebs
 *               2017 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_gnrc_mac
 * @{
 *
 * @file
 * @brief       Implementation of timeout module of GNRC_MAC
 *
 * @author      Daniel Krebs <github@daniel-krebs.net>
 * @author      Shuguo Zhuo  <shuguo.zhuo@inria.fr>
 * @}
 */

#include <assert.h>

#include "net/gnrc.h"
#include "net/gnrc/mac/timeout.h"

#define ENABLE_DEBUG 0
#include "debug.h"

void gnrc_mac_init_timeouts(gnrc_mac_timeout_t *mac_timeout,
                            gnrc_mac_timeout_event_t timeouts[],
                            uint8_t num)
{
    assert(mac_timeout);
    assert(timeouts);
    assert(num);

    mac_timeout->timeouts = timeouts;
    mac_timeout->timeout_num = num;

    for (int i = 0; i < mac_timeout->timeout_num; i++) {
        mac_timeout->timeouts[i].msg_event.event.next = NULL;
        mac_timeout->timeouts[i].type = GNRC_MAC_TIMEOUT_DISABLED;
    }

    evtimer_init_msg(&mac_timeout->evtimer);
}

int gnrc_mac_find_timeout(gnrc_mac_timeout_t *mac_timeout, gnrc_mac_timeout_type_t type)
{
    assert(mac_timeout);
    assert(mac_timeout->timeout_num);

    for (unsigned i = 0; i < mac_timeout->timeout_num; i++) {
        if (mac_timeout->timeouts[i].type == type) {
            return i;
        }
    }
    return -ENOENT;
}

gnrc_mac_timeout_event_t *_gnrc_mac_acquire_timeout(gnrc_mac_timeout_t *mac_timeout,
                                                    gnrc_mac_timeout_type_t type)
{
    assert(mac_timeout);
    assert(mac_timeout->timeout_num);

    if (gnrc_mac_timeout_is_running(mac_timeout, type)) {
        return NULL;
    }

    for (unsigned i = 0; i < mac_timeout->timeout_num; i++) {
        if (mac_timeout->timeouts[i].type == GNRC_MAC_TIMEOUT_DISABLED) {
            mac_timeout->timeouts[i].type = type;
            return &mac_timeout->timeouts[i];
        }
    }
    return NULL;
}

void gnrc_mac_set_timeout(gnrc_mac_timeout_t *mac_timeout, gnrc_mac_timeout_type_t type,
                          uint32_t offset, kernel_pid_t pid)
{
    assert(mac_timeout);

    gnrc_mac_timeout_event_t *timeout_event;
    if ((timeout_event = _gnrc_mac_acquire_timeout(mac_timeout, type))) {
        DEBUG("[gnrc_mac] Set timeout type-%d in %" PRIu32 " us\n",
              type, offset);
        timeout_event->msg_event.event.offset = offset;
        timeout_event->msg_event.msg.type = GNRC_MAC_EVENT_TIMEOUT_TYPE;
        timeout_event->msg_event.msg.content.ptr = (void *) timeout_event;
        timeout_event->msg_event.msg.sender_pid = pid;
        evtimer_add(&mac_timeout->evtimer, &timeout_event->msg_event.event);
    }
    else {
        DEBUG("[gnrc_mac] Cannot set timeout type-%d, too many concurrent timeouts\n",
              type);
    }
}

void gnrc_mac_clear_timeout(gnrc_mac_timeout_t *mac_timeout, gnrc_mac_timeout_type_t type)
{
    assert(mac_timeout);

    int index = gnrc_mac_find_timeout(mac_timeout, type);
    if (index >= 0) {
        mac_timeout->timeouts[index].type = GNRC_MAC_TIMEOUT_DISABLED;
        evtimer_del(&mac_timeout->evtimer,
                    &mac_timeout->timeouts[index].msg_event.event);
    }
}

bool gnrc_mac_timeout_is_expired(gnrc_mac_timeout_t *mac_timeout, gnrc_mac_timeout_type_t type)
{
    assert(mac_timeout);

    int index = gnrc_mac_find_timeout(mac_timeout, type);
    if (index >= 0) {
        evtimer_event_t *list;
        list = (evtimer_event_t *)&mac_timeout->evtimer.events;
        while (list->next) {
            if (list->next == &mac_timeout->timeouts[index].msg_event.event) {
                return false;
            }
            list = list->next;
        }

        /* if we reach here, timeout is expired */
        mac_timeout->timeouts[index].type = GNRC_MAC_TIMEOUT_DISABLED;
        return true;
    }
    return false;
}

void gnrc_mac_reset_timeouts(gnrc_mac_timeout_t *mac_timeout)
{
    assert(mac_timeout);
    assert(mac_timeout->timeout_num);

    for (unsigned i = 0; i < mac_timeout->timeout_num; i++) {
        if (mac_timeout->timeouts[i].type != GNRC_MAC_TIMEOUT_DISABLED) {
            mac_timeout->timeouts[i].type = GNRC_MAC_TIMEOUT_DISABLED;
            evtimer_del(&mac_timeout->evtimer,
                        &mac_timeout->timeouts[i].msg_event.event);
        }
    }
}

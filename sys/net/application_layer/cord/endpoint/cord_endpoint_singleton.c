/*
 * Copyright (C) 2017-2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_cord_endpoint_singleton
 * @{
 *
 * @file
 * @brief       Standalone extension for the CoRE RD endpoint implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdint.h>

#include "log.h"
#include "assert.h"
#include "event/thread.h"
#include "net/sock/util.h"
#include "net/gcoap.h"
#include "net/cord/endpoint.h"
#include "net/cord/config.h"
#include "net/cord/endpoint_singleton.h"

#define ENABLE_DEBUG        0
#include "debug.h"

/* stack configuration */
#define STACKSIZE           (THREAD_STACKSIZE_DEFAULT)
#define PRIO                (THREAD_PRIORITY_MAIN - 1)
#define TNAME               "cord_ep"

#define UPDATE_TIMEOUT      (0xe537)

#define TIMEOUT_MS          (CONFIG_CORD_UPDATE_INTERVAL * MS_PER_SEC)

static cord_endpoint_t _cord;

void cord_endpoint_singleton_run(void)
{
    sock_udp_ep_t remote;
    sock_udp_name2ep(&remote, CORD_ENDPOINT_SINGLETON_ADDRESS);
    if (remote.port == 0) {
        if (IS_USED(MODULE_GCOAP_DTLS)) {
            remote.port = CONFIG_GCOAPS_PORT;
        }
        else {
            remote.port = CONFIG_GCOAP_PORT;
        }
    }
    cord_endpoint_init(&_cord, CONFIG_CORD_ENDPOINT_SINGLETON_EVENT_QUEUE,
            &remote, NULL);
}

int cord_endpoint_singleton_update(void)
{
    return cord_endpoint_update(&_cord);
}

void cord_endpoint_singleton_remove(void)
{
    cord_endpoint_remove(&_cord);
}

int cord_endpoint_singleton_register(const sock_udp_ep_t *remote, const char *regif)
{
    return cord_endpoint_register(&_cord, remote, regif);
}

void cord_endpoint_singleton_dump_status(void)
{
    cord_endpoint_dump_status(&_cord);
}

cord_state_t cord_endpoint_singleton_get_state(void)
{
    return cord_endpoint_get_state(&_cord);
}

ssize_t cord_endpoint_singleton_get_location(char *buf, size_t buf_len)
{
    return cord_endpoint_get_location(&_cord, buf, buf_len);
}

ssize_t cord_endpoint_singleton_get_interface(char *buf, size_t buf_len)
{
    return cord_endpoint_get_interface(&_cord, buf, buf_len);
}

void cord_endpoint_singleton_event_source_attach(event_source_subscriber_t *subscriber)
{
    cord_endpoint_event_source_attach(&_cord, subscriber);
}

void cord_endpoint_singleton_event_source_detach(event_source_subscriber_t *subscriber)
{
    cord_endpoint_event_source_detach(&_cord, subscriber);
}

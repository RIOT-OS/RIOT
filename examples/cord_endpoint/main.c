/*
 * Copyright (C) 2017-2018 Freie Universität Berlin
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
 * @brief       CoRE Resource Directory endpoint (cord_endpoint) example
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "event/thread.h"
#include "fmt.h"
#include "shell.h"
#include "net/ipv6/addr.h"
#include "net/gcoap.h"
#include "net/cord/endpoint.h"
#include "net/cord/endpoint_singleton.h"

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];
/* TODO: define */
//static char _ep_name[64];

#define NODE_INFO  "SOME NODE INFORMATION"

/* we will use a custom event handler for dumping cord_endpoint events */
static void _on_ep_event(event_t *ev)
{
    (void)ev;
    cord_state_t state = cord_endpoint_singleton_get_state();
    switch (state) {
        case CORD_STATE_IDLE:
            puts("RD endpoint is idle");
            break;
        case CORD_STATE_DISCOVERY:
            puts("RD endpoint is performing discovery");
            break;
        case CORD_STATE_REGISTRY:
            puts("RD endpoint is performing registration");
            break;
        case CORD_STATE_REFRESH:
            puts("RD endpoint is registered");
            cord_endpoint_singleton_dump_status();
            break;
        default:
            break;
    }
}

/* define some dummy CoAP resources */
static ssize_t _handler_dummy(coap_pkt_t *pdu,
                              uint8_t *buf, size_t len, coap_request_ctx_t *ctx)
{
    (void)ctx;

    /* get random data */
    int16_t val = 23;

    gcoap_resp_init(pdu, buf, len, COAP_CODE_CONTENT);
    size_t resp_len = coap_opt_finish(pdu, COAP_OPT_FINISH_PAYLOAD);
    resp_len += fmt_s16_dec((char *)pdu->payload, val);
    return resp_len;
}

static ssize_t _handler_info(coap_pkt_t *pdu,
                             uint8_t *buf, size_t len, coap_request_ctx_t *ctx)
{
    (void)ctx;

    gcoap_resp_init(pdu, buf, len, COAP_CODE_CONTENT);
    size_t resp_len = coap_opt_finish(pdu, COAP_OPT_FINISH_PAYLOAD);
    size_t slen = sizeof(NODE_INFO);
    memcpy(pdu->payload, NODE_INFO, slen);
    return resp_len + slen;
}

static const coap_resource_t _resources[] = {
    { "/node/info",  COAP_GET, _handler_info, NULL },
    { "/sense/hum",  COAP_GET, _handler_dummy, NULL },
    { "/sense/temp", COAP_GET, _handler_dummy, NULL }
};

static gcoap_listener_t _listener = {
    .resources     = (coap_resource_t *)&_resources[0],
    .resources_len = ARRAY_SIZE(_resources),
    .next          = NULL
};

int main(void)
{
    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    puts("CoRE RD client example!\n");

    /* setup CoAP resources */
    gcoap_register_listener(&_listener);

    event_t ev = { .handler = _on_ep_event };
    event_source_subscriber_t sub = EVENT_SOURCE_SUBSCRIBER_INIT(&ev, EVENT_PRIO_LOWEST);
    cord_endpoint_singleton_event_source_attach(&sub);

    char name[32];
    cord_endpoint_get_name(name, sizeof(name));
    puts("Client information:");
    printf("  ep: %s\n", name);
    printf("  lt: %is\n", (int)CONFIG_CORD_LT);

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}

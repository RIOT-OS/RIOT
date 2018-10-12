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
 * @brief       CoRE Resource Directory client (rdcli) example application
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "fmt.h"
#include "shell.h"
#include "net/ipv6/addr.h"
#include "net/gcoap.h"
#include "net/rdcli_common.h"
#include "net/rdcli_standalone.h"

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

/* we will use a custom event handler for dumping rdcli_standalone events */
static void _on_rdcli_event(rdcli_standalone_event_t event)
{
    switch (event) {
        case RDCLI_REGISTERED:
            puts("rdcli event: now registered with a RD");
            break;
        case RDCLI_DEREGISTERED:
            puts("rdcli event: dropped client registration");
            break;
        case RDCLI_UPDATED:
            puts("rdcli event: successfully updated client registration");
            break;
    }
}

/* define some dummy CoAP resources */
static ssize_t _handler_dummy(coap_pkt_t *pdu,
                              uint8_t *buf, size_t len, void *ctx)
{
    (void)ctx;

    /* get random data */
    int16_t val = 23;

    gcoap_resp_init(pdu, buf, len, COAP_CODE_CONTENT);
    size_t plen = fmt_s16_dec((char *)pdu->payload, val);
    return gcoap_finish(pdu, plen, COAP_FORMAT_TEXT);
}

static ssize_t _handler_info(coap_pkt_t *pdu,
                             uint8_t *buf, size_t len, void *ctx)
{
    (void)ctx;

    gcoap_resp_init(pdu, buf, len, COAP_CODE_CONTENT);
    size_t slen = sizeof("SOME NODE INFOMRATION");
    memcpy(pdu->payload, "SOME NODE INFOMRATION", slen);
    return gcoap_finish(pdu, slen, COAP_FORMAT_TEXT);
}

static const coap_resource_t _resources[] = {
    { "/node/info",  COAP_GET, _handler_info, NULL },
    { "/sense/hum",  COAP_GET, _handler_dummy, NULL },
    { "/sense/temp", COAP_GET, _handler_dummy, NULL }
};

static gcoap_listener_t _listener = {
    .resources     = (coap_resource_t *)&_resources[0],
    .resources_len = sizeof(_resources) / sizeof(_resources[0]),
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

    /* register event callback with rdcli_standalone */
    rdcli_standalone_reg_cb(_on_rdcli_event);

    puts("Client information:");
    printf("  ep: %s\n", rdcli_common_get_ep());
    printf("  lt: %is\n", (int)RDCLI_LT);

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}

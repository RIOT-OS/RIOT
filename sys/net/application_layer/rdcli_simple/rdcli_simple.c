/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_rdcli_simple
 * @{
 *
 * @file
 * @brief       Simplified CoAP resource directory client implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <string.h>

#include "fmt.h"
#include "net/gcoap.h"
#include "net/rdcli_config.h"
#include "net/rdcli_common.h"
#include "net/rdcli_simple.h"
#include "net/ipv6/addr.h"

#define BUFSIZE             (128U)

/* we don't want to allocate the CoAP packet and scratch buffer on the stack,
 * as they are too large for that. */
static coap_pkt_t pkt;
static uint8_t buf[BUFSIZE];

int rdcli_simple_register(void)
{
    sock_udp_ep_t remote = {
        .family    = AF_INET6,
        .netif     = SOCK_ADDR_ANY_NETIF,
        .port      = RDCLI_SERVER_PORT,
    };

    /* parse RD server address */
    if (ipv6_addr_from_str((ipv6_addr_t *)&remote.addr.ipv6,
                           RDCLI_SERVER_ADDR) == NULL) {
        return RDCLI_SIMPLE_NOADDR;
    }

    /* build the initial CON packet */
    if (gcoap_req_init(&pkt, buf, sizeof(buf), COAP_METHOD_POST,
                             "/.well-known/core") < 0) {
        return RDCLI_SIMPLE_ERROR;
    }
    /* make packet confirmable */
    coap_hdr_set_type(pkt.hdr, COAP_TYPE_CON);
    /* add Uri-Query options */
    if (rdcli_common_add_qstring(&pkt) < 0) {
        return RDCLI_SIMPLE_ERROR;
    }
    /* finish, we don't have any payload */
    ssize_t len = gcoap_finish(&pkt, 0, COAP_FORMAT_LINK);
    if (gcoap_req_send2(buf, len, &remote, NULL) == 0) {
        return RDCLI_SIMPLE_ERROR;
    }

    return RDCLI_SIMPLE_OK;
}

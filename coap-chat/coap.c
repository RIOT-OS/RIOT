/*
 * Copyright (c) 2018 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     applications
 * @{
 *
 * @file
 * @brief       coap-chat - CoAP helper functions
 *
 * @author      Sebastian Meiling <s@mlng.net>
 *
 * @}
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "net/gcoap.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define COAP_CHAT_PATH      "/chat"

static ssize_t _chat_handler(coap_pkt_t *pdu, uint8_t *buf, size_t len, void *ctx);

/* CoAP resources */
static const coap_resource_t _resources[] = {
    { COAP_CHAT_PATH, COAP_POST, _chat_handler, NULL },
};

static gcoap_listener_t _listener = {
    &_resources[0],
    sizeof(_resources) / sizeof(_resources[0]),
    NULL
};

static ssize_t _chat_handler(coap_pkt_t *pdu, uint8_t *buf, size_t len, void *ctx)
{
    (void)ctx;

    if (pdu->payload_len > 0) {
        printf("\n[ CHAT ] %.*s\n\n", pdu->payload_len, (char *)pdu->payload);
        return gcoap_response(pdu, buf, len, COAP_CODE_CHANGED);
    }
    return gcoap_response(pdu, buf, len, COAP_CODE_BAD_REQUEST);
}

static size_t _send(uint8_t *buf, size_t len, char *addr_str)
{
    ipv6_addr_t addr;
    sock_udp_ep_t remote;

    remote.family = AF_INET6;

    /* parse for interface */
    int iface = ipv6_addr_split_iface(addr_str);
    if (iface == -1) {
        if (gnrc_netif_numof() == 1) {
            /* assign the single interface found in gnrc_netif_numof() */
            remote.netif = (uint16_t)gnrc_netif_iter(NULL)->pid;
        }
        else {
            remote.netif = SOCK_ADDR_ANY_NETIF;
        }
    }
    else {
        if (gnrc_netif_get_by_pid(iface) == NULL) {
            DEBUG("[CoAP] interface not valid");
            return 0;
        }
        remote.netif = iface;
    }

    /* parse destination address */
    if (ipv6_addr_from_str(&addr, addr_str) == NULL) {
        DEBUG("[CoAP] unable to parse destination address");
        return 0;
    }
    if ((remote.netif == SOCK_ADDR_ANY_NETIF) && ipv6_addr_is_link_local(&addr)) {
        DEBUG("[CoAP] must specify interface for link local target");
        return 0;
    }
    memcpy(&remote.addr.ipv6[0], &addr.u8[0], sizeof(addr.u8));

    /* parse port */
    remote.port = GCOAP_PORT;

    return gcoap_req_send2(buf, len, &remote, NULL);
}

int coap_post(char *addr, char *msgbuf, size_t msglen)
{
    assert(msglen < GCOAP_PDU_BUF_SIZE);

    coap_pkt_t pdu;
    uint8_t buf[GCOAP_PDU_BUF_SIZE];
    size_t len = 0;

    gcoap_req_init(&pdu, buf, GCOAP_PDU_BUF_SIZE, COAP_POST, COAP_CHAT_PATH);

    memcpy(pdu.payload, msgbuf, msglen);

    len = gcoap_finish(&pdu, msglen, COAP_FORMAT_TEXT);

    DEBUG("[CoAP] coap_post: sending msg ID %u, %u bytes\n",
          coap_get_id(&pdu), (unsigned) len);

    if (!_send(buf, len, addr)) {
        puts("[CoAP] coap_post: msg send failed");
        return -1;
    }
    return len;
}

void coap_init(void)
{
    gcoap_register_listener(&_listener);
}

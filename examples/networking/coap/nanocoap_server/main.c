/*
 * SPDX-FileCopyrightText: 2016 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       CoAP example server application (using nanocoap)
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @}
 */

#include <stdio.h>

#include "net/nanocoap_sock.h"
#include "ztimer.h"

#ifdef MODULE_LWIP_IPV4
#include "lwip/netif.h"
#include <arpa/inet.h>
#endif

#define COAP_INBUF_SIZE (256U)

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

#if MODULE_NANOCOAP_SERVER_DTLS
#include "net/credman.h"
#include "net/dsm.h"
#include "tinydtls_keys.h"

static const uint8_t psk_id_0[] = PSK_DEFAULT_IDENTITY;
static const uint8_t psk_key_0[] = PSK_DEFAULT_KEY;
static const credman_credential_t credential = {
    .type = CREDMAN_TYPE_PSK,
    .tag = CONFIG_NANOCOAP_SERVER_SOCK_DTLS_TAG,
    .params = {
        .psk = {
            .key = { .s = psk_key_0, .len = sizeof(psk_key_0) - 1, },
            .id = { .s = psk_id_0, .len = sizeof(psk_id_0) - 1, },
        }
    },
};
#endif

extern void setup_observe_event(void);

int main(void)
{
    puts("RIOT nanocoap example application");

#if MODULE_NANOCOAP_SERVER_DTLS
    int res = credman_add(&credential);
    if (res < 0 && res != CREDMAN_EXIST) {
        printf("nanocoap_server: cannot add credential to system: %d\n", res);
        return res;
    }
#endif

    /* nanocoap_server uses gnrc sock which uses gnrc which needs a msg queue */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    puts("Waiting for address autoconfiguration...");
    ztimer_sleep(ZTIMER_MSEC, 3 * MS_PER_SEC);

    if (IS_USED(MODULE_NANOCOAP_SERVER_OBSERVE)) {
        setup_observe_event();
    }

#ifdef MODULE_LWIP_IPV4
#define _TEST_ADDR4_LOCAL  (0x9664a8c0U)   /* 192.168.100.150 */
#define _TEST_ADDR4_MASK   (0x00ffffffU)   /* 255.255.255.0 */

    sys_lock_tcpip_core();
    struct netif *iface = netif_find("ET0");

#ifndef MODULE_LWIP_DHCP_AUTO
    ip4_addr_t ip, subnet;
    ip.addr = _TEST_ADDR4_LOCAL;
    subnet.addr = _TEST_ADDR4_MASK;
    netif_set_addr(iface, &ip, &subnet, NULL);
#endif
    sys_unlock_tcpip_core();

    /* print network addresses */
    printf("{\"IPv4 addresses\": [\"");
    char buffer[16];
    inet_ntop(AF_INET, netif_ip_addr4(iface), buffer, 16);
    printf("%s\"]}\n", buffer);

    /* initialize nanocoap server instance for IPv4*/
    uint8_t buf[COAP_INBUF_SIZE];
    sock_udp_ep_t local = {
#if MODULE_NANOCOAP_SERVER_DTLS
        .port=COAPS_PORT,
#else
        .port=COAP_PORT,
#endif
        .family=AF_INET
    };
    nanocoap_server(&local, buf, sizeof(buf));
#else
    /* print network addresses */
    printf("{\"IPv6 addresses\": [\"");
    netifs_print_ipv6("\", \"");
    puts("\"]}");

    /* initialize nanocoap server instance for IPv6*/
    uint8_t buf[COAP_INBUF_SIZE];
    sock_udp_ep_t local = {
#if MODULE_NANOCOAP_SERVER_DTLS
        .port=COAPS_PORT,
#else
        .port=COAP_PORT,
#endif
        .family=AF_INET6
    };
    nanocoap_server(&local, buf, sizeof(buf));
#endif

    /* should be never reached */
    return 0;
}

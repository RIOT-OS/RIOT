/*
 * Copyright (C) 2018 Inria
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
 * @brief       Example application  for sock_secure
 *
 * @author      Raul Fuentes <>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "net/sock/udp.h"
#include "net/sock/secure.h"

#include "msg.h" /* TODO */

#define ENABLE_DEBUG (1)
#include "debug.h"


#ifndef DTLS_DEFAULT_PORT
#define DTLS_DEFAULT_PORT 20220 /* DTLS default port */
#endif

#define READER_QUEUE_SIZE (8U)

static void _resp_handler(uint8_t *data, size_t data_size, void *sock);

extern sock_secure_session_t secure_session;

static void _resp_handler(uint8_t *data, size_t data_size, void *sock)
{
    (void) sock;

    printf("Got (string) data -- ");
    for (size_t i = 0; i < data_size; i++) {
        printf("%c", data[i]);
    }
    printf(" -- (Total: %i bytes)\n",data_size );

    /* TODO DEBUG lines of the remote peer  */
    /* NOTE: Remote is already in secure_session */
    sock_secure_write(&secure_session, data, data_size); /* echo back! */
}

void _server_wrapper(void *arg)
{
    (void) arg;

    /* FIXME The maximum size  */
    // ssize_t pckt_size = DTLS_MAX_BUF;
    // uint8_t pckt_rcvd[DTLS_MAX_BUF];

    sock_udp_t sock;
    sock_udp_ep_t local = SOCK_IPV6_EP_ANY;
    sock_udp_ep_t remote = SOCK_IPV6_EP_ANY;

    /* The behavior of sock_secure_read() will be different */
    secure_session.flag |= TLSMAN_FLAG_SIDE_SERVER;

    /* TODO Prepare (thread) messages reception */


    local.port = DTLS_DEFAULT_PORT;
    if (sock_udp_create(&sock, &local, NULL, 0) < 0) {
        puts("ERROR: Unable create sock.");
        return;
    }

    ssize_t res = sock_secure_initialized(&secure_session, _resp_handler, (void *)&sock,
                            (sock_secure_ep_t*)&local,
                            (sock_secure_ep_t *)&remote);

    if (res != 0) {
        puts("ERROR: Unable to init sock_secure!");
        return;
    }

    /* NOTE: Must not call sock_secure_update()? */

#if ENABLE_DEBUG
    ipv6_addr_t addrs[GNRC_NETIF_IPV6_ADDRS_NUMOF];
    gnrc_netif_t *netif = gnrc_netif_iter(NULL);
    if ((netif != NULL) &&
        ((res = gnrc_netif_ipv6_addrs_get(netif, addrs, sizeof(addrs))) > 0)) {
        printf("Listening (D)TLS request in: ");
        for (unsigned i = 0; i < (res / sizeof(ipv6_addr_t)); i++) {
            printf("[");
            ipv6_addr_print(&addrs[i]);
            printf("]:%u\n", DTLS_DEFAULT_PORT);
        }
    }
#endif

  while(sock_secure_read(&secure_session))
  {
    xtimer_usleep(500);
  }

  sock_secure_release(&secure_session);
  sock_udp_close(&sock);
  return;
}

int server_cmd(int argc, char **argv)
{
    (void) argc;
    (void) argv;
    /* TODO Upgrade this */
    _server_wrapper(NULL);
    return 0;
}

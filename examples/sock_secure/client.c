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

#include "net/sock/udp.h"
#include "net/sock/secure.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

/* Our custom response handler (optional) */
static void _resp_handler(uint8_t *data, size_t data_size, void *sock);

extern sock_secure_session_t secure_session;

static void _resp_handler(uint8_t *data, size_t data_size, void *sock)
{

    (void) sock;
    printf("Data received (%i bytes): \t--%s--\n", data_size, data);
}


int _client_side(char *addr_str,  uint16_t  port)
{

    /* TODO: Payload should be given by the user  */

    sock_udp_t sock;
    sock_udp_ep_t local = SOCK_IPV6_EP_ANY;
    sock_udp_ep_t remote = SOCK_IPV6_EP_ANY;

    uint8_t packet[] = "Hello world!";

    secure_session.flag |= TLSMAN_FLAG_SIDE_CLIENT;

    local.port = (unsigned short) port + 10;
    remote.port = (unsigned short) port;
    ipv6_addr_from_str((ipv6_addr_t *)&remote.addr.ipv6, addr_str);
    DEBUG("Remote server: [%s]:%u\n", addr_str, port);

    sock_udp_create(&sock, &local, &remote, 0);

    /* To be called when a new session is going to be created */
    ssize_t res = sock_secure_initialized(&secure_session, _resp_handler, (void *)&sock,
                            (sock_secure_ep_t*)&local,
                            (sock_secure_ep_t *)&remote);

    if (res != 0) {
        puts("ERROR: Unable to init sock_secure!");
        return -1;
    }

    /*
     * Optinal though can be very useful for asynchronous apps.
     * The main benefit should be to start the secure channel or restarted
     * before real data is ready.
     * If not called here, will be called by sock_secure_write() and
     * sock_secure_read()
     */
#if 0
    /* NOTE: After sock_secure_initialized() secure_session has all the sock info */
    sock_secure_update(&secure_session);
#endif

    /* We send the user data by means of the (D)TLS session */
    /* TODO: Handling error codes */
    sock_secure_write(&secure_session, packet, sizeof(packet));

    /* We check if there is Upper data received by the (D)TLS session
    * NOTE: IF not callback was given in sock_secure_initialized() the use of
    * DEBUG can help to see the raw received data
     (See sys/net/tlsman/tinydtls_sock.c).
    * Anyway, sock_secure_write() will operate properly without it.
    */
    xtimer_usleep(400); /* Just give time to the remote node to answer */
    /* FIXME The user will receive the data on the callback */
    /* TODO: Handling error codes */
    sock_secure_read(&secure_session);

    /* Release all the resources used for the secure session */
    sock_secure_release(&secure_session);
    /* NOTE: If update, write or send is called again at this point, a new secure session will be established */
    sock_udp_close(&sock);

    DEBUG("Client session finished\n");

    return 0;
}

int client_cmd(int argc, char **argv)
{

    if ((argc != 3)) {
        printf("usage: %s <IPv6 Address> <(UDP|TCP) Port>\n", argv[0]);
        return 1;
    }

    /* TODO Add safe guard with the atoi */

    return _client_side(argv[1], atoi(argv[2]) );
}

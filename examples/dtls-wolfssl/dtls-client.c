/*
 * Copyright (C) 2015 Freie Universität Berlin
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/*
 * **** This file incorporates work covered by the following copyright and ****
 * **** permission notice:                                                 ****
 *
 * Copyright (C) 2006-2018 wolfSSL Inc.
 *
 * This file is part of wolfSSL.
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 *
 */

#include <wolfssl/ssl.h>
#include <sock_tls.h>
#include <net/sock.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SERVER_PORT 11111

extern const unsigned char server_cert[788];
extern const unsigned long server_cert_len;

static sock_tls_t skv;
static sock_tls_t *sk = &skv;

int dtls_client(int argc, char **argv)
{
    int ret = 0;
    char buf[64] = "Hello from DTLS client!";
    int iface;
    char *addr_str = argv[1];
    sock_udp_ep_t local = SOCK_IPV6_EP_ANY;
    sock_udp_ep_t remote = SOCK_IPV6_EP_ANY;

    /* Parsing <address>[:<iface>]:Port */
    iface = ipv6_addr_split_iface(addr_str);
    if (iface == -1) {
        if (gnrc_netif_numof() == 1) {
            /* assign the single interface found in gnrc_netif_numof() */
            remote.netif = (uint16_t)gnrc_netif_iter(NULL)->pid;
        }
    }
    else {
        if (gnrc_netif_get_by_pid(iface) == NULL) {
            puts("ERROR: interface not valid");
            return -1;
        }
        remote.netif = (uint16_t)gnrc_netif_iter(NULL)->pid;
    }
    if (ipv6_addr_from_str((ipv6_addr_t *)remote.addr.ipv6, addr_str) == NULL) {
        puts("ERROR: unable to parse destination address");
        return -1;
    }
    if (argc != 2) {
        printf("Usage: %s <server-address>\n", argv[0]);
        return -1;
    }
    remote.port = SERVER_PORT;
    if (sock_dtls_create(sk, &local, &remote, 0, wolfDTLSv1_2_client_method()) != 0) {
        puts("ERROR: Unable to create DTLS sock");
        return -1;
    }

    /* Disable certificate validation from the client side */
    wolfSSL_CTX_set_verify(sk->ctx, SSL_VERIFY_NONE, 0);

    /* Load certificate file for the DTLS client */
    if (wolfSSL_CTX_use_certificate_buffer(sk->ctx, server_cert,
                server_cert_len, SSL_FILETYPE_ASN1 ) != SSL_SUCCESS)
    {
        printf("Error loading cert buffer\n");
        return -1;
    }

    /* attempt to connect until the connection is successful */
    do {
        printf("connecting to server...\n");
        if (sock_dtls_session_create(sk) < 0)
            return -1;
        ret = wolfSSL_connect(sk->ssl);
        if (ret != SSL_SUCCESS) {
            sock_dtls_session_destroy(sk);
        }
    } while(ret != SSL_SUCCESS);

    /* set remote endpoint */
    sock_dtls_set_endpoint(sk, &remote);

    /* send the hello message */
    wolfSSL_write(sk->ssl, buf, strlen(buf));

    /* wait for a reply, indefinitely */
    do {
        ret = wolfSSL_read(sk->ssl, buf, 63);
        printf("wolfSSL_read returned %d\r\n", ret);
    } while (ret <= 0);
    buf[ret] = (char)0;
    printf("Received: '%s'\r\n", buf);

    /* Clean up and exit. */
    printf("Closing connection.\r\n");
    sock_dtls_session_destroy(sk);
    sock_dtls_close(sk);
    return 0;
}

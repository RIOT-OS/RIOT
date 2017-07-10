/* Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * **** This file incorporates work covered by the following copyright and ****
 * **** permission notice:                                                 ****
 *
 * Copyright (C) 2006-2017 wolfSSL Inc.
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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <net/sock.h>
#include <wolfssl/ssl.h>

#define MODE_TLS 0
#define MODE_DTLS 1

void sock_dtls_close(sock_tls_t *sk)
{
    sock_udp_close(&sk->conn.udp);
}

void sock_dtls_set_endpoint(sock_tls_t *sk, const sock_udp_ep_t *addr)
{
    printf("wolfSSL: Setting peer address and port\n");
    memcpy(&sk->peer_addr, addr, sizeof (sock_udp_ep_t));
}

int sock_dtls_create(sock_tls_t *sock, const sock_udp_ep_t *local, const sock_udp_ep_t *remote, uint16_t flags, WOLFSSL_METHOD *method)
{
    int ret;
    if (!sock)
        return -EINVAL;
    XMEMSET(sock, 0, sizeof(sock_tls_t));
    sock->ctx = wolfSSL_CTX_new(method);
    if (!sock->ctx)
        return -ENOMEM;

    ret = sock_udp_create(&sock->conn.udp, local, remote, flags);
    if (ret < 0) {
        XFREE(sock->ctx, NULL, 0);
        return ret;
    }
    if (remote) {
        XMEMCPY(&sock->peer_addr, remote, sizeof(sock_udp_ep_t));
    }
    wolfSSL_SetIORecv(sock->ctx, GNRC_Receive);
    wolfSSL_SetIOSend(sock->ctx, GNRC_SendTo);
    return 0;
}

static int tls_session_create(sock_tls_t *sk)
{
    if (!sk || !sk->ctx)
        return -EINVAL;
    sk->ssl = wolfSSL_new(sk->ctx);
    if (sk->ssl == NULL) {
        printf("Error allocating ssl session\n");
        return -ENOMEM;
    }
    wolfSSL_SetIOReadCtx(sk->ssl, sk);
    wolfSSL_SetIOWriteCtx(sk->ssl, sk);
    return 0;
}

static void tls_session_destroy(sock_tls_t *sk)
{
    if (!sk || sk->ssl)
        return;
    wolfSSL_free(sk->ssl);
}

int sock_dtls_session_create(sock_tls_t *sk)
{
    return tls_session_create(sk);
}

void sock_dtls_session_destroy(sock_tls_t *sk)
{
    tls_session_destroy(sk);
}


#ifdef MODULE_SOCK_TCP
/* TODO */
#endif



#include <ctype.h>
int strncasecmp(const char *s1, const char * s2, unsigned int sz)
{
    for( ; sz>0; sz--)
        if(toupper(*s1++) != toupper(*s2++))
            return 1;
    return 0;
}

/*
 * Copyright (C) 2019 HAW Hamburg
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
 * @brief       DTLS sock client example
 *
 * @author      Aiman Ismail <muhammadaimanbin.ismail@haw-hamburg.de>
 */

#include <stdio.h>

#include "net/sock/udp.h"
#include "net/sock/dtls.h"
#include "net/ipv6/addr.h"
#include "net/credman.h"

#include "tinydtls_keys.h"

#ifndef DTLS_DEFAULT_PORT
#define DTLS_DEFAULT_PORT 20220 /* DTLS default port */
#endif

#define SOCK_DTLS_CLIENT_TAG (2)

#ifdef CONFIG_DTLS_ECC
static const ecdsa_public_key_t other_pubkeys[] = {
    { .x = ecdsa_pub_key_x, .y = ecdsa_pub_key_y },
};

static const credman_credential_t credential = {
    .type = CREDMAN_TYPE_ECDSA,
    .tag = SOCK_DTLS_CLIENT_TAG,
    .params = {
        .ecdsa = {
            .private_key = ecdsa_priv_key,
            .public_key = {
                .x = ecdsa_pub_key_x,
                .y = ecdsa_pub_key_y,
            },
            .client_keys = (ecdsa_public_key_t *)other_pubkeys,
            .client_keys_size = ARRAY_SIZE(other_pubkeys),
        }
    },
};

#else /* ifdef CONFIG_DTLS_PSK */
static const uint8_t psk_id_0[] = PSK_DEFAULT_IDENTITY;
static const uint8_t psk_key_0[] = PSK_DEFAULT_KEY;

static const credman_credential_t credential = {
    .type = CREDMAN_TYPE_PSK,
    .tag = SOCK_DTLS_CLIENT_TAG,
    .params = {
        .psk = {
            .key = { .s = psk_key_0, .len = sizeof(psk_key_0) - 1, },
            .id = { .s = psk_id_0, .len = sizeof(psk_id_0) - 1, },
        }
    },
};
#endif

static int client_send(char *addr_str, char *data, size_t datalen)
{
    ssize_t res;
    sock_udp_t udp_sock;
    sock_dtls_t dtls_sock;
    sock_dtls_session_t session;
    sock_udp_ep_t remote = SOCK_IPV6_EP_ANY;
    sock_udp_ep_t local = SOCK_IPV6_EP_ANY;
    local.port = 12345;
    remote.port = DTLS_DEFAULT_PORT;
    uint8_t buf[DTLS_HANDSHAKE_BUFSIZE];

    /* get interface */
    char* iface = ipv6_addr_split_iface(addr_str);
    if (iface) {
        int pid = atoi(iface);
        if (gnrc_netif_get_by_pid(pid) == NULL) {
            puts("Invalid network interface");
            return -1;
        }
        remote.netif = pid;
    } else if (gnrc_netif_numof() == 1) {
        /* assign the single interface found in gnrc_netif_numof() */
        remote.netif = gnrc_netif_iter(NULL)->pid;
    } else {
        /* no interface is given, or given interface is invalid */
        /* FIXME This probably is not valid with multiple interfaces */
        remote.netif = SOCK_ADDR_ANY_NETIF;
    }

    if (!ipv6_addr_from_str((ipv6_addr_t *)remote.addr.ipv6, addr_str)) {
        puts("Error parsing destination address");
        return -1;
    }

    if (sock_udp_create(&udp_sock, &local, NULL, 0) < 0) {
        puts("Error creating UDP sock");
        return -1;
    }

    if (sock_dtls_create(&dtls_sock, &udp_sock,
                         SOCK_DTLS_CLIENT_TAG,
                         SOCK_DTLS_1_2, SOCK_DTLS_CLIENT) < 0) {
        puts("Error creating DTLS sock");
        sock_udp_close(&udp_sock);
        return -1;
    }

    res = credman_add(&credential);
    if (res < 0 && res != CREDMAN_EXIST) {
        /* ignore duplicate credentials */
        printf("Error cannot add credential to system: %d\n", (int)res);
        return -1;
    }

    res = sock_dtls_session_init(&dtls_sock, &remote, &session);
    if (res <= 0) {
        return res;
    }

    res = sock_dtls_recv(&dtls_sock, &session, buf, sizeof(buf),
                         SOCK_NO_TIMEOUT);
    if (res != -SOCK_DTLS_HANDSHAKE) {
        printf("Error creating session: %d\n", (int)res);
        sock_dtls_close(&dtls_sock);
        sock_udp_close(&udp_sock);
        return -1;
    }
    printf("Connection to server successful\n");

    if (sock_dtls_send(&dtls_sock, &session, data, datalen, 0) < 0) {
        puts("Error sending data");
    }
    else {
        printf("Sent DTLS message\n");

        uint8_t rcv[512];
        if ((res = sock_dtls_recv(&dtls_sock, &session, rcv, sizeof(rcv),
                                    SOCK_NO_TIMEOUT)) >= 0) {
            printf("Received %d bytes: \"%.*s\"\n", (int)res, (int)res,
                   (char *)rcv);
        }
    }

    puts("Terminating");
    sock_dtls_session_destroy(&dtls_sock, &session);
    sock_dtls_close(&dtls_sock);
    sock_udp_close(&udp_sock);
    return 0;
}

int dtls_client_cmd(int argc, char **argv)
{
    if (argc != 3) {
        printf("usage %s <addr> <data>\n", argv[0]);
        return 1;
    }

    return client_send(argv[1], argv[2], strlen(argv[2]));
}

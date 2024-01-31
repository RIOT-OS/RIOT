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

#include "kernel_defines.h"

#include "net/gnrc/netif.h"
#include "net/sock/udp.h"
#include "net/sock/dtls.h"
#include "net/sock/dtls/creds.h"
#include "net/ipv6/addr.h"
#include "net/credman.h"
#include "net/sock/util.h"
#include "net/utils.h"

#include "tinydtls_keys.h"

#ifndef DTLS_DEFAULT_PORT
#define DTLS_DEFAULT_PORT 20220 /* DTLS default port */
#endif

#define SOCK_DTLS_CLIENT_TAG_0 (2)
#define SOCK_DTLS_CLIENT_TAG_1 (3)

#ifdef CONFIG_DTLS_ECC
static const ecdsa_public_key_t other_pubkeys0[] = {
    { .x = ecdsa_pub_key0_x, .y = ecdsa_pub_key0_y },
};

static const credman_credential_t credential0 = {
    .type = CREDMAN_TYPE_ECDSA,
    .tag = SOCK_DTLS_CLIENT_TAG_0,
    .params = {
        .ecdsa = {
            .private_key = ecdsa_priv_key0,
            .public_key = {
                .x = ecdsa_pub_key0_x,
                .y = ecdsa_pub_key0_y,
            },
            .client_keys = (ecdsa_public_key_t *)other_pubkeys0,
            .client_keys_size = ARRAY_SIZE(other_pubkeys0),
        }
    },
};

#else /* ifdef CONFIG_DTLS_PSK */
static const uint8_t psk_id_0[] = PSK_WRONG_IDENTITY;
static const uint8_t psk_key_0[] = PSK_WRONG_KEY;

static const uint8_t psk_id_1[] = PSK_DEFAULT_IDENTITY;
static const uint8_t psk_key_1[] = PSK_DEFAULT_KEY;
static const char psk_id_1_hint[] = PSK_DEFAULT_HINT;

static const credman_credential_t credential0 = {
    .type = CREDMAN_TYPE_PSK,
    .tag = SOCK_DTLS_CLIENT_TAG_0,
    .params = {
        .psk = {
            .key = { .s = psk_key_0, .len = sizeof(psk_key_0) - 1, },
            .id = { .s = psk_id_0, .len = sizeof(psk_id_0) - 1, },
        }
    },
};

static const credman_credential_t credential1 = {
    .type = CREDMAN_TYPE_PSK,
    .tag = SOCK_DTLS_CLIENT_TAG_1,
    .params = {
        .psk = {
            .key = { .s = psk_key_1, .len = sizeof(psk_key_1) - 1, },
            .id = { .s = psk_id_1, .len = sizeof(psk_id_1) - 1, },
            .hint = { .s = psk_id_1_hint, .len = sizeof(psk_id_1_hint) - 1, },
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
    credman_tag_t tag = SOCK_DTLS_CLIENT_TAG_0;

    /* get interface */
    netif_t *netif;
    res = netutils_get_ipv6((void *)&remote.addr, &netif, addr_str);
    if (res) {
        printf("Error parsing remote address\n");
        return res;
    }
    if (netif) {
        remote.netif = netif_get_id(netif);
    }

    res = credman_add(&credential0);
    if (res < 0 && res != CREDMAN_EXIST) {
        /* ignore duplicate credentials */
        printf("Error cannot add credential to system: %" PRIdSIZE "\n", res);
        sock_udp_close(&udp_sock);
        return -1;
    }

#if IS_ACTIVE(CONFIG_DTLS_PSK)
    /* register a second PSK credential */
    res = credman_add(&credential1);
    if (res < 0 && res != CREDMAN_EXIST) {
        /* ignore duplicate credentials */
        printf("Error cannot add credential to system: %" PRIdSIZE "\n", res);
        sock_udp_close(&udp_sock);
        return -1;
    }
    tag = SOCK_DTLS_CLIENT_TAG_1;
#endif

    res = sock_dtls_establish_session(&udp_sock, &dtls_sock, &session, tag,
                                      &local, &remote, buf, sizeof(buf));
   if (res) {
        sock_udp_close(&udp_sock);
        printf("Error establishing connection: %d\n", (int)res);
        return res;
    }

    printf("Connection to server successful\n");

    if (sock_dtls_send(&dtls_sock, &session, data, datalen, 0) < 0) {
        puts("Error sending data");
    }
    else {
        printf("Sent DTLS message\n");

        if ((res = sock_dtls_recv(&dtls_sock, &session, buf, sizeof(buf),
                                    SOCK_NO_TIMEOUT)) >= 0) {
            printf("Received %" PRIdSIZE " bytes: \"%.*s\"\n", res, (int)res,
                   (char *)buf);
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

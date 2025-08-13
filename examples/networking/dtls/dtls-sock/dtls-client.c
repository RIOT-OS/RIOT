/*
 * SPDX-FileCopyrightText: 2019 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
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
#include "shell.h"

#include "dtls_client_credentials.h"

#ifndef DTLS_DEFAULT_PORT
#define DTLS_DEFAULT_PORT (20220) /* DTLS default port */
#endif

static bool _client_credentials_configured = false;

/* Credman tags to select which credentials to use */
#define SOCK_DTLS_CLIENT_TAG_0 (2)
#define SOCK_DTLS_CLIENT_TAG_1 (3)

static const ecdsa_public_key_t server_public_keys[] = {
    { .x = known_server_public_key_0_x, .y = known_server_public_key_0_y },
    { .x = known_server_public_key_1_x, .y = known_server_public_key_1_y },
};

static const credman_credential_t ecc_credential_0 = {
    .type = CREDMAN_TYPE_ECDSA,
    .tag = SOCK_DTLS_CLIENT_TAG_0,
    .params = {
        .ecdsa = {
            .private_key = client_private_key_0,
            .public_key = {
                .x = client_public_key_0_x,
                .y = client_public_key_0_y,
            },
            .client_keys = (ecdsa_public_key_t *)server_public_keys,
            .client_keys_size = ARRAY_SIZE(server_public_keys),
        }
    },
};

static const uint8_t psk_id_0[] = CLIENT_PSK_IDENTITY_0;
static const uint8_t psk_key_0[] = CLIENT_PSK_IDENTITY_0_KEY;
static const char psk_id_0_hint[] = CLIENT_PSK_IDENTITY_0_HINT;

static const uint8_t psk_id_1[] = CLIENT_PSK_IDENTITY_1;
static const uint8_t psk_key_1[] = CLIENT_PSK_IDENTITY_1_KEY;
static const char psk_id_1_hint[] = CLIENT_PSK_IDENTITY_1_HINT;

static const credman_credential_t psk_credential_0 = {
    .type = CREDMAN_TYPE_PSK,
    .tag = SOCK_DTLS_CLIENT_TAG_0,
    .params = {
        .psk = {
            .key = { .s = psk_key_0, .len = sizeof(psk_key_0) - 1, },
            .id = { .s = psk_id_0, .len = sizeof(psk_id_0) - 1, },
            .hint = { .s = psk_id_0_hint, .len = sizeof(psk_id_0_hint) - 1, },
        }
    },
};

static const credman_credential_t psk_credential_1 = {
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

static credman_tag_t _client_psk_cb(sock_dtls_t *sock, sock_udp_ep_t *ep, credman_tag_t tags[],
                                    unsigned tags_len, const char *hint, size_t hint_len)
{
    (void) sock;
    (void) tags;
    (void) tags_len;

    char addrstr[IPV6_ADDR_MAX_STR_LEN];
    uint16_t port;

    sock_udp_ep_fmt(ep, addrstr, &port);
    printf("From [%s]:%" PRIu16" \n", addrstr, port);

    /* if we got a hint, try to use it to determine which PSK to use */
    if (hint && hint_len) {
        printf("Client got hint: %.*s\n", (unsigned)hint_len, hint);

        if (hint_len == sizeof(psk_id_0_hint) &&
            !memcmp(hint, psk_id_0_hint, sizeof(psk_id_0_hint) - 1)) {
            return SOCK_DTLS_CLIENT_TAG_0;
        }

        if (hint_len == sizeof(psk_id_1_hint) &&
            !memcmp(hint, psk_id_1_hint, sizeof(psk_id_1_hint) - 1)) {
            return SOCK_DTLS_CLIENT_TAG_1;
        }
    }

    return CREDMAN_TAG_EMPTY;
}

static int _configure_client_credentials(void)
{
    /* register the credentials on credman */
    if (IS_ACTIVE(CONFIG_DTLS_ECC)) {
        if (credman_add(&ecc_credential_0) != CREDMAN_OK) {
            puts("Error cannot add ECC credential 0 to system");
            return -1;
        }
    }
    else if (IS_ACTIVE(CONFIG_DTLS_PSK)) {
        if (credman_add(&psk_credential_0) != CREDMAN_OK) {
            puts("Error cannot add PSK credential 0 to system");
            return -1;
        }

        if (credman_add(&psk_credential_1) != CREDMAN_OK) {
            puts("Error cannot add PSK credential 1 to system");
            return -1;
        }
    }
    return 0;
}

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
    netif_t *netif;
    res = netutils_get_ipv6((void *)&remote.addr, &netif, addr_str);
    if (res) {
        printf("Error parsing remote address\n");
        return res;
    }
    if (netif) {
        remote.netif = netif_get_id(netif);
    }

    if (sock_udp_create(&udp_sock, &local, NULL, 0) < 0) {
        puts("Error creating UDP sock");
        return -1;
    }

    /*
     * Currently DTLS sock needs one and only one credential for the
     * initialization. Subsequent credentials are made available to the sock
     * by means of `sock_dtls_add_credential`.
     */
    if (sock_dtls_create(&dtls_sock, &udp_sock, SOCK_DTLS_CLIENT_TAG_0,
                         SOCK_DTLS_1_2, SOCK_DTLS_CLIENT) < 0) {
        puts("Error creating DTLS sock");
        sock_udp_close(&udp_sock);
        return -1;
    }

    if (IS_ACTIVE(CONFIG_DTLS_PSK)) {
        /* make the new credential available to the sock */
        if (sock_dtls_add_credential(&dtls_sock, SOCK_DTLS_CLIENT_TAG_1) < 0) {
            puts("Error cannot add second PSK credential to the sock");
            return -1;
        }

        /* register a callback for PSK credential selection */
        sock_dtls_set_client_psk_cb(&dtls_sock, _client_psk_cb);
    }

    res = sock_dtls_session_init(&dtls_sock, &remote, &session);
    if (res <= 0) {
        return res;
    }

    res = sock_dtls_recv(&dtls_sock, &session, buf, sizeof(buf),
                         SOCK_NO_TIMEOUT);
    if (res != -SOCK_DTLS_HANDSHAKE) {
        printf("Error creating session: %" PRIiSIZE "\n", res);
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

static int _client_cmd(int argc, char **argv)
{
    if (argc != 3) {
        printf("usage %s <addr> <data>\n", argv[0]);
        return 1;
    }

    if (!_client_credentials_configured) {
        int res = _configure_client_credentials();
        if (res < 0) {
            return res;
        }
        _client_credentials_configured = true;
    }

    return client_send(argv[1], argv[2], strlen(argv[2]));
}

SHELL_COMMAND(dtlsc, "Start a DTLS client", _client_cmd);

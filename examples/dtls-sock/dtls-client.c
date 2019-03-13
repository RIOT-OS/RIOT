#include <stdio.h>

#include "net/sock/udp.h"
#include "net/sock/dtls.h"
#include "net/ipv6/addr.h"

#include "client/keys.h"

#ifdef DTLS_ECC
#include "client/server_pub_keys.h"
#endif /* DTLS_ECC */

/* TinyDTLS WARNING check */
#ifdef WITH_RIOT_SOCKETS
#error TinyDTLS is set to use sockets but the app is configured for socks.
#endif

/* this credential management is too implementation specific, should be
 * should be improved later on */
#ifdef DTLS_PSK
static uint8_t psk_id_0[] = PSK_DEFAULT_IDENTITY;
static uint8_t psk_key_0[] = PSK_DEFAULT_KEY;
static int _id_storage(sock_dtls_t *sock, sock_dtls_session_t *session,
                       const unsigned char *hint, size_t hint_len,
                       unsigned char *id, size_t id_len)
{
    (void)sock;
    (void)session;
    (void)hint;
    (void)hint_len;
    if (id_len < sizeof(psk_id_0)) {
        return 0;
    }
    else {
        memcpy(id, psk_id_0, sizeof(psk_id_0) - 1);
        return sizeof(psk_id_0) - 1;
    }
}

static int _key_storage(sock_dtls_t *sock, sock_dtls_session_t *session,
                        const unsigned char *id, size_t id_len,
                        unsigned char *key, size_t key_len)
{
    (void)sock;
    (void)session;
    (void)id;
    (void)id_len;
    if (key_len < sizeof(psk_key_0) - 1) {
        return 0;
    }
    else {
        memcpy(key, psk_key_0, sizeof(psk_key_0) - 1);
        return sizeof(psk_key_0) - 1;
    }
}
#endif /* DTLS_PSK */

#ifdef DTLS_ECC
static dtls_ecdsa_key_t cert = {
    .priv_key = ecdsa_priv_key,
    .pub_key_x = ecdsa_pub_key_x,
    .pub_key_y = ecdsa_pub_key_y,
    .curve = DTLS_ECDH_CURVE_SECP256R1
};

static int _ecdsa_storage(sock_dtls_t *sock, sock_dtls_session_t *session,
                          dtls_ecdsa_key_t **key)
{
    (void)sock;
    (void)session;
    *key = &cert;
    return 0;
}

static int _ecdsa_verify(sock_dtls_t *sock, sock_dtls_session_t *session,
                         const unsigned char *pub_x, const unsigned char *pub_y,
                         size_t key_size)
{
    (void)sock;
    (void)session;
    if (!key_size) {
        return -1;
    }
    /* just checking every byte for now */
    for (unsigned i = 0; i < key_size; i++) {
        if ((pub_x[i] != server_ecdsa_pub_key_x[i]) ||
            (pub_y[i] != server_ecdsa_pub_key_y[i])) {
            puts("Server keys do not match");
            return -1;
        }
    }
    return 0;
}

#endif /* DTLS_ECC */

static void client_send(char *addr_str, uint8_t port, char *data, size_t datalen)
{
    uint8_t rcv[512];
    ssize_t res;
    sock_udp_ep_t remote_ep;
    sock_dtls_session_t remote;
    sock_dtls_t sock = {
#ifdef DTLS_PSK
        .psk = {
            .psk_id_storage = _id_storage,
            .psk_key_storage = _key_storage
        },
#endif /* DTLS_PSK */
#ifdef DTLS_ECC
        .ecdsa = {
            .ecdsa_storage = _ecdsa_storage,
            .ecdsa_verify = _ecdsa_verify
        }
#endif /* DTLS_ECC */
    };

    sock_udp_ep_t local_ep = SOCK_IPV6_EP_ANY;
    local_ep.port = 12345;
    sock_udp_t udp_sock;
    sock_udp_create(&udp_sock, &local_ep, NULL, 0);

    ipv6_addr_from_str((ipv6_addr_t *)&remote_ep.addr.ipv6, addr_str);
    //remote_ep.port = port;
    (void)port;
    remote_ep.port = 20220;

    res = sock_dtls_create(&sock, &udp_sock, 0);
    if (res < 0) {
        puts("Error creating DTLS sock");
        return;
    }

    res = sock_dtls_establish_session(&sock, &remote_ep, &remote);
    if (res < 0) {
        printf("Error establishing session: %d\n", res);
        return;
    }

    res = sock_dtls_send(&sock, &remote, data, datalen);
    if (res < 0) {
        printf("Error sending DTLS message: %d\n", res);
        return;
    }
    printf("Sent %d bytes of DTLS message: %s\n", res, data);

    res = sock_dtls_recv(&sock, &remote, rcv, sizeof(rcv), SOCK_NO_TIMEOUT);
    if (res < 0) {
        printf("Error receiving DTLS message: %d\n", res);
        return;
    }
    else if (res == 0) {
        puts("No message received");
    }
    printf("Received %d bytes of DTLS message: %.*s\n", res, res, rcv);

    puts("Terminating");
    sock_dtls_close_session(&sock, &remote);
    sock_dtls_destroy(&sock);
}

int dtls_client_cmd(int argc, char **argv)
{
    if (argc != 4) {
        printf("usage %s <addr> <port> <data>\n", argv[0]);
        return 1;
    }
    client_send(argv[1], atoi(argv[2]), argv[3], strlen(argv[3]));
    return 0;
}


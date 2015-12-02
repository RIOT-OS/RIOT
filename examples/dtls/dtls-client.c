/*
 * Copyright (C) 2015 Olaf Bergmann <bergmann@tzi.org>
 * Copyright (C) 2015 Hauke Mehrtens <hauke@hauke-m.de>
 * Copyright (C) 2015 INRIA
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
 * @brief       TinyDTLS client
 *
 * @author      Olaf Bergmann <bergmann@tzi.org>
 * @author      Hauke Mehrtens <hauke@hauke-m.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 *
 * @}
 */

#include "tinydtls.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <signal.h>

#include "thread.h"
#include "assert.h"
#include "byteorder.h"
#include "global.h"
#include "tinydtls_debug.h"
#include "dtls.h"
#include "riot_tinydtls.h"

#include "dtls_app.h"

#define DEFAULT_PORT 20220

#define PSK_DEFAULT_IDENTITY "Client_identity"
#define PSK_DEFAULT_KEY      "secretPSK"
#define PSK_OPTIONS          "i:k:"

#define CLIENT_QUEUE_SIZE     (8)
static msg_t _client_msg_queue[CLIENT_QUEUE_SIZE];

char client_reader[THREAD_STACKSIZE_DEFAULT];

typedef struct {
    size_t length;              /* length of string */
    unsigned char *s;           /* string data */
} dtls_str;

static dtls_context_t *dtls_context = NULL;
static session_t dst;


static const unsigned char ecdsa_priv_key[] = {
    0x41, 0xC1, 0xCB, 0x6B, 0x51, 0x24, 0x7A, 0x14,
    0x43, 0x21, 0x43, 0x5B, 0x7A, 0x80, 0xE7, 0x14,
    0x89, 0x6A, 0x33, 0xBB, 0xAD, 0x72, 0x94, 0xCA,
    0x40, 0x14, 0x55, 0xA1, 0x94, 0xA9, 0x49, 0xFA
};

static const unsigned char ecdsa_pub_key_x[] = {
    0x36, 0xDF, 0xE2, 0xC6, 0xF9, 0xF2, 0xED, 0x29,
    0xDA, 0x0A, 0x9A, 0x8F, 0x62, 0x68, 0x4E, 0x91,
    0x63, 0x75, 0xBA, 0x10, 0x30, 0x0C, 0x28, 0xC5,
    0xE4, 0x7C, 0xFB, 0xF2, 0x5F, 0xA5, 0x8F, 0x52
};

static const unsigned char ecdsa_pub_key_y[] = {
    0x71, 0xA0, 0xD4, 0xFC, 0xDE, 0x1A, 0xB8, 0x78,
    0x5A, 0x3C, 0x78, 0x69, 0x35, 0xA7, 0xCF, 0xAB,
    0xE9, 0x3F, 0x98, 0x72, 0x09, 0xDA, 0xED, 0x0B,
    0x4F, 0xAB, 0xC3, 0x6F, 0xC7, 0x72, 0xF8, 0x29
};

#ifdef DTLS_PSK
/* The PSK information for DTLS */
#define PSK_ID_MAXLEN 256
#define PSK_MAXLEN 256
static unsigned char psk_id[PSK_ID_MAXLEN];
static size_t psk_id_length = 0;
static unsigned char psk_key[PSK_MAXLEN];
static size_t psk_key_length = 0;

/* This function is the "key store" for tinyDTLS. It is called to
 * retrieve a key for the given identity within this particular
 * session. */
static int get_psk_info(struct dtls_context_t *ctx, const session_t *session,
                        dtls_credentials_type_t type, const unsigned char *id,
                        size_t id_len, unsigned char *result,
                        size_t result_length)
{
    (void) ctx;
    (void) session;

    switch (type) {
        case DTLS_PSK_IDENTITY:
            if (id_len) {
                dtls_debug("got psk_identity_hint: '%.*s'\n", id_len, id);
            }

            if (result_length < psk_id_length) {
                dtls_warn("cannot set psk_identity -- buffer too small\n");
                return dtls_alert_fatal_create(DTLS_ALERT_INTERNAL_ERROR);
            }

            memcpy(result, psk_id, psk_id_length);
            return psk_id_length;
        case DTLS_PSK_KEY:
            if (id_len != psk_id_length || memcmp(psk_id, id, id_len) != 0) {
                dtls_warn("PSK for unknown id requested, exiting\n");
                return dtls_alert_fatal_create(DTLS_ALERT_ILLEGAL_PARAMETER);
            }
            else if (result_length < psk_key_length) {
                dtls_warn("cannot set psk -- buffer too small\n");
                return dtls_alert_fatal_create(DTLS_ALERT_INTERNAL_ERROR);
            }

            memcpy(result, psk_key, psk_key_length);
            return psk_key_length;
        default:
            dtls_warn("unsupported request type: %d\n", type);
    }

    return dtls_alert_fatal_create(DTLS_ALERT_INTERNAL_ERROR);
}
#endif /* DTLS_PSK */

#ifdef DTLS_ECC
static int get_ecdsa_key(struct dtls_context_t *ctx, const session_t *session,
                         const dtls_ecdsa_key_t **result)
{
    (void) ctx;
    (void) session;
    static const dtls_ecdsa_key_t ecdsa_key = {
        .curve      = DTLS_ECDH_CURVE_SECP256R1,
        .priv_key   = ecdsa_priv_key,
        .pub_key_x  = ecdsa_pub_key_x,
        .pub_key_y  = ecdsa_pub_key_y
    };

    *result = &ecdsa_key;
    return 0;
}

static int verify_ecdsa_key(struct dtls_context_t *ctx,
                            const session_t *session,
                            const unsigned char *other_pub_x,
                            const unsigned char *other_pub_y, size_t key_size)
{
    (void) ctx;
    (void) session;
    (void) other_pub_x;
    (void) other_pub_y;
    (void) key_size;
    return 0;
}
#endif /* DTLS_ECC */

static int try_send(struct dtls_context_t *ctx, session_t *dst, char *buf, int len)
{
    return dtls_write(ctx, dst, (uint8_t *)buf, len);
}


static int read_from_peer(struct dtls_context_t *ctx, session_t *session,
                          uint8_t *data, size_t len)
{
    (void) ctx;
    (void) session;
    size_t i;

    for (i = 0; i < len; i++)
    {
        printf("%c", data[i]);
    }
    return 0;
}

static dtls_handler_t cb = {
    .write              = send_to_peer,
    .read               = read_from_peer,
    .event              = NULL,
#ifdef DTLS_PSK
    .get_psk_info       = get_psk_info,
#endif /* DTLS_PSK */
#ifdef DTLS_ECC
    .get_ecdsa_key      = get_ecdsa_key,
    .verify_ecdsa_key   = verify_ecdsa_key
#endif /* DTLS_ECC */
};

#define DTLS_CLIENT_CMD_CLOSE "client:close"
#define DTLS_CLIENT_CMD_RENEGOTIATE "client:renegotiate"

static int fd;

void *_client_trampoline(void *args)
{
    msg_init_queue(_client_msg_queue, CLIENT_QUEUE_SIZE);
    log_t log_level = DTLS_LOG_DEBUG;

    dtls_init();

#ifdef DTLS_PSK
    psk_id_length = strlen(PSK_DEFAULT_IDENTITY);
    psk_key_length = strlen(PSK_DEFAULT_KEY);
    memcpy(psk_id, PSK_DEFAULT_IDENTITY, psk_id_length);
    memcpy(psk_key, PSK_DEFAULT_KEY, psk_key_length);
#endif /* DTLS_PSK */

    dtls_set_log_level(log_level);

    memset(&dst, 0, sizeof(session_t));
    dst.size = sizeof(struct sockaddr_in6);
    dst.addr.sin6.sin6_family = AF_INET6;

    /* parse destination address */
    if (inet_pton(AF_INET6, (char*) args, &dst.addr.sin6.sin6_addr) != 1) {
        puts("Error: unable to parse destination address");
        return NULL;
    }

    /* use port number from command line when specified or the listen
       port, otherwise */
    dst.addr.sin6.sin6_port = byteorder_htons(DEFAULT_PORT).u16;

    /* init socket and set it to non-blocking */
    fd = socket(dst.addr.sa.sa_family, SOCK_DGRAM, 0);

    if (fd < 0) {
        dtls_alert("socket: %s\n", strerror(errno));
        return 0;
    }

    dtls_context = dtls_new_context(&fd);
    if (!dtls_context) {
        dtls_emerg("cannot create context\n");
        exit(-1);
    }

    dtls_set_handler(dtls_context, &cb);

    dtls_connect(dtls_context, &dst);

    dtls_reader(dtls_context);

    return NULL;
}

int dtls_client_cmd(int argc, char **argv)
{
    if (argc < 0) {
        printf("Usage: %s <dst>\n", argv[0]);
        return -1;
    }

    thread_create(client_reader, sizeof(client_reader), THREAD_PRIORITY_MAIN - 1,
                  CREATE_STACKTEST, _client_trampoline, argv[1], "DTLS client");


    return 0;
}

int dtls_send_cmd(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: %s <data>\n", argv[1]);
        return -1;
    }
    try_send(dtls_context, &dst, argv[1], strlen(argv[1]));
    return 0;
}

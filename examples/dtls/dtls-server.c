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
 * @brief       TinyDTLS server
 *
 * @author      Olaf Bergmann <bergmann@tzi.org>
 * @author      Hauke Mehrtens <hauke@hauke-m.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 *
 * @}
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <signal.h>

#include "thread.h"
#include "tinydtls.h"
#include "dtls.h"
#include "tinydtls_debug.h"
#include "riot_tinydtls.h"
#include "dtls_app.h"

#define DEFAULT_PORT 20220

char server_stack[THREAD_STACKSIZE_DEFAULT];

static const unsigned char ecdsa_priv_key[] = {
    0xD9, 0xE2, 0x70, 0x7A, 0x72, 0xDA, 0x6A, 0x05,
    0x04, 0x99, 0x5C, 0x86, 0xED, 0xDB, 0xE3, 0xEF,
    0xC7, 0xF1, 0xCD, 0x74, 0x83, 0x8F, 0x75, 0x70,
    0xC8, 0x07, 0x2D, 0x0A, 0x76, 0x26, 0x1B, 0xD4
};

static const unsigned char ecdsa_pub_key_x[] = {
    0xD0, 0x55, 0xEE, 0x14, 0x08, 0x4D, 0x6E, 0x06,
    0x15, 0x59, 0x9D, 0xB5, 0x83, 0x91, 0x3E, 0x4A,
    0x3E, 0x45, 0x26, 0xA2, 0x70, 0x4D, 0x61, 0xF2,
    0x7A, 0x4C, 0xCF, 0xBA, 0x97, 0x58, 0xEF, 0x9A
};

static const unsigned char ecdsa_pub_key_y[] = {
    0xB4, 0x18, 0xB6, 0x4A, 0xFE, 0x80, 0x30, 0xDA,
    0x1D, 0xDC, 0xF4, 0xF4, 0x2E, 0x2F, 0x26, 0x31,
    0xD0, 0x43, 0xB1, 0xFB, 0x03, 0xE2, 0x2F, 0x4D,
    0x17, 0xDE, 0x43, 0xF9, 0xF9, 0xAD, 0xEE, 0x70
};

#if 0
/* SIGINT handler: set quit to 1 for graceful termination */
void
handle_sigint(int signum)
{
    dsrv_stop(dsrv_get_context());
}
#endif

#ifdef DTLS_PSK
/* This function is the "key store" for tinyDTLS. It is called to
 * retrieve a key for the given identity within this particular
 * session. */
static int get_psk_info(struct dtls_context_t *ctx,
                        const session_t *session,
                        dtls_credentials_type_t type,
                        const unsigned char *id, size_t id_len,
                        unsigned char *result,
                        size_t result_length)
{
    (void) ctx;
    (void) session;

    struct keymap_t {
        unsigned char *id;
        size_t id_length;
        unsigned char *key;
        size_t key_length;
    } psk[3] = {
        { (unsigned char *)"Client_identity",  15,
          (unsigned char *)"secretPSK", 9 },
        { (unsigned char *)"default identity", 16,
          (unsigned char *)"\x11\x22\x33", 3 },
        { (unsigned char *)"\0",			   2,
          (unsigned char *)"", 1 }
    };

    if (type != DTLS_PSK_KEY) {
        return 0;
    }

    if (id) {
        size_t i;
        for (i = 0; i < sizeof(psk) / sizeof(struct keymap_t); i++) {
            if (id_len == psk[i].id_length && memcmp(id, psk[i].id, id_len) == 0) {
                if (result_length < psk[i].key_length) {
                    dtls_warn("buffer too small for PSK");
                    return dtls_alert_fatal_create(DTLS_ALERT_INTERNAL_ERROR);
                }

                memcpy(result, psk[i].key, psk[i].key_length);
                return psk[i].key_length;
            }
        }
    }

    return dtls_alert_fatal_create(DTLS_ALERT_DECRYPT_ERROR);
}

#endif /* DTLS_PSK */

#ifdef DTLS_ECC
static int get_ecdsa_key(struct dtls_context_t *ctx,
                         const session_t *session,
                         const dtls_ecdsa_key_t **result)
{
    (void) ctx;
    (void) session;
    static const dtls_ecdsa_key_t ecdsa_key = {
        .curve		= DTLS_ECDH_CURVE_SECP256R1,
        .priv_key	= ecdsa_priv_key,
        .pub_key_x	= ecdsa_pub_key_x,
        .pub_key_y	= ecdsa_pub_key_y
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

#define DTLS_SERVER_CMD_CLOSE "server:close"
#define DTLS_SERVER_CMD_RENEGOTIATE "server:renegotiate"

static int read_from_peer(struct dtls_context_t *ctx,
                          session_t *session, uint8 *data,
                          size_t len)
{
    size_t i;

    for (i = 0; i < len; i++)
        printf("%c", data[i]);
    if (len >= strlen(DTLS_SERVER_CMD_CLOSE) &&
        !memcmp(data, DTLS_SERVER_CMD_CLOSE, strlen(DTLS_SERVER_CMD_CLOSE))) {
        printf("server: closing connection\n");
        dtls_close(ctx, session);
        return len;
    }
    else if (len >= strlen(DTLS_SERVER_CMD_RENEGOTIATE) &&
             !memcmp(data, DTLS_SERVER_CMD_RENEGOTIATE, strlen(DTLS_SERVER_CMD_RENEGOTIATE))) {
        printf("server: renegotiate connection\n");
        dtls_renegotiate(ctx, session);
        return len;
    }

    return dtls_write(ctx, session, data, len);
}

static dtls_handler_t cb = {
    .write				= send_to_peer,
    .read				= read_from_peer,
    .event				= NULL,
#ifdef DTLS_PSK
    .get_psk_info		= get_psk_info,
#endif /* DTLS_PSK */
#ifdef DTLS_ECC
    .get_ecdsa_key		= get_ecdsa_key,
    .verify_ecdsa_key	= verify_ecdsa_key
#endif /* DTLS_ECC */
};

int fd;
void *_server_trampoline(void *args)
{
    (void) args;
    msg_init_queue(_reader_queue, READER_QUEUE_SIZE);
    dtls_context_t *the_context = NULL;
    log_t log_level = DTLS_LOG_WARN;
    struct sockaddr_in6 listen_addr;

    memset(&listen_addr, 0, sizeof(struct sockaddr_in6));

    /* fill extra field for 4.4BSD-based systems (see RFC 3493, section 3.4) */
#if defined(SIN6_LEN) || defined(HAVE_SOCKADDR_IN6_SIN6_LEN)
    listen_addr.sin6_len = sizeof(struct sockaddr_in6);
#endif

    listen_addr.sin6_family = AF_INET6;
    listen_addr.sin6_port = htons(DEFAULT_PORT);
    listen_addr.sin6_addr = in6addr_any;

    dtls_set_log_level(log_level);

    /* init socket and set it to non-blocking */
    fd = socket(listen_addr.sin6_family, SOCK_DGRAM, 0);

    if (fd < 0) {
        dtls_alert("socket: %s\n", strerror(errno));
        return 0;
    }

    if (bind(fd, (struct sockaddr *)&listen_addr, sizeof(listen_addr)) < 0) {
        dtls_alert("bind: %s\n", strerror(errno));
        goto error;
    }

    dtls_init();

    the_context = dtls_new_context(&fd);

    dtls_set_handler(the_context, &cb);
    dtls_reader(the_context);
    return NULL;
error:
    dtls_free_context(the_context);
    return NULL;
}

int dtls_server_cmd(int argc, char **argv)
{
    (void) argc;
    (void) argv;
    thread_create(server_stack, sizeof(server_stack), THREAD_PRIORITY_MAIN - 1,
                  CREATE_STACKTEST, _server_trampoline, NULL, "DTLS server");
    return 0;
}

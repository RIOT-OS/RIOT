/*
 * SPDX-FileCopyrightText: 2016 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "event/callback.h"
#include "event/periodic.h"
#include "event/thread.h"
#include "event/timeout.h"
#include "fmt.h"
#include "net/nanocoap.h"
#include "net/nanocoap_sock.h"
#include "hashes/sha256.h"

/* internal value that can be read/written via CoAP */
static uint8_t internal_value = 0;

static const uint8_t block2_intro[] = "This is RIOT (Version: ";
static const uint8_t block2_board[] = " running on a ";
static const uint8_t block2_mcu[] = " board with a ";

static ssize_t _echo_handler(coap_pkt_t *pkt, uint8_t *buf, size_t len, coap_request_ctx_t *context)
{
    (void)context;
    char uri[CONFIG_NANOCOAP_URI_MAX];

    if (coap_get_uri_path(pkt, (uint8_t *)uri) <= 0) {
        return coap_reply_simple(pkt, COAP_CODE_INTERNAL_SERVER_ERROR, buf,
                                 len, COAP_FORMAT_TEXT, NULL, 0);
    }
    char *sub_uri = uri + strlen("/echo/");
    size_t sub_uri_len = strlen(sub_uri);
    return coap_reply_simple(pkt, COAP_CODE_CONTENT, buf, len, COAP_FORMAT_TEXT,
                             (uint8_t *)sub_uri, sub_uri_len);
}

static ssize_t _riot_board_handler(coap_pkt_t *pkt, uint8_t *buf, size_t len, coap_request_ctx_t *context)
{
    (void)context;
    return coap_reply_simple(pkt, COAP_CODE_205, buf, len,
            COAP_FORMAT_TEXT, (uint8_t*)RIOT_BOARD, strlen(RIOT_BOARD));
}

static ssize_t _riot_block2_handler(coap_pkt_t *pkt, uint8_t *buf, size_t len, coap_request_ctx_t *context)
{
    (void)context;
    coap_block_slicer_t slicer;
    coap_block2_init(pkt, &slicer);
    uint8_t *payload = buf + coap_get_response_hdr_len(pkt);

    uint8_t *bufpos = payload;

    bufpos += coap_put_option_ct(bufpos, 0, COAP_FORMAT_TEXT);
    bufpos += coap_opt_put_block2(bufpos, COAP_OPT_CONTENT_FORMAT, &slicer, 1);
    *bufpos++ = COAP_PAYLOAD_MARKER;

    /* Add actual content */
    bufpos += coap_blockwise_put_bytes(&slicer, bufpos, block2_intro, sizeof(block2_intro)-1);
    bufpos += coap_blockwise_put_bytes(&slicer, bufpos, (uint8_t*)RIOT_VERSION, strlen(RIOT_VERSION));
    bufpos += coap_blockwise_put_char(&slicer, bufpos, ')');
    bufpos += coap_blockwise_put_bytes(&slicer, bufpos, block2_board, sizeof(block2_board)-1);
    bufpos += coap_blockwise_put_bytes(&slicer, bufpos, (uint8_t*)RIOT_BOARD, strlen(RIOT_BOARD));
    bufpos += coap_blockwise_put_bytes(&slicer, bufpos, block2_mcu, sizeof(block2_mcu)-1);
    bufpos += coap_blockwise_put_bytes(&slicer, bufpos, (uint8_t*)RIOT_CPU, strlen(RIOT_CPU));
    /* To demonstrate individual chars */
    bufpos += coap_blockwise_put_char(&slicer, bufpos, ' ');
    bufpos += coap_blockwise_put_char(&slicer, bufpos, 'M');
    bufpos += coap_blockwise_put_char(&slicer, bufpos, 'C');
    bufpos += coap_blockwise_put_char(&slicer, bufpos, 'U');
    bufpos += coap_blockwise_put_char(&slicer, bufpos, '.');

    unsigned payload_len = bufpos - payload;
    return coap_block2_build_reply(pkt, COAP_CODE_205,
                                   buf, len, payload_len, &slicer);
}

static ssize_t _riot_value_handler(coap_pkt_t *pkt, uint8_t *buf, size_t len, coap_request_ctx_t *context)
{
    (void) context;

    ssize_t p = 0;
    char rsp[16];
    unsigned code = COAP_CODE_EMPTY;

    /* read coap method type in packet */
    unsigned method_flag = coap_method2flag(coap_get_code_detail(pkt));

    switch(method_flag) {
    case COAP_GET:
        /* write the response buffer with the internal value */
        p += fmt_u32_dec(rsp, internal_value);
        code = COAP_CODE_205;
        break;
    case COAP_PUT:
    case COAP_POST:
        if (pkt->payload_len < 16) {
            /* convert the payload to an integer and update the internal value */
            char payload[16] = { 0 };
            memcpy(payload, (char*)pkt->payload, pkt->payload_len);
            internal_value = strtol(payload, NULL, 10);
            code = COAP_CODE_CHANGED;
        }
        else {
            code = COAP_CODE_REQUEST_ENTITY_TOO_LARGE;
        }
    }

    return coap_reply_simple(pkt, code, buf, len,
            COAP_FORMAT_TEXT, (uint8_t*)rsp, p);
}

ssize_t _sha256_handler(coap_pkt_t* pkt, uint8_t *buf, size_t len, coap_request_ctx_t *context)
{
    (void)context;

    /* using a shared sha256 context *will* break if two requests are handled
     * at the same time.  doing it anyways, as this is meant to showcase block1
     * support, not proper synchronisation. */
    static sha256_context_t sha256;

    uint8_t digest[SHA256_DIGEST_LENGTH];

    uint32_t result = COAP_CODE_204;

    coap_block1_t block1;
    int blockwise = coap_get_block1(pkt, &block1);

    printf("_sha256_handler(): received data: offset=%" PRIuSIZE " len=%u blockwise=%i more=%i\n",
            block1.offset, pkt->payload_len, blockwise, block1.more);

    if (block1.offset == 0) {
        puts("_sha256_handler(): init");
        sha256_init(&sha256);
    }

    sha256_update(&sha256, pkt->payload, pkt->payload_len);

    if (block1.more == 1) {
        result = COAP_CODE_CONTINUE;
    }

    size_t result_len = 0;
    if (!blockwise || !block1.more) {
        puts("_sha256_handler(): finish");
        sha256_final(&sha256, digest);
        result_len = SHA256_DIGEST_LENGTH * 2;
    }

    ssize_t reply_len = coap_build_reply(pkt, result, buf, len, 0);
    if (reply_len <= 0) {
        return reply_len;
    }

    uint8_t *pkt_pos = (uint8_t*)pkt->hdr + reply_len;
    if (blockwise) {
        pkt_pos += coap_opt_put_block1_control(pkt_pos, 0, &block1);
    }
    if (result_len) {
        *pkt_pos++ = 0xFF;
        pkt_pos += fmt_bytes_hex((char *)pkt_pos, digest, sizeof(digest));
    }

    return pkt_pos - (uint8_t*)pkt->hdr;
}

NANOCOAP_RESOURCE(echo) {
    .path = "/echo/", .methods = COAP_GET | COAP_MATCH_SUBTREE, .handler = _echo_handler
};
NANOCOAP_RESOURCE(board) {
    .path = "/riot/board", .methods = COAP_GET, .handler = _riot_board_handler
};
NANOCOAP_RESOURCE(value) {
    .path = "/riot/value", .methods = COAP_GET | COAP_PUT | COAP_POST, .handler = _riot_value_handler
};
NANOCOAP_RESOURCE(ver) {
    .path = "/riot/ver", .methods = COAP_GET, .handler = _riot_block2_handler
};
NANOCOAP_RESOURCE(sha256) {
    .path = "/sha256", .methods = COAP_POST, .handler = _sha256_handler
};

/* separate response requires an event thread to execute it */
#ifdef MODULE_EVENT_THREAD
static nanocoap_server_response_ctx_t _separate_ctx;

static void _send_response(void *ctx)
{
    const char response[] = "This is a delayed response.";

    puts("_separate_handler(): send delayed response");
    nanocoap_server_send_separate(ctx, COAP_CODE_CONTENT, COAP_TYPE_NON,
                                  response, sizeof(response));
}

static ssize_t _separate_handler(coap_pkt_t *pkt, uint8_t *buf, size_t len, coap_request_ctx_t *context)
{
    static event_timeout_t event_timeout;
    static event_callback_t event_timed = EVENT_CALLBACK_INIT(_send_response, &_separate_ctx);

    if (event_timeout_is_pending(&event_timeout)) {
        if (nanocoap_server_is_remote_in_response_ctx(&_separate_ctx, context)) {
            /* duplicate of the request a separate response is already scheduled
             * for --> resending the ACK */
            return coap_build_empty_ack(pkt, (void *)buf);
        }
        puts("_separate_handler(): response already scheduled");
        return coap_build_reply(pkt, COAP_CODE_SERVICE_UNAVAILABLE, buf, len, 0);
    }

    if (nanocoap_server_prepare_separate(&_separate_ctx, pkt, context)) {
        puts("_separate_handler(): failed to prepare context for separate response");
        /* send a reset message, as we don't support large tokens here */
        return coap_build_reply(pkt, 0, buf, len, 0);
    }

    puts("_separate_handler(): send ACK, schedule response");

    event_timeout_ztimer_init(&event_timeout, ZTIMER_MSEC, EVENT_PRIO_MEDIUM,
                              &event_timed.super);
    event_timeout_set(&event_timeout, 1 * MS_PER_SEC);

    return coap_build_empty_ack(pkt, (void *)buf);
}

NANOCOAP_RESOURCE(separate) {
    .path = "/separate", .methods = COAP_GET, .handler = _separate_handler,
};
#endif /* MODULE_EVENT_THREAD */

#ifdef MODULE_NANOCOAP_SERVER_OBSERVE
static ssize_t _time_handler(coap_pkt_t *pkt, uint8_t *buf, size_t len, coap_request_ctx_t *context)
{
    uint32_t obs;
    bool registered = false;
    if (coap_opt_get_uint(pkt, COAP_OPT_OBSERVE, &obs)) {
        /* No (valid) observe option present */
        obs = UINT32_MAX;
    }

    uint32_t now = ztimer_now(ZTIMER_MSEC);

    switch (obs) {
    case 0:
        /* register */
        if (nanocoap_register_observer(context, pkt) == 0) {
            registered = true;
        }
        else {
            puts("_time_handler: can't register observer");
        }
        break;
    case 1:
        /* unregister */
        nanocoap_unregister_observer(context, pkt);
        break;
    default:
        /* No (valid) observe option present --> ignore observe and handle
         * as regular GET */
        break;
    }

    const size_t estimated_data_len =
        4       /* Max Observe Option size */
        + 1     /* payload marker */
        + 10    /* strlen("4294967295"), 4294967295 == UINT32_MAX */
        + 1;    /* '\n' */
    ssize_t hdr_len = coap_build_reply(pkt, COAP_CODE_CONTENT, buf, len, estimated_data_len);

    if (hdr_len < 0) {
        /* we undo any potential registration if we cannot reply */
        nanocoap_unregister_observer(context, pkt);
        return len;
    }

    if (hdr_len == 0) {
        /* no response required, probably because of no-response option matching
         * the response class */
        return 0;
    }

    /* coap_build_reply() is a bit goofy: It returns the size of the written
     * header + `estiamted_data_len`, so we have to subtract it again to obtain
     * the size of data written. */
    uint8_t *pos = buf + hdr_len - estimated_data_len;

    if (registered) {
        uint16_t last_opt = 0;
        pos += coap_opt_put_observe(pos, last_opt, now);
    }
    *pos++ = COAP_PAYLOAD_MARKER;
    pos += fmt_u32_dec((void *)pos, now);
    *pos++ = '\n';

    return (uintptr_t)pos - (uintptr_t)buf;
}

NANOCOAP_RESOURCE(time) {
    .path = "/time", .methods = COAP_GET, .handler = _time_handler,
};

static void _notify_observer_handler(event_t *ev)
{
    (void)ev;
    uint32_t now = ztimer_now(ZTIMER_MSEC);
    uint8_t buf[32];
    uint8_t *pos = buf;
    uint16_t last_opt = 0;
    pos += coap_opt_put_observe(pos, last_opt, now);
    *pos++ = COAP_PAYLOAD_MARKER;
    pos += fmt_u32_dec((void *)pos, now);
    *pos++ = '\n';
    iolist_t data = {
        .iol_base = buf,
        .iol_len = (uintptr_t)pos - (uintptr_t)buf,
    };

    /* `NANOCOAP_RESOURCE(time)` expends to XFA magic adding an entry named
     * `coap_resource_time`. */
    nanocoap_notify_observers(&coap_resource_time, &data);
}

void setup_observe_event(void)
{
    static event_t ev = {
        .handler = _notify_observer_handler
    };
    static event_periodic_t pev;

    event_periodic_init(&pev, ZTIMER_MSEC, EVENT_PRIO_MEDIUM, &ev);
    event_periodic_start(&pev, MS_PER_SEC);
}
#endif /* MODULE_NANOCOAP_SERVER_OBSERVE */

/* we can also include the fileserver module */
#ifdef MODULE_NANOCOAP_FILESERVER
#include "net/nanocoap/fileserver.h"
#include "vfs_default.h"

NANOCOAP_RESOURCE(fileserver) {
    .path = "/vfs",
    .methods = COAP_GET
#if IS_USED(MODULE_NANOCOAP_FILESERVER_PUT)
      | COAP_PUT
#endif
#if IS_USED(MODULE_NANOCOAP_FILESERVER_DELETE)
      | COAP_DELETE
#endif
      | COAP_MATCH_SUBTREE,
    .handler = nanocoap_fileserver_handler,
    .context = VFS_DEFAULT_DATA
};
#endif

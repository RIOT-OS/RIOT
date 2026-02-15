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
#include "hashes/sha256.h"
#include "net/nanocoap.h"
#include "net/nanocoap_sock.h"

/* internal value that can be read/written via CoAP */
static uint8_t internal_value = 0;

static const uint8_t block2_intro[] = "This is RIOT (Version: ";
static const uint8_t block2_board[] = " running on a ";
static const uint8_t block2_mcu[] = " board with a ";

static ssize_t _echo_handler(coap_pkt_t *pkt, uint8_t *buf, size_t len, coap_request_ctx_t *context)
{
    (void)context;
    char uri[CONFIG_NANOCOAP_URI_MAX];

    if (coap_get_uri_path(pkt, uri) <= 0) {
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
    coap_builder_t state;
    coap_block2_init(pkt, &slicer);
    int err;
    err = coap_builder_init_reply(&state, buf, len, pkt, COAP_CODE_CONTENT);
    if (err) {
        return err;
    }

    /* Note the implicit error handling: If adding an option / payload fails,
     * state.size is set to 0 and all subsequent operations fail as well. At the
     * end of the function, `coap_builder_msg_size()` returns -EOVERFLOW and
     * passes on the error correctly. We could also check the return value if
     * we don't want to spent more resources on processing a request, but we
     * opt to optimize for the common case instead of the edge case here. */
    coap_opt_put_ct(&state, COAP_FORMAT_TEXT);
    coap_opt_put_block2(&state, &slicer);

    /* Add actual content */
    coap_builder_add_payload_marker(&state);
    coap_blockwise_put_bytes(&state, &slicer, block2_intro, sizeof(block2_intro)-1);
    coap_blockwise_put_bytes(&state, &slicer, RIOT_VERSION, strlen(RIOT_VERSION));
    coap_blockwise_put_char(&state, &slicer, ')');
    coap_blockwise_put_bytes(&state, &slicer,  block2_board, sizeof(block2_board)-1);
    coap_blockwise_put_bytes(&state, &slicer, RIOT_BOARD, strlen(RIOT_BOARD));
    coap_blockwise_put_bytes(&state, &slicer, block2_mcu, sizeof(block2_mcu)-1);
    coap_blockwise_put_bytes(&state, &slicer, RIOT_CPU, strlen(RIOT_CPU));

    /* To demonstrate individual chars */
    coap_blockwise_put_char(&state, &slicer, ' ');
    coap_blockwise_put_char(&state, &slicer, 'M');
    coap_blockwise_put_char(&state, &slicer, 'C');
    coap_blockwise_put_char(&state, &slicer, 'U');
    coap_blockwise_put_char(&state, &slicer, '.');

    coap_block2_finish(&slicer);
    return coap_builder_msg_size(&state);
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

    coap_builder_t state;
    int err = coap_builder_init_reply(&state, buf, len, pkt, result);
    if (err) {
        return err;
    }

    if (blockwise) {
        coap_opt_put_block1_control(&state, &block1);
    }
    if (result_len) {
        /* two hex chars per byte */
        size_t pld_len = 2 * sizeof(digest);
        char *pld = coap_builder_allocate_payload(&state, pld_len);
        if (!pld) {
            return -EOVERFLOW;
        }
        fmt_bytes_hex(pld, digest, sizeof(digest));
    }

    return coap_builder_msg_size(&state);
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

    coap_builder_t state;
    int err = coap_builder_init_reply(&state, buf, len, pkt, COAP_CODE_CONTENT);
    if (err) {
        /* we undo any potential registration if we cannot reply */
        nanocoap_unregister_observer(context, pkt);
        return err;
    }

    if (registered) {
        coap_opt_put_observe(&state, now);
    }

    /* Calculate payload size first. One extra byte for line break. */
    size_t pld_len = fmt_u32_dec(NULL, now) + 1;
    char *pld = coap_builder_allocate_payload(&state, pld_len);
    if (!pld) {
        /* undo any potential registration on error */
        nanocoap_unregister_observer(context, pkt);
        return -EOVERFLOW;
    }
    /* Now calling fmt_u32_dec() again to actually write the payload */
    pld += fmt_u32_dec(pld, now);
    *pld = '\n';

    return coap_builder_msg_size(&state);
}

NANOCOAP_RESOURCE(time) {
    .path = "/time", .methods = COAP_GET, .handler = _time_handler,
};

static void _notify_observer_handler(event_t *ev)
{
    (void)ev;
    uint32_t now = ztimer_now(ZTIMER_MSEC);
    uint8_t buf[32];
    coap_builder_t state;
    if (coap_builder_init(&state, buf, sizeof(buf), 0)) {
        return;
    }
    if (coap_opt_put_observe(&state, now)) {
        return;
    }
    char pld[11]; /* == strlen("4294967295") + 1 == strlen(#UINT32_MAX) + 1*/
    size_t pld_len = fmt_u32_dec(pld, now);
    pld[pld_len++] = '\n';
    if (coap_builder_add_payload(&state, pld, pld_len)) {
        return;
    }

    iolist_t data = {
        .iol_base = buf,
        .iol_len = coap_builder_msg_size(&state),
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

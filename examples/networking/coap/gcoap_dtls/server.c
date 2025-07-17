/*
 * SPDX-FileCopyrightText: 2015-2017 Ken Bannister
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       gcoap CLI support
 *
 * @author      Ken Bannister <kb2ma@runbox.com>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "event/periodic_callback.h"
#include "event/timeout.h"
#include "event/thread.h"
#include "fmt.h"
#include "net/coap.h"
#include "net/gcoap.h"
#include "net/nanocoap.h"
#include "net/sock/util.h"
#include "net/utils.h"
#include "od.h"
#include "periph/rtc.h"
#include "time_units.h"

#include "gcoap_example.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#if IS_USED(MODULE_GCOAP_DTLS)
#include "net/credman.h"
#include "net/dsm.h"
#include "tinydtls_keys.h"

/* Example credential tag for credman. Tag together with the credential type needs to be unique. */
#define GCOAP_DTLS_CREDENTIAL_TAG 10

static const uint8_t psk_id_0[] = PSK_DEFAULT_IDENTITY;
static const uint8_t psk_key_0[] = PSK_DEFAULT_KEY;
static const credman_credential_t credential = {
    .type = CREDMAN_TYPE_PSK,
    .tag = GCOAP_DTLS_CREDENTIAL_TAG,
    .params = {
        .psk = {
            .key = { .s = psk_key_0, .len = sizeof(psk_key_0) - 1, },
            .id = { .s = psk_id_0, .len = sizeof(psk_id_0) - 1, },
        }
    },
};
#endif

static ssize_t _encode_link(const coap_resource_t *resource, char *buf,
                            size_t maxlen, coap_link_encoder_ctx_t *context);
static ssize_t _stats_handler(coap_pkt_t* pdu, uint8_t *buf, size_t len, coap_request_ctx_t *ctx);
static ssize_t _riot_board_handler(coap_pkt_t* pdu, uint8_t *buf, size_t len, coap_request_ctx_t *ctx);
#if IS_USED(MODULE_PERIPH_RTC)
static ssize_t _rtc_handler(coap_pkt_t* pdu, uint8_t *buf, size_t len, coap_request_ctx_t *ctx);
#endif
static ssize_t _separate_handler(coap_pkt_t *pkt, uint8_t *buf, size_t len, coap_request_ctx_t *context);

/* CoAP resources. Must be sorted by path (ASCII order). */
static const coap_resource_t _resources[] = {
    { "/cli/stats", COAP_GET | COAP_PUT, _stats_handler, NULL },
    { "/riot/board", COAP_GET, _riot_board_handler, NULL },
#if IS_USED(MODULE_PERIPH_RTC)
    { "/rtc", COAP_GET, _rtc_handler, NULL },
#endif
    { "/separate", COAP_GET, _separate_handler, NULL },
};

static const char *_link_params[] = {
    ";ct=0;rt=\"count\";obs",
    NULL
};

static gcoap_listener_t _listener = {
    &_resources[0],
    ARRAY_SIZE(_resources),
    GCOAP_SOCKET_TYPE_UNDEF,
    _encode_link,
    NULL,
    NULL
};

/* Adds link format params to resource list */
static ssize_t _encode_link(const coap_resource_t *resource, char *buf,
                            size_t maxlen, coap_link_encoder_ctx_t *context) {
    ssize_t res = gcoap_encode_link(resource, buf, maxlen, context);
    if (res > 0) {
        if (_link_params[context->link_pos]
                && (strlen(_link_params[context->link_pos]) < (maxlen - res))) {
            if (buf) {
                memcpy(buf+res, _link_params[context->link_pos],
                       strlen(_link_params[context->link_pos]));
            }
            return res + strlen(_link_params[context->link_pos]);
        }
    }

    return res;
}

#if IS_USED(MODULE_PERIPH_RTC)
static void _rtc_notify_observers(void *arg)
{
    (void)arg;
    struct tm tm_now;
    if (rtc_get_time(&tm_now)) {
        DEBUG_PUTS("gcoap_server: RTC error");
        return;
    }
    size_t len;
    char str_time[20] = "";
    uint8_t buf[sizeof(coap_hdr_t) + COAP_TOKEN_LENGTH_MAX + 1 + sizeof(str_time)];
    coap_pkt_t pdu;
    const coap_resource_t *rtc_resource = NULL;
    const gcoap_listener_t *listener = NULL;
    while ((rtc_resource = gcoap_get_resource_by_path_iterator(&listener, rtc_resource, "/rtc"))) {
        if (!strcmp(rtc_resource->path, "/rtc")) {
            break; /* exact match */
        }
    }
    if (rtc_resource) {
        switch (gcoap_obs_init(&pdu, buf, sizeof(buf), rtc_resource)) {
            case GCOAP_OBS_INIT_OK:
                len = coap_opt_finish(&pdu, COAP_OPT_FINISH_PAYLOAD);
                memcpy(pdu.payload, str_time, strftime(str_time, sizeof(str_time), "%Y-%m-%d %H:%M:%S", &tm_now));
                pdu.payload_len = strlen(str_time);
                len += pdu.payload_len;
                if (!gcoap_obs_send(buf, len, rtc_resource)) {
                    DEBUG_PUTS("gcoap_server: cannot send /rtc notification");
                }
                break;
            case GCOAP_OBS_INIT_UNUSED:
                DEBUG_PUTS("gcoap_server: no observer for /rtc");
                break;
            case GCOAP_OBS_INIT_ERR:
                DEBUG_PUTS("gcoap_server: error initializing /rtc notification");
                break;
        }
    }
}

static ssize_t _rtc_handler(coap_pkt_t* pdu, uint8_t *buf, size_t len, coap_request_ctx_t *ctx)
{
    (void)ctx;
    struct tm tm_now;
    rtc_get_time(&tm_now);
    gcoap_resp_init(pdu, buf, len, COAP_CODE_CONTENT);
    size_t resp_len = coap_opt_finish(pdu, COAP_OPT_FINISH_PAYLOAD);
    char str_time[20] = "";
    memcpy(pdu->payload, str_time, strftime(str_time, sizeof(str_time), "%Y-%m-%d %H:%M:%S", &tm_now));
    pdu->payload_len = strlen(str_time);
    resp_len += pdu->payload_len;
    return resp_len;
}
#endif

/*
 * Server callback for /cli/stats. Accepts either a GET or a PUT.
 *
 * GET: Returns the count of packets sent by the CLI.
 * PUT: Updates the count of packets. Rejects an obviously bad request, but
 *      allows any two byte value for example purposes. Semantically, the only
 *      valid action is to set the value to 0.
 */
static ssize_t _stats_handler(coap_pkt_t* pdu, uint8_t *buf, size_t len, coap_request_ctx_t *ctx)
{
    (void)ctx;

    /* read coap method type in packet */
    unsigned method_flag = coap_method2flag(coap_get_code_detail(pdu));

    switch (method_flag) {
        case COAP_GET:
            gcoap_resp_init(pdu, buf, len, COAP_CODE_CONTENT);
            coap_opt_add_format(pdu, COAP_FORMAT_TEXT);
            size_t resp_len = coap_opt_finish(pdu, COAP_OPT_FINISH_PAYLOAD);

            /* write the response buffer with the request count value */
            resp_len += fmt_u16_dec((char *)pdu->payload, req_count);
            return resp_len;

        case COAP_PUT:
            /* convert the payload to an integer and update the internal
               value */
            if (pdu->payload_len <= 5) {
                char payload[6] = { 0 };
                memcpy(payload, (char *)pdu->payload, pdu->payload_len);
                req_count = (uint16_t)strtoul(payload, NULL, 10);
                return gcoap_response(pdu, buf, len, COAP_CODE_CHANGED);
            }
            else {
                return gcoap_response(pdu, buf, len, COAP_CODE_BAD_REQUEST);
            }
    }

    return 0;
}

static ssize_t _riot_board_handler(coap_pkt_t *pdu, uint8_t *buf, size_t len, coap_request_ctx_t *ctx)
{
    (void)ctx;
    gcoap_resp_init(pdu, buf, len, COAP_CODE_CONTENT);
    coap_opt_add_format(pdu, COAP_FORMAT_TEXT);
    size_t resp_len = coap_opt_finish(pdu, COAP_OPT_FINISH_PAYLOAD);

    /* write the RIOT board name in the response buffer */
    if (pdu->payload_len >= strlen(RIOT_BOARD)) {
        memcpy(pdu->payload, RIOT_BOARD, strlen(RIOT_BOARD));
        return resp_len + strlen(RIOT_BOARD);
    }
    else {
        puts("gcoap_cli: msg buffer too small");
        return gcoap_response(pdu, buf, len, COAP_CODE_INTERNAL_SERVER_ERROR);
    }
}

/* separate response requires an event thread to execute it */
#ifndef CONFIG_EXAMPLE_GCOAP_SEPARATE_EVENT_QUEUE
#  ifdef MODULE_EVENT_THREAD
#    define CONFIG_EXAMPLE_GCOAP_SEPARATE_EVENT_QUEUE   EVENT_PRIO_MEDIUM
#  else
#    define CONFIG_EXAMPLE_GCOAP_SEPARATE_EVENT_QUEUE   gcoap_get_event_queue()
#  endif
#endif
#ifndef CONFIG_EXAMPLE_GCOAP_SEPARATE_RESP_PDU_BUF_SIZE
#  define CONFIG_EXAMPLE_GCOAP_SEPARATE_RESP_PDU_BUF_SIZE     (32)
#endif
#ifndef CONFIG_EXAMPLE_GCOAP_SEPARATE_REQ_PDU_BUF_SIZE
#  define CONFIG_EXAMPLE_GCOAP_SEPARATE_REQ_PDU_BUF_SIZE      (32)
#endif

static void _send_separate_response_cb(event_t *ev)
{
    gcoap_separate_response_ctx_t *ctx = container_of(ev, gcoap_separate_response_ctx_t, ev);
    coap_pkt_t req = { 0 };
    /* parse request again */
    coap_parse(&req, ctx->req_buf, ctx->req_buf_size);
    /* determine response type */
    unsigned type = coap_get_type(&req) == COAP_TYPE_CON ? COAP_TYPE_CON : COAP_TYPE_NON;
    /* determine response code */
    unsigned code = ctx->result < 0 ? COAP_CODE_INTERNAL_SERVER_ERROR : COAP_CODE_CONTENT;
    /* build simple response without further options or payload */
    coap_pkt_t resp = req;
    if (gcoap_separate_resp_init(&resp, ctx->resp_buf, ctx->resp_buf_size, type, code) < 0) {
        puts("_send_separate_response(): failed to init separate response");
        return;
    }
    /* maybe add options */
    /* add payload */
    ssize_t len;
    if ((len = coap_opt_finish(&resp, COAP_OPT_FINISH_PAYLOAD)) < 0) {
        printf("_send_separate_response(): failed to finish response %"PRIdSIZE"\n", len);
        return;
    }
    if (resp.payload_len) {
        strncpy((char *)resp.payload, "This is a delayed response.", resp.payload_len - 1);
        resp.payload_len = strlen((char *)resp.payload);
        len += resp.payload_len;
    }
    /* send response */
    if ((len = gcoap_resp_send_separate(ctx->resp_buf, len, &ctx->remote, &ctx->local, ctx->tl)) <= 0) {
        printf("_send_separate_response(): failed to send response %"PRIdSIZE"\n", len);
        return;
    }
}

/* buffer size can be adjusted to fit the expectation of a resource */
static uint8_t _sep_req[CONFIG_EXAMPLE_GCOAP_SEPARATE_RESP_PDU_BUF_SIZE];
static uint8_t _sep_resp[CONFIG_EXAMPLE_GCOAP_SEPARATE_REQ_PDU_BUF_SIZE];
static gcoap_separate_response_ctx_t _separate_ctx = {
    .ev = {
        .handler = _send_separate_response_cb,
    }
};

static ssize_t _separate_handler(coap_pkt_t *pkt, uint8_t *buf, size_t len, coap_request_ctx_t *context)
{
    static event_timeout_t _event_timeout;

    if (event_timeout_is_pending(&_event_timeout)) {
        if (!sock_udp_ep_equal(context->remote, &_separate_ctx.remote)) {
            puts("_separate_handler(): response already scheduled");
            return gcoap_response(pkt, buf, len, COAP_CODE_SERVICE_UNAVAILABLE);
        }
        else {
            /* ignore, maybe ACK got lost */
            goto empty_ack;
        }
    }
    if (gcoap_resp_prepare_separate(&_separate_ctx,
                                    _sep_req, sizeof(_sep_req),
                                    _sep_resp, sizeof(_sep_resp),
                                    pkt, context) < 0) {
        puts("_separate_handler(): request too large");
        return gcoap_response(pkt, buf, len, COAP_CODE_REQUEST_ENTITY_TOO_LARGE);
    }

    puts("_separate_handler(): schedule response");

    event_timeout_ztimer_init(&_event_timeout, ZTIMER_MSEC,
                              CONFIG_EXAMPLE_GCOAP_SEPARATE_EVENT_QUEUE,
                              &_separate_ctx.ev);
    event_timeout_set(&_event_timeout, 3 * MS_PER_SEC);

empty_ack:
    /* gcoap handles transmission of empty ACK */
    return 0;
}

void notify_observers(void)
{
    size_t len;
    uint8_t buf[CONFIG_GCOAP_PDU_BUF_SIZE];
    coap_pkt_t pdu;

    /* send Observe notification for /cli/stats */
    switch (gcoap_obs_init(&pdu, &buf[0], CONFIG_GCOAP_PDU_BUF_SIZE,
            &_resources[0])) {
    case GCOAP_OBS_INIT_OK:
        DEBUG("gcoap_cli: creating /cli/stats notification\n");
        coap_opt_add_format(&pdu, COAP_FORMAT_TEXT);
        len = coap_opt_finish(&pdu, COAP_OPT_FINISH_PAYLOAD);
        len += fmt_u16_dec((char *)pdu.payload, req_count);
        gcoap_obs_send(&buf[0], len, &_resources[0]);
        break;
    case GCOAP_OBS_INIT_UNUSED:
        DEBUG("gcoap_cli: no observer for /cli/stats\n");
        break;
    case GCOAP_OBS_INIT_ERR:
        DEBUG("gcoap_cli: error initializing /cli/stats notification\n");
        break;
    }
}

void server_init(void)
{
#if IS_USED(MODULE_GCOAP_DTLS)
    int res = credman_add(&credential);
    if (res < 0 && res != CREDMAN_EXIST) {
        /* ignore duplicate credentials */
        printf("gcoap: cannot add credential to system: %d\n", res);
        return;
    }
    sock_dtls_t *gcoap_sock_dtls = gcoap_get_sock_dtls();
    res = sock_dtls_add_credential(gcoap_sock_dtls, GCOAP_DTLS_CREDENTIAL_TAG);
    if (res < 0) {
        printf("gcoap: cannot add credential to DTLS sock: %d\n", res);
    }
#endif

    gcoap_register_listener(&_listener);
#if IS_USED(MODULE_PERIPH_RTC)
    static event_periodic_callback_t _ev_pcb_rtc;
    event_periodic_callback_init(&_ev_pcb_rtc, ZTIMER_MSEC, EVENT_PRIO_MEDIUM, _rtc_notify_observers, NULL);
    event_periodic_callback_start(&_ev_pcb_rtc, 10 * MS_PER_SEC);
#endif
}

/*
 * Copyright (c) 2015-2016 Ken Bannister. All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_gcoap
 * @{
 *
 * @file
 * @brief       GNRC's implementation of CoAP protocol
 *
 * Runs a thread (_pid) to manage request/response messaging.
 *
 * @author      Ken Bannister <kb2ma@runbox.com>
 */

#include <errno.h>
#include "net/gcoap.h"
#include "random.h"
#include "thread.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/** @brief Stack size for module thread */
#define GCOAP_STACK_SIZE (THREAD_STACKSIZE_DEFAULT + DEBUG_EXTRA_STACKSIZE)

/* Internal functions */
static void *_event_loop(void *arg);
static void _listen(sock_udp_t *sock);
static ssize_t _well_known_core_handler(coap_pkt_t* pdu, uint8_t *buf, size_t len);
static ssize_t _write_options(coap_pkt_t *pdu, uint8_t *buf, size_t len);
static size_t _handle_req(coap_pkt_t *pdu, uint8_t *buf, size_t len);
static ssize_t _finish_pdu(coap_pkt_t *pdu, uint8_t *buf, size_t len);
static void _expire_request(gcoap_request_memo_t *memo);
static void _find_req_memo(gcoap_request_memo_t **memo_ptr, coap_pkt_t *pdu,
                                                            uint8_t *buf, size_t len);

/* Internal variables */
const coap_resource_t _default_resources[] = {
    { "/.well-known/core", COAP_GET, _well_known_core_handler },
};

static gcoap_listener_t _default_listener = {
    (coap_resource_t *)&_default_resources[0],
    sizeof(_default_resources) / sizeof(_default_resources[0]),
    NULL
};

static gcoap_state_t _coap_state = {
    .listeners   = &_default_listener,
};

static kernel_pid_t _pid = KERNEL_PID_UNDEF;
static char _msg_stack[GCOAP_STACK_SIZE];
static sock_udp_t _sock;


/* Event/Message loop for gcoap _pid thread. */
static void *_event_loop(void *arg)
{
    msg_t msg_rcvd, msg_queue[GCOAP_MSG_QUEUE_SIZE];
    (void)arg;

    msg_init_queue(msg_queue, GCOAP_MSG_QUEUE_SIZE);

    sock_udp_ep_t local;
    memset(&local, 0, sizeof(sock_udp_ep_t));
    local.family = AF_INET6;
    local.netif  = SOCK_ADDR_ANY_NETIF;
    local.port   = GCOAP_PORT;

    int res = sock_udp_create(&_sock, &local, NULL, 0);
    if (res < 0) {
        DEBUG("gcoap: cannot create sock: %d\n", res);
        return 0;
    }

    while(1) {
        res = msg_try_receive(&msg_rcvd);

        if (res > 0) {
            switch (msg_rcvd.type) {
                case GCOAP_MSG_TYPE_TIMEOUT:
                    _expire_request((gcoap_request_memo_t *)msg_rcvd.content.ptr);
                    break;
                case GCOAP_MSG_TYPE_INTR:
                    /* next _listen() timeout will account for open requests */
                    break;
                default:
                    break;
            }
        }

        _listen(&_sock);
    }

    return 0;
}

/* Listen for an incoming CoAP message. */
static void _listen(sock_udp_t *sock)
{
    coap_pkt_t pdu;
    uint8_t buf[GCOAP_PDU_BUF_SIZE];
    sock_udp_ep_t remote;
    gcoap_request_memo_t *memo = NULL;
    uint8_t open_reqs;

    gcoap_op_state(&open_reqs);

    ssize_t res = sock_udp_recv(sock, buf, sizeof(buf),
                                open_reqs > 0 ? GCOAP_RECV_TIMEOUT : SOCK_NO_TIMEOUT,
                                &remote);
    if (res <= 0) {
#if ENABLE_DEBUG
        if (res < 0 && res != -ETIMEDOUT) {
            DEBUG("gcoap: udp recv failure: %d\n", res);
        }
#endif
        return;
    }

    res = coap_parse(&pdu, buf, res);
    if (res < 0) {
        DEBUG("gcoap: parse failure: %d\n", res);
        /* If a response, can't clear memo, but it will timeout later. */
        return;
    }

    /* incoming request */
    if (coap_get_code_class(&pdu) == COAP_CLASS_REQ) {
        size_t pdu_len = _handle_req(&pdu, buf, sizeof(buf));
        if (pdu_len > 0) {
            sock_udp_send(sock, buf, pdu_len, &remote);
        }
    }
    /* incoming response */
    else {
        _find_req_memo(&memo, &pdu, buf, sizeof(buf));
        if (memo) {
            xtimer_remove(&memo->response_timer);
            memo->resp_handler(memo->state, &pdu);
            memo->state = GCOAP_MEMO_UNUSED;
        }
    }
}

/*
 * Main request handler: generates response PDU in the provided buffer.
 *
 * Caller must finish the PDU and send it.
 */
static size_t _handle_req(coap_pkt_t *pdu, uint8_t *buf, size_t len)
{
    unsigned method_flag = coap_method2flag(coap_get_code_detail(pdu));

    /* Find path for CoAP msg among listener resources and execute callback. */
    gcoap_listener_t *listener = _coap_state.listeners;
    while (listener) {
        coap_resource_t *resource = listener->resources;
        for (size_t i = 0; i < listener->resources_len; i++) {
            if (i) {
                resource++;
            }
            if (! (resource->methods & method_flag)) {
                continue;
            }

            int res = strcmp((char *)&pdu->url[0], resource->path);
            if (res > 0) {
                continue;
            }
            else if (res < 0) {
                /* resources expected in alphabetical order */
                break;
            }
            else {
                ssize_t pdu_len = resource->handler(pdu, buf, len);
                if (pdu_len < 0) {
                    pdu_len = gcoap_response(pdu, buf, len,
                                             COAP_CODE_INTERNAL_SERVER_ERROR);
                }
                return pdu_len;
            }
        }
        listener = listener->next;
    }
    /* resource not found */
    return gcoap_response(pdu, buf, len, COAP_CODE_PATH_NOT_FOUND);
}

/*
 * Finishes handling a PDU -- write options and reposition payload.
 *
 * Returns the size of the PDU within the buffer, or < 0 on error.
 */
static ssize_t _finish_pdu(coap_pkt_t *pdu, uint8_t *buf, size_t len)
{
    ssize_t hdr_len = _write_options(pdu, buf, len);
    DEBUG("gcoap: header length: %u\n", hdr_len);

    if (hdr_len > 0) {
        /* move payload over unused space after options */
        if (pdu->payload_len) {
            memmove(buf + hdr_len, pdu->payload, pdu->payload_len);
        }

        return hdr_len + pdu->payload_len;
    }
    else {
        return -1;      /* generic failure code */
    }
}

/*
 * Finds the memo for an outstanding request within the _coap_state.open_reqs
 * array. Matches on token.
 *
 * src_pdu Source for the match token
 */
static void _find_req_memo(gcoap_request_memo_t **memo_ptr, coap_pkt_t *src_pdu,
                                                            uint8_t *buf, size_t len)
{
    gcoap_request_memo_t *memo;
    coap_pkt_t memo_pdu = { .token = NULL };
    (void) buf;
    (void) len;

    for (int i = 0; i < GCOAP_REQ_WAITING_MAX; i++) {
        if (_coap_state.open_reqs[i].state == GCOAP_MEMO_UNUSED)
            continue;

        /* setup memo PDU from memo header */
        memo                 = &_coap_state.open_reqs[i];
        coap_hdr_t *memo_hdr = (coap_hdr_t *) &memo->hdr_buf[0];
        memo_pdu.hdr         = memo_hdr;
        if (coap_get_token_len(&memo_pdu)) {
            memo_pdu.token = &memo_hdr->data[0];
        }
        /* match on token */
        if (coap_get_token_len(src_pdu) == coap_get_token_len(&memo_pdu)) {
            uint8_t *src_byte  = src_pdu->token;
            uint8_t *memo_byte = memo_pdu.token;
            size_t j;
            for (j = 0; j < coap_get_token_len(src_pdu); j++) {
                if (*src_byte++ != *memo_byte++) {
                    break;      /* token mismatch */
                }
            }
            if (j == coap_get_token_len(src_pdu)) {
                *memo_ptr = memo;
            }
        }
    }
}

/* Calls handler callback on receipt of a timeout message. */
static void _expire_request(gcoap_request_memo_t *memo)
{
    coap_pkt_t req;

    DEBUG("coap: received timeout message\n");
    if (memo->state == GCOAP_MEMO_WAIT) {
        memo->state = GCOAP_MEMO_TIMEOUT;
        /* Pass response to handler */
        if (memo->resp_handler) {
            req.hdr = (coap_hdr_t *)&memo->hdr_buf[0];   /* for reference */
            memo->resp_handler(memo->state, &req);
        }
        memo->state = GCOAP_MEMO_UNUSED;
    }
    else {
        /* Response already handled; timeout must have fired while response */
        /* was in queue. */
    }
}

/*
 * Handler for /.well-known/core. Lists registered handlers, except for
 * /.well-known/core itself.
 */
static ssize_t _well_known_core_handler(coap_pkt_t* pdu, uint8_t *buf, size_t len)
{
   /* write header */
    gcoap_resp_init(pdu, buf, len, COAP_CODE_CONTENT);

    /* skip the first listener, gcoap itself */
    gcoap_listener_t *listener = _coap_state.listeners->next;

    /* write payload */
    uint8_t *bufpos            = pdu->payload;

    while (listener) {
        coap_resource_t *resource = listener->resources;
        for (size_t i = 0; i < listener->resources_len; i++) {
            /* Don't overwrite buffer if paths are too long. */
            if (bufpos + strlen(resource->path) + 3 > buf + len) {
               break;
            }
            if (i) {
                *bufpos++ = ',';
                resource++;
            }
            *bufpos++ = '<';
            unsigned url_len = strlen(resource->path);
            memcpy(bufpos, resource->path, url_len);
            bufpos   += url_len;
            *bufpos++ = '>';
        }
        listener = listener->next;
    }

    /* response content */
    return gcoap_finish(pdu, bufpos - pdu->payload, COAP_FORMAT_LINK);
}

/*
 * Creates CoAP options and sets payload marker, if any.
 *
 * Returns length of header + options, or -EINVAL on illegal path.
 */
static ssize_t _write_options(coap_pkt_t *pdu, uint8_t *buf, size_t len)
{
    uint8_t last_optnum = 0;
    (void)len;

    uint8_t *bufpos = buf + coap_get_total_hdr_len(pdu);  /* position for write */

    /* Uri-Path for request */
    if (coap_get_code_class(pdu) == COAP_CLASS_REQ) {
        size_t url_len = strlen((char *)pdu->url);
        if (url_len) {
            if (pdu->url[0] != '/') {
                return -EINVAL;
            }
            bufpos += coap_put_option_url(bufpos, last_optnum, (char *)&pdu->url[0]);
            last_optnum = COAP_OPT_URI_PATH;
        }
    }

    /* Content-Format */
    if (pdu->content_type != COAP_FORMAT_NONE) {
        bufpos += coap_put_option_ct(bufpos, last_optnum, pdu->content_type);
        /* uncomment when add an option after Content-Format */
        /* last_optnum = COAP_OPT_CONTENT_FORMAT; */
    }

    /* write payload marker */
    if (pdu->payload_len) {
        *bufpos++ = GCOAP_PAYLOAD_MARKER;
    }
    return bufpos - buf;
}

/*
 * gcoap interface functions
 */

kernel_pid_t gcoap_init(void)
{
    if (_pid != KERNEL_PID_UNDEF) {
        return -EEXIST;
    }
    _pid = thread_create(_msg_stack, sizeof(_msg_stack), THREAD_PRIORITY_MAIN - 1,
                            THREAD_CREATE_STACKTEST, _event_loop, NULL, "coap");

    /* Blank list of open requests so we know if an entry is available. */
    memset(&_coap_state.open_reqs[0], 0, sizeof(_coap_state.open_reqs));
    /* randomize initial value */
    _coap_state.last_message_id = random_uint32() & 0xFFFF;

    return _pid;
}

void gcoap_register_listener(gcoap_listener_t *listener)
{
    /* Add the listener to the end of the linked list. */
    gcoap_listener_t *_last = _coap_state.listeners;
    while (_last->next)
        _last = _last->next;

    listener->next = NULL;
    _last->next = listener;
}

int gcoap_req_init(coap_pkt_t *pdu, uint8_t *buf, size_t len, unsigned code,
                                                              char *path) {
    uint8_t token[GCOAP_TOKENLEN];
    ssize_t hdrlen;
    (void)len;

    pdu->hdr = (coap_hdr_t *)buf;
    memset(pdu->url, 0, NANOCOAP_URL_MAX);

    /* generate token */
    for (size_t i = 0; i < GCOAP_TOKENLEN; i += 4) {
        uint32_t rand = random_uint32();
        memcpy(&token[i],
               &rand,
               (GCOAP_TOKENLEN - i >= 4) ? 4 : GCOAP_TOKENLEN - i);
    }
    hdrlen = coap_build_hdr(pdu->hdr, COAP_TYPE_NON, &token[0], GCOAP_TOKENLEN,
                                                     code,
                                                   ++_coap_state.last_message_id);

    if (hdrlen > 0) {
        /* Reserve some space between the header and payload to write options later */
        pdu->payload      = buf + coap_get_total_hdr_len(pdu) + strlen(path)
                                                              + GCOAP_REQ_OPTIONS_BUF;
        /* Payload length really zero at this point, but we set this to the available
         * length in the buffer. Allows us to reconstruct buffer length later. */
        pdu->payload_len  = len - (pdu->payload - buf);
        pdu->content_type = COAP_FORMAT_NONE;

        memcpy(&pdu->url[0], path, strlen(path));
        return 0;
    }
    else {
        /* reason for negative hdrlen is not defined, so we also are vague */
        return -1;
    }
}

ssize_t gcoap_finish(coap_pkt_t *pdu, size_t payload_len, unsigned format)
{
    /* reconstruct full PDU buffer length */
    size_t len = pdu->payload_len + (pdu->payload - (uint8_t *)pdu->hdr);

    pdu->content_type = format;
    pdu->payload_len  = payload_len;
    return _finish_pdu(pdu, (uint8_t *)pdu->hdr, len);
}

size_t gcoap_req_send(uint8_t *buf, size_t len, ipv6_addr_t *addr, uint16_t port,
                                                 gcoap_resp_handler_t resp_handler)
{
    sock_udp_ep_t remote;

    remote.family = AF_INET6;
    remote.netif  = SOCK_ADDR_ANY_NETIF;
    remote.port   = port;

    memcpy(&remote.addr.ipv6[0], &addr->u8[0], sizeof(addr->u8));

    return gcoap_req_send2(buf, len, &remote, resp_handler);
}

size_t gcoap_req_send2(uint8_t *buf, size_t len, sock_udp_ep_t *remote,
                                                 gcoap_resp_handler_t resp_handler)
{
    gcoap_request_memo_t *memo = NULL;
    assert(remote != NULL);
    assert(resp_handler != NULL);

    /* Find empty slot in list of open requests. */
    for (int i = 0; i < GCOAP_REQ_WAITING_MAX; i++) {
        if (_coap_state.open_reqs[i].state == GCOAP_MEMO_UNUSED) {
            memo = &_coap_state.open_reqs[i];
            memo->state = GCOAP_MEMO_WAIT;
            break;
        }
    }
    if (memo) {
        memcpy(&memo->hdr_buf[0], buf, GCOAP_HEADER_MAXLEN);
        memo->resp_handler = resp_handler;

        size_t res = sock_udp_send(&_sock, buf, len, remote);

        if (res && (GCOAP_NON_TIMEOUT > 0)) {
            /* interrupt sock listening (to set a listen timeout) */
            msg_t mbox_msg;
            mbox_msg.type          = GCOAP_MSG_TYPE_INTR;
            mbox_msg.content.value = 0;
            if (mbox_try_put(&_sock.reg.mbox, &mbox_msg)) {
                /* start response wait timer */
                memo->timeout_msg.type        = GCOAP_MSG_TYPE_TIMEOUT;
                memo->timeout_msg.content.ptr = (char *)memo;
                xtimer_set_msg(&memo->response_timer, GCOAP_NON_TIMEOUT,
                                                      &memo->timeout_msg, _pid);
            }
            else {
                memo->state = GCOAP_MEMO_UNUSED;
                DEBUG("gcoap: can't wake up mbox; no timeout for msg\n");
            }
        }
        else if (!res) {
            memo->state = GCOAP_MEMO_UNUSED;
            DEBUG("gcoap: sock send failed: %d\n", res);
        }
        return res;
    } else {
        DEBUG("gcoap: dropping request; no space for response tracking\n");
        return 0;
    }
}

int gcoap_resp_init(coap_pkt_t *pdu, uint8_t *buf, size_t len, unsigned code)
{
    /* Assume NON type request, so response type is the same. */
    coap_hdr_set_code(pdu->hdr, code);
    /* Create message ID since NON? */

    /* Reserve some space between the header and payload to write options later */
    pdu->payload      = buf + coap_get_total_hdr_len(pdu) + GCOAP_RESP_OPTIONS_BUF;
    /* Payload length really zero at this point, but we set this to the available
     * length in the buffer. Allows us to reconstruct buffer length later. */
    pdu->payload_len  = len - (pdu->payload - buf);
    pdu->content_type = COAP_FORMAT_NONE;

    return 0;
}

void gcoap_op_state(uint8_t *open_reqs)
{
    uint8_t count = 0;
    for (int i = 0; i < GCOAP_REQ_WAITING_MAX; i++) {
        if (_coap_state.open_reqs[i].state != GCOAP_MEMO_UNUSED) {
            count++;
        }
    }
    *open_reqs = count;
}

/** @} */

/*
 * Copyright (c) 2015-2016 Ken Bannister. All rights reserved.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_gnrc_coap
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
#include "net/gnrc/coap.h"
#include "random.h"
#include "thread.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/** @brief Stack size for module thread */
#if ENABLE_DEBUG
#define GCOAP_STACK_SIZE (THREAD_STACKSIZE_DEFAULT + THREAD_EXTRA_STACKSIZE_PRINTF)
#else
#define GCOAP_STACK_SIZE (THREAD_STACKSIZE_DEFAULT)
#endif

/* Internal functions */
static void *_event_loop(void *arg);
static int _register_port(gnrc_netreg_entry_t *netreg_port, uint16_t port);
static void _receive(gnrc_pktsnip_t *pkt, ipv6_addr_t *src, uint16_t port);
static size_t _send(gnrc_pktsnip_t *coap_snip, ipv6_addr_t *addr, uint16_t port);
static ssize_t _well_known_core_handler(coap_pkt_t* pdu, uint8_t *buf, size_t len);
static ssize_t _write_options(coap_pkt_t *pdu, uint8_t *buf, size_t len);
static size_t _handle_req(coap_pkt_t *pdu, uint8_t *buf, size_t len);
static ssize_t _finish_pdu(coap_pkt_t *pdu, uint8_t *buf, size_t len);
static size_t _send_buf( uint8_t *buf, size_t len, ipv6_addr_t *src, uint16_t port);
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
    .netreg_port = {NULL, 0, KERNEL_PID_UNDEF},
    .listeners   = &_default_listener,
};

static kernel_pid_t _pid = KERNEL_PID_UNDEF;
static char _msg_stack[GCOAP_STACK_SIZE];


/* Event/Message loop for gcoap _pid thread. */
static void *_event_loop(void *arg)
{
    msg_t msg_rcvd, msg_queue[GCOAP_MSG_QUEUE_SIZE];
    gnrc_pktsnip_t *pkt, *udp_snip, *ipv6_snip;
    ipv6_addr_t *src_addr;
    uint16_t port;

    (void)arg;
    msg_init_queue(msg_queue, GCOAP_MSG_QUEUE_SIZE);

    while (1) {
        msg_receive(&msg_rcvd);

        switch (msg_rcvd.type) {
            case GNRC_NETAPI_MSG_TYPE_RCV:
                /* find client from UDP destination port */
                DEBUG("coap: GNRC_NETAPI_MSG_TYPE_RCV\n");
                pkt = (gnrc_pktsnip_t *)msg_rcvd.content.ptr;
                if (pkt->type != GNRC_NETTYPE_UNDEF) {
                    gnrc_pktbuf_release(pkt);
                    break;
                }
                udp_snip = pkt->next;
                if (udp_snip->type != GNRC_NETTYPE_UDP) {
                    gnrc_pktbuf_release(pkt);
                    break;
                }

                /* read source port and address */
                port = byteorder_ntohs(((udp_hdr_t *)udp_snip->data)->src_port);

                LL_SEARCH_SCALAR(udp_snip, ipv6_snip, type, GNRC_NETTYPE_IPV6);
                assert(ipv6_snip != NULL);
                src_addr = &((ipv6_hdr_t *)ipv6_snip->data)->src;

                _receive(pkt, src_addr, port);
                break;

            case GCOAP_NETAPI_MSG_TYPE_TIMEOUT:
                _expire_request((gcoap_request_memo_t *)msg_rcvd.content.ptr);
                break;

            default:
                break;
        }
    }
    return 0;
}

/* Handles incoming network IPC message. */
static void _receive(gnrc_pktsnip_t *pkt, ipv6_addr_t *src, uint16_t port)
{
    coap_pkt_t pdu;
    uint8_t buf[GCOAP_PDU_BUF_SIZE];
    size_t pdu_len = 0;
    gcoap_request_memo_t *memo = NULL;

    /* If too big, handle below based on request vs. response */
    size_t pkt_size = (pkt->size > sizeof(buf))
                            ? sizeof(buf) : pkt->size;

    /* Copy request into temporary buffer, and parse it as CoAP. */
    memcpy(buf, pkt->data, pkt_size);

    int result = coap_parse(&pdu, buf, pkt_size);
    if (result < 0) {
        DEBUG("gcoap: parse failure: %d\n", result);
        /* If a response, can't clear memo, but it will timeout later. */
        goto exit;
    }

    /* incoming request */
    if (coap_get_code_class(&pdu) == COAP_CLASS_REQ) {
        if (pkt->size > sizeof(buf)) {
            DEBUG("gcoap: request too big: %u\n", pkt->size);
            pdu_len = gcoap_response(&pdu, buf, sizeof(buf),
                                           COAP_CODE_REQUEST_ENTITY_TOO_LARGE);
        } else {
            pdu_len = _handle_req(&pdu, buf, sizeof(buf));
        }
        if (pdu_len > 0) {
            _send_buf(buf, pdu_len, src, port);
        }
    }
    /* incoming response */
    else {
        _find_req_memo(&memo, &pdu, buf, sizeof(buf));
        if (memo) {
            xtimer_remove(&memo->response_timer);
            if (pkt->size > sizeof(buf)) {
                memo->state = GCOAP_MEMO_ERR;
                DEBUG("gcoap: response too big: %u\n", pkt->size);
            }
            memo->resp_handler(memo->state, &pdu);
            memo->state = GCOAP_MEMO_UNUSED;
        }
    }

exit:
    gnrc_pktbuf_release(pkt);
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

/* Registers receive/send port with GNRC registry. */
static int _register_port(gnrc_netreg_entry_t *netreg_port, uint16_t port)
{
    if (!gnrc_netreg_lookup(GNRC_NETTYPE_UDP, port)) {
        netreg_port->demux_ctx  = port;
        netreg_port->target.pid = _pid;
        gnrc_netreg_register(GNRC_NETTYPE_UDP, netreg_port);
        DEBUG("coap: registered UDP port %" PRIu32 "\n",
               netreg_port->demux_ctx);
        return 0;
    }
    else {
        return -EINVAL;
    }
}

/*
 * Sends a CoAP message to the provided host/port.
 *
 * @return Length of the packet
 * @return 0 if cannot send
 */
static size_t _send(gnrc_pktsnip_t *coap_snip, ipv6_addr_t *addr, uint16_t port)
{
    gnrc_pktsnip_t *udp, *ip;
    size_t pktlen;

    /* allocate UDP header */
    udp = gnrc_udp_hdr_build(coap_snip, (uint16_t)_coap_state.netreg_port.demux_ctx,
                                                                               port);
    if (udp == NULL) {
        DEBUG("gcoap: unable to allocate UDP header\n");
        gnrc_pktbuf_release(coap_snip);
        return 0;
    }
    /* allocate IPv6 header */
    ip = gnrc_ipv6_hdr_build(udp, NULL, addr);
    if (ip == NULL) {
        DEBUG("gcoap: unable to allocate IPv6 header\n");
        gnrc_pktbuf_release(udp);
        return 0;
    }
    pktlen = gnrc_pkt_len(ip);          /* count length now; snips deallocated after send */

    /* send message */
    if (!gnrc_netapi_dispatch_send(GNRC_NETTYPE_UDP, GNRC_NETREG_DEMUX_CTX_ALL, ip)) {
        DEBUG("gcoap: unable to locate UDP thread\n");
        gnrc_pktbuf_release(ip);
        return 0;
    }
    return pktlen;
}

/*
 * Copies the request/response buffer to a pktsnip and sends it.
 *
 * @return Length of the packet
 * @return 0 if cannot send
 */
static size_t _send_buf(uint8_t *buf, size_t len, ipv6_addr_t *src, uint16_t port)
{
    gnrc_pktsnip_t *snip;

    snip = gnrc_pktbuf_add(NULL, NULL, len, GNRC_NETTYPE_UNDEF);
    if (!snip) {
        return 0;
    }
    memcpy(snip->data, buf, len);

    return _send(snip, src, port);
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

    /* must establish pid first */
    if (_register_port(&_coap_state.netreg_port, GCOAP_PORT) < 0) {
        return -EINVAL;
    }
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
    gcoap_request_memo_t *memo = NULL;
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

        size_t res = _send_buf(buf, len, addr, port);
        if (res && GCOAP_NON_TIMEOUT) {
            /* start response wait timer */
            memo->timeout_msg.type        = GCOAP_NETAPI_MSG_TYPE_TIMEOUT;
            memo->timeout_msg.content.ptr = (char *)memo;
            xtimer_set_msg(&memo->response_timer, GCOAP_NON_TIMEOUT,
                                                  &memo->timeout_msg, _pid);
        }
        else if (!res) {
            memo->state = GCOAP_MEMO_UNUSED;
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

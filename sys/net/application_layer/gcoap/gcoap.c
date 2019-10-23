/*
 * Copyright (c) 2015-2017 Ken Bannister. All rights reserved.
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
#include <stdint.h>
#include <stdatomic.h>
#include <string.h>

#include "assert.h"
#include "net/gcoap.h"
#include "net/sock/util.h"
#include "mutex.h"
#include "random.h"
#include "thread.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/* Return values used by the _find_resource function. */
#define GCOAP_RESOURCE_FOUND 0
#define GCOAP_RESOURCE_WRONG_METHOD -1
#define GCOAP_RESOURCE_NO_PATH -2

/* Internal functions */
static void *_event_loop(void *arg);
static void _listen(sock_udp_t *sock);
static ssize_t _well_known_core_handler(coap_pkt_t* pdu, uint8_t *buf, size_t len, void *ctx);
static size_t _handle_req(coap_pkt_t *pdu, uint8_t *buf, size_t len,
                                                         sock_udp_ep_t *remote);
static void _expire_request(gcoap_request_memo_t *memo);
static void _find_req_memo(gcoap_request_memo_t **memo_ptr, coap_pkt_t *pdu,
                           const sock_udp_ep_t *remote);
static int _find_resource(coap_pkt_t *pdu, const coap_resource_t **resource_ptr,
                                            gcoap_listener_t **listener_ptr);
static int _find_observer(sock_udp_ep_t **observer, sock_udp_ep_t *remote);
static int _find_obs_memo(gcoap_observe_memo_t **memo, sock_udp_ep_t *remote,
                                                       coap_pkt_t *pdu);
static void _find_obs_memo_resource(gcoap_observe_memo_t **memo,
                                   const coap_resource_t *resource);

/* Internal variables */
const coap_resource_t _default_resources[] = {
    { "/.well-known/core", COAP_GET, _well_known_core_handler, NULL },
};

static gcoap_listener_t _default_listener = {
    &_default_resources[0],
    ARRAY_SIZE(_default_resources),
    NULL,
    NULL
};

/* Container for the state of gcoap itself */
typedef struct {
    mutex_t lock;                       /* Shares state attributes safely */
    gcoap_listener_t *listeners;        /* List of registered listeners */
    gcoap_request_memo_t open_reqs[GCOAP_REQ_WAITING_MAX];
                                        /* Storage for open requests; if first
                                           byte of an entry is zero, the entry
                                           is available */
    atomic_uint next_message_id;        /* Next message ID to use */
    sock_udp_ep_t observers[GCOAP_OBS_CLIENTS_MAX];
                                        /* Observe clients; allows reuse for
                                           observe memos */
    gcoap_observe_memo_t observe_memos[GCOAP_OBS_REGISTRATIONS_MAX];
                                        /* Observed resource registrations */
    uint8_t resend_bufs[GCOAP_RESEND_BUFS_MAX][GCOAP_PDU_BUF_SIZE];
                                        /* Buffers for PDU for request resends;
                                           if first byte of an entry is zero,
                                           the entry is available */
} gcoap_state_t;

static gcoap_state_t _coap_state = {
    .listeners   = &_default_listener,
};

static kernel_pid_t _pid = KERNEL_PID_UNDEF;
static char _msg_stack[GCOAP_STACK_SIZE];
static msg_t _msg_queue[GCOAP_MSG_QUEUE_SIZE];
static sock_udp_t _sock;


/* Event/Message loop for gcoap _pid thread. */
static void *_event_loop(void *arg)
{
    msg_t msg_rcvd;
    (void)arg;

    msg_init_queue(_msg_queue, GCOAP_MSG_QUEUE_SIZE);

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
            case GCOAP_MSG_TYPE_TIMEOUT: {
                gcoap_request_memo_t *memo = (gcoap_request_memo_t *)msg_rcvd.content.ptr;

                /* no retries remaining */
                if ((memo->send_limit == GCOAP_SEND_LIMIT_NON)
                        || (memo->send_limit == 0)) {
                    _expire_request(memo);
                }
                /* reduce retries remaining, double timeout and resend */
                else {
                    memo->send_limit--;
#ifdef GCOAP_NO_RETRANS_BACKOFF
                    unsigned i        = 0;
#else
                    unsigned i        = COAP_MAX_RETRANSMIT - memo->send_limit;
#endif
                    uint32_t timeout  = ((uint32_t)COAP_ACK_TIMEOUT << i) * US_PER_SEC;
#if COAP_ACK_VARIANCE > 0
                    uint32_t variance = ((uint32_t)COAP_ACK_VARIANCE << i) * US_PER_SEC;
                    timeout = random_uint32_range(timeout, timeout + variance);
#endif

                    ssize_t bytes = sock_udp_send(&_sock, memo->msg.data.pdu_buf,
                                                  memo->msg.data.pdu_len,
                                                  &memo->remote_ep);
                    if (bytes > 0) {
                        xtimer_set_msg(&memo->response_timer, timeout,
                                       &memo->timeout_msg, _pid);
                    }
                    else {
                        DEBUG("gcoap: sock resend failed: %d\n", (int)bytes);
                        _expire_request(memo);
                    }
                }
                break;
            }
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
    uint8_t open_reqs = gcoap_op_state();

    /* We expect a -EINTR response here when unlimited waiting (SOCK_NO_TIMEOUT)
     * is interrupted when sending a message in gcoap_req_send(). While a
     * request is outstanding, sock_udp_recv() is called here with limited
     * waiting so the request's timeout can be handled in a timely manner in
     * _event_loop(). */
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
        DEBUG("gcoap: parse failure: %d\n", (int)res);
        /* If a response, can't clear memo, but it will timeout later. */
        return;
    }

    if (pdu.hdr->code == COAP_CODE_EMPTY) {
        DEBUG("gcoap: empty messages not handled yet\n");
        return;
    }

    /* validate class and type for incoming */
    switch (coap_get_code_class(&pdu)) {
    /* incoming request */
    case COAP_CLASS_REQ:
        if (coap_get_type(&pdu) == COAP_TYPE_NON
                || coap_get_type(&pdu) == COAP_TYPE_CON) {
            size_t pdu_len = _handle_req(&pdu, buf, sizeof(buf), &remote);
            if (pdu_len > 0) {
                ssize_t bytes = sock_udp_send(sock, buf, pdu_len, &remote);
                if (bytes <= 0) {
                    DEBUG("gcoap: send response failed: %d\n", (int)bytes);
                }
            }
        }
        else {
            DEBUG("gcoap: illegal request type: %u\n", coap_get_type(&pdu));
        }
        break;

    /* incoming response */
    case COAP_CLASS_SUCCESS:
    case COAP_CLASS_CLIENT_FAILURE:
    case COAP_CLASS_SERVER_FAILURE:
        _find_req_memo(&memo, &pdu, &remote);
        if (memo) {
            switch (coap_get_type(&pdu)) {
            case COAP_TYPE_NON:
            case COAP_TYPE_ACK:
                xtimer_remove(&memo->response_timer);
                memo->state = GCOAP_MEMO_RESP;
                if (memo->resp_handler) {
                    memo->resp_handler(memo->state, &pdu, &remote);
                }

                if (memo->send_limit >= 0) {        /* if confirmable */
                    *memo->msg.data.pdu_buf = 0;    /* clear resend PDU buffer */
                }
                memo->state = GCOAP_MEMO_UNUSED;
                break;
            case COAP_TYPE_CON:
                DEBUG("gcoap: separate CON response not handled yet\n");
                break;
            default:
                DEBUG("gcoap: illegal response type: %u\n", coap_get_type(&pdu));
                break;
            }
        }
        else {
            DEBUG("gcoap: msg not found for ID: %u\n", coap_get_id(&pdu));
        }
        break;
    default:
        DEBUG("gcoap: illegal code class: %u\n", coap_get_code_class(&pdu));
    }
}

/*
 * Main request handler: generates response PDU in the provided buffer.
 *
 * Caller must finish the PDU and send it.
 *
 * return length of response pdu, or < 0 if can't handle
 */
static size_t _handle_req(coap_pkt_t *pdu, uint8_t *buf, size_t len,
                                                         sock_udp_ep_t *remote)
{
    const coap_resource_t *resource     = NULL;
    gcoap_listener_t *listener          = NULL;
    sock_udp_ep_t *observer             = NULL;
    gcoap_observe_memo_t *memo          = NULL;
    gcoap_observe_memo_t *resource_memo = NULL;

    switch (_find_resource(pdu, &resource, &listener)) {
        case GCOAP_RESOURCE_WRONG_METHOD:
            return gcoap_response(pdu, buf, len, COAP_CODE_METHOD_NOT_ALLOWED);
        case GCOAP_RESOURCE_NO_PATH:
            return gcoap_response(pdu, buf, len, COAP_CODE_PATH_NOT_FOUND);
        case GCOAP_RESOURCE_FOUND:
            /* find observe registration for resource */
            _find_obs_memo_resource(&resource_memo, resource);
            break;
    }

    if (coap_get_observe(pdu) == COAP_OBS_REGISTER) {
        /* lookup remote+token */
        int empty_slot = _find_obs_memo(&memo, remote, pdu);
        /* validate re-registration request */
        if (resource_memo != NULL) {
            if (memo != NULL) {
                if (memo != resource_memo) {
                    /* reject token already used for a different resource */
                    memo = NULL;
                    coap_clear_observe(pdu);
                    DEBUG("gcoap: can't change resource for token\n");
                }
                /* otherwise OK to re-register resource with the same token */
            }
            else if (sock_udp_ep_equal(remote, resource_memo->observer)) {
                /* accept new token for resource */
                memo = resource_memo;
            }
        }
        /* initialize new registration request */
        if ((memo == NULL) && coap_has_observe(pdu)) {
            /* verify resource not already registered (for another endpoint) */
            if ((empty_slot >= 0) && (resource_memo == NULL)) {
                int obs_slot = _find_observer(&observer, remote);
                /* cache new observer */
                if (observer == NULL) {
                    if (obs_slot >= 0) {
                        observer = &_coap_state.observers[obs_slot];
                        memcpy(observer, remote, sizeof(sock_udp_ep_t));
                    } else {
                        DEBUG("gcoap: can't register observer\n");
                    }
                }
                if (observer != NULL) {
                    memo = &_coap_state.observe_memos[empty_slot];
                    memo->observer = observer;
                }
            }
            if (memo == NULL) {
                coap_clear_observe(pdu);
                DEBUG("gcoap: can't register observe memo\n");
            }
        }
        /* finish registration */
        if (memo != NULL) {
            /* resource may be assigned here if it is not already registered */
            memo->resource = resource;
            memo->token_len = coap_get_token_len(pdu);
            if (memo->token_len) {
                memcpy(&memo->token[0], pdu->token, memo->token_len);
            }
            DEBUG("gcoap: Registered observer for: %s\n", memo->resource->path);
        }

    } else if (coap_get_observe(pdu) == COAP_OBS_DEREGISTER) {
        _find_obs_memo(&memo, remote, pdu);
        /* clear memo, and clear observer if no other memos */
        if (memo != NULL) {
            DEBUG("gcoap: Deregistering observer for: %s\n", memo->resource->path);
            memo->observer = NULL;
            memo           = NULL;
            _find_obs_memo(&memo, remote, NULL);
            if (memo == NULL) {
                _find_observer(&observer, remote);
                if (observer != NULL) {
                    observer->family = AF_UNSPEC;
                }
            }
        }
        coap_clear_observe(pdu);

    } else if (coap_has_observe(pdu)) {
        /* bogus request; don't respond */
        DEBUG("gcoap: Observe value unexpected: %" PRIu32 "\n", coap_get_observe(pdu));
        return -1;
    }

    ssize_t pdu_len = resource->handler(pdu, buf, len, resource->context);
    if (pdu_len < 0) {
        pdu_len = gcoap_response(pdu, buf, len,
                                 COAP_CODE_INTERNAL_SERVER_ERROR);
    }
    return pdu_len;
}

/*
 * Searches listener registrations for the resource matching the path in a PDU.
 *
 * param[out] resource_ptr -- found resource
 * param[out] listener_ptr -- listener for found resource
 * return `GCOAP_RESOURCE_FOUND` if the resource was found,
 *        `GCOAP_RESOURCE_WRONG_METHOD` if a resource was found but the method
 *        code didn't match and `GCOAP_RESOURCE_NO_PATH` if no matching
 *        resource was found.
 */
static int _find_resource(coap_pkt_t *pdu, const coap_resource_t **resource_ptr,
                                            gcoap_listener_t **listener_ptr)
{
    int ret = GCOAP_RESOURCE_NO_PATH;
    coap_method_flags_t method_flag = coap_method2flag(coap_get_code_detail(pdu));

    /* Find path for CoAP msg among listener resources and execute callback. */
    gcoap_listener_t *listener = _coap_state.listeners;

    uint8_t uri[NANOCOAP_URI_MAX];
    if (coap_get_uri_path(pdu, uri) <= 0) {
        return GCOAP_RESOURCE_NO_PATH;
    }

    while (listener) {
        const coap_resource_t *resource = listener->resources;
        for (size_t i = 0; i < listener->resources_len; i++) {
            if (i) {
                resource++;
            }

            int res = coap_match_path(resource, uri);
            if (res > 0) {
                continue;
            }
            else if (res < 0) {
                /* resources expected in alphabetical order */
                break;
            }
            else {
                if (! (resource->methods & method_flag)) {
                    ret = GCOAP_RESOURCE_WRONG_METHOD;
                    continue;
                }

                *resource_ptr = resource;
                *listener_ptr = listener;
                return GCOAP_RESOURCE_FOUND;
            }
        }
        listener = listener->next;
    }

    return ret;
}

/*
 * Finds the memo for an outstanding request within the _coap_state.open_reqs
 * array. Matches on remote endpoint and token.
 *
 * memo_ptr[out] -- Registered request memo, or NULL if not found
 * src_pdu[in] -- PDU for token to match
 * remote[in] -- Remote endpoint to match
 */
static void _find_req_memo(gcoap_request_memo_t **memo_ptr, coap_pkt_t *src_pdu,
                           const sock_udp_ep_t *remote)
{
    *memo_ptr = NULL;
    /* no need to initialize struct; we only care about buffer contents below */
    coap_pkt_t memo_pdu_data;
    coap_pkt_t *memo_pdu = &memo_pdu_data;
    unsigned cmplen      = coap_get_token_len(src_pdu);

    for (int i = 0; i < GCOAP_REQ_WAITING_MAX; i++) {
        if (_coap_state.open_reqs[i].state == GCOAP_MEMO_UNUSED)
            continue;

        gcoap_request_memo_t *memo = &_coap_state.open_reqs[i];
        if (memo->send_limit == GCOAP_SEND_LIMIT_NON) {
            memo_pdu->hdr = (coap_hdr_t *) &memo->msg.hdr_buf[0];
        }
        else {
            memo_pdu->hdr = (coap_hdr_t *) memo->msg.data.pdu_buf;
        }

        if (coap_get_token_len(memo_pdu) == cmplen) {
            memo_pdu->token = coap_hdr_data_ptr(memo_pdu->hdr);
            if ((memcmp(src_pdu->token, memo_pdu->token, cmplen) == 0)
                    && sock_udp_ep_equal(&memo->remote_ep, remote)) {
                *memo_ptr = memo;
                break;
            }
        }
    }
}

/* Calls handler callback on receipt of a timeout message. */
static void _expire_request(gcoap_request_memo_t *memo)
{
    DEBUG("coap: received timeout message\n");
    if (memo->state == GCOAP_MEMO_WAIT) {
        memo->state = GCOAP_MEMO_TIMEOUT;
        /* Pass response to handler */
        if (memo->resp_handler) {
            coap_pkt_t req;
            if (memo->send_limit == GCOAP_SEND_LIMIT_NON) {
                req.hdr = (coap_hdr_t *)&memo->msg.hdr_buf[0];   /* for reference */
            }
            else {
                req.hdr = (coap_hdr_t *)memo->msg.data.pdu_buf;
            }
            memo->resp_handler(memo->state, &req, NULL);
        }
        if (memo->send_limit != GCOAP_SEND_LIMIT_NON) {
            *memo->msg.data.pdu_buf = 0;    /* clear resend buffer */
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
static ssize_t _well_known_core_handler(coap_pkt_t* pdu, uint8_t *buf, size_t len,
                                        void *ctx)
{
    (void)ctx;

    gcoap_resp_init(pdu, buf, len, COAP_CODE_CONTENT);
    coap_opt_add_format(pdu, COAP_FORMAT_LINK);
    ssize_t plen = coap_opt_finish(pdu, COAP_OPT_FINISH_PAYLOAD);

    plen += gcoap_get_resource_list(pdu->payload, (size_t)pdu->payload_len,
                                    COAP_FORMAT_LINK);
    return plen;
}

/*
 * Find registered observer for a remote address and port.
 *
 * observer[out] -- Registered observer, or NULL if not found
 * remote[in] -- Endpoint to match
 *
 * return Index of empty slot, suitable for registering new observer; or -1
 *        if no empty slots. Undefined if observer found.
 */
static int _find_observer(sock_udp_ep_t **observer, sock_udp_ep_t *remote)
{
    int empty_slot = -1;
    *observer      = NULL;
    for (unsigned i = 0; i < GCOAP_OBS_CLIENTS_MAX; i++) {

        if (_coap_state.observers[i].family == AF_UNSPEC) {
            empty_slot = i;
        }
        else if (sock_udp_ep_equal(&_coap_state.observers[i], remote)) {
            *observer = &_coap_state.observers[i];
            break;
        }
    }
    return empty_slot;
}

/*
 * Find registered observe memo for a remote address and token.
 *
 * memo[out] -- Registered observe memo, or NULL if not found
 * remote[in] -- Endpoint for address to match
 * pdu[in] -- PDU for token to match, or NULL to match only on remote address
 *
 * return Index of empty slot, suitable for registering new memo; or -1 if no
 *        empty slots. Undefined if memo found.
 */
static int _find_obs_memo(gcoap_observe_memo_t **memo, sock_udp_ep_t *remote,
                                                       coap_pkt_t *pdu)
{
    int empty_slot = -1;
    *memo          = NULL;

    sock_udp_ep_t *remote_observer = NULL;
    _find_observer(&remote_observer, remote);

    for (unsigned i = 0; i < GCOAP_OBS_REGISTRATIONS_MAX; i++) {
        if (_coap_state.observe_memos[i].observer == NULL) {
            empty_slot = i;
            continue;
        }

        if (_coap_state.observe_memos[i].observer == remote_observer) {
            if (pdu == NULL) {
                *memo = &_coap_state.observe_memos[i];
                break;
            }

            if (_coap_state.observe_memos[i].token_len == coap_get_token_len(pdu)) {
                unsigned cmplen = _coap_state.observe_memos[i].token_len;
                if (cmplen &&
                        memcmp(&_coap_state.observe_memos[i].token[0], &pdu->token[0],
                                                                       cmplen) == 0) {
                    *memo = &_coap_state.observe_memos[i];
                    break;
                }
            }
        }
    }
    return empty_slot;
}

/*
 * Find registered observe memo for a resource.
 *
 * memo[out] -- Registered observe memo, or NULL if not found
 * resource[in] -- Resource to match
 */
static void _find_obs_memo_resource(gcoap_observe_memo_t **memo,
                                   const coap_resource_t *resource)
{
    *memo = NULL;
    for (int i = 0; i < GCOAP_OBS_REGISTRATIONS_MAX; i++) {
        if (_coap_state.observe_memos[i].observer != NULL
                && _coap_state.observe_memos[i].resource == resource) {
            *memo = &_coap_state.observe_memos[i];
            break;
        }
    }
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

    mutex_init(&_coap_state.lock);
    /* Blank lists so we know if an entry is available. */
    memset(&_coap_state.open_reqs[0], 0, sizeof(_coap_state.open_reqs));
    memset(&_coap_state.observers[0], 0, sizeof(_coap_state.observers));
    memset(&_coap_state.observe_memos[0], 0, sizeof(_coap_state.observe_memos));
    memset(&_coap_state.resend_bufs[0], 0, sizeof(_coap_state.resend_bufs));
    /* randomize initial value */
    atomic_init(&_coap_state.next_message_id, (unsigned)random_uint32());

    return _pid;
}

void gcoap_register_listener(gcoap_listener_t *listener)
{
    /* Add the listener to the end of the linked list. */
    gcoap_listener_t *_last = _coap_state.listeners;
    while (_last->next) {
        _last = _last->next;
    }

    listener->next = NULL;
    if (!listener->link_encoder) {
        listener->link_encoder = gcoap_encode_link;
    }
    _last->next = listener;
}

int gcoap_req_init(coap_pkt_t *pdu, uint8_t *buf, size_t len,
                   unsigned code, const char *path)
{
    assert((path == NULL) || (path[0] == '/'));

    pdu->hdr = (coap_hdr_t *)buf;

    /* generate token */
#if GCOAP_TOKENLEN
    uint8_t token[GCOAP_TOKENLEN];
    for (size_t i = 0; i < GCOAP_TOKENLEN; i += 4) {
        uint32_t rand = random_uint32();
        memcpy(&token[i],
               &rand,
               (GCOAP_TOKENLEN - i >= 4) ? 4 : GCOAP_TOKENLEN - i);
    }
    uint16_t msgid = (uint16_t)atomic_fetch_add(&_coap_state.next_message_id, 1);
    ssize_t res = coap_build_hdr(pdu->hdr, COAP_TYPE_NON, &token[0], GCOAP_TOKENLEN,
                                 code, msgid);
#else
    uint16_t msgid = (uint16_t)atomic_fetch_add(&_coap_state.next_message_id, 1);
    ssize_t res = coap_build_hdr(pdu->hdr, COAP_TYPE_NON, NULL, GCOAP_TOKENLEN,
                                 code, msgid);
#endif

    coap_pkt_init(pdu, buf, len - GCOAP_REQ_OPTIONS_BUF, res);
    if (path != NULL) {
        res = coap_opt_add_string(pdu, COAP_OPT_URI_PATH, path, '/');
    }
    return (res > 0) ? 0 : res;
}

/*
 * Assumes pdu.payload_len attribute was reduced in gcoap_xxx_init() to
 * ensure enough space in PDU buffer to write Content-Format option and
 * payload marker here.
 */
ssize_t gcoap_finish(coap_pkt_t *pdu, size_t payload_len, unsigned format)
{
    assert( !(pdu->options_len) ||
            !(payload_len) ||
            (format == COAP_FORMAT_NONE) ||
            (pdu->options[pdu->options_len-1].opt_num < COAP_OPT_CONTENT_FORMAT));

    if (payload_len) {
        /* determine Content-Format option length */
        unsigned format_optlen = 1;
        if (format == COAP_FORMAT_NONE) {
            format_optlen = 0;
        }
        else if (format > 255) {
            format_optlen = 3;
        }
        else if (format > 0) {
            format_optlen = 2;
        }

        /* move payload to accommodate option and payload marker */
        memmove(pdu->payload+format_optlen+1, pdu->payload, payload_len);

        if (format_optlen) {
            coap_opt_add_uint(pdu, COAP_OPT_CONTENT_FORMAT, format);
        }
        *pdu->payload++ = 0xFF;
    }
    /* must write option before updating PDU with actual length */
    pdu->payload_len = payload_len;

    return pdu->payload_len + (pdu->payload - (uint8_t *)pdu->hdr);
}

size_t gcoap_req_send(const uint8_t *buf, size_t len,
                      const sock_udp_ep_t *remote,
                      gcoap_resp_handler_t resp_handler)
{
    gcoap_request_memo_t *memo = NULL;
    unsigned msg_type  = (*buf & 0x30) >> 4;
    uint32_t timeout   = 0;

    assert(remote != NULL);

    /* Only allocate memory if necessary (i.e. if user is interested in the
     * response or request is confirmable) */
    if ((resp_handler != NULL) || (msg_type == COAP_TYPE_CON)) {
        mutex_lock(&_coap_state.lock);
        /* Find empty slot in list of open requests. */
        for (int i = 0; i < GCOAP_REQ_WAITING_MAX; i++) {
            if (_coap_state.open_reqs[i].state == GCOAP_MEMO_UNUSED) {
                memo = &_coap_state.open_reqs[i];
                memo->state = GCOAP_MEMO_WAIT;
                break;
            }
        }
        if (!memo) {
            mutex_unlock(&_coap_state.lock);
            DEBUG("gcoap: dropping request; no space for response tracking\n");
            return 0;
        }

        memo->resp_handler = resp_handler;
        memcpy(&memo->remote_ep, remote, sizeof(sock_udp_ep_t));

        switch (msg_type) {
        case COAP_TYPE_CON:
            /* copy buf to resend_bufs record */
            memo->msg.data.pdu_buf = NULL;
            for (int i = 0; i < GCOAP_RESEND_BUFS_MAX; i++) {
                if (!_coap_state.resend_bufs[i][0]) {
                    memo->msg.data.pdu_buf = &_coap_state.resend_bufs[i][0];
                    memcpy(memo->msg.data.pdu_buf, buf, GCOAP_PDU_BUF_SIZE);
                    memo->msg.data.pdu_len = len;
                    break;
                }
            }
            if (memo->msg.data.pdu_buf) {
                memo->send_limit  = COAP_MAX_RETRANSMIT;
                timeout           = (uint32_t)COAP_ACK_TIMEOUT * US_PER_SEC;
#if COAP_ACK_VARIANCE > 0
                uint32_t variance = (uint32_t)COAP_ACK_VARIANCE * US_PER_SEC;
                timeout = random_uint32_range(timeout, timeout + variance);
#endif
            }
            else {
                memo->state = GCOAP_MEMO_UNUSED;
                DEBUG("gcoap: no space for PDU in resend bufs\n");
            }
            break;

        case COAP_TYPE_NON:
            memo->send_limit = GCOAP_SEND_LIMIT_NON;
            memcpy(&memo->msg.hdr_buf[0], buf, GCOAP_HEADER_MAXLEN);
            timeout = GCOAP_NON_TIMEOUT;
            break;
        default:
            memo->state = GCOAP_MEMO_UNUSED;
            DEBUG("gcoap: illegal msg type %u\n", msg_type);
            break;
        }
        mutex_unlock(&_coap_state.lock);
        if (memo->state == GCOAP_MEMO_UNUSED) {
            return 0;
        }
    }

    /* Memos complete; send msg and start timer */
    ssize_t res = sock_udp_send(&_sock, buf, len, remote);

    /* timeout may be zero for non-confirmable */
    if ((memo != NULL) && (res > 0) && (timeout > 0)) {
        /* We assume gcoap_req_send() is called on some thread other than
         * gcoap's. First, put a message in the mbox for the sock udp object,
         * which will interrupt listening on the gcoap thread. (When there are
         * no outstanding requests, gcoap blocks indefinitely in _listen() at
         * sock_udp_recv().) While the message sent here is outstanding, the
         * sock_udp_recv() call will be set to a short timeout so the request
         * timer below, also on the gcoap thread, is processed in a timely
         * manner. */
        msg_t mbox_msg;
        mbox_msg.type          = GCOAP_MSG_TYPE_INTR;
        mbox_msg.content.value = 0;
        if (mbox_try_put(&_sock.reg.mbox, &mbox_msg)) {
            /* start response wait timer on the gcoap thread */
            memo->timeout_msg.type        = GCOAP_MSG_TYPE_TIMEOUT;
            memo->timeout_msg.content.ptr = (char *)memo;
            xtimer_set_msg(&memo->response_timer, timeout, &memo->timeout_msg, _pid);
        }
        else {
            res = 0;
            DEBUG("gcoap: can't wake up mbox; no timeout for msg\n");
        }
    }
    if (res <= 0) {
        if (memo != NULL) {
            if (msg_type == COAP_TYPE_CON) {
                *memo->msg.data.pdu_buf = 0;    /* clear resend buffer */
            }
            memo->state = GCOAP_MEMO_UNUSED;
        }
        DEBUG("gcoap: sock send failed: %d\n", (int)res);
    }
    return (size_t)((res > 0) ? res : 0);
}

int gcoap_resp_init(coap_pkt_t *pdu, uint8_t *buf, size_t len, unsigned code)
{
    if (coap_get_type(pdu) == COAP_TYPE_CON) {
        coap_hdr_set_type(pdu->hdr, COAP_TYPE_ACK);
    }
    coap_hdr_set_code(pdu->hdr, code);

    unsigned header_len  = coap_get_total_hdr_len(pdu);

    pdu->options_len = 0;
    pdu->payload     = buf + header_len;
    pdu->payload_len = len - header_len - GCOAP_RESP_OPTIONS_BUF;

    if (coap_get_observe(pdu) == COAP_OBS_REGISTER) {
        /* generate initial notification value */
        uint32_t now       = xtimer_now_usec();
        pdu->observe_value = (now >> GCOAP_OBS_TICK_EXPONENT) & 0xFFFFFF;
        coap_opt_add_uint(pdu, COAP_OPT_OBSERVE, pdu->observe_value);
    }

    return 0;
}

int gcoap_obs_init(coap_pkt_t *pdu, uint8_t *buf, size_t len,
                                                  const coap_resource_t *resource)
{
    gcoap_observe_memo_t *memo = NULL;

    _find_obs_memo_resource(&memo, resource);
    if (memo == NULL) {
        /* Unique return value to specify there is not an observer */
        return GCOAP_OBS_INIT_UNUSED;
    }

    pdu->hdr       = (coap_hdr_t *)buf;
    uint16_t msgid = (uint16_t)atomic_fetch_add(&_coap_state.next_message_id, 1);
    ssize_t hdrlen = coap_build_hdr(pdu->hdr, COAP_TYPE_NON, &memo->token[0],
                                    memo->token_len, COAP_CODE_CONTENT, msgid);

    if (hdrlen > 0) {
        coap_pkt_init(pdu, buf, len - GCOAP_OBS_OPTIONS_BUF, hdrlen);

        uint32_t now       = xtimer_now_usec();
        pdu->observe_value = (now >> GCOAP_OBS_TICK_EXPONENT) & 0xFFFFFF;
        coap_opt_add_uint(pdu, COAP_OPT_OBSERVE, pdu->observe_value);

        return GCOAP_OBS_INIT_OK;
    }
    else {
        /* reason for negative hdrlen is not defined, so we also are vague */
        return GCOAP_OBS_INIT_ERR;
    }
}

size_t gcoap_obs_send(const uint8_t *buf, size_t len,
                      const coap_resource_t *resource)
{
    gcoap_observe_memo_t *memo = NULL;

    _find_obs_memo_resource(&memo, resource);

    if (memo) {
        ssize_t bytes = sock_udp_send(&_sock, buf, len, memo->observer);
        return (size_t)((bytes > 0) ? bytes : 0);
    }
    else {
        return 0;
    }
}

uint8_t gcoap_op_state(void)
{
    uint8_t count = 0;
    for (int i = 0; i < GCOAP_REQ_WAITING_MAX; i++) {
        if (_coap_state.open_reqs[i].state != GCOAP_MEMO_UNUSED) {
            count++;
        }
    }
    return count;
}

int gcoap_get_resource_list(void *buf, size_t maxlen, uint8_t cf)
{
    assert(cf == COAP_FORMAT_LINK);

    /* skip the first listener, gcoap itself (we skip /.well-known/core) */
    gcoap_listener_t *listener = _coap_state.listeners->next;

    char *out = (char *)buf;
    size_t pos = 0;

    coap_link_encoder_ctx_t ctx;
    ctx.content_format = cf;
    /* indicate initial link for the list */
    ctx.flags = COAP_LINK_FLAG_INIT_RESLIST;

    /* write payload */
    while (listener) {
        if (!listener->link_encoder) {
            continue;
        }
        ctx.link_pos = 0;

        for (; ctx.link_pos < listener->resources_len; ctx.link_pos++) {
            ssize_t res;
            if (out) {
                res = listener->link_encoder(&listener->resources[ctx.link_pos],
                                             &out[pos], maxlen - pos, &ctx);
            }
            else {
                res = listener->link_encoder(&listener->resources[ctx.link_pos],
                                             NULL, 0, &ctx);
            }

            if (res > 0) {
                pos += res;
                ctx.flags &= ~COAP_LINK_FLAG_INIT_RESLIST;
            }
            else {
                break;
            }
        }

        listener = listener->next;
    }

    return (int)pos;
}

ssize_t gcoap_encode_link(const coap_resource_t *resource, char *buf,
                          size_t maxlen, coap_link_encoder_ctx_t *context)
{
    size_t path_len = strlen(resource->path);
     /* count target separators and any link separator */
    size_t exp_size = path_len + 2
                        + ((context->flags & COAP_LINK_FLAG_INIT_RESLIST) ? 0 : 1);

    if (buf) {
        unsigned pos = 0;
        if (exp_size > maxlen) {
            return -1;
        }

        if (!(context->flags & COAP_LINK_FLAG_INIT_RESLIST)) {
            buf[pos++] = ',';
        }
        buf[pos++] = '<';
        memcpy(&buf[pos], resource->path, path_len);
        buf[pos+path_len] = '>';
    }

    return exp_size;
}

int gcoap_add_qstring(coap_pkt_t *pdu, const char *key, const char *val)
{
    char qs[NANOCOAP_QS_MAX];
    size_t len = strlen(key);
    size_t val_len = (val) ? (strlen(val) + 1) : 0;

    /* test if the query string fits, account for the zero termination */
    if ((len + val_len + 1) >= NANOCOAP_QS_MAX) {
        return -1;
    }

    memcpy(&qs[0], key, len);
    if (val) {
        qs[len] = '=';
        /* the `=` character was already counted in `val_len`, so subtract it here */
        memcpy(&qs[len + 1], val, (val_len - 1));
        len += val_len;
    }
    qs[len] = '\0';

    return coap_opt_add_string(pdu, COAP_OPT_URI_QUERY, qs, '&');
}

/** @} */

/*
 * Copyright (c) 2015-2020 Ken Bannister. All rights reserved.
 *               2019 Freie Universität Berlin
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
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include <errno.h>
#include <stdint.h>
#include <stdatomic.h>
#include <string.h>

#include "assert.h"
#include "net/coap.h"
#include "net/gcoap.h"
#include "net/gcoap/forward_proxy.h"
#include "net/ipv6/addr.h"
#include "net/nanocoap.h"
#include "net/nanocoap/cache.h"
#include "net/sock/async/event.h"
#include "net/sock/udp.h"
#include "net/sock/util.h"
#include "mutex.h"
#include "random.h"
#include "thread.h"
#include "ztimer.h"

#if IS_USED(MODULE_GCOAP_DTLS)
#include "net/sock/dtls.h"
#include "net/credman.h"
#include "net/dsm.h"
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

/* Sentinel value indicating that no immediate response is required */
#define NO_IMMEDIATE_REPLY (-1)

/* End of the range to pick a random timeout */
#define TIMEOUT_RANGE_END ((uint32_t)CONFIG_COAP_ACK_TIMEOUT_MS * CONFIG_COAP_RANDOM_FACTOR_1000 / 1000)

/* Internal functions */
static void *_event_loop(void *arg);
static void _on_sock_udp_evt(sock_udp_t *sock, sock_async_flags_t type, void *arg);
static void _process_coap_pdu(gcoap_socket_t *sock, sock_udp_ep_t *remote, sock_udp_aux_tx_t *aux,
                              uint8_t *buf, size_t len, bool truncated);
static int _tl_init_coap_socket(gcoap_socket_t *sock, gcoap_socket_type_t type);
static ssize_t _tl_send(gcoap_socket_t *sock, const void *data, size_t len,
                        const sock_udp_ep_t *remote, sock_udp_aux_tx_t *aux);
static ssize_t _tl_authenticate(gcoap_socket_t *sock, const sock_udp_ep_t *remote,
                                uint32_t timeout);
static ssize_t _well_known_core_handler(coap_pkt_t* pdu, uint8_t *buf, size_t len,
                                        coap_request_ctx_t *ctx);
static void _cease_retransmission(gcoap_request_memo_t *memo);
static size_t _handle_req(gcoap_socket_t *sock, coap_pkt_t *pdu, uint8_t *buf,
                          size_t len, sock_udp_ep_t *remote, sock_udp_aux_tx_t *aux);
static void _expire_request(gcoap_request_memo_t *memo);
static gcoap_request_memo_t* _find_req_memo_by_mid(const sock_udp_ep_t *remote,
                                                   uint16_t mid);
static gcoap_request_memo_t* _find_req_memo_by_token(const sock_udp_ep_t *remote,
                                            const uint8_t *token, size_t tkl);
static gcoap_request_memo_t* _find_req_memo_by_pdu_token(const coap_pkt_t *src_pdu,
                                                const sock_udp_ep_t *remote);
static int _find_resource(gcoap_socket_type_t tl_type,
                          coap_pkt_t *pdu,
                          const coap_resource_t **resource_ptr,
                          gcoap_listener_t **listener_ptr);
static int _find_observer(sock_udp_ep_t **observer, sock_udp_ep_t *remote);
static int _find_notifier(sock_udp_ep_t **notifier, sock_udp_ep_t *local);
static int _find_obs_memo(gcoap_observe_memo_t **memo,
                          sock_udp_ep_t *remote, sock_udp_ep_t *local,
                          coap_pkt_t *pdu);
static void _find_obs_memo_resource(gcoap_observe_memo_t **memo,
                                   const coap_resource_t *resource);

static void _check_and_expire_obs_memo_last_mid(sock_udp_ep_t *remote,
                                                uint16_t last_notify_mid);

static nanocoap_cache_entry_t *_cache_lookup_memo(gcoap_request_memo_t *cache_key);
static void _cache_process(gcoap_request_memo_t *memo,
                           coap_pkt_t *pdu);
static ssize_t _cache_build_response(nanocoap_cache_entry_t *ce, coap_pkt_t *pdu,
                                     uint8_t *buf, size_t len);
static void _receive_from_cache_cb(void *arg);

static int _request_matcher_default(gcoap_listener_t *listener,
                                    const coap_resource_t **resource,
                                    coap_pkt_t *pdu);

#if IS_USED(MODULE_GCOAP_DTLS)
static void _on_sock_dtls_evt(sock_dtls_t *sock, sock_async_flags_t type, void *arg);
static void _dtls_free_up_session(void *arg);
#endif

static char _ipv6_addr_str[IPV6_ADDR_MAX_STR_LEN];

/* Internal variables */
const coap_resource_t _default_resources[] = {
    { "/.well-known/core", COAP_GET, _well_known_core_handler, NULL },
};

static gcoap_listener_t _default_listener = {
    &_default_resources[0],
    ARRAY_SIZE(_default_resources),
    GCOAP_SOCKET_TYPE_UNDEF,
    NULL,
    NULL,
    _request_matcher_default
};

/* Container for the state of gcoap itself */
typedef struct {
    mutex_t lock;                       /* Shares state attributes safely */
    gcoap_listener_t *listeners;        /* List of registered listeners */
    gcoap_request_memo_t open_reqs[CONFIG_GCOAP_REQ_WAITING_MAX];
                                        /* Storage for open requests; if first
                                           byte of an entry is zero, the entry
                                           is available */
    atomic_uint next_message_id;        /* Next message ID to use */
    sock_udp_ep_t observers[CONFIG_GCOAP_OBS_CLIENTS_MAX];
                                        /* Observe clients; allows reuse for
                                           observe memos */
    /**
     * @brief   Local endpoint aliases to send notifications from
     */
    sock_udp_ep_t notifiers[CONFIG_GCOAP_OBS_NOTIFIERS_MAX];
    gcoap_observe_memo_t observe_memos[CONFIG_GCOAP_OBS_REGISTRATIONS_MAX];
                                        /* Observed resource registrations */
    uint8_t resend_bufs[CONFIG_GCOAP_RESEND_BUFS_MAX][CONFIG_GCOAP_PDU_BUF_SIZE];
                                        /* Buffers for PDU for request resends;
                                           if first byte of an entry is zero,
                                           the entry is available */
} gcoap_state_t;

static gcoap_state_t _coap_state = {
    .listeners   = &_default_listener,
};

static kernel_pid_t _pid = KERNEL_PID_UNDEF;
static char _msg_stack[GCOAP_STACK_SIZE];
static event_queue_t _queue;
static uint8_t _listen_buf[CONFIG_GCOAP_PDU_BUF_SIZE];
static sock_udp_t _sock_udp;
static event_callback_t _receive_from_cache;

#if IS_USED(MODULE_GCOAP_DTLS)
/* DTLS variables and definitions */
#define SOCK_DTLS_CLIENT_TAG (2)
static sock_udp_t _sock_dtls_base;
static sock_dtls_t _sock_dtls;
static kernel_pid_t _auth_waiting_thread;

static event_timeout_t _dtls_session_free_up_tmout;
static event_callback_t _dtls_session_free_up_tmout_cb;
#endif

/* Event loop for gcoap _pid thread. */
static void *_event_loop(void *arg)
{
    (void)arg;

    sock_udp_ep_t local;
    memset(&local, 0, sizeof(sock_udp_ep_t));

    /* FIXME: Once the problems with IPv4/IPv6 dual stack use in RIOT are fixed, adapt these lines
     *        (and e.g. use AF_UNSPEC) */
#ifdef SOCK_HAS_IPV4
    local.family = AF_INET;
#endif

#ifdef SOCK_HAS_IPV6
    local.family = AF_INET6;
#endif

    local.netif  = SOCK_ADDR_ANY_NETIF;
    local.port = CONFIG_GCOAP_PORT;
    int res = sock_udp_create(&_sock_udp, &local, NULL, 0);
    if (res < 0) {
        DEBUG("gcoap: cannot create sock: %d\n", res);
        return 0;
    }

    event_queue_init(&_queue);
    sock_udp_event_init(&_sock_udp, &_queue, _on_sock_udp_evt, NULL);

    if (IS_USED(MODULE_GCOAP_DTLS)) {
#if IS_USED(MODULE_GCOAP_DTLS)
        local.port = CONFIG_GCOAPS_PORT;
        if (sock_udp_create(&_sock_dtls_base, &local, NULL, 0)) {
            DEBUG("gcoap: error creating DTLS transport sock\n");
            return 0;
        }
        if (sock_dtls_create(&_sock_dtls, &_sock_dtls_base,
                            CREDMAN_TAG_EMPTY,
                            SOCK_DTLS_1_2, SOCK_DTLS_SERVER) < 0) {
            DEBUG("gcoap: error creating DTLS sock\n");
            sock_udp_close(&_sock_dtls_base);
            return 0;
        }
        sock_dtls_event_init(&_sock_dtls, &_queue, _on_sock_dtls_evt,
                            NULL);
#endif
    }

    event_loop(&_queue);
    return 0;
}

#if IS_USED(MODULE_GCOAP_DTLS)
/* Handles DTLS socket events from the event queue */
static void _on_sock_dtls_evt(sock_dtls_t *sock, sock_async_flags_t type, void *arg) {
    (void)arg;
    gcoap_socket_t socket = { .type = GCOAP_SOCKET_TYPE_DTLS, .socket.dtls = sock};

    if (type & SOCK_ASYNC_CONN_RECV) {
        ssize_t res = sock_dtls_recv(sock, &socket.ctx_dtls_session,
                            _listen_buf, sizeof(_listen_buf),
                            CONFIG_GCOAP_DTLS_HANDSHAKE_TIMEOUT_MSEC);
        if (res != -SOCK_DTLS_HANDSHAKE) {
            DEBUG("gcoap: could not establish DTLS session: %" PRIdSIZE "\n", res);
            sock_dtls_session_destroy(sock, &socket.ctx_dtls_session);
            return;
        }
        dsm_state_t prev_state = dsm_store(sock, &socket.ctx_dtls_session,
                                           SESSION_STATE_ESTABLISHED, false);

        /* If session is already stored and the state was SESSION_STATE_HANDSHAKE
        before, the handshake has been initiated internally by a gcoap client request
        and another thread is waiting for the handshake. Send message to the
        waiting thread to inform about established session */
        if (prev_state == SESSION_STATE_HANDSHAKE) {
            msg_t msg = { .type = DTLS_EVENT_CONNECTED };
            msg_send(&msg, _auth_waiting_thread);
        } else if (prev_state == NO_SPACE) {
            /* No space in session management. Should not happen. If it occurs,
            we lost track of sessions */
            DEBUG("gcoap: no space in session management. We lost track of sessions!");
            sock_dtls_session_destroy(sock, &socket.ctx_dtls_session);
        }

        /* If not enough session slots left: set timeout to free up session */
        uint8_t minimum_free = CONFIG_GCOAP_DTLS_MINIMUM_AVAILABLE_SESSIONS;
        if (dsm_get_num_available_slots() < minimum_free)
        {
            uint32_t timeout = CONFIG_GCOAP_DTLS_MINIMUM_AVAILABLE_SESSIONS_TIMEOUT_MSEC;
            event_callback_init(&_dtls_session_free_up_tmout_cb,
                                _dtls_free_up_session, NULL);
            event_timeout_ztimer_init(&_dtls_session_free_up_tmout, ZTIMER_MSEC, &_queue,
                               &_dtls_session_free_up_tmout_cb.super);
            event_timeout_set(&_dtls_session_free_up_tmout, timeout);
        }
    }

    if (type & SOCK_ASYNC_CONN_FIN) {
        if (sock_dtls_get_event_session(sock, &socket.ctx_dtls_session)) {
            /* Session is already destroyed, only remove it from dsm */
            dsm_remove(sock, &socket.ctx_dtls_session);
        } else {
            puts("gcoap: A session was closed, but the corresponding session " \
            "could not be retrieved from the socket!");
            return;
        }
        sock_udp_ep_t ep;
        sock_dtls_session_get_udp_ep(&socket.ctx_dtls_session, &ep);

        /* Remove all memos of the concerned session. TODO: oberservable memos! */
        for (int i = 0; i < CONFIG_GCOAP_REQ_WAITING_MAX; i++) {
            if (_coap_state.open_reqs[i].state == GCOAP_MEMO_UNUSED) {
                continue;
            }
            gcoap_request_memo_t *memo = &_coap_state.open_reqs[i];
            if (sock_udp_ep_equal(&memo->remote_ep, &ep)) {
                _expire_request(memo);
                event_timeout_clear(&memo->resp_evt_tmout);
            }
        }
    }

    if (type & SOCK_ASYNC_MSG_RECV) {
        ssize_t res = sock_dtls_recv(sock, &socket.ctx_dtls_session, _listen_buf,
                                    sizeof(_listen_buf), 0);
        if (res <= 0) {
            DEBUG("gcoap: DTLS recv failure: %" PRIdSIZE "\n", res);
            return;
        }
        sock_udp_ep_t ep;
        sock_dtls_session_get_udp_ep(&socket.ctx_dtls_session, &ep);
        /* Truncated DTLS messages would already have gotten lost at verification */
        _process_coap_pdu(&socket, &ep, NULL, _listen_buf, res, false);
    }
}

/* Timeout function to free up a session when too many session slots are occupied */
static void _dtls_free_up_session(void *arg) {
    (void)arg;
    sock_dtls_session_t session;

    uint8_t minimum_free = CONFIG_GCOAP_DTLS_MINIMUM_AVAILABLE_SESSIONS;
    if (dsm_get_num_available_slots() < minimum_free) {
        if (dsm_get_least_recently_used_session(&_sock_dtls, &session) != -1) {
            /* free up session */
            dsm_remove(&_sock_dtls, &session);
            sock_dtls_session_destroy(&_sock_dtls, &session);
        }
    }
}
#endif /* MODULE_GCOAP_DTLS */

/* Handles UDP socket events from the event queue. */
static void _on_sock_udp_evt(sock_udp_t *sock, sock_async_flags_t type, void *arg)
{
    (void)arg;
    sock_udp_ep_t remote;

    if (type & SOCK_ASYNC_MSG_RECV) {
        void *stackbuf;
        void *buf_ctx = NULL;
        bool truncated = false;
        size_t cursor = 0;
        sock_udp_aux_rx_t aux_in = {
            .flags = SOCK_AUX_GET_LOCAL,
        };

        /* The zero-copy _buf API is not used to its full potential here -- we
         * still copy out data in what is a manual version of sock_udp_recv,
         * but this gives the direly needed overflow information.
         *
         * A version that actually doesn't copy would vastly change the way
         * gcoap passes the buffer to be read from and written into to the
         * handler. Also, given that neither nanocoap nor the handler expects
         * to gather scattered data, it'd need to rely on the data coming in a
         * single slice (but that may be a realistic assumption).
         */
        while (true) {
            ssize_t res = sock_udp_recv_buf_aux(sock, &stackbuf, &buf_ctx, 0, &remote, &aux_in);
            if (res < 0) {
                DEBUG("gcoap: udp recv failure: %" PRIdSIZE "\n", res);
                return;
            }
            if (res == 0) {
                break;
            }
            if (cursor + res > sizeof(_listen_buf)) {
                res = sizeof(_listen_buf) - cursor;
                truncated = true;
            }
            memcpy(&_listen_buf[cursor], stackbuf, res);
            cursor += res;
        }

        /* make sure we reply with the same address that the request was
         * destined for -- except in the multicast case */
        sock_udp_aux_tx_t *aux_out_ptr;
        sock_udp_aux_tx_t aux_out = {
            .flags = SOCK_AUX_SET_LOCAL,
            .local = aux_in.local,
        };
        if (sock_udp_ep_is_multicast(&aux_in.local)) {
            /* This eventually gets passed to sock_udp_send_aux, where NULL
             * simply does not set any flags */
            aux_out_ptr = NULL;
        } else {
            aux_out_ptr = &aux_out;
        }

        gcoap_socket_t socket = {
            .type = GCOAP_SOCKET_TYPE_UDP,
            .socket.udp = sock,
         };

        _process_coap_pdu(&socket, &remote, aux_out_ptr, _listen_buf, cursor, truncated);
    }
}

static void _memo_clear_resend_buffer(gcoap_request_memo_t *memo)
{
    uint8_t hdr[GCOAP_HEADER_MAXLEN];
    if (memo->send_limit >= 0 && memo->msg.data.pdu_buf) {
        /* store header from retransmission buffer */
        memcpy(hdr, memo->msg.data.pdu_buf, sizeof(hdr));
        /* mark referenced retransmission buffer as available again */
        *memo->msg.data.pdu_buf = 0;
        /* but store the header to keep the token for Observe notifications */
        memcpy(memo->msg.hdr_buf, hdr, sizeof(hdr));
        /* no further retransmissions should be made and
           gcoap_request_memo_get_hdr() has to know how to get the header for Observe notifications */
        memo->send_limit = GCOAP_SEND_LIMIT_NON;
    }
}

/* Processes and evaluates the coap pdu */
static void _process_coap_pdu(gcoap_socket_t *sock, sock_udp_ep_t *remote, sock_udp_aux_tx_t *aux,
                              uint8_t *buf, size_t len, bool truncated)
{
    coap_pkt_t pdu;
    gcoap_request_memo_t *memo = NULL;
    /* Code paths that necessitate a response on the message layer can set a
     * response type here (COAP_TYPE_RST or COAP_TYPE_ACK). If set, at the end
     * of the function there will be
     *   * that value will be put in the code field,
     *   * token length cleared,
     *   * code set to EMPTY, and
     *   * the message is returned with the rest of its header intact.
     */
    int8_t messagelayer_emptyresponse_type = NO_IMMEDIATE_REPLY;

    ssize_t res = coap_parse(&pdu, buf, len);
    if (res < 0) {
        DEBUG("gcoap: parse failure: %" PRIdSIZE "\n", res);
        /* If a response, can't clear memo, but it will timeout later.
         *
         * There are *some* error cases in which we could continue (eg. all
         * sorts of "packet ends mid-options" in truncated cases, and maybe
         * also when the maximum option count is exceeded to at least respond
         * with Bad Request), but these would likely require incompatible
         * changes to nanocoap.
         */
        return;
    }

    if (coap_get_type(&pdu) == COAP_TYPE_RST) {
        DEBUG("gcoap: received RST, expiring potentially existing memo\n");
        memo = _find_req_memo_by_mid(remote, pdu.hdr->id);
        if (memo) {
            event_timeout_clear(&memo->resp_evt_tmout);
            _expire_request(memo);
        }

        /* check if this RST is due to the client not being interested
         * in receiving observe notifications anymore. */
        _check_and_expire_obs_memo_last_mid(remote, coap_get_id(&pdu));
    }

    /* validate class and type for incoming */
    unsigned code_class = coap_get_code_class(&pdu);
    switch (code_class) {
    /* incoming request or empty */
    case COAP_CLASS_REQ:
        if (coap_get_code_raw(&pdu) == COAP_CODE_EMPTY) {
            /* ping request */
            if (coap_get_type(&pdu) == COAP_TYPE_CON) {
                messagelayer_emptyresponse_type = COAP_TYPE_RST;
                DEBUG("gcoap: Answering empty CON request with RST\n");
            } else if (coap_get_type(&pdu) == COAP_TYPE_ACK) {
                memo = _find_req_memo_by_mid(remote, pdu.hdr->id);
                if ((memo != NULL) && (memo->send_limit != GCOAP_SEND_LIMIT_NON)) {
                    DEBUG("gcoap: empty ACK processed, stopping retransmissions\n");
                    /* for a separate CON response the memo is deleted because there is no response handler set  */
                    _cease_retransmission(memo);
                } else {
                    DEBUG("gcoap: empty ACK matches no known CON, ignoring\n");
                }
            } else {
                DEBUG("gcoap: Ignoring empty non-CON request\n");
            }
        }
        /* normal request */
        else if (coap_get_type(&pdu) == COAP_TYPE_NON
                || coap_get_type(&pdu) == COAP_TYPE_CON) {
            ssize_t pdu_len;

            if (truncated) {
                /* TBD: Set a Size1 */
                pdu_len = gcoap_response(&pdu, _listen_buf, sizeof(_listen_buf),
                                         COAP_CODE_REQUEST_ENTITY_TOO_LARGE);
            } else {
                pdu_len = _handle_req(sock, &pdu, _listen_buf,
                                      sizeof(_listen_buf), remote, aux);
            }

            if (pdu_len > 0) {
                ssize_t bytes = _tl_send(sock, _listen_buf, pdu_len, remote, aux);
                if (bytes <= 0) {
                    DEBUG("gcoap: send response failed: %" PRIdSIZE "\n", bytes);
                }
            }
            else if (!pdu_len && coap_get_type(&pdu) == COAP_TYPE_CON) {
                messagelayer_emptyresponse_type = COAP_TYPE_ACK;
                DEBUG_PUTS("gcoap: Suspending CON request with empty ACK");
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
        memo = _find_req_memo_by_pdu_token(&pdu, remote);
        if (memo) {
            switch (coap_get_type(&pdu)) {
            case COAP_TYPE_CON:
                messagelayer_emptyresponse_type = COAP_TYPE_ACK;
                DEBUG("gcoap: Answering CON response with ACK\n");
                /* fall through */
            case COAP_TYPE_NON:
            case COAP_TYPE_ACK:
                if (memo->resp_evt_tmout.queue) {
                    event_timeout_clear(&memo->resp_evt_tmout);
                }
                memo->state = truncated ? GCOAP_MEMO_RESP_TRUNC : GCOAP_MEMO_RESP;
                if (IS_USED(MODULE_NANOCOAP_CACHE)) {
                    nanocoap_cache_entry_t *ce = NULL;

                    if ((pdu.hdr->code == COAP_CODE_VALID) &&
                        (ce = _cache_lookup_memo(memo))) {
                        /* update max_age from response and send cached response */
                        uint32_t max_age = 60;

                        coap_opt_get_uint(&pdu, COAP_OPT_MAX_AGE, &max_age);
                        ce->max_age = ztimer_now(ZTIMER_SEC) + max_age;
                        /* copy all options and possible payload from the cached response
                         * to the new response */
                        assert((uint8_t *)pdu.hdr == &_listen_buf[0]);
                        if (_cache_build_response(ce, &pdu, _listen_buf,
                                                  sizeof(_listen_buf)) < 0) {
                            memo->state = GCOAP_MEMO_ERR;
                        }
                        if (ce->truncated) {
                            memo->state = GCOAP_MEMO_RESP_TRUNC;
                        }
                    }
                    /* TODO: resend request if VALID but no cache entry? */
                    else if ((pdu.hdr->code != COAP_CODE_VALID)) {
                        _cache_process(memo, &pdu);
                    }
                }

                bool observe_notification = coap_has_observe(&pdu);

                if (memo->resp_handler) {
                    memo->resp_handler(memo, &pdu, remote);
                }

                _memo_clear_resend_buffer(memo);

                /* The memo must be kept if the response is an observe notification.
                 * Non-2.xx notifications indicate that the associated observe entry
                 * was removed on the server side. Then also free the memo here. */
                if (!observe_notification || (code_class != COAP_CLASS_SUCCESS)) {
                    /* setting the state to unused frees (drops) the memo entry */
                    memo->state = GCOAP_MEMO_UNUSED;
                }

                break;
            default:
                DEBUG("gcoap: illegal response type: %u\n", coap_get_type(&pdu));
                break;
            }
        }
        else {
            DEBUG("gcoap: msg not found for ID: %u\n", coap_get_id(&pdu));
            if (coap_get_type(&pdu) == COAP_TYPE_CON) {
                /* we might run into this if an ACK to a sender got lost
                 * see https://datatracker.ietf.org/doc/html/rfc7252#section-5.3.2 */
                messagelayer_emptyresponse_type = COAP_TYPE_RST;
                DEBUG("gcoap: Answering unknown CON response with RST to "
                      "shut up sender\n");
            } else {
                /* if the response was a (NON) observe notification and there is no
                 * matching request, the server must be informed that this node is
                 * no longer interested in this notification. */
                if (coap_has_observe(&pdu)) {
                    messagelayer_emptyresponse_type = COAP_TYPE_RST;
                }
            }
        }
        break;
    default:
        DEBUG("gcoap: illegal code class: %u\n", coap_get_code_class(&pdu));
    }

    if (messagelayer_emptyresponse_type != NO_IMMEDIATE_REPLY) {
        coap_hdr_set_type(pdu.hdr, (uint8_t)messagelayer_emptyresponse_type);
        coap_pkt_set_code(&pdu, COAP_CODE_EMPTY);
        /* Set the token length to 0, preserving the CoAP version as it was and
         * the empty message type that was just set.
         *
         * FIXME: Introduce an internal function to set or truncate the token
         * */
        pdu.hdr->ver_t_tkl &= 0xf0;

        ssize_t bytes = _tl_send(sock, buf, sizeof(coap_hdr_t), remote, aux);
        if (bytes <= 0) {
            DEBUG("gcoap: empty response failed: %" PRIdSIZE "\n", bytes);
        }
    }
}

/* Handles response timeout for a request; resend confirmable if needed. */
static void _on_resp_timeout(void *arg) {
    gcoap_request_memo_t *memo = (gcoap_request_memo_t *)arg;

    /* no retries remaining */
    if ((memo->send_limit == GCOAP_SEND_LIMIT_NON) || (memo->send_limit == 0)) {
        _expire_request(memo);
    }
    /* reduce retries remaining, double timeout and resend */
    else {
        memo->send_limit--;
#ifdef CONFIG_GCOAP_NO_RETRANS_BACKOFF
        unsigned i        = 0;
#else
        unsigned i        = CONFIG_COAP_MAX_RETRANSMIT - memo->send_limit;
#endif
        uint32_t timeout  = (uint32_t)CONFIG_COAP_ACK_TIMEOUT_MS << i;
#if CONFIG_COAP_RANDOM_FACTOR_1000 > 1000
        uint32_t end = (uint32_t)TIMEOUT_RANGE_END << i;
        timeout = random_uint32_range(timeout, end);
#endif
        event_timeout_set(&memo->resp_evt_tmout, timeout);

        if (memo->state == GCOAP_MEMO_WAIT) {
            /* See _cease_retransmission: Still going through the timeouts and
             * rescheduling, but not actually sending any more */
            return;
        }

        ssize_t bytes = _tl_send(&memo->socket, memo->msg.data.pdu_buf,
                                 memo->msg.data.pdu_len, &memo->remote_ep, NULL);
        if (bytes <= 0) {
            DEBUG("gcoap: sock resend failed: %" PRIdSIZE "\n", bytes);
            _expire_request(memo);
        }
    }
}

/* Change the retransmission of the memo such that no requests are sent any more.
 *
 * This is used in response to an empty ACK.
 *
 * The current implementation does not touch the timers, but merely sets the
 * memo's state to GCOAP_MEMO_WAIT. This approach needs less complex code at
 * the cost of the remaining `send_limit` timers firing and some memory not
 * being freed until the actual response arrives.
 *
 * An alternative implementation would stop the timeouts, and either free the
 * whole memo if it has no response handler, or calculate the remaining timeout
 * from `send_limit` to set a final timeout then. In that case, it might also
 * free the gcoap_resend_t data and move it back into hdr_buf (along with a
 * change in the discriminator for that). (That's not an option with the
 * current design because the discriminator is the send_limit field, which is
 * still used to count down).
 *
 * @param[in,out]  memo   The memo indicating the pending request
 *
 * @pre The @p memo is GCOAP_MEMO_RETRANSMIT or GCOAP_MEMO_WAIT, and its
 *      send_limit is not GCOAP_SEND_LIMIT_NON.
 */
static void _cease_retransmission(gcoap_request_memo_t *memo) {
    memo->state = GCOAP_MEMO_WAIT;
    /* there is also no response handler to wait for => expire memo */
    if (memo->resp_handler == NULL) {
        event_timeout_clear(&memo->resp_evt_tmout);
        _expire_request(memo);
    }
}

/*
 * Main request handler: generates response PDU in the provided buffer.
 *
 * Caller must finish the PDU and send it.
 *
 * return length of response pdu, or < 0 if can't handle
 */
static size_t _handle_req(gcoap_socket_t *sock, coap_pkt_t *pdu, uint8_t *buf,
                          size_t len, sock_udp_ep_t *remote, sock_udp_aux_tx_t *aux)
{
    const coap_resource_t *resource     = NULL;
    gcoap_listener_t *listener          = NULL;
    sock_udp_ep_t *observer             = NULL;
    sock_udp_ep_t *notifier             = NULL;
    gcoap_observe_memo_t *memo          = NULL;
    gcoap_observe_memo_t *resource_memo = NULL;

    switch (_find_resource(sock->type, pdu, &resource, &listener)) {
        case GCOAP_RESOURCE_WRONG_METHOD:
            return gcoap_response(pdu, buf, len, COAP_CODE_METHOD_NOT_ALLOWED);
        case GCOAP_RESOURCE_NO_PATH:
            return gcoap_response(pdu, buf, len, COAP_CODE_PATH_NOT_FOUND);
        case GCOAP_RESOURCE_FOUND:
            /* find observe registration for resource */
            _find_obs_memo_resource(&resource_memo, resource);
            break;
        case GCOAP_RESOURCE_ERROR:
        default:
            return gcoap_response(pdu, buf, len, COAP_CODE_INTERNAL_SERVER_ERROR);
            break;
    }

    if (coap_get_observe(pdu) == COAP_OBS_REGISTER) {
        /* lookup remote+token */
        int empty_slot = _find_obs_memo(&memo, remote, NULL, pdu);
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
            else if ((sock->type == resource_memo->socket.type) &&
                     sock_udp_ep_equal(remote, resource_memo->observer)) {
                /* accept new token for resource */
                memo = resource_memo;
            }
        }
        /* initialize new registration request */
        if ((memo == NULL) && coap_has_observe(pdu)) {
            /* verify resource not already registered (for another endpoint) */
            if ((empty_slot >= 0) && (resource_memo == NULL)) {
                int slot = _find_observer(&observer, remote);
                /* cache new observer */
                if (observer == NULL) {
                    if (slot >= 0) {
                        observer = &_coap_state.observers[slot];
                    } else {
                        DEBUG("gcoap: can't register observer\n");
                    }
                }
                slot = _find_notifier(&notifier, &aux->local);
                if (notifier == NULL) {
                    if (slot >= 0) {
                        notifier = &_coap_state.notifiers[slot];
                    } else {
                        DEBUG("gcoap: can't allocate notifier\n");
                    }
                }
                if (observer && notifier) {
                    memcpy(observer, remote, sizeof(*remote));
                    memcpy(notifier, &aux->local, sizeof(aux->local));
                    memo = &_coap_state.observe_memos[empty_slot];
                    memo->notifier = notifier;
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
            memo->socket = *sock;
            if (memo->token_len) {
                memcpy(&memo->token[0], coap_get_token(pdu), memo->token_len);
            }
            DEBUG("gcoap: Registered observer for: %s\n", memo->resource->path);
        }

    } else if (coap_get_observe(pdu) == COAP_OBS_DEREGISTER) {
        _find_obs_memo(&memo, remote, NULL, pdu);
        /* clear memo, and clear observer if no other memos */
        if (memo != NULL) {
            DEBUG("gcoap: Deregistering observer for: %s\n", memo->resource->path);
            memo->observer = NULL;
            gcoap_observe_memo_t *other_memo = NULL;
            _find_obs_memo(&other_memo, remote, NULL, NULL);
            if (other_memo == NULL) {
                _find_observer(&observer, remote);
                if (observer != NULL) {
                    observer->family = AF_UNSPEC;
                }
            }
            other_memo = NULL;
            _find_obs_memo(&other_memo, NULL, memo->notifier, NULL);
            if (!other_memo) {
                memo->notifier->family = AF_UNSPEC;
            }
            memo->notifier = NULL;
        }
        coap_clear_observe(pdu);

    } else if (coap_has_observe(pdu)) {
        /* bogus request; don't respond */
        DEBUG("gcoap: Observe value unexpected: %" PRIu32 "\n", coap_get_observe(pdu));
        return -1;
    }

    ssize_t pdu_len;

    coap_request_ctx_t ctx = {
        .resource = resource,
        .tl_type = (uint32_t)sock->type,
        .remote = remote,
        .local = aux ? &aux->local : NULL,
    };

    pdu_len = resource->handler(pdu, buf, len, &ctx);
    if (pdu_len < 0) {
        pdu_len = gcoap_response(pdu, buf, len,
                                 COAP_CODE_INTERNAL_SERVER_ERROR);
    }
    return pdu_len;
}

static const coap_resource_t *_match_resource_path_iterator(const gcoap_listener_t *listener,
                                                            const coap_resource_t *last,
                                                            const uint8_t *uri_path)
{
    size_t i;
    if (!last) {
        i = 0;
    }
    else {
        assert(listener);
        assert(last >= listener->resources);
        assert(last < listener->resources + listener->resources_len);
        i = (last - listener->resources) + 1;
    }
    for (; i < listener->resources_len; i++) {
        if (!coap_match_path(&listener->resources[i], uri_path)) {
            return &listener->resources[i];
        }
    }
    return NULL;
}

static int _request_matcher_default(gcoap_listener_t *listener,
                                    const coap_resource_t **resource,
                                    coap_pkt_t *pdu)
{
    uint8_t uri[CONFIG_NANOCOAP_URI_MAX];
    int ret = GCOAP_RESOURCE_NO_PATH;

    if (coap_get_uri_path(pdu, uri) <= 0) {
        /* The Uri-Path options are longer than
         * CONFIG_NANOCOAP_URI_MAX, and thus do not match anything
         * that could be found by this handler. */
        return GCOAP_RESOURCE_NO_PATH;
    }

    coap_method_flags_t method_flag = coap_method2flag(
        coap_get_code_detail(pdu));

    *resource = NULL;
    while ((*resource = _match_resource_path_iterator(listener, *resource, uri))) {
        /* potential match, check for method */
        if (!((*resource)->methods & method_flag)) {
            /* record wrong method error for next iteration, in case
             * another resource with the same URI and correct method
             * exists */
            ret = GCOAP_RESOURCE_WRONG_METHOD;
            continue;
        }
        else {
            return GCOAP_RESOURCE_FOUND;
        }
    }

    return ret;
}

/*
 * Searches listener registrations for the resource matching the path in a PDU.
 *
 * param[in]  tl_type -- transport the request for the resource came over.
 * param[in]  pdu -- the PDU to check the resource for
 * param[out] resource_ptr -- found resource
 * param[out] listener_ptr -- listener for found resource
 * return `GCOAP_RESOURCE_FOUND` if the resource was found,
 *        `GCOAP_RESOURCE_WRONG_METHOD` if a resource was found but the method
 *        code didn't match and `GCOAP_RESOURCE_NO_PATH` if no matching
 *        resource was found.
 */
static int _find_resource(gcoap_socket_type_t tl_type,
                          coap_pkt_t *pdu,
                          const coap_resource_t **resource_ptr,
                          gcoap_listener_t **listener_ptr)
{
    int ret = GCOAP_RESOURCE_NO_PATH;

    /* Find path for CoAP msg among listener resources and execute callback. */
    gcoap_listener_t *listener = _coap_state.listeners;

    while (listener) {
        const coap_resource_t *resource = NULL;
        int res;

        /* only makes sense to check if non-UDP transports are supported,
         * so check if module is used first. */
        if (IS_USED(MODULE_GCOAP_DTLS) &&
            (listener->tl_type != GCOAP_SOCKET_TYPE_UNDEF) &&
            !(listener->tl_type & tl_type)) {
            listener = listener->next;
            continue;
        }
        res = listener->request_matcher(listener, &resource, pdu);
        /* check next resource on mismatch */
        if (res == GCOAP_RESOURCE_NO_PATH) {
            listener = listener->next;
            continue;
        }
        /* found a resource, but methods do not match */
        else if (res == GCOAP_RESOURCE_WRONG_METHOD) {
            ret = GCOAP_RESOURCE_WRONG_METHOD;
            listener = listener->next;
            continue;
        }
        /* found a suitable resource */
        else if (res == GCOAP_RESOURCE_FOUND) {
            *resource_ptr = resource;
            *listener_ptr = listener;
            return GCOAP_RESOURCE_FOUND;
        }
        /* res is probably GCOAP_RESOURCE_ERROR or some other
         * unhandled error */
        else {
            return GCOAP_RESOURCE_ERROR;
        }
    }

    return ret;
}

/*
 * Finds the memo for an outstanding request within the _coap_state.open_reqs
 * array. Matches on remote endpoint and token.
 *
 * remote[in]     Remote endpoint to match
 * token[in]      Token to match
 * tkl[in]        Length of the token in bytes
 *
 * return         Registered request memo, or NULL if not found
 */
static gcoap_request_memo_t* _find_req_memo_by_token(const sock_udp_ep_t *remote,
                                                     const uint8_t *token, size_t tkl)
{
    for (int i = 0; i < CONFIG_GCOAP_REQ_WAITING_MAX; i++) {
        if (_coap_state.open_reqs[i].state == GCOAP_MEMO_UNUSED) {
            continue;
        }

        gcoap_request_memo_t *memo = &_coap_state.open_reqs[i];
        coap_hdr_t *hdr = gcoap_request_memo_get_hdr(memo);

        /* verbose debug to catch bugs with request/response matching */
#if SOCK_HAS_IPV4
        DEBUG("Seeking memo for remote=%s, tkn=0x%02x%02x%02x%02x%02x%02x%02x%02x, tkl=%"PRIuSIZE"\n",
              ipv4_addr_to_str(_ipv6_addr_str, (ipv4_addr_t *)&remote->addr.ipv4,
                               IPV6_ADDR_MAX_STR_LEN),
              token[0], token[1], token[2], token[3], token[4], token[5], token[6], token[7],
              tkl);
#else
        DEBUG("Seeking memo for remote=%s, tkn=0x%02x%02x%02x%02x%02x%02x%02x%02x, tkl=%"PRIuSIZE"\n",
              ipv6_addr_to_str(_ipv6_addr_str, (ipv6_addr_t *)&remote->addr.ipv6,
                               IPV6_ADDR_MAX_STR_LEN),
              token[0], token[1], token[2], token[3], token[4], token[5], token[6], token[7],
              tkl);
#endif

        size_t memo_tkl = coap_hdr_get_token_len(hdr);
        if (memo_tkl != tkl) {
            DEBUG("Token length mismatch %" PRIuSIZE "\n", memo_tkl);
            continue;
        }
        const uint8_t *memo_token = coap_hdr_get_token(hdr);
        if (memcmp(token, memo_token, tkl)) {
            DEBUG("Token mismatch 0x%02x%02x%02x%02x%02x%02x%02x%02x\n",
                  memo_token[0], memo_token[1], memo_token[2], memo_token[3],
                  memo_token[4], memo_token[5], memo_token[6], memo_token[7]);
            continue;
        }
        if (!sock_udp_ep_equal(&memo->remote_ep, remote)) {
            if (sock_udp_ep_is_multicast(&memo->remote_ep)) {
                DEBUG("matching multicast response\n");
            }
            else {
#if SOCK_HAS_IPV4
                DEBUG("Remote address mismatch %s\n",
                      ipv4_addr_to_str(_ipv6_addr_str, (ipv4_addr_t *)&memo->remote_ep.addr.ipv4,
                                       IPV6_ADDR_MAX_STR_LEN));
#else
                DEBUG("Remote address mismatch %s\n",
                      ipv6_addr_to_str(_ipv6_addr_str, (ipv6_addr_t *)&memo->remote_ep.addr.ipv6,
                                       IPV6_ADDR_MAX_STR_LEN));
#endif
                continue;
            }
        }
        return memo;
    }
    return NULL;
}

/*
 * Utility wrapper for _find_req_memo_by_token(), using the pdu token.
 * Finds the memo for an outstanding request within the _coap_state.open_reqs
 * array. Matches on remote endpoint and token of the pdu.
 *
 * src_pdu[in]    PDU which holds the token for matching
 * remote[in]     Remote endpoint to match
 *
 * return         Registered request memo, or NULL if not found
 */
static gcoap_request_memo_t* _find_req_memo_by_pdu_token(
                                                  const coap_pkt_t *src_pdu,
                                                  const sock_udp_ep_t *remote)
{
    unsigned tkl = coap_get_token_len(src_pdu);
    uint8_t *token = coap_get_token(src_pdu);
    return _find_req_memo_by_token(remote, token, tkl);
}

/*
 * Finds the memo for an outstanding request within the _coap_state.open_reqs
 * array. Matches on remote endpoint and message ID.
 *
 * remote[in]     Remote endpoint to match
 * mid[in]        Message ID to match
 *
 * return         Registered request memo, or NULL if not found
 */
static gcoap_request_memo_t* _find_req_memo_by_mid(const sock_udp_ep_t *remote, uint16_t mid)
{
    for (int i = 0; i < CONFIG_GCOAP_REQ_WAITING_MAX; i++) {
        if (_coap_state.open_reqs[i].state == GCOAP_MEMO_UNUSED) {
            continue;
        }

        gcoap_request_memo_t *memo = &_coap_state.open_reqs[i];

        if ((mid == gcoap_request_memo_get_hdr(memo)->id) &&
            sock_udp_ep_equal(&memo->remote_ep, remote)) {
            return memo;
        }
    }
    return NULL;
}

/* Calls handler callback on receipt of a timeout message. */
static void _expire_request(gcoap_request_memo_t *memo)
{
    DEBUG("coap: received timeout message\n");
    if ((memo->state == GCOAP_MEMO_RETRANSMIT) || (memo->state == GCOAP_MEMO_WAIT)) {
        memo->state = GCOAP_MEMO_TIMEOUT;
        /* Pass response to handler */
        if (memo->resp_handler) {
            coap_pkt_t req;
            memset(&req, 0, sizeof(req));
            /* 0 means there is an observe option value */
            coap_clear_observe(&req);
            req.hdr = gcoap_request_memo_get_hdr(memo);
            memo->resp_handler(memo, &req, NULL);
        }
        _memo_clear_resend_buffer(memo);
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
                                        coap_request_ctx_t *ctx)
{
    (void)ctx;

    gcoap_resp_init(pdu, buf, len, COAP_CODE_CONTENT);
    coap_opt_add_format(pdu, COAP_FORMAT_LINK);
    ssize_t plen = coap_opt_finish(pdu, COAP_OPT_FINISH_PAYLOAD);

    plen += gcoap_get_resource_list(pdu->payload, (size_t)pdu->payload_len,
                                       COAP_FORMAT_LINK,
                                       (gcoap_socket_type_t)coap_request_ctx_get_tl_type(ctx));
    return plen;
}

/*
 * Find registered observer or notification endpoint for a remote or local address and port.
 *
 * out[in,out] -- in: endpoint array to scan, out: found endpoint or NULL if not found
 * in[in] -- Endpoint to match
 *
 * return Index of empty slot, suitable for registering new endpoint; or -1
 *        if no empty slots. Undefined if observer found.
 */
static int _find_endpoint(sock_udp_ep_t **out, sock_udp_ep_t *in, unsigned max)
{
    int empty_slot = -1;
    sock_udp_ep_t *ep_array = *out;
    *out = NULL;
    for (unsigned i = 0; i < max; i++) {

        if (ep_array[i].family == AF_UNSPEC) {
            empty_slot = i;
        }
        else if (sock_udp_ep_equal(&ep_array[i], in)) {
            *out = &ep_array[i];
            break;
        }
    }
    return empty_slot;
}

static int _find_observer(sock_udp_ep_t **observer, sock_udp_ep_t *remote)
{
    *observer = _coap_state.observers;
    return _find_endpoint(observer, remote, CONFIG_GCOAP_OBS_CLIENTS_MAX);
}

static int _find_notifier(sock_udp_ep_t **notifier, sock_udp_ep_t *local)
{
    *notifier = _coap_state.notifiers;
    return _find_endpoint(notifier, local, CONFIG_GCOAP_OBS_NOTIFIERS_MAX);
}

/*
 * Find registered observe memo for a remote address and token.
 *
 * memo[out] -- Registered observe memo, or NULL if not found
 * remote[in] -- Remote endpoint for address to match if not NULL
 * local[in] -- Local endpoint for address to match if not NULL
 * pdu[in] -- PDU for token to match, or NULL to match only on remote address
 *
 * return Index of empty slot, suitable for registering new memo; or -1 if no
 *        empty slots. Undefined if memo found.
 */
static int _find_obs_memo(gcoap_observe_memo_t **memo,
                          sock_udp_ep_t *remote, sock_udp_ep_t *local,
                          coap_pkt_t *pdu)
{
    int empty_slot = -1;
    *memo          = NULL;

    sock_udp_ep_t *remote_observer = NULL;
    sock_udp_ep_t *local_notifier = NULL;
    if (remote) {
        _find_observer(&remote_observer, remote);
    }
    if (local) {
        _find_notifier(&local_notifier, local);
    }
    for (unsigned i = 0; i < CONFIG_GCOAP_OBS_REGISTRATIONS_MAX; i++) {
        if (_coap_state.observe_memos[i].observer == NULL) {
            empty_slot = i;
            continue;
        }

        if ((_coap_state.observe_memos[i].observer == remote_observer || !remote_observer) &&
            (_coap_state.observe_memos[i].notifier == local_notifier || !local_notifier)) {
            if (pdu == NULL) {
                *memo = &_coap_state.observe_memos[i];
                break;
            }
            unsigned memo_token_len = _coap_state.observe_memos[i].token_len;
            if (memo_token_len == coap_get_token_len(pdu)
                && memo_token_len
                && memcmp(&_coap_state.observe_memos[i].token[0],
                          coap_get_token(pdu),
                          memo_token_len) == 0) {
                *memo = &_coap_state.observe_memos[i];
                break;
            }
        }
    }
    return empty_slot;
}

/*
 * Checks if an observe memo exists for which a notification with the given
 * msg ID was sent out. If so, it expires the memo and frees up the
 * observer entry if needed.
 *
 * remote[in]             The remote to check for a stale observe memo.
 * last_notify_mid[in]    The message ID of the last notification send to the
 *                        given remote.
 */
static void _check_and_expire_obs_memo_last_mid(sock_udp_ep_t *remote,
                                                uint16_t last_notify_mid)
{
    /* find observer entry from remote */
    sock_udp_ep_t *observer;
    _find_observer(&observer, remote);

    if (observer) {
        gcoap_observe_memo_t *stale_obs_memo = NULL;
        /* get the observe memo corresponding to the notification with the
         * given msg ID. */
        for (unsigned i = 0; i < CONFIG_GCOAP_OBS_REGISTRATIONS_MAX; i++) {
            if (_coap_state.observe_memos[i].observer == NULL) {
                continue;
            }
            if ((_coap_state.observe_memos[i].observer == observer) &&
                (last_notify_mid == _coap_state.observe_memos[i].last_msgid)) {
                stale_obs_memo = &_coap_state.observe_memos[i];
                break;
            }
        }

        if (stale_obs_memo) {
            stale_obs_memo->observer = NULL; /* clear memo */
             /* check if no other memo is referencing the same local endpoint ...  */
            gcoap_observe_memo_t *other_memo = NULL;
            _find_obs_memo(&other_memo, NULL, stale_obs_memo->notifier, NULL);
            if (!other_memo) {
                /* ... if not -> also free the notifier entry */
                stale_obs_memo->notifier->family = AF_UNSPEC;
            }
            /* then unreference notifier */
            stale_obs_memo->notifier = NULL;

            /* check if the observer has more observe memos registered... */
            stale_obs_memo = NULL;
            _find_obs_memo(&stale_obs_memo, observer, NULL, NULL);
            if (stale_obs_memo == NULL) {
                /* ... if not -> also free the observer entry */
                observer->family = AF_UNSPEC;
            }
        }
    }
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
    for (int i = 0; i < CONFIG_GCOAP_OBS_REGISTRATIONS_MAX; i++) {
        if (_coap_state.observe_memos[i].observer != NULL
                && _coap_state.observe_memos[i].resource == resource) {
            *memo = &_coap_state.observe_memos[i];
            break;
        }
    }
}

/*
 * Transport layer functions
 */

static int _tl_init_coap_socket(gcoap_socket_t *sock, gcoap_socket_type_t type)
{
    switch (type) {
#if !IS_USED(MODULE_GCOAP_DTLS)
        case GCOAP_SOCKET_TYPE_UNDEF:
#endif
        case GCOAP_SOCKET_TYPE_UDP:
            sock->type = GCOAP_SOCKET_TYPE_UDP;
            sock->socket.udp = &_sock_udp;
            break;
#if IS_USED(MODULE_GCOAP_DTLS)
        case GCOAP_SOCKET_TYPE_UNDEF:
        case GCOAP_SOCKET_TYPE_DTLS:
            sock->type = GCOAP_SOCKET_TYPE_DTLS;
            sock->socket.dtls = &_sock_dtls;
            break;
#else
        default:
            return -1;
#endif
    }
    return 0;
}

static ssize_t _tl_send(gcoap_socket_t *sock, const void *data, size_t len,
                        const sock_udp_ep_t *remote, sock_udp_aux_tx_t *aux)
{
    ssize_t res = -1;
    switch (sock->type) {
        case GCOAP_SOCKET_TYPE_UDP:
            res = sock_udp_send_aux(sock->socket.udp, data, len, remote, aux);
            break;
#if IS_USED(MODULE_GCOAP_DTLS)
        case GCOAP_SOCKET_TYPE_DTLS:
            /* prepare session */
            sock_dtls_session_set_udp_ep(&sock->ctx_dtls_session, remote);
            dsm_state_t session_state = dsm_store(sock->socket.dtls,
                                                  &sock->ctx_dtls_session,
                                                  SESSION_STATE_HANDSHAKE,
                                                  true);
            if (session_state == NO_SPACE) {
                return -1;
            }

            /* send application data */
            res = sock_dtls_send(sock->socket.dtls, &sock->ctx_dtls_session, data, len,
                                    SOCK_NO_TIMEOUT);
            switch (res) {
            case -EHOSTUNREACH:
            case -ENOTCONN:
            case 0:
                DEBUG("gcoap: DTLS sock not connected or remote unreachable. "
                      "Destroying session.\n");
                dsm_remove(sock->socket.dtls, &sock->ctx_dtls_session);
                sock_dtls_session_destroy(sock->socket.dtls, &sock->ctx_dtls_session);
                break;
            default:
                /* Temporary error. Keeping the DTLS session */
                break;
            }
            break;
#endif
        default:
            DEBUG("gcoap: undefined socket type\n");
            break;
    }
    return res;
}

static ssize_t _tl_authenticate(gcoap_socket_t *sock, const sock_udp_ep_t *remote,
                                uint32_t timeout)
{
#if !IS_USED(MODULE_GCOAP_DTLS)
    (void)sock;
    (void)remote;
    (void)timeout;
    return 0;
#else
    int res;

    if (sock->type != GCOAP_SOCKET_TYPE_DTLS) {
        return 0;
    }
    /* prepare session */
    sock_dtls_session_set_udp_ep(&sock->ctx_dtls_session, remote);
    dsm_state_t session_state = dsm_store(sock->socket.dtls, &sock->ctx_dtls_session,
                                          SESSION_STATE_HANDSHAKE, true);
    if (session_state == SESSION_STATE_ESTABLISHED) {
        return 0;
    }
    if (session_state == NO_SPACE) {
        DEBUG("gcoap: no space in dsm\n");
        return -ENOTCONN;
    }

    /* start handshake */
    _auth_waiting_thread = thread_getpid();
    res = sock_dtls_session_init(sock->socket.dtls, remote, &sock->ctx_dtls_session);
    if (res == 0) {
        /* session already exists */
        _auth_waiting_thread = -1;
        return res;
    }

    msg_t msg;
    bool is_timed_out = false;
    do {
        uint32_t start = ztimer_now(ZTIMER_MSEC);
        res = ztimer_msg_receive_timeout(ZTIMER_MSEC, &msg, timeout);

        /* ensure whole timeout time for the case we receive other messages than
         * DTLS_EVENT_CONNECTED */
        if (timeout != SOCK_NO_TIMEOUT) {
            uint32_t diff = (ztimer_now(ZTIMER_MSEC) - start);
            timeout = (diff > timeout) ? 0: timeout - diff;
            is_timed_out = (res < 0) || (timeout == 0);
        }
    }
    while (!is_timed_out && (msg.type != DTLS_EVENT_CONNECTED));
    if (is_timed_out &&  (msg.type != DTLS_EVENT_CONNECTED)) {
        DEBUG("gcoap: authentication timed out\n");
        dsm_remove(sock->socket.dtls, &sock->ctx_dtls_session);
        sock_dtls_session_destroy(sock->socket.dtls, &sock->ctx_dtls_session);
        return -ENOTCONN;
    }
    return 0;
#endif
}

static nanocoap_cache_entry_t *_cache_lookup_memo(gcoap_request_memo_t *memo)
{
#if IS_USED(MODULE_NANOCOAP_CACHE)
    /* cache_key in memo is pre-processor guarded so we need to as well */
    return nanocoap_cache_key_lookup(memo->cache_key);
#else
    (void)memo;
    return NULL;
#endif
}

static void _cache_process(gcoap_request_memo_t *memo,
                           coap_pkt_t *pdu)
{
    if (!IS_USED(MODULE_NANOCOAP_CACHE)) {
        return;
    }
    coap_pkt_t req;

    req.hdr = gcoap_request_memo_get_hdr(memo);
    size_t pdu_len = pdu->payload_len +
        (pdu->payload - (uint8_t *)pdu->hdr);
#if IS_USED(MODULE_NANOCOAP_CACHE)
    nanocoap_cache_entry_t *ce;
    /* cache_key in memo is pre-processor guarded so we need to as well */
    if ((ce = nanocoap_cache_process(memo->cache_key, coap_get_code_raw(&req), pdu, pdu_len))) {
        ce->truncated = (memo->state == GCOAP_MEMO_RESP_TRUNC);
    }
#else
    (void)req;
    (void)pdu_len;
#endif
}

static ssize_t _cache_build_response(nanocoap_cache_entry_t *ce, coap_pkt_t *pdu,
                                     uint8_t *buf, size_t len)
{
    if (!IS_USED(MODULE_NANOCOAP_CACHE)) {
        return -ENOTSUP;
    }
    if (len < ce->response_len) {
        return -ENOBUFS;
    }
    /* Use the same code from the cached content. Use other header
     * fields from the incoming request */
    gcoap_resp_init(pdu, buf, len, ce->response_pkt.hdr->code);
    /* copy all options and possible payload from the cached response
     * to the new response */
    unsigned header_len_req = coap_get_total_hdr_len(pdu);
    unsigned header_len_cached = coap_get_total_hdr_len(&ce->response_pkt);
    unsigned opt_payload_len = ce->response_len - header_len_cached;

    /* copy all options and possible payload from the cached response
     * to the new response */
    memcpy((buf + header_len_req),
           (ce->response_buf + header_len_cached),
           opt_payload_len);
    /* parse into pdu including all options and payload pointers etc */
    coap_parse(pdu, buf, header_len_req + opt_payload_len);
    return ce->response_len;
}

static void _copy_hdr_from_req_memo(coap_pkt_t *pdu, gcoap_request_memo_t *memo)
{
    const coap_hdr_t *hdr = gcoap_request_memo_get_hdr(memo);
    size_t hdr_len = coap_hdr_len(hdr);
    memcpy(pdu->hdr, hdr, hdr_len);
}

static void _receive_from_cache_cb(void *ctx)
{
    if (!IS_USED(MODULE_NANOCOAP_CACHE)) {
        return;
    }

    gcoap_request_memo_t *memo = ctx;
    nanocoap_cache_entry_t *ce = NULL;

    if ((ce = _cache_lookup_memo(memo))) {
        if (memo->resp_handler) {
            /* copy header from request so gcoap_resp_init in _cache_build_response works correctly
             */
            coap_pkt_t pdu = { .hdr = (coap_hdr_t *)_listen_buf };
            _copy_hdr_from_req_memo(&pdu, memo);
            if (_cache_build_response(ce, &pdu, _listen_buf, sizeof(_listen_buf)) >= 0) {
                memo->state = (ce->truncated) ? GCOAP_MEMO_RESP_TRUNC : GCOAP_MEMO_RESP;
                memo->resp_handler(memo, &pdu, &memo->remote_ep);
                _memo_clear_resend_buffer(memo);
                memo->state = GCOAP_MEMO_UNUSED;
            }
        }
    }
    else {
        /* oops we somehow lost the cache entry */
        DEBUG("gcoap: cache entry was lost\n");
        if (memo->resp_handler) {
            memo->state = GCOAP_MEMO_ERR;
            memo->resp_handler(memo, NULL, &memo->remote_ep);
        }
    }
}

static void _update_memo_cache_key(gcoap_request_memo_t *memo, uint8_t *cache_key)
{
#if IS_USED(MODULE_NANOCOAP_CACHE)
    if (memo) {
        /* memo->cache_key is guarded by MODULE_NANOCOAP_CACHE, so preprocessor
         * magic is needed */
        memcpy(memo->cache_key, cache_key, CONFIG_NANOCOAP_CACHE_KEY_LENGTH);
    }
#else
    (void)memo;
    (void)cache_key;
#endif
}

static bool _cache_lookup(gcoap_request_memo_t *memo,
                          coap_pkt_t *pdu,
                          nanocoap_cache_entry_t **ce)
{
    if (IS_USED(MODULE_NANOCOAP_CACHE)) {
        uint8_t cache_key[SHA256_DIGEST_LENGTH];
        ztimer_now_t now = ztimer_now(ZTIMER_SEC);

        nanocoap_cache_key_generate(pdu, cache_key);
        *ce = nanocoap_cache_key_lookup(cache_key);

        _update_memo_cache_key(memo, cache_key);
        /* cache hit, methods are equal, and cache entry is not stale */
        if (*ce &&
            ((*ce)->request_method == coap_get_code_raw(pdu)) &&
            !nanocoap_cache_entry_is_stale(*ce, now)) {
            return true;
        }
    }

    return false;
}

static ssize_t _cache_check(const uint8_t *buf, size_t len,
                            gcoap_request_memo_t *memo,
                            bool *cache_hit)
{
    if (!IS_USED(MODULE_NANOCOAP_CACHE)) {
        return len;
    }
    coap_pkt_t req;
    nanocoap_cache_entry_t *ce = NULL;
    /* XXX cast to const might cause problems here :-/ */
    ssize_t res = coap_parse(&req, (uint8_t *)buf, len);

    if (res < 0) {
        DEBUG("gcoap: parse failure for cache lookup: %" PRIdSIZE "\n", res);
        return -EINVAL;
    }
    if (coap_get_code_class(&req) != COAP_CLASS_REQ) {
        /* Not a request so ignore, as gcoap_req_send might have been used with
         * its undocumented function to send a CON response from submodule */
        return len;
    }

    *cache_hit = _cache_lookup(memo, &req, &ce);

    if (!(*cache_hit) && (ce != NULL)) {
        /* Cache entry was found, but it is stale. Try to validate */
        uint8_t *resp_etag;
        /* Searching for more ETags might become necessary in the future */
        ssize_t resp_etag_len = coap_opt_get_opaque(&ce->response_pkt, COAP_OPT_ETAG, &resp_etag);

        /* ETag found, but don't act on illegal ETag size */
        if ((resp_etag_len > 0) && ((size_t)resp_etag_len <= COAP_ETAG_LENGTH_MAX)) {
            uint8_t *tmp_etag;
            ssize_t tmp_etag_len = coap_opt_get_opaque(&req, COAP_OPT_ETAG, &tmp_etag);
            if (tmp_etag_len >= resp_etag_len) {
                /* peak length without padding */
                size_t rem_len = (len - (tmp_etag + tmp_etag_len - buf));

                if ((tmp_etag < buf) || (tmp_etag > (buf + len)) ||
                    (rem_len > (len - ((tmp_etag + COAP_ETAG_LENGTH_MAX) - buf)))) {
                    DEBUG("gcoap: invalid calculated padding length (%lu) for ETag injection "
                          "during cache lookup.\n", (long unsigned)rem_len);
                    /* something fishy happened in the request. Better don't return cache entry */
                    *cache_hit = false;
#if IS_USED(MODULE_NANOCOAP_CACHE)
                    memset(memo->cache_key, 0, sizeof(memo->cache_key));
#endif
                    return -EINVAL;
                }
                memcpy(tmp_etag, resp_etag, resp_etag_len);
                /* shorten ETag option if necessary */
                if ((size_t)resp_etag_len < COAP_ETAG_LENGTH_MAX) {
                    /* now we need the start of the option (not its value) so dig once more */
                    uint8_t *start = coap_find_option(&req, COAP_OPT_ETAG);
                    /* option length must always be <= COAP_ETAG_LENGTH_MAX = 8 < 12, so the length
                     * is encoded in the first byte, see also RFC 7252, section 3.1 */
                    *start &= 0xf0;
                    /* first if around here should make sure we are <= 8 < 0xf, so we don't need to
                     * bitmask resp_etag_len */
                    *start |= (uint8_t)resp_etag_len;
                    /* remove padding */
                    memmove(tmp_etag + resp_etag_len, tmp_etag + COAP_ETAG_LENGTH_MAX, rem_len);
                    len -= (COAP_ETAG_LENGTH_MAX - resp_etag_len);
                }
            }
        }
        else {
            len = coap_opt_remove(&req, COAP_OPT_ETAG);
        }
    }
    else {
        len = coap_opt_remove(&req, COAP_OPT_ETAG);
    }
    return len;
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
                            0, _event_loop, NULL, "gcoap");

    mutex_init(&_coap_state.lock);
    /* Blank lists so we know if an entry is available. */
    memset(&_coap_state.open_reqs[0], 0, sizeof(_coap_state.open_reqs));
    memset(&_coap_state.observers[0], 0, sizeof(_coap_state.observers));
    memset(&_coap_state.observe_memos[0], 0, sizeof(_coap_state.observe_memos));
    memset(&_coap_state.resend_bufs[0], 0, sizeof(_coap_state.resend_bufs));
    /* randomize initial value */
    atomic_init(&_coap_state.next_message_id, (unsigned)random_uint32());

    if (IS_USED(MODULE_NANOCOAP_CACHE)) {
        nanocoap_cache_init();
    }
    /* initialize the forward proxy operation, if compiled */
    if (IS_ACTIVE(MODULE_GCOAP_FORWARD_PROXY)) {
        gcoap_forward_proxy_init();
    }

#ifdef MODULE_NANOCOAP_RESOURCES
    /* add CoAP resources from XFA */
    XFA_USE_CONST(coap_resource_t, coap_resources_xfa);
    static gcoap_listener_t _xfa_listener = {
        .resources = coap_resources_xfa,
    };
    _xfa_listener.resources_len = XFA_LEN(coap_resource_t, coap_resources_xfa),

    gcoap_register_listener(&_xfa_listener);
#endif

    return _pid;
}

uint16_t gcoap_next_msg_id(void)
{
    return (uint16_t)atomic_fetch_add(&_coap_state.next_message_id, 1);
}

void gcoap_register_listener(gcoap_listener_t *listener)
{
    /* That item will be overridden, ensure that the user expecting different
     * behavior will notice this. */
    assert(listener->next == NULL);

    listener->next = _coap_state.listeners;
    _coap_state.listeners = listener;

    if (!listener->link_encoder) {
        listener->link_encoder = gcoap_encode_link;
    }

    if (!listener->request_matcher) {
        listener->request_matcher = _request_matcher_default;
    }
}

const coap_resource_t *gcoap_get_resource_by_path_iterator(const gcoap_listener_t **last_listener,
                                                           const coap_resource_t *last_resource,
                                                           const char *uri_path)
{
    const gcoap_listener_t *listener = *last_listener ? *last_listener : _coap_state.listeners;
    const coap_resource_t *resource = last_resource;

    while (listener) {
        if ((resource = _match_resource_path_iterator(listener, resource, (const uint8_t *)uri_path))) {
            *last_listener = listener;
            return resource;
        }
        listener = listener->next;
    }

    return NULL;
}

int gcoap_req_init_path_buffer(coap_pkt_t *pdu, uint8_t *buf, size_t len,
                               unsigned code, const char *path, size_t path_len)
{
    assert((path == NULL) || (path[0] == '/'));

    pdu->hdr = (coap_hdr_t *)buf;

    /* generate token */
    uint16_t msgid = gcoap_next_msg_id();
    ssize_t res;
    if (code) {
#if CONFIG_GCOAP_TOKENLEN
        uint8_t token[CONFIG_GCOAP_TOKENLEN];
        for (size_t i = 0; i < CONFIG_GCOAP_TOKENLEN; i += 4) {
            uint32_t rand = random_uint32();
            memcpy(&token[i],
                   &rand,
                   (CONFIG_GCOAP_TOKENLEN - i >= 4) ? 4 : CONFIG_GCOAP_TOKENLEN - i);
        }
        res = coap_build_hdr(pdu->hdr, COAP_TYPE_NON, &token[0],
                             CONFIG_GCOAP_TOKENLEN, code, msgid);
#else
        res = coap_build_hdr(pdu->hdr, COAP_TYPE_NON, NULL,
                             CONFIG_GCOAP_TOKENLEN, code, msgid);
#endif
    }
    else {
        /* ping request */
        res = coap_build_hdr(pdu->hdr, COAP_TYPE_CON, NULL, 0, code, msgid);
    }

    coap_pkt_init(pdu, buf, len, res);
    if (IS_USED(MODULE_NANOCOAP_CACHE)) {
        static const uint8_t tmp[COAP_ETAG_LENGTH_MAX] = { 0 };
        /* add slack to maybe add an ETag on stale cache hit later */
        res = coap_opt_add_opaque(pdu, COAP_OPT_ETAG, tmp, sizeof(tmp));
    }
    if ((res > 0) && (path != NULL) && (path_len > 0)) {
        res = coap_opt_add_uri_path_buffer(pdu, path, path_len);
    }
    return (res > 0) ? 0 : res;
}

int gcoap_obs_req_forget(const sock_udp_ep_t *remote, const uint8_t *token,
                         size_t tokenlen) {
    int res = -ENOENT;
    gcoap_request_memo_t *obs_req_memo;
    mutex_lock(&_coap_state.lock);
    /* Find existing request memo of the observe */
    obs_req_memo = _find_req_memo_by_token(remote, token, tokenlen);
    if (obs_req_memo) {
        /* forget the existing observe memo. */
        obs_req_memo->state = GCOAP_MEMO_UNUSED;
        res = 0;
    }

    mutex_unlock(&_coap_state.lock);
    return res;
}

ssize_t gcoap_req_send(const uint8_t *buf, size_t len,
                       const sock_udp_ep_t *remote, const sock_udp_ep_t *local,
                       gcoap_resp_handler_t resp_handler, void *context,
                       gcoap_socket_type_t tl_type)
{
    gcoap_socket_t socket = { 0 };
    gcoap_request_memo_t *memo = NULL;
    unsigned msg_type  = (*buf & 0x30) >> 4;
    uint32_t timeout   = 0;
    ssize_t res = 0;
    bool cache_hit = false;

    assert(remote != NULL);

    res = _tl_init_coap_socket(&socket, tl_type);
    if (res < 0) {
        return -EINVAL;
    }
    /* Only allocate memory if necessary (i.e. if user is interested in the
     * response or request is confirmable) */
    if ((resp_handler != NULL) || (msg_type == COAP_TYPE_CON)) {
        mutex_lock(&_coap_state.lock);
        /* Find empty slot in list of open requests. */
        for (int i = 0; i < CONFIG_GCOAP_REQ_WAITING_MAX; i++) {
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
        memo->context = context;
        memcpy(&memo->remote_ep, remote, sizeof(sock_udp_ep_t));
        memo->socket = socket;

        if (IS_USED(MODULE_NANOCOAP_CACHE)) {
            ssize_t res = _cache_check(buf, len, memo, &cache_hit);

            if (res < 0) {
                DEBUG("gcoap: Error from cache check");
                memo->state = GCOAP_MEMO_UNUSED;
                mutex_unlock(&_coap_state.lock);
                return res;
            }
            len = res;
        }

        switch (msg_type) {
        case COAP_TYPE_CON:
            /* Can't store it for retransmission, even though sending it from
             * the provided buffer once is possible */
            if (len > CONFIG_GCOAP_PDU_BUF_SIZE) {
                DEBUG("gcoap: Request too large for retransmit buffer");
                memo->state = GCOAP_MEMO_UNUSED;
                mutex_unlock(&_coap_state.lock);
                return -EINVAL;
            }

            /* copy buf to resend_bufs record */
            memo->msg.data.pdu_buf = NULL;
            for (int i = 0; i < CONFIG_GCOAP_RESEND_BUFS_MAX; i++) {
                if (!_coap_state.resend_bufs[i][0]) {
                    memo->msg.data.pdu_buf = &_coap_state.resend_bufs[i][0];
                    memcpy(memo->msg.data.pdu_buf, buf,
                           len);
                    memo->msg.data.pdu_len = len;
                    break;
                }
            }
            if (memo->msg.data.pdu_buf) {
                memo->send_limit  = CONFIG_COAP_MAX_RETRANSMIT;
                timeout           = (uint32_t)CONFIG_COAP_ACK_TIMEOUT_MS;
#if CONFIG_COAP_RANDOM_FACTOR_1000 > 1000
                timeout = random_uint32_range(timeout, TIMEOUT_RANGE_END);
#endif
                memo->state = GCOAP_MEMO_RETRANSMIT;
            }
            else {
                memo->state = GCOAP_MEMO_UNUSED;
                DEBUG("gcoap: no space for PDU in resend bufs\n");
            }
            break;

        case COAP_TYPE_NON:
            memo->send_limit = GCOAP_SEND_LIMIT_NON;
            memcpy(&memo->msg.hdr_buf[0], buf, GCOAP_HEADER_MAXLEN);
            timeout = CONFIG_GCOAP_NON_TIMEOUT_MSEC;
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
        if (cache_hit) {
            /* post to receive cache entry */
            event_callback_init(&_receive_from_cache,
                                _receive_from_cache_cb,
                                memo);
            event_callback_post(&_queue, &_receive_from_cache);
            return len;
        }
    }
    /* check cache without memo */
    else if (IS_USED(MODULE_NANOCOAP_CACHE)) {
        ssize_t res = _cache_check(buf, len, NULL, &cache_hit);

        if (res < 0) {
            return res;
        }
        if (cache_hit > 0) {
            return res;
        }
    }

    _tl_init_coap_socket(&socket, tl_type);
    if (IS_USED(MODULE_GCOAP_DTLS) && socket.type == GCOAP_SOCKET_TYPE_DTLS) {
        res = _tl_authenticate(&socket, remote, CONFIG_GCOAP_DTLS_HANDSHAKE_TIMEOUT_MSEC);
    }

    /* set response timeout; may be zero for non-confirmable */
    if (memo != NULL && res == 0) {
        if (timeout > 0) {
            event_callback_init(&memo->resp_tmout_cb, _on_resp_timeout, memo);
            event_timeout_ztimer_init(&memo->resp_evt_tmout, ZTIMER_MSEC, &_queue,
                               &memo->resp_tmout_cb.super);
            event_timeout_set(&memo->resp_evt_tmout, timeout);
        }
        else {
            memset(&memo->resp_evt_tmout, 0, sizeof(event_timeout_t));
        }
    }

    if (res == 0) {
        sock_udp_aux_tx_t aux = { 0 };
        if (local) {
            memcpy(&aux.local, local, sizeof(sock_udp_ep_t));
            aux.flags = SOCK_AUX_SET_LOCAL;
        }
        res = _tl_send(&socket, buf, len, remote, &aux);
    }
    if (res <= 0) {
        if (memo != NULL) {
            _memo_clear_resend_buffer(memo);
            if (timeout > 0) {
                event_timeout_clear(&memo->resp_evt_tmout);
            }
            memo->state = GCOAP_MEMO_UNUSED;
    }
        DEBUG("gcoap: sock send failed: %" PRIdSIZE "\n", res);
    }
    return ((res > 0 || res == -ENOTCONN) ? res : 0);
}

static void _add_generated_observe_option(coap_pkt_t *pdu)
{
        /* generate initial notification value */
        uint32_t now       = ztimer_now(ZTIMER_MSEC);
        pdu->observe_value = (now >> GCOAP_OBS_TICK_EXPONENT) & 0xFFFFFF;
        coap_opt_add_uint(pdu, COAP_OPT_OBSERVE, pdu->observe_value);
}

int gcoap_resp_init(coap_pkt_t *pdu, uint8_t *buf, size_t len, unsigned code)
{
    int header_len = coap_build_reply(pdu, code, buf, len, 0);

    /* request contained no-response option or not enough space for response */
    if (header_len <= 0) {
        return -1;
    }

    pdu->hdr = (coap_hdr_t *)buf;
    pdu->options_len = 0;
    pdu->payload     = buf + header_len;
    pdu->payload_len = len - header_len;

    if (coap_get_observe(pdu) == COAP_OBS_REGISTER) {
        _add_generated_observe_option(pdu);
    }

    return 0;
}

int gcoap_separate_resp_init(coap_pkt_t *pdu, uint8_t *buf, size_t len, unsigned type, unsigned code)
{
    int header_len = coap_build_separate_reply(pdu, type, code, buf, len, 0);

    /* request contained no-response option or not enough space for response */
    if (header_len <= 0) {
        return -1;
    }

    pdu->hdr = (coap_hdr_t *)buf;
    pdu->hdr->id = gcoap_next_msg_id();
    pdu->options_len = 0;
    pdu->payload     = buf + header_len;
    pdu->payload_len = len - header_len;

    if (coap_get_observe(pdu) == COAP_OBS_REGISTER) {
        _add_generated_observe_option(pdu);
    }

    return 0;
}

int gcoap_obs_init(coap_pkt_t *pdu, uint8_t *buf, size_t len,
                                                  const coap_resource_t *resource)
{
    gcoap_observe_memo_t *memo = NULL;

    mutex_lock(&_coap_state.lock);
    _find_obs_memo_resource(&memo, resource);
    if (memo == NULL) {
        /* Unique return value to specify there is not an observer */
        mutex_unlock(&_coap_state.lock);
        return GCOAP_OBS_INIT_UNUSED;
    }

    pdu->hdr       = (coap_hdr_t *)buf;
    uint16_t msgid = gcoap_next_msg_id();
    ssize_t hdrlen = coap_build_hdr(pdu->hdr, COAP_TYPE_NON, &memo->token[0],
                                    memo->token_len, COAP_CODE_CONTENT, msgid);

    if (hdrlen <= 0) {
        /* reason for negative hdrlen is not defined, so we also are vague */
        mutex_unlock(&_coap_state.lock);
        return GCOAP_OBS_INIT_ERR;
    }

    coap_pkt_init(pdu, buf, len, hdrlen);

    _add_generated_observe_option(pdu);
    /* Store message ID of the last notification sent. This is needed
        * to match a potential RST returned by a client in order to signal
        * it does not recognize this notification. */
    memo->last_msgid = msgid;

    return GCOAP_OBS_INIT_OK;
}

size_t gcoap_obs_send(const uint8_t *buf, size_t len,
                      const coap_resource_t *resource)
{
    ssize_t ret = 0;
    gcoap_observe_memo_t *memo = NULL;
    _find_obs_memo_resource(&memo, resource);

    if (memo) {
        sock_udp_aux_tx_t aux = { 0 };
        if (memo->notifier) {
            memcpy(&aux.local, memo->notifier, sizeof(*memo->notifier));
            aux.flags = SOCK_AUX_SET_LOCAL;
        }
        ret = _tl_send(&memo->socket, buf, len, memo->observer, &aux);
    }
    mutex_unlock(&_coap_state.lock);
    return ret <= 0 ? 0 : (size_t)ret;
}

uint8_t gcoap_op_state(void)
{
    uint8_t count = 0;
    for (int i = 0; i < CONFIG_GCOAP_REQ_WAITING_MAX; i++) {
        if (_coap_state.open_reqs[i].state != GCOAP_MEMO_UNUSED) {
            count++;
        }
    }
    return count;
}

int gcoap_get_resource_list(void *buf, size_t maxlen, uint8_t cf,
                               gcoap_socket_type_t tl_type)
{
    assert(cf == COAP_FORMAT_LINK);

    gcoap_listener_t *listener = _coap_state.listeners;

    char *out = (char *)buf;
    size_t pos = 0;

    coap_link_encoder_ctx_t ctx;
    ctx.content_format = cf;
    /* indicate initial link for the list */
    ctx.flags = COAP_LINK_FLAG_INIT_RESLIST;

    /* write payload */
    for (; listener != NULL; listener = listener->next) {
        if (!listener->link_encoder) {
            continue;
        }
        /* only makes sense to check if non-UDP transports are supported,
         * so check if module is used first. */
        if (IS_USED(MODULE_GCOAP_DTLS) &&
            (tl_type != GCOAP_SOCKET_TYPE_UNDEF) &&
            (listener->tl_type != GCOAP_SOCKET_TYPE_UNDEF) &&
            ((listener->tl_type & GCOAP_SOCKET_TYPE_UDP) != (tl_type & GCOAP_SOCKET_TYPE_UDP)) &&
            ((listener->tl_type & GCOAP_SOCKET_TYPE_DTLS) != (tl_type & GCOAP_SOCKET_TYPE_DTLS))) {
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

#if IS_USED(MODULE_GCOAP_DTLS)
sock_dtls_t *gcoap_get_sock_dtls(void)
{
    return &_sock_dtls;
}
#endif

/*  */

void gcoap_forward_proxy_find_req_memo(gcoap_request_memo_t **memo_ptr,
                                       coap_pkt_t *src_pdu,
                                       const sock_udp_ep_t *remote)
{
    *memo_ptr = _find_req_memo_by_pdu_token(src_pdu, remote);
}

void gcoap_forward_proxy_post_event(void *arg)
{
    event_post(&_queue, arg);
}

/* separate response API */

int gcoap_resp_prepare_separate(gcoap_separate_response_ctx_t *ctx,
                                void *req_buf, size_t req_buf_size,
                                void *resp_buf, size_t resp_buf_size,
                                const coap_pkt_t *req_pkt,
                                const coap_request_ctx_t *req_ctx)
{
    size_t len = coap_get_total_len(req_pkt);
    if (req_buf_size < len) {
        return -ENOSPC;
    }
    ctx->tl = req_ctx->tl_type;
    ctx->req_buf = req_buf;
    ctx->req_buf_size = req_buf_size;
    ctx->resp_buf = resp_buf;
    ctx->resp_buf_size = resp_buf_size;
    memcpy(ctx->req_buf, req_pkt->hdr, len);
    memcpy(&ctx->remote, req_ctx->remote, sizeof(ctx->remote));
#ifdef MODULE_SOCK_AUX_LOCAL
    assert(req_ctx->local);
    memcpy(&ctx->local, req_ctx->local, sizeof(ctx->local));
#endif
    return 0;
}

ssize_t gcoap_resp_send_separate(const uint8_t *buf, size_t len,
                                 const sock_udp_ep_t *remote, const sock_udp_ep_t *local,
                                 gcoap_socket_type_t tl_type)
{
    /* mostly copied from gcoap_req_send() */
    gcoap_socket_t socket = { 0 };
    gcoap_request_memo_t *memo = NULL;
    unsigned msg_type  = (*buf & 0x30) >> 4;
    uint32_t timeout   = 0;

    assert(remote != NULL);

    ssize_t res = _tl_init_coap_socket(&socket, tl_type);
    if (res < 0) {
        return -1;
    }
    if (msg_type == COAP_TYPE_CON) {
        mutex_lock(&_coap_state.lock);
        /* Find empty slot in list of open requests. */
        for (int i = 0; i < CONFIG_GCOAP_REQ_WAITING_MAX; i++) {
            if (_coap_state.open_reqs[i].state == GCOAP_MEMO_UNUSED) {
                memo = &_coap_state.open_reqs[i];
                memo->state = GCOAP_MEMO_WAIT;
                break;
            }
        }
        if (!memo) {
            mutex_unlock(&_coap_state.lock);
            DEBUG("gcoap: dropping response; no space for response tracking\n");
            return 0;
        }
        memo->resp_handler = NULL;
        memo->context = NULL;
        memcpy(&memo->remote_ep, remote, sizeof(sock_udp_ep_t));
        memo->socket = socket;
        /* copy buf to resend_bufs record */
        memo->msg.data.pdu_buf = NULL;
        for (int i = 0; i < CONFIG_GCOAP_RESEND_BUFS_MAX; i++) {
            if (!_coap_state.resend_bufs[i][0]) {
                memo->msg.data.pdu_buf = &_coap_state.resend_bufs[i][0];
                memcpy(memo->msg.data.pdu_buf, buf, CONFIG_GCOAP_PDU_BUF_SIZE);
                memo->msg.data.pdu_len = len;
                break;
            }
        }
        if (memo->msg.data.pdu_buf) {
            memo->send_limit  = CONFIG_COAP_MAX_RETRANSMIT;
            timeout           = (uint32_t)CONFIG_COAP_ACK_TIMEOUT_MS;
#if CONFIG_COAP_RANDOM_FACTOR_1000 > 1000
            timeout = random_uint32_range(timeout, TIMEOUT_RANGE_END);
#endif
            memo->state = GCOAP_MEMO_RETRANSMIT;
        }
        else {
            memo->state = GCOAP_MEMO_UNUSED;
            DEBUG("gcoap: no space for PDU in resend bufs\n");
        }
        mutex_unlock(&_coap_state.lock);
    }

    _tl_init_coap_socket(&socket, tl_type);
    /* set response timeout; may be zero for non-confirmable */
    if (memo) {
        if (timeout > 0) {
            event_callback_init(&memo->resp_tmout_cb, _on_resp_timeout, memo);
            event_timeout_ztimer_init(&memo->resp_evt_tmout, ZTIMER_MSEC, &_queue,
                                      &memo->resp_tmout_cb.super);
            event_timeout_set(&memo->resp_evt_tmout, timeout);
        }
        else {
            memset(&memo->resp_evt_tmout, 0, sizeof(event_timeout_t));
        }
    }

    sock_udp_aux_tx_t aux = { 0 };
    if (local) {
        memcpy(&aux.local, local, sizeof(sock_udp_ep_t));
        aux.flags = SOCK_AUX_SET_LOCAL;
    }
    if ((res = _tl_send(&socket, buf, len, remote, &aux)) <= 0) {
        if (memo) {
            if (msg_type == COAP_TYPE_CON) {
                *memo->msg.data.pdu_buf = 0;    /* clear resend buffer */
            }
            if (timeout > 0) {
                event_timeout_clear(&memo->resp_evt_tmout);
            }
            memo->state = GCOAP_MEMO_UNUSED;
        }
        DEBUG("gcoap: sock send failed: %" PRIdSIZE "\n", res);
    }
    return res < 0 ? -1 : res;
}

/** @} */

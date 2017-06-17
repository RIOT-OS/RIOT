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

#ifdef MODULE_GNRC_DTLS
#include "net/dtls/gdtls.h"
#include "mutex.h"
#endif

#define ENABLE_DEBUG (0)
#include "debug.h"

/* Internal functions */
static void *_event_loop(void *arg);
#ifndef MODULE_GNRC_DTLS
static void _listen(sock_udp_t *sock);
#else
static void _listen(sock_udp_t *sock, dtls_context_t *dtls_context, sock_udp_ep_t *client_peer);
#endif
static ssize_t _well_known_core_handler(coap_pkt_t* pdu, uint8_t *buf, size_t len);
static ssize_t _write_options(coap_pkt_t *pdu, uint8_t *buf, size_t len);
static size_t _handle_req(coap_pkt_t *pdu, uint8_t *buf, size_t len,
                                                         sock_udp_ep_t *remote);
static ssize_t _finish_pdu(coap_pkt_t *pdu, uint8_t *buf, size_t len);
static void _expire_request(gcoap_request_memo_t *memo);
static void _find_req_memo(gcoap_request_memo_t **memo_ptr, coap_pkt_t *pdu,
                                                            uint8_t *buf, size_t len);
static void _find_resource(coap_pkt_t *pdu, coap_resource_t **resource_ptr,
                                            gcoap_listener_t **listener_ptr);
static void _find_obs_memo_resource(gcoap_observe_memo_t **memo,
                                            const coap_resource_t *resource);
static int _find_observer(sock_udp_ep_t **observer, sock_udp_ep_t *remote);
static int _find_obs_memo(gcoap_observe_memo_t **memo, sock_udp_ep_t *remote,
                                            coap_pkt_t *pdu);
#ifdef MODULE_GNRC_DTLS
static int _dtlsc_data_try_send(dtls_data_app_t *data,
                                dtls_context_t *dtls_context, session_t *dst);
int _read_from_peer(struct dtls_context_t *ctx,
                    session_t *session, uint8 *data, size_t len);
dtls_context_t *_dtlsc_create_channel(dtls_remote_peer_t *remote_peer,
                                      dtls_context_t *dtls_context,
                                      dtls_handler_t *dtls_cb_clnt,
                                      session_t *dst );
static void *_dtlsc_loop(void *arg);
int8_t  gcoap_end_dtls_session(void);
#endif

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

#ifdef MODULE_GNRC_DTLS

extern int8_t dtls_connected;

mutex_t gcoap_mutex = MUTEX_INIT ;

static kernel_pid_t _pid_dtlsc = KERNEL_PID_UNDEF;
static char _msg_stack_dtlsc[DTLS_STACKSIZE];


static int _dtlsc_data_try_send(dtls_data_app_t *data,
                                dtls_context_t *dtls_context,  session_t *dst)
{
    int app_data_buf = *data->buffer_size;

    mutex_lock(&gcoap_mutex); /* Racing condition? (With udp_sock_rcvd ) */
    while (app_data_buf > 0) {
        DEBUG("gcoap: send2 sending CoAP packet as DTLS App data record\n");
        app_data_buf = try_send(dtls_context, dst,
                                data->buffer, app_data_buf);
        if (app_data_buf == -1) {
            break;
        }
    }
    mutex_unlock(&gcoap_mutex);

    return app_data_buf;
}

/**
 * @brief Reception of a DTLS Applicaiton data record for GCOAP module.
 */
int _read_from_peer(struct dtls_context_t *ctx, session_t *session,
                    uint8 *data, size_t len)
{
    (void) ctx;
    (void) session;

    coap_pkt_t pdu;
    gcoap_request_memo_t *memo = NULL;

    dtls_remote_peer_t *remote_peer;
    remote_peer =  (dtls_remote_peer_t *) dtls_get_app_data(ctx);

    ssize_t res = coap_parse(&pdu, data, len);
    if (res < 0) {
        DEBUG("gcoap: parse failure: %d\n", res);
        /* If a response, can't clear memo, but it will timeout later. */
        return res;
    }

    /* incoming request */
    if (coap_get_code_class(&pdu) == COAP_CLASS_REQ) {
        size_t pdu_len = _handle_req(&pdu, data, len, remote_peer->remote);
        if (pdu_len > 0) {

            return dtls_write(ctx, session, data, len);
        }
    }
    /* incoming response */
    else {
        _find_req_memo(&memo, &pdu, data, len);
        if (memo) {
            xtimer_remove(&memo->response_timer);
            memo->resp_handler(memo->state, &pdu);
            memo->state = GCOAP_MEMO_UNUSED;
        }
    }
    return 0;

}

dtls_context_t *_dtlsc_create_channel(dtls_remote_peer_t *remote_peer,
                                      dtls_context_t *dtls_context,
                                      dtls_handler_t *dtls_cb_clnt,
                                      session_t *dst )
{

    msg_t mbox_msg;

    /* NOTE: This avoid conflicts between both sides of (the internal) DTLS  */
    remote_peer->local->port = (unsigned short)  remote_peer->remote->port + 1;

    ssize_t res = sock_udp_create(remote_peer->sock, remote_peer->local, NULL, 0);
    if (res != 0) {
        DEBUG("dtlsc: Cannot create DTLS client sock: %d\n", res);
        return NULL;
    }

    dst->size = sizeof(uint8_t) * 16 + sizeof(unsigned short);
    dst->port = remote_peer->remote->port;
    if (memcpy(dst->addr.u8, remote_peer->remote->addr.ipv6, 16) == NULL) {
        DEBUG("dtlsc: Unable to prepare context\n");
        return NULL;
    }

    /* Stopping (temporary) the DTLS server side */
    mbox_msg.type = DTLS_MSG_SERVER_STOP;
    mbox_msg.content.ptr = NULL;
    msg_send_receive(&mbox_msg, &mbox_msg, _pid );

    dtls_context = dtls_new_context(remote_peer);
    if (!dtls_context) {
        dtls_crit("dtlsc: Unable to create DTLS context!\n");
        return NULL;
    }

    dtls_set_handler(dtls_context, dtls_cb_clnt);

    mbox_msg.type = DTLS_MSG_CLIENT_START;
    mbox_msg.content.ptr = dtls_context;
    msg_send_receive(&mbox_msg, &mbox_msg, _pid );

    /* We start the DTLS connection */
    if (dtls_connect(dtls_context, dst) < 0) { /* The first dtls flight */
        dtls_crit("dtlsc: Unable to start a DTLS channel!\n");
        return NULL;
    }

    mbox_msg.type = DTLS_MSG_CLIENT_HNDSHK;
    mbox_msg.content.ptr = NULL;

    /* We listen the medium for other DTLS flights */
    int8_t watchdog = GDTLS_MAX_RETRANSMISIONS;
    while (!dtls_connected && (watchdog > 0)) { /* Preparing DTLS channel */

        DEBUG("dtlsc: handshake stage (watchdog %i)\n", watchdog);
        msg_send_receive(&mbox_msg, &mbox_msg, _pid);
        watchdog--;

        /* TODO Try to restart the connection? */
        //dtls_connect(dtls_context, dst); /*Try to re-connect? */

    }  /* WHILE */

    if (!dtls_connected) {
        return NULL;
    }

    return dtls_context;
}

/* Create, preserve and close the DTLS session for the client side */
static void *_dtlsc_loop(void *arg)
{

    /* FIXME Make this more dynamic (more than one client instance) */
    static dtls_context_t *dtls_context = NULL;
    dtls_data_app_t *data;

    msg_t msg_rcvd, msg_snd, msg_queue[GCOAP_MSG_QUEUE_SIZE];

    /*The server and client side uses different remotes_peer */
    /*FIXME Probably not the best solution in terms of resources */
    static dtls_remote_peer_t remote_peer = DTLS_REMOTE_PEER_NULL;
    static sock_udp_ep_t local = SOCK_IPV6_EP_ANY;
    sock_udp_t sock;
    static session_t dtlsc_session;  /* TODO: One by request? */

    static dtls_handler_t dtls_cb_clnt = {
        .write = _send_to_peer,
        .read = _read_from_peer,
        .event = _client_events,
        #ifdef DTLS_PSK
        .get_psk_info = _client_peer_get_psk_info,
        #endif  /* DTLS_PSK */
        #ifdef DTLS_ECC
        .get_ecdsa_key = _peer_get_ecdsa_key,
        .verify_ecdsa_key = _peer_verify_ecdsa_key
        #endif  /* DTLS_ECC */
    };

    msg_init_queue(msg_queue, GCOAP_MSG_QUEUE_SIZE);

    (void) arg;

    while (1) {
        int res = msg_receive(&msg_rcvd);
        (void) res;
        switch (msg_rcvd.type) {
            case DTLS_MSG_CLIENT_START_CHANNEL:
                remote_peer.remote = (sock_udp_ep_t *) msg_rcvd.content.ptr;
                remote_peer.sock = &sock;
                remote_peer.local = &local;
                msg_snd.type = DTLS_MSG_CLIENT_ANSWER; /* USELESS? */

                /* NOTE: Risk of race condition here? */
                mutex_lock(&gcoap_mutex);
                if (dtls_connected == 0) {
                    dtls_context = _dtlsc_create_channel(&remote_peer, dtls_context,
                                                         &dtls_cb_clnt, &dtlsc_session);
                    msg_snd.content.ptr = dtls_context;
                }
                else {   /*There is a context ready! */
                    msg_snd.content.ptr = dtls_context;
                }
                mutex_unlock(&gcoap_mutex);
                msg_reply(&msg_rcvd, &msg_snd); /* TODO Damage control? */
                break;

            case DTLS_MSG_CLIENT_STOP_CHANNEL:
                msg_snd.content.value = !dtls_connected;
                if (dtls_connected == 1) {
                    DEBUG("Releaseing DTLS context..\n");

                    sock_udp_ep_t remote = SOCK_IPV6_EP_ANY;
                    remote_peer.remote = &remote;

                    memcpy(remote_peer.remote->addr.ipv6, &dtlsc_session.addr, 16);
                    remote_peer.remote->port = dtlsc_session.port;

                    dtls_free_context(dtls_context);

                    msg_snd.type = DTLS_MSG_SERVER_RESTART;
                    msg_send_receive(&msg_snd, &msg_snd, _pid );

                    sock_udp_close(remote_peer.sock);

                    dtls_connected = 0;
                }
                msg_reply(&msg_rcvd, &msg_rcvd);
                break;

            case DTLS_MSG_CLIENT_SEND_DATA:
                data = (dtls_data_app_t *) msg_rcvd.content.ptr;

                sock_udp_ep_t remote = SOCK_IPV6_EP_ANY;
                remote_peer.remote = &remote;

                memcpy(remote_peer.remote->addr.ipv6, &dtlsc_session.addr, 16);
                remote_peer.remote->port = dtlsc_session.port;

                _dtlsc_data_try_send(data, dtls_context, &dtlsc_session);

                msg_reply(&msg_rcvd, &msg_rcvd);
                break;

            /* TODO renegotiation case */

            default:
                DEBUG("dtlsc: Request not supported: %i", res);
                break;
        }   /* END switch*/
    }       /*END while */


    return 0;
}

int8_t gcoap_end_dtls_session(void)
{

    /* Actually, this should not happen*/
    if (_pid_dtlsc == KERNEL_PID_UNDEF) {
        DEBUG("DTLS client thread not running!\n");
        return -1;
    }

    msg_t mbox_msg, msg_answ;
    mbox_msg.type = DTLS_MSG_CLIENT_STOP_CHANNEL;
    msg_send_receive(&mbox_msg, &msg_answ, _pid_dtlsc);

    if (msg_answ.content.value) {
        DEBUG("DTLSC stoppded!\n");
    }
    else {
        DEBUG("A DTLS client side is not started yet!\n");
    }
    return 0;
}

#endif


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

#ifdef MODULE_GNRC_DTLS

    dtls_remote_peer_t *remote_aux;
    dtls_remote_peer_t remote_peer = DTLS_REMOTE_PEER_NULL;
    dtls_context_t *dtls_context_srvr = NULL;
    dtls_context_t *dtls_context_clnt = NULL;

    remote_peer.sock = &_sock;
    remote_peer.local = &local;
    uint8_t flag =  DTLS_FLAGS_SERVER_LISTEN;

    static dtls_handler_t dtls_cb_srvr = {
        .write = _send_to_peer,
        .read = _read_from_peer,
        .event = NULL,
      #ifdef DTLS_PSK
        .get_psk_info = _server_peer_get_psk_info, /*DANGER WE have a big problem here! */
      #endif  /* DTLS_PSK */
      #ifdef DTLS_ECC
        .get_ecdsa_key = _peer_get_ecdsa_key,
        .verify_ecdsa_key = _peer_verify_ecdsa_key
      #endif  /* DTLS_ECC */
    };

    dtls_context_srvr = dtls_new_context(&remote_peer);
    if (dtls_context_srvr) {
        dtls_set_handler(dtls_context_srvr, &dtls_cb_srvr);
    }
    else {
        DEBUG("gcoap: DTLS context not loaded\n");
        return 0;
    }

#endif /* MODULE_GNRC_DTLS  */

#ifndef MODULE_GNRC_DTLS
    int8_t res = sock_udp_create(&_sock, &local, NULL, 0);
#else
    int8_t res = sock_udp_create(remote_peer.sock, remote_peer.local, NULL, 0);
#endif

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
#ifdef MODULE_GNRC_DTLS
                case DTLS_MSG_CLIENT_START:
                    dtls_context_clnt = (dtls_context_t *) msg_rcvd.content.ptr;
#if ENABLE_DEBUG == 1
                    if (!dtls_context_clnt) {
                        DEBUG("Client context not passed!\n");
                    }
                    else {
                        DEBUG("DTLS now listen to client side only!\n");
                    }
#endif
                    remote_aux =  (dtls_remote_peer_t *) dtls_get_app_data(dtls_context_clnt);
                    msg_reply(&msg_rcvd, &msg_rcvd);
                    break;

                case DTLS_MSG_SERVER_STOP:
                    // dtls_free_context(dtls_context_srvr);
                    // dtls_context_srvr = NULL;
                    flag = DTLS_FLAGS_CLIENT_LISTEN_ONLY;
                    DEBUG("DTLS Server temporary disabled!\n");
                    msg_reply(&msg_rcvd, &msg_rcvd);
                    break;

                case DTLS_MSG_SERVER_RESTART:
                    flag = DTLS_FLAGS_SERVER_LISTEN;
                    dtls_context_clnt = NULL;
                    DEBUG("DTLS Server listening!\n");
                    msg_reply(&msg_rcvd, &msg_rcvd);
                    xtimer_usleep(500U);
                    break;

                case DTLS_MSG_CLIENT_HNDSHK: /* Nothing to do */
#endif
                default:
                    break;
            }
        }

#ifndef MODULE_GNRC_DTLS
        _listen(&_sock);
#else

        if ((flag == DTLS_FLAGS_SERVER_LISTEN) && !dtls_context_clnt) {
            remote_aux =  &remote_peer;
            _listen(remote_aux->sock, dtls_context_srvr, NULL);
        }
        else if ((flag == DTLS_FLAGS_CLIENT_LISTEN_ONLY) &&  dtls_context_clnt) {

            remote_aux =  (dtls_remote_peer_t *) dtls_get_app_data(dtls_context_clnt);
            _listen(remote_aux->sock, dtls_context_clnt, remote_aux->remote );

            if (msg_rcvd.type == DTLS_MSG_CLIENT_HNDSHK) {
                msg_reply(&msg_rcvd, &msg_rcvd);
            }
        }
        else {
            /* This is a safety check, should only happens once time */
            xtimer_usleep(500U);
        }

#endif
    }

    return 0;
}

/* Listen for an incoming CoAP message. */
#ifndef MODULE_GNRC_DTLS
static void _listen(sock_udp_t *sock)
#else
static void _listen(sock_udp_t *sock, dtls_context_t *dtls_context, sock_udp_ep_t *client_peer)
#endif
{

    uint8_t buf[GCOAP_PDU_BUF_SIZE];
    sock_udp_ep_t remote;



#ifndef MODULE_GNRC_DTLS
    coap_pkt_t pdu;
    gcoap_request_memo_t *memo = NULL;
    uint8_t open_reqs = gcoap_op_state();
    ssize_t res = sock_udp_recv(sock, buf, sizeof(buf),
                                open_reqs > 0 ? GCOAP_RECV_TIMEOUT : SOCK_NO_TIMEOUT,
                                &remote);
#else
    assert(dtls_context != NULL);
    ssize_t res = sock_udp_recv(sock, buf, sizeof(buf),
                                3000000U, /* */
                                &remote);
#endif
    if (res <= 0) {
#if ENABLE_DEBUG
        if (res < 0 && res != -ETIMEDOUT) {
            DEBUG("gcoap: udp recv failure: %d\n", res);
        }
#endif
        return;
    }

#ifndef MODULE_GNRC_DTLS
    res = coap_parse(&pdu, buf, res);
    if (res < 0) {
        DEBUG("gcoap: parse failure: %d\n", res);
        /* If a response, can't clear memo, but it will timeout later. */
        return;
    }

    /* incoming request */
    if (coap_get_code_class(&pdu) == COAP_CLASS_REQ) {
        size_t pdu_len = _handle_req(&pdu, buf, sizeof(buf), &remote);
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
#else /* MODULE_GNRC_DTLS */

    /* The request is handled now by DTLS */
    dtls_remote_peer_t *remote_peer =  (dtls_remote_peer_t *) dtls_get_app_data(dtls_context);

    if (!client_peer) {
        remote_peer->remote = &remote;
    }
    /* NOTE: DTLS records are of dynamic sizes */
    dtls_handle_read_sock(dtls_context, buf, res );

#endif

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
    coap_resource_t *resource;
    gcoap_listener_t *listener;
    sock_udp_ep_t *observer    = NULL;
    gcoap_observe_memo_t *memo = NULL;
    gcoap_observe_memo_t *resource_memo = NULL;

    _find_resource(pdu, &resource, &listener);
    if (resource == NULL) {
        return gcoap_response(pdu, buf, len, COAP_CODE_PATH_NOT_FOUND);
    }
    else {
        /* used below to ensure a memo not already recorded for the resource */
        _find_obs_memo_resource(&resource_memo, resource);
    }

    if (coap_get_observe(pdu) == COAP_OBS_REGISTER) {
        int empty_slot = _find_obs_memo(&memo, remote, pdu);
        /* record observe memo */
        if (memo == NULL) {
            if (empty_slot >= 0 && resource_memo == NULL) {

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
                }
            }
            if (memo == NULL) {
                coap_clear_observe(pdu);
                DEBUG("gcoap: can't register observe memo\n");
            }
        }
        if (memo != NULL) {
            memo->observer  = observer;
            memo->resource  = resource;
            memo->token_len = coap_get_token_len(pdu);
            if (memo->token_len) {
                memcpy(&memo->token[0], pdu->token, memo->token_len);
            }
            DEBUG("gcoap: Registered observer for: %s\n", memo->resource->path);
            /* generate initial notification value */
            uint32_t now       = xtimer_now_usec();
            pdu->observe_value = (now >> GCOAP_OBS_TICK_EXPONENT) & 0xFFFFFF;
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

    ssize_t pdu_len = resource->handler(pdu, buf, len);
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
 */
static void _find_resource(coap_pkt_t *pdu, coap_resource_t **resource_ptr,
                                            gcoap_listener_t **listener_ptr)
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
                *resource_ptr = resource;
                *listener_ptr = listener;
                return;
            }
        }
        listener = listener->next;
    }
    /* resource not found */
    *resource_ptr = NULL;
    *listener_ptr = NULL;
}

/*
 * Finishes handling a PDU -- write options and reposition payload.
 *
 * Returns the size of the PDU within the buffer, or < 0 on error.
 */
static ssize_t _finish_pdu(coap_pkt_t *pdu, uint8_t *buf, size_t len)
{
    ssize_t hdr_len = _write_options(pdu, buf, len);
    DEBUG("gcoap: header length: %i\n", (int)hdr_len);

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

    /* Observe for notification or registration response */
    if (coap_get_code_class(pdu) == COAP_CLASS_SUCCESS && coap_has_observe(pdu)) {
        uint32_t nval  = htonl(pdu->observe_value);
        uint8_t *nbyte = (uint8_t *)&nval;
        unsigned i;
        /* find address of non-zero MSB; max 3 bytes */
        for (i = 1; i < 4; i++) {
            if (*(nbyte+i) > 0) {
                break;
            }
        }
        bufpos += coap_put_option(bufpos, last_optnum, COAP_OPT_OBSERVE,
                                                       nbyte+i, 4-i);
        last_optnum = COAP_OPT_OBSERVE;
    }

    /* Uri-Path for request */
    if (coap_get_code_class(pdu) == COAP_CLASS_REQ) {
        size_t url_len = strlen((char *)pdu->url);
        if (url_len) {
            if (pdu->url[0] != '/') {
                DEBUG("gcoap: _write_options: path does not start with '/'\n");
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
        unsigned cmplen = 0;

        if (_coap_state.observers[i].family == AF_UNSPEC) {
            cmplen     = 0;
            empty_slot = i;
        }
        else if (_coap_state.observers[i].family == AF_INET6) {
            cmplen = 16;
        }
        else {
            cmplen = 4;
        }
        if (cmplen &&
                memcmp(&_coap_state.observers[i].addr.ipv6[0], &remote->addr.ipv6[0],
                                                               cmplen) == 0
                && _coap_state.observers[i].port == remote->port) {

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

    /* Blank lists so we know if an entry is available. */
    memset(&_coap_state.open_reqs[0], 0, sizeof(_coap_state.open_reqs));
    memset(&_coap_state.observers[0], 0, sizeof(_coap_state.observers));
    memset(&_coap_state.observe_memos[0], 0, sizeof(_coap_state.observe_memos));
    /* randomize initial value */
    _coap_state.last_message_id = random_uint32() & 0xFFFF;

#ifdef MODULE_GNRC_DTLS
    if (_pid_dtlsc != KERNEL_PID_UNDEF) {
        /*TODO: What to do here? (Should not happen) */
        return -EEXIST;
    }

    _pid_dtlsc = thread_create(_msg_stack_dtlsc, sizeof(_msg_stack_dtlsc), THREAD_PRIORITY_MAIN - 1,
                               THREAD_CREATE_STACKTEST, _dtlsc_loop, NULL, "dtlsc");
    dtls_connected = 0;
#endif

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
    _last->next = listener;
}

int gcoap_req_init(coap_pkt_t *pdu, uint8_t *buf, size_t len, unsigned code,
                                                              char *path) {
    ssize_t hdrlen;
    (void)len;

    pdu->hdr = (coap_hdr_t *)buf;
    memset(pdu->url, 0, NANOCOAP_URL_MAX);

    /* generate token */
#if GCOAP_TOKENLEN
    uint8_t token[GCOAP_TOKENLEN];
    for (size_t i = 0; i < GCOAP_TOKENLEN; i += 4) {
        uint32_t rand = random_uint32();
        memcpy(&token[i],
               &rand,
               (GCOAP_TOKENLEN - i >= 4) ? 4 : GCOAP_TOKENLEN - i);
    }
    hdrlen = coap_build_hdr(pdu->hdr, COAP_TYPE_NON, &token[0], GCOAP_TOKENLEN,
                            code, ++_coap_state.last_message_id);
#else
    hdrlen = coap_build_hdr(pdu->hdr, COAP_TYPE_NON, NULL, GCOAP_TOKENLEN,
                            code, ++_coap_state.last_message_id);
#endif

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

size_t gcoap_req_send(const uint8_t *buf, size_t len, const ipv6_addr_t *addr,
                      uint16_t port, gcoap_resp_handler_t resp_handler)
{
    sock_udp_ep_t remote;

    remote.family = AF_INET6;
    remote.netif  = SOCK_ADDR_ANY_NETIF;
    remote.port   = port;

    memcpy(&remote.addr.ipv6[0], &addr->u8[0], sizeof(addr->u8));

    return gcoap_req_send2(buf, len, &remote, resp_handler);
}

size_t gcoap_req_send2(const uint8_t *buf, size_t len,
                       const sock_udp_ep_t *remote,
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

#ifdef MODULE_GNRC_DTLS

    /*First, DTLS session must be established */
    msg_t mbox_msg, msg_answ; /* Universal, used for the mailboxes and IPC-s msgs */

    /*Retrieve the DTLS channel (or create it) */
    mbox_msg.type = DTLS_MSG_CLIENT_START_CHANNEL;
    mbox_msg.content.ptr = (sock_udp_ep_t *) remote;
    msg_send_receive(&mbox_msg, &msg_answ, _pid_dtlsc);

    if (msg_answ.content.ptr == NULL) {
        DEBUG("send2: Unable to process without DTLS\n");
        return 0;
    }

    if (memo) {

        memcpy(&memo->hdr_buf[0], buf, GCOAP_HEADER_MAXLEN);
        memo->resp_handler = resp_handler;


        if (len > DTLS_MAX_BUF) {
            DEBUG("Payload to send Exceeded maximum size!\n");
            return 0;
        }

        /*We send the CoAP message by means of DTLS */
        dtls_data_app_t data_app = DTLS_DATA_APP_NULL;
        data_app.buffer = (uint8_t *) buf;
        data_app.buffer_size = &len;

        mbox_msg.type = DTLS_MSG_CLIENT_SEND_DATA;
        mbox_msg.content.ptr = &data_app;
        msg_send_receive(&mbox_msg, &mbox_msg, _pid_dtlsc);


        if (/*(app_data_buf >= 0) &&*/ (GCOAP_NON_TIMEOUT > 0)) {
            /* interrupt sock listening (to set a listen timeout) */
            msg_t mbox_msg;
            mbox_msg.type = GCOAP_MSG_TYPE_INTR;
            mbox_msg.content.value = 0;
            if (mbox_try_put(&_sock.reg.mbox, &mbox_msg)) {
                /* start response wait timer */
                memo->timeout_msg.type = GCOAP_MSG_TYPE_TIMEOUT;
                memo->timeout_msg.content.ptr = (char *)memo;
                xtimer_set_msg(&memo->response_timer, GCOAP_NON_TIMEOUT,
                               &memo->timeout_msg, _pid);
            }
            else {
                memo->state = GCOAP_MEMO_UNUSED;
                DEBUG("gcoap: can't wake up mbox; no timeout for msg\n");
                return 0;
            }
        }/* IF GCOAP_NON_TIMEOUT */
    }/* IF - memo */

    return 1;

#else
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


#endif /* MODULE_GNRC_DTLS */
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

int gcoap_obs_init(coap_pkt_t *pdu, uint8_t *buf, size_t len,
                                                  const coap_resource_t *resource)
{
    ssize_t hdrlen;
    gcoap_observe_memo_t *memo = NULL;

    _find_obs_memo_resource(&memo, resource);
    if (memo == NULL) {
        /* Unique return value to specify there is not an observer */
        return GCOAP_OBS_INIT_UNUSED;
    }

    pdu->hdr = (coap_hdr_t *)buf;
    hdrlen   = coap_build_hdr(pdu->hdr, COAP_TYPE_NON, &memo->token[0],
                              memo->token_len, COAP_CODE_CONTENT,
                              ++_coap_state.last_message_id);
    if (hdrlen > 0) {
        uint32_t now       = xtimer_now_usec();
        pdu->observe_value = (now >> GCOAP_OBS_TICK_EXPONENT) & 0xFFFFFF;

        /* Reserve some space between the header and payload to write options later */
        pdu->payload       = buf + coap_get_total_hdr_len(pdu) + GCOAP_OBS_OPTIONS_BUF;
        /* Payload length really zero at this point, but we set this to the available
         * length in the buffer. Allows us to reconstruct buffer length later. */
        pdu->payload_len   = len - (pdu->payload - buf);
        pdu->content_type  = COAP_FORMAT_NONE;

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
        return sock_udp_send(&_sock, buf, len, memo->observer);
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

/** @} */

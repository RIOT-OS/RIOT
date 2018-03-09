/*
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_tlsman  TLSMAN
 * @ingroup     net
 * @brief       High-level interface to (D)TLS stack Manager
 *
 * @{
 *
 * @file
 * @brief       tlsman handlers for tinyDTLS
 *
 * @author      Raul Fuentes <raul.fuentes-samaniego@inria.fr>
 *
 * @}
 */
#ifndef NET_TLSMAN_TINYDTLS_H
#define NET_TLSMAN_TINYDTLS_H

#include "dtls.h"
#include "dtls_debug.h"

#if WITH_RIOT_GNRC
#include "net/sock/udp.h"
#endif /* WITH_RIOT_GNRC */

#if WITH_RIOT_SOCKETS
#error "Not tested yet!"
#endif /* WITH_RIOT_SOCKETS */

#ifdef __cplusplus
extern "C" {
#endif

/* NOTE DISCUSSION: This should not be at tlsman.h? (but circular reference?)*/
/**
 * @brief   Handler function for the reception of a DTLS data app record.
 *
 */
typedef void (*tlsman_resp_handler_t)(uint8_t *data, size_t data_size, void *tlsman_session);

/**
 * @brief The DTLS handshake states
 */
enum {
    TINYDTLS_EVENTS_ZERO = 0x00,    /**< Not DTLS handshake has been runned */
    TINYDTLS_EVENT_START,           /**< Firts DTLS Client Hello sent */
    TINYDTLS_EVENT_FINISHED,        /**< DTLS Handshake finished */
    TINYDTLS_EVENT_RENEGOTIATE      /**< Signals that a new handshake must take place */
};

/**
 *  @brief The state of the handshake process
 *
 * Those determine if a new DTLS record was received in middle of a DTLS Handhsake
 * and if it's requries to restart the handhsake.
 */
enum {
    TINYDTLS_HANDSHAKE_NON_RECORD = 0x00,   /**< Non DTLS record was received */
    TINYDTLS_HANDSHAKE_NEW_RECORD,          /**< A valid handshake record was received */
    TINYDTLS_HANDSHAKE_RESTART_SVP,         /**< Call of dtls_connect() is required */
    TINYDTLS_HANDSHAKE_IS_FATAL             /**< TODO */
};

/**
 * @brief tinyDTLS metadata for handling a secure session between peers
 */
typedef struct {
    uint8_t is_connected;               /**< Status of the DTLS channel >*/
    bool is_data_app;                   /**< Status of the reception of DTLS Data APpp records> */
    session_t dst;                      /**< The secure session */
    dtls_context_t *context;            /**< The DTLS Context for the secure session */
    dtls_context_t *cache;              /**< A temporary cached context required for renegotiation*/
    tlsman_resp_handler_t resp_handler; /**< Response callback */

#if WITH_RIOT_SOCKETS
    /* TODO */
#endif /* WITH_RIOT_SOCKETS */

#if WITH_RIOT_GNRC
    /* FIXME Be able to recover local and/or remote from sock */
    sock_udp_t *sock;       /**< The Sock used for establishing the Secure session */
    sock_udp_ep_t *local;   /**< The local peer of the sock */
    sock_udp_ep_t *remote;  /**< The remote peer of the sock */

#ifdef SOCK_HAS_ASYNC
    event_queue_t *sock_event_queue; /**< Event queue for asynhcronous sock */
#endif /* SOCK_HAS_ASYNC */

#endif /* WITH_RIOT_GNRC */
} tinydtls_session_t;

/**
 * @brief Determine if the list of cipher suites is valid
 *
 * The only two cipher suites supported by tinyDTLS are selected at compilation
 * time. This only verifies if said cipher suites are present in the compiled
 * code and at least one of them matches the cipher suite listed.
 *
 * @param[in] cipher    Cipher suite to check
 *
 * @return true if cipher sutie is supported
 */
bool is_cipher_tinydtls(int cipher);

/**
 * @brief Creates the tinydtls context
 *
 * @param[inout] dtls_session A tinydtls_session_t to store the the new context
 * @param[in] resp_cb         Callback handler for the (D)TLS Data App record
 * @param[in] Flags           Determine the behavior.
 *
 * @return  0 on success
 * @return TLSMAN_ERROR_UNABLE_CONTEXT if unable to load the context
 */
ssize_t tlsman_tinydtls_init_context(tinydtls_session_t *dtls_session,
                                     tlsman_resp_handler_t resp_cb,
                                     const uint8_t flags);

/**
 * @brief Starts the DTLS channel. If blocking, stay there until is ready.
 *
 * If the behavior is non-blocking this returns as soon as the first DTLS Hello
 * Client record is put in the sending buffer. Otherwise, it will wait until the
 * handshake is finished, or a timeout had happened.
 *
 * @param[in] dtls_session      Tinydtls session
 * @param[in] pkt_buff          Packet buffer to use for sending DTLS handshake records.
 * @param[in] pkt_size          Size of the packet buffer.
 * @param[in] flags             Determine the behavior (blocking or non-blocking)
 *
 * @return 0 on success
 * @return TLSMAN_ERROR_FATAL if unable to generate (or send) the first DTLS record.
 * @return TLSMAN_ERROR_HANDSHAKE_TIMEOUT if the handshake was not completed.
 **/
ssize_t tlsman_tinydtls_connect(tinydtls_session_t *dtls_session,
                                uint8_t *pkt_buff, size_t pkt_size,
                                uint8_t flags);

/**
 * @brief  Send data by means of DTLS Data app records
 *
 * @param[in] dtls_session  Tinydtls session
 * @param[in] data          Data buffer
 * @param[in] data_size     Size of the data buffer.
 * @param[in] flags         Determine the behavior (blocking or non-blocking)
 *
 * @return 0 on sucess.
 * @return TLSMAN_ERROR_DATA_APP_WRITE if unable to send package.
 */
ssize_t tlsman_tinydtls_send(tinydtls_session_t *dtls_session,
                             uint8_t *data, size_t data_size,
                             uint8_t flags);
/**
 * @brief Retrieve the last DTLS data app record received
 *
 * The blocking behavior impact on this function. If settled with non-blocking
 * (SOCK_HAS_ASYNC is set). This will remain on listening loop for time equal
 * to TINYDTLS_HDSK_TIMEOUT seconds (it's expected to be running in its own
 * thread). If blocking state, this will only check if there is a DTLS record
 * available in the receive buffer.
 *
 * TODO DISCUSSION: Maybe both behaviors should do exactly the same?
 *
 * @param[in] dtls_session      Tinydtls session
 * @param[in] pkt_buff          Packet buffer to use for sending DTLS handshake records.
 * @param[in] pkt_size          Size of the packet buffer.
 *
 * @return  If behavior is non-blocking, return 0 once a timeout is passed
 * @return  If the behavior is blocking, returns 0 or code error if the DTLS session was closed.
 */
size_t tlsman_tintdytls_rcv(tinydtls_session_t *dtls_session,
                            void *pkt_buff,
                            size_t pkt_size);

/**
 * @brief Release the resources associated to the DTLS session
 *
 * @param[in] dtls_session      Tinydtls session
 */
void tlsman_tinydtls_free_context(tinydtls_session_t *dtls_session);

/**
 * @brief Renegotiates the DTLS session
 *
 * TODO FIXME (Known issues with tinydtls)
 */
ssize_t tlsman_tinydtls_renegotiate(tinydtls_session_t *dtls_session,
                                    uint8_t *pkt_buff, size_t pkt_size,
                                    uint8_t flags);

/**
 * @brief Rehandshake connection
 *
 * TODO FIXME (Known issues with tinydtls)
 */
ssize_t tlsman_tinydtls_rehandshake(tinydtls_session_t *dtls_session,
                                    uint8_t *pkt_buff, size_t pkt_size,
                                    uint8_t flags);

/**
 * @brief Listening mode for the server side
 *
 * @param[in] dtls_session      Tinydtls session
 * @param[in] pkt_buff          Packet buffer to use for sending DTLS handshake records.
 * @param[in] pkt_size          Size of the packet buffer.
 *
 * returns 0 on normal operation
 */
ssize_t tlsman_tinydtls_listening(tinydtls_session_t *dtls_session,
                            void *pkt_buff,
                            size_t pkt_size);

#ifdef SOCK_HAS_ASYNC
void tlsman_tinydtls_set_async_parameters(tinydtls_session_t *dtls_session,
                                          event_queue_t *sock_event_queue,
                                          uint8_t *buffer, ssize_t size);

/**
 * @brief Launch an timer event
 *
 * @note A timer event is used for those scenarios where not extra threads are being used
 */
void tlsman_tinydtls_set_listening_timeout(void);

#endif /* SOCK_HAS_ASYNC */

#ifdef __cplusplus
}
#endif

#endif /* NET_TLSMAN_TINYDTLS_H */
/** @} */

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
 * @brief       tlsman definition
 *
 * @author      Raul Fuentes <raul.fuentes-samaniego@inria.fr>
 *
 * @}
 */

#ifndef NET_TLSMAN_H
#define NET_TLSMAN_H

#include <stdint.h>
#include <stdio.h>

#ifdef MODULE_TLSMAN_TINYDTLS
#ifdef WITH_RIOT_GNRC
#include "net/tlsman/tinydtls_sock.h"
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _sock_tl_ep tlsman_ep_t; /**< Endpoint for a tlsman object */

/*
 * Control flags for the (D)TLS stacks
 *
 * Flags UU-BB-S-TTT: Unused (U), Blocking behavior (B), Side behavior (S),
 * (D)TLS stack (T)
 *
 */
#define TLSMAN_FLAG_STACK_UNIVERSAL (0x00)  /**< Loads the default (D)TLS stack (bits TTT) */
#define TLSMAN_FLAG_STACK_TLS (0x01)        /**< Only loads TLS stack (bits TTT) */
#define TLSMAN_FLAG_STACK_DTLS (0x02)       /**< Only loads DTLS stack (bits TTT) */
#define TLSMAN_FLAG_STACK_IPSEC (0x03)      /**< Only loads IPSEC (bits TTT) @note not supported yet  */
#define TLSMAN_FLAG_NONBLOCKING (0x10)      /**< Define [non-]blocking behavior (BB) @note If supported */
#define TLSMAN_FLAG_SIDE_CLIENT (0x00)      /**< Define the behavior as a client side (bits S) */
#define TLSMAN_FLAG_SIDE_SERVER (0x04)      /**< Define the behavior as a server side (bits S)*/

/** Maximum number of attempts for completing a (D)TLS Handshake */
#define TLSMAN_HANDSHAKE_MAX_TRY 10

/**
 * Error codes
 */
enum {
    TLSMAN_ERROR_SUCCESS    = 0x00,         /**< Non-error */
    /* FIXME negative values for errors */
    TLSMAN_ERROR_FATAL      = 0x9C,         /**< Fatal error (non specified below) */
    TLSMAN_ERROR_UNABLE_CONTEXT,            /**< Unable to create context */
    TLSMAN_ERROR_TRANSPORT_NOT_SUPPORTED,   /**< TCP/UDP stack selected not supported  */
    TLSMAN_ERROR_CIPHER_NOT_SUPPORTED,      /**< Cipher suite(s) selected not supported */
    TLSMAN_ERROR_HANDSHAKE_TIMEOUT,         /**< Timeout for the handshake (first negotiation) */
    TLSMAN_ERROR_DATA_APP_WRITE,            /**< Unable to send a DTLS Data App record   */
    TLSMAN_ERROR_SESSION_REN,               /**< TODO: Separated both types of re-negotiation? */
    TLSMAN_ERROR_NON_STARTED,               /**< Non secure session has been done yet */
    TLSMAN_ERROR_REMOTE_RESET,              /**< Got a remote DTLS Alert record */
    TLSMAN_ERROR_UNABLE_CLOSE               /**< Unable to close the (D)TLS session */

};

/* DISCUSSION: tlsman_controller ?  tlsman_settings? */
typedef struct tlsman_driver {

  tlsman_session_t session; /**  Abstract (D)TLS secure session between peers. */

  ssize_t (*tlsman_load_stack)(uint16_t *ciphersuites_list, size_t total, uint8_t flags);

  /**
   * Creates the (D)TLS context.
   *
   * @param[inout] session   The (D)TLS session to create (it can be NULL)
   * @param[in] resp_cb      Callback handler for the (D)TLS Data App record
   * @param[in] flags        Flags for handling the (D)TLS context
   *
   * @return 0 in success.
   * @return TLSMAN_ERROR_UNABLE_CONTEXT if it was impossible to load a new context.
   */
  ssize_t (*tlsman_init_context)(tlsman_session_t *session, tlsman_resp_handler_t resp_cb,
                          const uint8_t flags);


  /**
   * @brief Prepare the (D)TLS peer and start a new secure channel.
   *
   * The (D)TLS handshake process is started here.
   *
   * @param[inout] session The (D)TLS session to create (it can be NULL)
   * @param[in] flags        Defines the peer side (client/server) and the
   *                         blocking/non-blocking behavior.
   * @param[in] pkt_buff  The buffer to use for handling (D)TLS records
   * @param[in] pkt_size  The size of the buffer.
   *
   * @return 0 on success
   * @return TLSMAN_ERROR_HANDSHAKE_TIMEOUT if the handshake was not completed
   * into a defined interval (default 10 seconds). And only if the behavior is
   * blocking.
   */
  ssize_t (*tlsman_create_channel)(tlsman_session_t *session, uint8_t flags,
                            uint8_t *pkt_buff, size_t pkt_size);

#if defined(SOCK_HAS_ASYNC) || defined(DOXYGEN)

  /**
   * @brief Sets the parameters required for sock asynchronous.
   *
   * This sets the queue for the sock and also provides pointers for the buffer to
   * be used for sending and receiving (D)TLS records.
   *
   * @param[inout] session        (D)TLS session to use.
   * @param[in] sock_event_queue  The event queue to link to the (D)TLS session
   * @param[in] pkt_buff          Buffer to use for handling (D)TLS records.
   * @param[in] pkt_size          Size of the buffer.
   */
  void (*tlsman_set_async_parameters)(tlsman_session_t *session,
                                   event_queue_t *sock_event_queue,
                                   uint8_t *pkt_buff, ssize_t pkt_size);

  /**
   * Enables the non-blocking listening state for the (D)TLS stack.
   */
  void (*tlsman_set_async_listening)(void);
#endif /* SOCK_HAS_ASYNC */

  /**
   * @brief Determines if the (D)TLS session is ready for sending data.
   *
   * If the behavior is non-blocking, the application must not proceed until
   * this return True.
   *
   * @note TODO: Creates a queue buffer?
   *
   * @param[in] session   (D)TLS session to use.
   *
   * @return True if it's ready
   */
  bool (*tlsman_is_channel_ready)(tlsman_session_t *session);

  /**
   * @brief Sends upper layer data by means of the secure session.
   *
   * The data will be sent in one or multiples (D)TLS records.
   * @note TODO: Use a queue buffer(?)
   *
   * @param[in] session       (D)TLS secure session to use
   * @param[in] data          The (plaintext) data to send
   * @param[in] len           Size of the data buffer
   *
   * @return 0 on success
   * @return TLSMAN_ERROR_DATA_APP_WRITE if the packet(s) were unable to be sent
   *
   */
  ssize_t (*tlsman_send_data_app)(tlsman_session_t *session, const void *data, size_t len);

  /**
   * Processes the last (D)TLS Data Application record available.
   *
   * @param[in] session      (D)TLS secure session to use.
   * @param[in] pkt_buff     Pointer where the received data should be stored.
   * @param[in] max_len      Maximum space available in the data buffer.
   *
   * @return If SOCK_HAS_ASYNC return 0 on success
   * @return If !SOCK_HAS_ASYNC return 0 on success otherwise code error (FIXME)
   * @return TLSMAN_ERROR_REMOTE_RESET if got an DTLS record alert
   */
  ssize_t (*tlsman_retrieve_data_app)(tlsman_session_t *session, void *pkt_buff, size_t max_len);

  /**
   * @brief Close the (D)TLS channel established between the peers of the session.
   *
   * @param[in]    session  (D)TLS secure session to use.
   *
   * @return  0 on success,
   * @return TLSMAN_ERROR_UNABLE_CLOSE if unable to close the session
   */
  ssize_t (*tlsman_close_channel)(tlsman_session_t *session);

  /**
   * @brief Release all the resources involved in the (D) TLS channel.
   *
   * This also sends a last DTLS Alert record and releases memory used for the
   * session.
   *
   * @param[in]    session  (D)TLS secure session to use.
   */
  void (*tlsman_release_resources)(tlsman_session_t *dtls_session);

  /**
   * @brief Renegotiates the current secure session between the peers.
   *
   * @param[in]  session  (D)TLS secure session to use.
   * @param[in]  flags    Flags for definning the [non-]blocking behavior.
   * @param[in]  pkt_buff If Nona-blocking, pointer to packet buffer to be used. Otherwise can be NULL
   * @param[in]  pkt_size Size of pkt_buff
   *
   * @return 0 on success.
   * @return TLSMAN_ERROR_SESSION_REN if renegotiation failed.
   */
  ssize_t (*tlsman_renegotiate)(tlsman_session_t *session, uint8_t flags, uint8_t *pkt_buff, size_t pkt_size);

  /**
   *  @brief Generates a new (D)TLS handshake for the current secure session.
   *
   * @param[in]  session  (D)TLS secure session to use.
   * @param[in]  flags    Flags for defining the [non-]blocking behavior.
   * @param[in]  pkt_buff If Nona-blocking, pointer to packet buffer to be used. Otherwise can be NULL
   * @param[in]  pkt_size Size of pkt_buff
   *
   * @return 0 on success.
   * @return TLSMAN_ERROR_SESSION_REN if renegotiation failed.
   */
  ssize_t (*tlsman_rehandshake)(tlsman_session_t *session, uint8_t flags, uint8_t *pkt_buff, size_t pkt_size);

  /**
   * @brief Listen for an incoming connection request
   *
   * The server side for any (D)TLS channel. The reception handling of any type
   * of (D)TLS record, including those involved in the handshake is done here.
   *
   * For answers incoming requests with (D)TLS Data Application records, please
   * use tlsman_send_data_app() inside of the tlsman_resp_handler_t callback.
   *
   * @note TODO Create the non-blocking behavior
   *
   * @param[in]  session  (D)TLS secure session to use.
   * @param[in]  flags    Flags for defining the behavior
   * @param[in]  pkt_buff Pointer to packet buffer to be used (for reception).
   * @param[in]  pkt_size Buffer size.
   *
   * @return 0 on success.
   */
  ssize_t (*tlsman_listening)(tlsman_session_t *session, uint8_t flags,
                    uint8_t *pkt_buff, size_t pkt_size);

} tlsman_driver_t;


/**
 * @brief Initializes the (D)TLS stack loaded.
 *
 * Loads the initialization of the (D)TLS stack. It's expected to be only one
 * compiled stack (tinydtls, wolfssl, etc.).
 *
 * @note Not all the (D)TLS stacks can support loading cipher suites (e.g.
 * tinyDTLS). In such cases, is only checked that the stack was compiled with
 * one or more of the cipher suites provided in cipersuites_list.
 *
 * FIXME session
 * @param[in]  ciphersuites_list An array of cipher suites ID to load
 * @param[in]  total             Size of cipher suites_list
 * @param[in]  flags             Flags for handling the initialization.
 *
 * @return  0 in Success
 * @return  TLSMAN_ERROR_TRANSPORT_NOT_SUPPORTED if the TCP/UDP stack intended
 *            is not supported by the (D)TLS stack (e.g. tinyDTLS does not
 *            support TCP)
 * @return  TLSMAN_ERROR_CIPHER_NOT_SUPPORTED if the cipher suite selected is
 *          not supported (or compiled) for the (D)TLS stack.
 */
ssize_t tlsman_load_stack(tlsman_driver_t *session,
                                   uint16_t *ciphersuites_list, size_t total,
                                   uint8_t flags);

/**
 * return the last error known (Alerts, retransmission, wrong cipher suites,
 * etc.).
 *
 * TODO tlsman_session
 * @return 0 on success
 * @return TLSMAN_ERROR_NON_STARTED if not recorded.
 */
ssize_t tlsman_return_last_known_error(tlsman_driver_t *tlsman_session);

/**
 * Processes the error_id and identifies if the (D)TLS session must be terminated.
 *
 * @param[in] error_id  Error ID
 * @return True if error is non-fatal.
 */
bool tlsman_process_is_error_code_nonfatal(ssize_t error_id);


#ifdef __cplusplus
}
#endif

#endif /* NET_TLSMAN_H */
/** @} */

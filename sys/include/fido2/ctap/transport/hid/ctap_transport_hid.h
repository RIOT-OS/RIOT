/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    fido2_ctap_transport_hid CTAP USB_HID transport binding - CTAPHID
 * @ingroup     fido2_ctap_transport
 * @brief       CTAP_HID defines and functions
 *
 * @{
 *
 * @file
 * @brief       Definition for CTAPHID helper functions
 *
 * @author      Nils Ollrogge <nils-ollrogge@outlook.de>
 */

#ifndef FIDO2_CTAP_TRANSPORT_HID_CTAP_TRANSPORT_HID_H
#define FIDO2_CTAP_TRANSPORT_HID_CTAP_TRANSPORT_HID_H

#include <stdint.h>
#include "timex.h"

#include "usb/usbus/hid.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name CTAP_HID packet type payload sizes
 *
 * @{
 */
#define CTAP_HID_INIT_PAYLOAD_SIZE  (CONFIG_USBUS_HID_INTERRUPT_EP_SIZE - 7)    /**< endpoint size - init packet metadata */
#define CTAP_HID_CONT_PAYLOAD_SIZE  (CONFIG_USBUS_HID_INTERRUPT_EP_SIZE - 5)    /**< endpoint size - cont packet metadata */
/** @} */

/**
 * @brief CTAP_HID protocol version
 */
#define CTAP_HID_PROTOCOL_VERSION 0x02

/**
 * @name CTAP_HID packet type identifiers
 *
 * @{
 */
#define CTAP_HID_INIT_PACKET 0x80
#define CTAP_HID_CONT_PACKET 0x00
/** @} */

/**
 * @brief CTAP_HID size of nonce for init request
 */
#define CTAP_HID_INIT_NONCE_SIZE 8

/**
 * @brief CTAP_HID transaction timeout
 *
 */
#define CTAP_HID_TRANSACTION_TIMEOUT    (0.75 * US_PER_SEC)

/**
 * @brief CTAP_HID max message payload size
 *
 * CTAP specification (version 20190130) section 8.2.4.
 */
#define CTAP_HID_BUFFER_SIZE 7609

/**
 * @name CTAP_HID commands
 *
 * @{
 */
#define CTAP_HID_COMMAND_PING       (0x01 | CTAP_HID_INIT_PACKET)
#define CTAP_HID_COMMAND_MSG        (0x03 | CTAP_HID_INIT_PACKET)
#define CTAP_HID_COMMAND_LOCK       (0x04 | CTAP_HID_INIT_PACKET)
#define CTAP_HID_COMMAND_INIT       (0x06 | CTAP_HID_INIT_PACKET)
#define CTAP_HID_COMMAND_WINK       (0x08 | CTAP_HID_INIT_PACKET)
#define CTAP_HID_COMMAND_CBOR       (0x10 | CTAP_HID_INIT_PACKET)
#define CTAP_HID_COMMAND_CANCEL     (0x11 | CTAP_HID_INIT_PACKET)
#define CTAP_HID_COMMAND_KEEPALIVE  (0x3b | CTAP_HID_INIT_PACKET)
#define CTAP_HID_COMMAND_ERROR      (0x3f | CTAP_HID_INIT_PACKET)
/** @} */

/**
 * @name CTAP_HID capability flags
 *
 * @{
 */
#define CTAP_HID_CAPABILITY_WINK 0x01   /**< If set, authenticator implements CTAPHID_WINK function */
#define CTAP_HID_CAPABILITY_CBOR 0x04   /**< If set, authenticator implements CTAPHID_CBOR function */
#define CTAP_HID_CAPABILITY_NMSG 0x08   /**< If set, authenticator DOES NOT implement CTAPHID_MSG function (CTAP1 / U2F) */
/** @} */

/**
 * @name CTAP_HID error codes
 *
 * @{
 */
#define CTAP_HID_OK                   0x00      /**< Success */
#define CTAP_HID_ERR_INVALID_CMD      0x01      /**< The command in the request is invalid */
#define CTAP_HID_ERR_INVALID_PAR      0x02      /**< The parameter(s) in the request is invalid */
#define CTAP_HID_ERR_INVALID_LEN      0x03      /**< The length field (BCNT) is invalid for the request */
#define CTAP_HID_ERR_INVALID_SEQ      0x04      /**< The sequence does not match expected value */
#define CTAP_HID_ERR_MSG_TIMEOUT      0x05      /**< The message has timed out */
#define CTAP_HID_ERR_CHANNEL_BUSY     0x06      /**< The device is busy for the requesting channel */
#define CTAP_HID_ERR_LOCK_REQUIRED    0x0a      /**< Command requires channel lock */
#define CTAP_HID_ERR_INVALID_CHANNEL  0x0b      /**< CID is not valid. */
#define CTAP_HID_ERR_OTHER            0x7f      /**< Unspecified error */
/** @} */

/**
 * @name CTAP_HID status codes
 *
 * @{
 */
#define CTAP_HID_STATUS_PROCESSING 0x01
#define CTAP_HID_STATUS_UPNEEDED   0x02
/** @} */

/**
 * @brief CTAP_HID max number of channels
 *
 */
#define CTAP_HID_CIDS_MAX 0x08

/**
 * @brief CTAP_HID animation delay in milliseconds for wink command
 */
#define CTAP_HID_WINK_DELAY 400

/**
 * @brief CTAP_HID broadcast channel identifier
 *
 */
#define CTAP_HID_BROADCAST_CID 0xffffffff

/**
 * @name CTAP_HID buffer status
 *
 * @{
 */
#define CTAP_HID_BUFFER_STATUS_BUFFERING    0x00
#define CTAP_HID_BUFFER_STATUS_DONE         0x01
#define CTAP_HID_BUFFER_STATUS_ERROR        0x02
/** @} */

/**
 * @brief CTAP_HID initialization packet struct
 *
 */
typedef struct {
    uint8_t cmd;                                    /**< CTAP_HID command */
    uint8_t bcnth;                                  /**< higher byte */
    uint8_t bcntl;                                  /**< lower byte */
    uint8_t payload[CTAP_HID_INIT_PAYLOAD_SIZE];    /**< packet payload */
} ctap_hid_init_pkt_t;

/**
 * @brief CTAP_HID continuation packet struct
 *
 */
typedef struct {
    uint8_t seq;                                    /**< packet sequence number */
    uint8_t payload[CTAP_HID_CONT_PAYLOAD_SIZE];    /**< packet payload */
} ctap_hid_cont_pkt_t;

/**
 * @brief CTAP_HID packet struct
 *
 */
typedef struct {
    uint32_t cid;                   /**< channel identifier */
    union {
        ctap_hid_init_pkt_t init;   /**< initialization packet */
        ctap_hid_cont_pkt_t cont;   /**< continuation packet */
    };
} ctap_hid_pkt_t;

/**
 * @brief CTAP_HID initialization response struct
 *
 * CTAP specification (version 20190130) 8.1.9.1.3
 */
typedef struct __attribute__((packed)){
    uint8_t nonce[CTAP_HID_INIT_NONCE_SIZE];    /**< nonce */
    uint32_t cid;                               /**< channel identifier */
    uint8_t protocol_version;                   /**< CTAP_HID protocol version */
    uint8_t version_major;                      /**< major device version */
    uint8_t version_minor;                      /**< minor device version */
    uint8_t build_version;                      /**< build device version */
    uint8_t capabilities;                       /**< capabilities flags */
} ctap_hid_init_resp_t;

/**
 * @brief CTAP_HID channel identifier struct
 *
 * Used to keep state information about logical channels
 */
typedef struct {
    bool taken;                             /**< is cid taken? */
    uint32_t cid;                           /**< channel identifier */
    uint64_t last_used;                     /**< timestamp of last usage */
} ctap_hid_cid_t;

/**
 * @brief CTAP_HID buffer struct
 *
 */
typedef struct {
    uint32_t cid;                           /**< channel identifier */
    uint8_t cmd;                            /**< CTAP_HID command */
    uint8_t buffer[CTAP_HID_BUFFER_SIZE];   /**< data buffer */
    uint16_t offset;                        /**< current offset into data buffer */
    int16_t seq;                            /**< current sequence number */
    uint16_t bcnt;                          /**< expected amount of bytes to be received */
    uint8_t err;                            /**< error type if error  */
    bool is_locked;                         /**< buffer is locked by transaction */
    bool should_cancel;                     /**< current transaction should be cancelled */
} ctap_hid_buffer_t;

/**
 * @brief Initialize USB, create needed threads
 *
 */
void fido2_ctap_transport_hid_create(void);

/**
 * @brief Handle CTAP_HID packet
 *
 * @param[in] pkt_raw   CTAP_HID packet
 * @param[in] size      size of packet
 *
 */
void fido2_ctap_transport_hid_handle_packet(void *pkt_raw, int size);

/**
 * @brief Send keepalive packet
 *
 * @param[in] status    CTAP_HID status code
 *
 */
void fido2_ctap_transport_hid_send_keepalive(uint8_t status);

/**
 * @brief Check logical channels for timeouts
 *
 */
void fido2_ctap_transport_hid_check_timeouts(void);

#ifdef __cplusplus
}
#endif
#endif /* FIDO2_CTAP_TRANSPORT_HID_CTAP_TRANSPORT_HID_H */
/** @} */

/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_someip SOME/IP
 * @ingroup     net
 * @brief       Provides SOME/IP header and field defines
 *
 * @{
 *
 * @file
 * @brief   SOME/IP header and field definitions
 *
 * @author  Jannes Volkens <jannes.volkens@haw-hamburg.de>
 */
#ifndef NET_SOMEIP_H
#define NET_SOMEIP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief SOME/IP header length size, excluding payload
 */
#define SOMEIP_HDR_LENGTH       (8U)
/**
 * @brief SOME/IP full header size, including all field expect for the payload
 */
#define SOMEIP_FULL_HDR_SIZE    (16U)

/**
 * @brief       Structure of the Message ID
 */
typedef struct {
    uint16_t service_id; /**< Service ID */
    uint16_t method_id;  /**< Method ID */
} someip_message_id_t;

/**
 * @brief       Structure of the Request ID
 */
typedef struct {
    uint16_t client_id;  /**< Client ID */
    uint16_t session_id; /**< Session ID */
} someip_request_id_t;

/**
 * @brief       SOME/IP header
 */
typedef struct __attribute__((packed)) {
    someip_message_id_t message_id; /**< Message ID */
    uint32_t length;                /**< Length */
    someip_request_id_t request_id; /**< Request ID */
    uint8_t protocol_version;       /**< Protocol Version */
    uint8_t interface_version;      /**< Interface Version*/
    uint8_t msg_type;               /**< Message Type*/
    uint8_t return_code;            /**< Return Code*/
} someip_hdr_t;

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* NET_SOMEIP_H */

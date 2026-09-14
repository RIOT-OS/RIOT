/*
 * SPDX-FileCopyrightText: 2020 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

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

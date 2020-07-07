/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_coap CoAP defines
 * @ingroup     net
 * @brief       Generic CoAP values as defined by RFC7252
 * @{
 *
 * @file
 * @brief       Generic CoAP values as defined by RFC7252
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 */

#ifndef NET_COAP_H
#define NET_COAP_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default CoAP port
 */
#define COAP_PORT               (5683)

/**
 * @name    CoAP option numbers
 * @{
 */
#define COAP_OPT_URI_HOST       (3)
#define COAP_OPT_OBSERVE        (6)
#define COAP_OPT_LOCATION_PATH  (8)
#define COAP_OPT_URI_PATH       (11)
#define COAP_OPT_CONTENT_FORMAT (12)
#define COAP_OPT_URI_QUERY      (15)
#define COAP_OPT_ACCEPT         (17)
#define COAP_OPT_LOCATION_QUERY (20)
#define COAP_OPT_BLOCK2         (23)
#define COAP_OPT_BLOCK1         (27)
#define COAP_OPT_PROXY_URI      (35)
#define COAP_OPT_PROXY_SCHEME   (39)
/** @} */

/**
 * @name    Message types -- confirmable, non-confirmable, etc.
 * @{
 */
#define COAP_TYPE_CON           (0)
#define COAP_TYPE_NON           (1)
#define COAP_TYPE_ACK           (2)
#define COAP_TYPE_RST           (3)
/** @} */

/**
 * @name    CoAP method codes used in header
 * @{
 */
#define COAP_CLASS_REQ          (0)
#define COAP_METHOD_GET         (1)
#define COAP_METHOD_POST        (2)
#define COAP_METHOD_PUT         (3)
#define COAP_METHOD_DELETE      (4)
#define COAP_METHOD_FETCH       (5)
#define COAP_METHOD_PATCH       (6)
#define COAP_METHOD_IPATCH      (7)
/** @} */

/**
 * @name    Empty CoAP message code
 * @{
 */
#define COAP_CODE_EMPTY         (0)
/** @} */

/**
 * @name    Response message codes: success
 * @{
 */
#define COAP_CLASS_SUCCESS      (2)
#define COAP_CODE_CREATED      ((2 << 5) | 1)
#define COAP_CODE_DELETED      ((2 << 5) | 2)
#define COAP_CODE_VALID        ((2 << 5) | 3)
#define COAP_CODE_CHANGED      ((2 << 5) | 4)
#define COAP_CODE_204          ((2 << 5) | 4)
#define COAP_CODE_CONTENT      ((2 << 5) | 5)
#define COAP_CODE_205          ((2 << 5) | 5)
#define COAP_CODE_CONTINUE     ((2 << 5) | 31)
#define COAP_CODE_231          ((2 << 5) | 31)
/** @} */

/**
 * @name    Response message codes: client error
 * @{
 */
#define COAP_CLASS_CLIENT_FAILURE             (4)
#define COAP_CODE_BAD_REQUEST                ((4 << 5) | 0)
#define COAP_CODE_UNAUTHORIZED               ((4 << 5) | 1)
#define COAP_CODE_BAD_OPTION                 ((4 << 5) | 2)
#define COAP_CODE_FORBIDDEN                  ((4 << 5) | 3)
#define COAP_CODE_PATH_NOT_FOUND             ((4 << 5) | 4)
#define COAP_CODE_404                        ((4 << 5) | 4)
#define COAP_CODE_METHOD_NOT_ALLOWED         ((4 << 5) | 5)
#define COAP_CODE_NOT_ACCEPTABLE             ((4 << 5) | 6)
#define COAP_CODE_REQUEST_ENTITY_INCOMPLETE  ((4 << 5) | 8)
#define COAP_CODE_CONFLICT                   ((4 << 5) | 9)
#define COAP_CODE_PRECONDITION_FAILED        ((4 << 5) | 12)
#define COAP_CODE_REQUEST_ENTITY_TOO_LARGE   ((4 << 5) | 13)
#define COAP_CODE_UNSUPPORTED_CONTENT_FORMAT ((4 << 5) | 15)
#define COAP_CODE_UNPROCESSABLE_ENTITY       ((4 << 5) | 22)
/** @} */

/**
 * @name    Response message codes: server error
 * @{
 */
#define COAP_CLASS_SERVER_FAILURE             (5)
#define COAP_CODE_INTERNAL_SERVER_ERROR      ((5 << 5) | 0)
#define COAP_CODE_NOT_IMPLEMENTED            ((5 << 5) | 1)
#define COAP_CODE_BAD_GATEWAY                ((5 << 5) | 2)
#define COAP_CODE_SERVICE_UNAVAILABLE        ((5 << 5) | 3)
#define COAP_CODE_GATEWAY_TIMEOUT            ((5 << 5) | 4)
#define COAP_CODE_PROXYING_NOT_SUPPORTED     ((5 << 5) | 5)
/** @} */

/**
 * @name    Content-Format option codes
 * @anchor  net_coap_format
 * @{
 */
#define COAP_FORMAT_TEXT                      (0)
#define COAP_FORMAT_LINK                     (40)
#define COAP_FORMAT_XML                      (41)
#define COAP_FORMAT_OCTET                    (42)
#define COAP_FORMAT_EXI                      (47)
#define COAP_FORMAT_JSON                     (50)
#define COAP_FORMAT_JSON_PATCH_JSON          (51)
#define COAP_FORMAT_MERGE_PATCH_JSON         (52)
#define COAP_FORMAT_CBOR                     (60)
#define COAP_FORMAT_SENML_JSON              (110)
#define COAP_FORMAT_SENSML_JSON             (111)
#define COAP_FORMAT_SENML_CBOR              (112)
#define COAP_FORMAT_SENSML_CBOR             (113)
#define COAP_FORMAT_SENML_EXI               (114)
#define COAP_FORMAT_SENSML_EXI              (115)
#define COAP_FORMAT_SENML_XML               (310)
#define COAP_FORMAT_SENSML_XML              (311)
/** @} */

/**
 * @name    Observe (RFC 7641) constants
 * @{
 */
#define COAP_OBS_REGISTER        (0)
#define COAP_OBS_DEREGISTER      (1)
/** @} */

/**
 * @name    CoAP message format constants
 * @{
 */
#define COAP_TOKEN_LENGTH_MAX    (8)
/** @} */

/**
 * @defgroup net_coap_conf    CoAP compile configurations
 * @ingroup  net_coap
 * @ingroup  config
 * @{
 */
/**
 * @name    Timing parameters
 *
 * These parameters are defined as configurable in [RFC 7252, section 4.8.1]
 * (https://tools.ietf.org/html/rfc7252#section-4.8.1).
 * @{
 */
/**
 * @brief    Timeout in seconds for a response to a confirmable request
 *
 * This value is for the response to the *initial* confirmable message. The
 * timeout doubles for subsequent retries. To avoid synchronization of resends
 * across hosts, the actual timeout is chosen randomly between
 * @ref CONFIG_COAP_ACK_TIMEOUT and
 * (@ref CONFIG_COAP_ACK_TIMEOUT * @ref CONFIG_COAP_RANDOM_FACTOR_1000 / 1000).
 */
#ifndef CONFIG_COAP_ACK_TIMEOUT
#define CONFIG_COAP_ACK_TIMEOUT        (2U)
#endif

/**
 * @brief   Used to calculate upper bound for timeout
 *
 * This represents the `ACK_RANDOM_FACTOR`
 * ([RFC 7252, section 4.2](https://tools.ietf.org/html/rfc7252#section-4.2))
 * multiplied by 1000, to avoid floating point arithmetic.
 *
 * See @ref CONFIG_COAP_ACK_TIMEOUT
 */
#ifndef CONFIG_COAP_RANDOM_FACTOR_1000
#define CONFIG_COAP_RANDOM_FACTOR_1000      (1500)
#endif

/** @brief   Maximum number of retransmissions for a confirmable request */
#ifndef CONFIG_COAP_MAX_RETRANSMIT
#define CONFIG_COAP_MAX_RETRANSMIT     (4)
#endif
/** @} */
/** @} */

/**
 * @name    Fixed timing parameters
 * @{
 */
#define COAP_NSTART             (1)
#define COAP_DEFAULT_LEISURE    (5)
/** @} */

/**
 * @name Blockwise transfer (RFC7959)
 * @{
 */
#define COAP_BLOCKWISE_NUM_OFF  (4)
#define COAP_BLOCKWISE_MORE_OFF (3)
#define COAP_BLOCKWISE_SZX_MASK (0x07)
#define COAP_BLOCKWISE_SZX_MAX  (7)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* NET_COAP_H */
/** @} */

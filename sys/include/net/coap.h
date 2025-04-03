/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef NET_COAP_H
#define NET_COAP_H

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

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default CoAP port
 */
#define COAP_PORT               (5683)

/**
 * @brief   Default CoAP DTLS port
 */
#define COAPS_PORT              (5684)

#define COAP_V1                 (1)     /**< Identifier for CoAP version 1 (RFC 7252) */

/**
 * @name    CoAP option numbers
 * @{
 */
#define COAP_OPT_IF_MATCH       (1)
#define COAP_OPT_URI_HOST       (3)
#define COAP_OPT_ETAG           (4)
#define COAP_OPT_IF_NONE_MATCH  (5)
#define COAP_OPT_OBSERVE        (6)
#define COAP_OPT_LOCATION_PATH  (8)
/**
 * @brief OSCORE option
 *
 * Indicates that the CoAP message is an OSCORE message and that it contains a
 * compressed COSE object.
 *
 * @see [RFC 8613](https://datatracker.ietf.org/doc/html/rfc8613)
 */
#define COAP_OPT_OSCORE         (9)
#define COAP_OPT_URI_PATH       (11)
#define COAP_OPT_CONTENT_FORMAT (12)
#define COAP_OPT_MAX_AGE        (14)
#define COAP_OPT_URI_QUERY      (15)
/**
 * @brief Hop-Limit option
 *
 * Used to prevent infinite loops when communicating over multiple proxies.
 *
 * @see [RFC 8768](https://datatracker.ietf.org/doc/html/rfc8768)
 */
#define COAP_OPT_HOP_LIMIT      (16)
#define COAP_OPT_ACCEPT         (17)
/**
 * @brief Q-Block1 option
 *
 * Used for block-wise transfer supporting robust transmission in requests.
 *
 * @see [RFC 9177](https://datatracker.ietf.org/doc/html/rfc9177)
 */
#define COAP_OPT_Q_BLOCK1       (19)
#define COAP_OPT_LOCATION_QUERY (20)
/**
 * @brief EDHOC option
 *
 * Used in a CoAP request to signal that the request payload conveys both an
 * EDHOC message_3 and OSCORE protected data, combined together.
 *
 * @see [draft-ietf-core-oscore-edhoc-02](https://datatracker.ietf.org/doc/draft-ietf-core-oscore-edhoc/02/)
 */
#define COAP_OPT_EDHOC          (21)
#define COAP_OPT_BLOCK2         (23)
#define COAP_OPT_BLOCK1         (27)
/**
 * @brief Size2 option
 *
 * Used by clients to request an estimate of a resource's
 * total size from a server during block-wise transfer and by
 * servers to inform clients about the size.
 *
 * @see [RFC 7959](https://datatracker.ietf.org/doc/html/rfc7959)
 * @see [RFC 8613](https://datatracker.ietf.org/doc/html/rfc8613)
 */
#define COAP_OPT_SIZE2          (28)
/**
 * @brief Q-Block2 option
 *
 * Used for block-wise transfer supporting robust transmission in responses.
 *
 * @see [RFC 9177](https://datatracker.ietf.org/doc/html/rfc9177)
 */
#define COAP_OPT_Q_BLOCK2       (31)
#define COAP_OPT_PROXY_URI      (35)
#define COAP_OPT_PROXY_SCHEME   (39)
/**
 * @brief Size1 option
 *
 * Used by clients to give servers an estimate of the total request
 * payload size during block-wise server and by servers to indicate
 * the maximum acceptable payload size in a 4.13 ("Request Entity
 * Too Large") response.
 *
 * @see [RFC 7252](https://datatracker.ietf.org/doc/html/rfc7252)
 * @see [RFC 7959](https://datatracker.ietf.org/doc/html/rfc7959)
 * @see [RFC 8613](https://datatracker.ietf.org/doc/html/rfc8613)
 */
#define COAP_OPT_SIZE1          (60)
/**
 * @brief Echo option
 *
 * Enables a CoAP server to verify the freshness of a request or to force a
 * client to demonstrate reachability at its claimed network address.
 *
 * @see [RFC 9175](https://datatracker.ietf.org/doc/html/rfc9175)
 */
#define COAP_OPT_ECHO           (252)
/**
 * @brief suppress CoAP response
 * @see [RFC 7968](https://datatracker.ietf.org/doc/html/rfc7967)
 */
#define COAP_OPT_NO_RESPONSE    (258)
/**
 * @brief Request-Tag option
 *
 * Allows a CoAP server to match block-wise message fragments belonging to the
 * same request.
 *
 * @see [RFC 9175](https://datatracker.ietf.org/doc/html/rfc9175)
 */
#define COAP_OPT_REQUEST_TAG    (292)
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
#define COAP_CLASS_REQ          (0) /**< Code Class for Request */

/**
 * @brief   CoAP method codes used in request
 */
typedef enum {
    COAP_METHOD_GET    = 1,         /**< GET request (no paylod) */
    COAP_METHOD_POST   = 2,         /**< POST request (resource processes payload) */
    COAP_METHOD_PUT    = 3,         /**< PUT request (update resource with payload) */
    COAP_METHOD_DELETE = 4,         /**< DELETE request (no payload, remove resource)*/
    COAP_METHOD_FETCH  = 5,         /**< FETCH request (RFC 8132) */
    COAP_METHOD_PATCH  = 6,         /**< PATCH request (RFC 8132) */
    COAP_METHOD_IPATCH = 7,         /**< iPATCH request (RFC 8132) */
} coap_method_t;
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
#define COAP_CODE_TOO_MANY_REQUESTS          ((4 << 5) | 29)
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
/**
 * @brief   Content-Type `application/cose; cose-type="cose-encrypt0"`
 * @see     [RFC 9052](https://datatracker.ietf.org/doc/html/rfc9052)
 */
#define COAP_FORMAT_COSE_ENCRYPT0            (16)
/**
 * @brief   Content-Type `application/cose; cose-type="cose-mac0"`
 * @see     [RFC 9052](https://datatracker.ietf.org/doc/html/rfc9052)
 */
#define COAP_FORMAT_COSE_MAC0                (17)
/**
 * @brief   Content-Type `application/cose; cose-type="cose-sign1"`
 * @see     [RFC 9052](https://datatracker.ietf.org/doc/html/rfc9052)
 */
#define COAP_FORMAT_COSE_SIGN1               (18)
/**
 * @brief   Content-Type `application/ace+cbor`
 * @see     [RFC 9200](https://datatracker.ietf.org/doc/html/rfc9200)
 */
#define COAP_FORMAT_ACE_CBOR                 (19)
/**
 * @brief   Content-Type `image/gif`
 * @see     https://www.w3.org/Graphics/GIF/spec-gif89a.txt
 */
#define COAP_FORMAT_IMAGE_GIF                (21)
/**
 * @brief   Content-Type `image/jpeg`
 * @see     [ISO/IEC 10918-5](https://www.itu.int/rec/T-REC-T.871-201105-I/en)
 */
#define COAP_FORMAT_IMAGE_JPEG               (22)
/**
 * @brief   Content-Type `image/png`
 * @see     [RFC 2083](https://datatracker.ietf.org/doc/html/rfc2083)
 */
#define COAP_FORMAT_IMAGE_PNG                (23)
#define COAP_FORMAT_LINK                     (40)
#define COAP_FORMAT_XML                      (41)
#define COAP_FORMAT_OCTET                    (42)
#define COAP_FORMAT_EXI                      (47)
#define COAP_FORMAT_JSON                     (50)
#define COAP_FORMAT_JSON_PATCH_JSON          (51)
#define COAP_FORMAT_MERGE_PATCH_JSON         (52)
#define COAP_FORMAT_CBOR                     (60)
/**
 * @brief   Content-Type `application/cwt`
 * @see     [RFC 8392](https://datatracker.ietf.org/doc/html/rfc8392)
 */
#define COAP_FORMAT_CWT                      (61)
/**
 * @brief   Content-Type `application/multipart-core`
 * @see     [RFC 8710](https://datatracker.ietf.org/doc/html/rfc8710)
 */
#define COAP_FORMAT_MULTIPART_CORE           (62)
/**
 * @brief   Content-Type `application/cbor-seq`
 * @see     [RFC 8742](https://datatracker.ietf.org/doc/html/rfc8742)
 */
#define COAP_FORMAT_CBOR_SEQ                 (63)
/**
 * @brief   Content-Type `application/cose; cose-type="cose-encrypt"`
 * @see     [RFC 9052](https://datatracker.ietf.org/doc/html/rfc9052)
 */
#define COAP_FORMAT_COSE_ENCRYPT             (96)
/**
 * @brief   Content-Type `application/cose; cose-type="cose-mac"`
 * @see     [RFC 9052](https://datatracker.ietf.org/doc/html/rfc9052)
 */
#define COAP_FORMAT_COSE_MAC                 (97)
/**
 * @brief   Content-Type `application/cose; cose-type="cose-sign"`
 * @see     [RFC 9052](https://datatracker.ietf.org/doc/html/rfc9052)
 */
#define COAP_FORMAT_COSE_SIGN                (98)
/**
 * @brief   Content-Type `application/cose-key`
 * @see     [RFC 9052](https://datatracker.ietf.org/doc/html/rfc9052)
 */
#define COAP_FORMAT_COSE_KEY                (101)
/**
 * @brief   Content-Type `application/cose-key-set`
 * @see     [RFC 9052](https://datatracker.ietf.org/doc/html/rfc9052)
 */
#define COAP_FORMAT_COSE_KEY_SET            (102)
#define COAP_FORMAT_SENML_JSON              (110)
#define COAP_FORMAT_SENSML_JSON             (111)
#define COAP_FORMAT_SENML_CBOR              (112)
#define COAP_FORMAT_SENSML_CBOR             (113)
#define COAP_FORMAT_SENML_EXI               (114)
#define COAP_FORMAT_SENSML_EXI              (115)
/**
 * @brief   Content-Type `application/yang-data+cbor; id=sid`
 * @see     [RFC 9254](https://datatracker.ietf.org/doc/html/rfc9254)
 */
#define COAP_FORMAT_YANG_DATA_CBOR_SID      (140)
/**
 * @brief   Content-Type `application/coap-group+json`
 * @see     [RFC 7390](https://datatracker.ietf.org/doc/html/rfc7390)
 */
#define COAP_FORMAT_COAP_GROUP_JSON         (256)
/**
 * @brief   Content-Type `application/concise-problem-details+cbor`
 * @see     [RFC 9290](https://datatracker.ietf.org/doc/html/rfc9290)
 */
#define COAP_FORMAT_PROBLEM_DETAILS_CBOR    (257)
/**
 * @brief   Content-Type `application/swid+cbor`
 * @see     [RFC 9393](https://datatracker.ietf.org/doc/html/rfc9393)
 */
#define COAP_FORMAT_SWID_CBOR               (258)
/**
 * @brief   Content-Type `application/pkixcmp`
 * @see     [draft-ietf-ace-cmpv2-coap-transport](https://datatracker.ietf.org/doc/draft-ietf-ace-cmpv2-coap-transport/)
 * @see     [RFC 4210](https://datatracker.ietf.org/doc/html/rfc4210)
 */
#define COAP_FORMAT_PKIXCMP                 (259)
/**
 * @brief   Content-Type `application/dots+cbor`
 * @see     [RFC 9132](https://datatracker.ietf.org/doc/html/rfc9132)
 */
#define COAP_FORMAT_DOTS_CBOR               (271)
/**
 * @brief   Content-Type `application/missing-blocks+cbor-seq`
 * @see     [RFC 9177](https://datatracker.ietf.org/doc/html/rfc9177)
 */
#define COAP_FORMAT_MISSING_BLOCKS_CBOR_SEQ (272)
/**
 * @brief   Content-Type `application/pkcs7-mime; smime-type=server-generated-key`
 * @see     [RFC 7030](https://datatracker.ietf.org/doc/html/rfc7030)
 * @see     [RFC 8551](https://datatracker.ietf.org/doc/html/rfc8551)
 * @see     [RFC 9148](https://datatracker.ietf.org/doc/html/rfc9148)
 */
#define COAP_FORMAT_PKCS7_MIME_SERVER_GEN   (280)
/**
 * @brief   Content-Type `application/pkcs7-mime; smime-type=certs-only`
 * @see     [RFC 8551](https://datatracker.ietf.org/doc/html/rfc8551)
 * @see     [RFC 9148](https://datatracker.ietf.org/doc/html/rfc9148)
 */
#define COAP_FORMAT_PKCS7_MIME_CERTS_ONLY   (281)
/**
 * @brief   Content-Type `application/pkcs8`
 * @see     [RFC 5958](https://datatracker.ietf.org/doc/html/rfc5958)
 * @see     [RFC 8551](https://datatracker.ietf.org/doc/html/rfc8551)
 * @see     [RFC 9148](https://datatracker.ietf.org/doc/html/rfc9148)
 */
#define COAP_FORMAT_PKCS8                   (284)
/**
 * @brief   Content-Type `application/csrattrs`
 * @see     [RFC 7030](https://datatracker.ietf.org/doc/html/rfc7030)
 * @see     [RFC 9148](https://datatracker.ietf.org/doc/html/rfc9148)
 */
#define COAP_FORMAT_CSRATTRS                (285)
/**
 * @brief   Content-Type `application/pkcs10`
 * @see     [RFC 5967](https://datatracker.ietf.org/doc/html/rfc5967)
 * @see     [RFC 8551](https://datatracker.ietf.org/doc/html/rfc8551)
 * @see     [RFC 9148](https://datatracker.ietf.org/doc/html/rfc9148)
 */
#define COAP_FORMAT_PKCS10                  (286)
/**
 * @brief   Content-Type `application/pkix-cert`
 * @see     [RFC 2585](https://datatracker.ietf.org/doc/html/rfc2585)
 * @see     [RFC 9148](https://datatracker.ietf.org/doc/html/rfc9148)
 */
#define COAP_FORMAT_PKIX_CERT               (287)
/**
 * @brief   Content-Type `application/aif+cbor`
 * @see     [RFC 9237](https://datatracker.ietf.org/doc/html/rfc9237)
 */
#define COAP_FORMAT_AIF_CBOR                (290)
/**
 * @brief   Content-Type `application/aif+json`
 * @see     [RFC 9237](https://datatracker.ietf.org/doc/html/rfc9237)
 */
#define COAP_FORMAT_AIF_JSON                (291)
#define COAP_FORMAT_SENML_XML               (310)
#define COAP_FORMAT_SENSML_XML              (311)
/**
 * @brief   Content-Type `application/senml-etch+json`
 * @see     [RFC 8790](https://datatracker.ietf.org/doc/html/rfc8790)
 */
#define COAP_FORMAT_SNML_ETCH_JSON          (320)
/**
 * @brief   Content-Type `application/senml-etch+cbor`
 * @see     [RFC 8790](https://datatracker.ietf.org/doc/html/rfc8790)
 */
#define COAP_FORMAT_SNML_ETCH_CBOR          (322)
/**
 * @brief   Content-Type `application/yang-data+cbor`
 * @see     [RFC 9254](https://datatracker.ietf.org/doc/html/rfc9254)
 */
#define COAP_FORMAT_YAML_DATA_CBOR          (340)
/**
 * @brief   Content-Type `application/yang-data+cbor; id=name`
 * @see     [RFC 9254](https://datatracker.ietf.org/doc/html/rfc9254)
 */
#define COAP_FORMAT_YAML_DATA_CBOR_ID_NAME  (341)
/**
 * @brief   Content-Type `application/td+json`
 * @see     [Web of Things (WoT) Thing Description 1.1](https://www.w3.org/TR/wot-thing-description11/)
 */
#define COAP_FORMAT_TD_JSON                 (432)
/**
 * @brief   Content-Type `application/tm+json`
 * @see     [Web of Things (WoT) Thing Description 1.1](https://www.w3.org/TR/wot-thing-description11/)
 */
#define COAP_FORMAT_TM_JSON                 (433)
/**
 * @brief   Content-Type `application/dns-message`
 * @see     [draft-ietf-core-dns-over-coap](https://datatracker.ietf.org/doc/draft-ietf-core-dns-over-coap/)
 */
#define COAP_FORMAT_DNS_MESSAGE             (553)
/**
 * @brief   Content-Type `application/voucher-cose+cbor`
 * @see     [draft-ietf-anima-constrained-voucher](https://datatracker.ietf.org/doc/draft-ietf-anima-constrained-voucher/)
 * @note    Temporary registration until April 12, 2024.
 */
#define COAP_FORMAT_VOUCER_COSE_CBOR        (836)
/**
 * @brief   Content-Type `application/vnd.ocf+cbor`
 */
#define COAP_FORMAT_VND_OCF_CBOR          (10000)
/**
 * @brief   Content-Type `application/oscore`
 * @see     [RFC 8613](https://datatracker.ietf.org/doc/html/rfc8613)
 */
#define COAP_FORMAT_OSCORE                (10001)
/**
 * @brief   Content-Type `application/javascript`
 * @see     [RFC 4329](https://datatracker.ietf.org/doc/html/rfc4329)
 */
#define COAP_FORMAT_JAVASCRIPT            (10002)
/**
 * @brief   Content-Type `application/json` with Content Coding `deflate`
 * @see     [RFC 8259](https://datatracker.ietf.org/doc/html/rfc8259)
 * @see     [RFC 9110,  Section 8.4.1.2](https://datatracker.ietf.org/doc/html/rfc9110)
 */
#define COAP_FORMAT_JSON_DEFLATE          (11050)
/**
 * @brief   Content-Type `application/cbor` with Content Coding `deflate`
 * @see     [RFC 8949](https://datatracker.ietf.org/doc/html/rfc8949)
 * @see     [RFC 9110,  Section 8.4.1.2](https://datatracker.ietf.org/doc/html/rfc9110)
 */
#define COAP_FORMAT_CBOR_DEFLATE          (11060)
/**
 * @brief   Content-Type `application/vnd.oma.lwm2m+tlv`
 * @see     [OMA-TS-LightweightM2M-V1_0](https://www.openmobilealliance.org/release/LightweightM2M/V1_0-20170208-A/OMA-TS-LightweightM2M-V1_0-20170208-A.pdf)
 */
#define COAP_FORMAT_VND_OMA_LWM2M_TLV     (11542)
/**
 * @brief   Content-Type `application/vnd.oma.lwm2m+json`
 * @see     [OMA-TS-LightweightM2M-V1_0](https://www.openmobilealliance.org/release/LightweightM2M/V1_0-20170208-A/OMA-TS-LightweightM2M-V1_0-20170208-A.pdf)
 */
#define COAP_FORMAT_VND_OMA_LWM2M_JSON    (11543)
/**
 * @brief   Content-Type `application/vnd.oma.lwm2m+cbor`
 * @see     [OMA-TS-LightweightM2M-V1_2](https://www.openmobilealliance.org/release/LightweightM2M/V1_2-20201110-A/HTML-Version/OMA-TS-LightweightM2M_Core-V1_2-20201110-A.html)
 */
#define COAP_FORMAT_VND_OMA_LWM2M_CBOR    (11544)
/**
 * @brief   Content-Type `text/css`
 * @see     https://datatracker.ietf.org/doc/html/rfc2318
 */
#define COAP_FORMAT_TEXT_CSS              (20000)
/**
 * @brief   Content-Type `image/svg+xml`
 * @see     [RFC 2318](https://www.w3.org/TR/SVG/mimereg.html)
 */
#define COAP_FORMAT_IMAGE_SVG_XML         (30000)
/** @} */

/**
 * @name    Observe (RFC 7641) constants
 * @{
 */
#define COAP_OBS_REGISTER        (0)
#define COAP_OBS_DEREGISTER      (1)
#define COAP_OBS_MAX_VALUE_MASK  (0xffffff) /**< observe value is 24 bits */
/** @} */

/**
 * @name    CoAP message format constants
 * @{
 */
#define COAP_TOKEN_LENGTH_MAX    (8)
/** @} */

/**
 * @name    CoAP option constants
 * @{
 */
#define COAP_ETAG_LENGTH_MAX     (8U)   /**< maximum length of the ETag option */
/** @} */

/**
 * @brief   Marks the boundary between header and payload
 */
#define COAP_PAYLOAD_MARKER      (0xFF)
#define COAP_PAYLOAD_MARKER_SIZE (1U)   /**< Size of the payload marker */

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
 * @brief    Timeout in milliseconds for a response to a confirmable request
 *
 * This value is for the response to the *initial* confirmable message. The
 * timeout doubles for subsequent retries. To avoid synchronization of resends
 * across hosts, the actual timeout is chosen randomly between
 * @ref CONFIG_COAP_ACK_TIMEOUT_MS and
 * (@ref CONFIG_COAP_ACK_TIMEOUT_MS * @ref CONFIG_COAP_RANDOM_FACTOR_1000 / 1000).
 */
#ifndef CONFIG_COAP_ACK_TIMEOUT_MS
#define CONFIG_COAP_ACK_TIMEOUT_MS     (2000)
#endif

/**
 * @brief   Used to calculate upper bound for timeout
 *
 * This represents the `ACK_RANDOM_FACTOR`
 * ([RFC 7252, section 4.2](https://tools.ietf.org/html/rfc7252#section-4.2))
 * multiplied by 1000, to avoid floating point arithmetic.
 *
 * See @ref CONFIG_COAP_ACK_TIMEOUT_MS
 */
#ifndef CONFIG_COAP_RANDOM_FACTOR_1000
#define CONFIG_COAP_RANDOM_FACTOR_1000      (1500)
#endif

/**
 * @brief    Timeout in milliseconds for a separate (deferred) response
 *           sent after an empty ACK.
 */
#ifndef CONFIG_COAP_SEPARATE_RESPONSE_TIMEOUT_MS
#define CONFIG_COAP_SEPARATE_RESPONSE_TIMEOUT_MS    (10 * MS_PER_SEC)
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

/**
 * @brief Coap block-wise-transfer size SZX
 */
typedef enum {
    COAP_BLOCKSIZE_16 = 0,
    COAP_BLOCKSIZE_32,
    COAP_BLOCKSIZE_64,
    COAP_BLOCKSIZE_128,
    COAP_BLOCKSIZE_256,
    COAP_BLOCKSIZE_512,
    COAP_BLOCKSIZE_1024,
} coap_blksize_t;

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* NET_COAP_H */

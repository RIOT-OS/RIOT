/*
 * Copyright (C) 2023 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_dns_cbor DNS CBOR message parser and composer
 * @ingroup     net_dns
 * @brief       Parsing and composition of DNS messages in CBOR format
 * @see         [draft-lenders-dns-cbor](https://datatracker.ietf.org/doc/draft-lenders-dns-cbor/)
 * @experimental
 * @{
 *
 * @file
 * @brief   CBOR+DNS definitions
 *
 * @author  Martine S. Lenders <martine.lenders@tu-dresden.de>
 */
#ifndef NET_DNS_CBOR_H
#define NET_DNS_CBOR_H

#include <stdint.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup net_dns_cbor_conf CBOR+DNS message parsing and composition configuration
 * @ingroup  config
 * @{
 */
#define CONFIG_DNS_CBOR_LEN         (128U)  /**< maximum DNS message length */
#define CONFIG_DNS_CBOR_NAME_LEN    (32U)   /**< maximum domain name length in CBOR+DNS questions */
#define CONFIG_DNS_CBOR_OUT_LEN     (16U)   /**< maximum length for parsing out buffer */
#define CONFIG_DNS_CBOR_NAME_COMPS  (12U)   /**< expected total name components in CBOR+DNS responses */
/** @} */

/**
 * @brief Parsing status and composition input/output
 */
typedef struct {
    /**
     * @brief The question within the query
     */
    struct {
        char domain_name[CONFIG_DNS_CBOR_NAME_LEN];     /**< The queried domain name*/
        uint16_t type;                                  /**< The queried record type */
        uint16_t class;                                 /**< The queried record class */
    } question;
} dns_cbor_status_t;

#ifdef __cplusplus
}
#endif

/**
 * @brief   Composes a DNS query message
 *
 * @pre @p buf != NULL
 * @pre @p buf fits CONFIG_DNS_CBOR_LEN bytes.
 * @pre @p status != NULL
 *
 * @param[in,out] status    The question to put into the query on in.
 *                          Query status for the response parser on out.
 *                          Must not be NULL.
 * @param[out] buf          A buffer of length @ref CONFIG_DNS_CBOR_LEN
 *                          Must not be NULL.
 *
 * @return  Size of the composed query in bytes.
 * @return  -ENOBUFS, if resulting message does not fit @p dns_buf
 */
ssize_t dns_cbor_compose_query(dns_cbor_status_t *status, void *buf);


/**
 * @brief   Parse a DNS response message
 *
 * @pre @p buf != NULL
 * @pre @p status != NULL
 * @pre @p out != NULL
 * @pre @p out fits CONFIG_DNS_CBOR_OUT_LEN bytes.
 * @pre @p ttl != NULL
 *
 * @param[in] buf           The CBOR+DNS response message to parse. Must not be NULL.
 * @param[in] len           The length of @p buf.
 * @param[in,out] status    Query status on in (see @ref dns_cbor_compose_query()).
 *                          The name, type, and class for @p out on out (may be changed since in,
 *                          e.g. if ANY record was queried).
 * @param[out] out          The record data for the queried name, type, and class.
 *                          Must fit CONFIG_DNS_CBOR_OUT_LEN bytes.
 * @param[out] ttl          The TTL for @p out.
 *
 * @return  Size of @p out on success in bytes.
 * @return  -EINVAL, on CBOR type error.
 * @return  -EBADMSG, if the response does not contain any record data for the queried name, type,
 *          and class.
 * @return  -ENOBUFS, if name, type, and class was found in answer but rdata is too
 *          large to be copied into @p out.
 * @return  -ENOMEM, if system resources are insufficient to parse the message.
 * @return  -ENOSPC, if name in response is too long to be stored in @p status.
 */
ssize_t dns_cbor_parse_response(const void *buf, size_t len, dns_cbor_status_t *status,
                                void *out, uint32_t *ttl);

#endif /* NET_DNS_CBOR_H */
/** @} */

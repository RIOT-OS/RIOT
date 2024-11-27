/*
 * Copyright (C) 2019 Freie Universität Berlin
 * Copyright (C) 2023 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup net_dns
 * @brief
 * @{
 *
 * @file
 * @brief   Generic DNS values
 *
 * @author  Martine S. Lenders <martine.lenders@tu-dresden.de>
 */
#ifndef NET_DNS_TYPES_H
#define NET_DNS_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name DNS defines
 * @{
 */
#define DNS_TYPE_A              1U      /**< Host IPv4 address */
#define DNS_TYPE_AAAA           28U     /**< Host IPv6 address */
#define DNS_CLASS_IN            1U      /**< Internet class */
/** @} */

/**
 * @name    Field lengths
 * @{
 */
#define RR_TYPE_LENGTH      2U          /**< Length of the record type field in bytes */
#define RR_CLASS_LENGTH     2U          /**< Length of the record class field in bytes */
#define RR_TTL_LENGTH       4U          /**< Length of the record TTL field in bytes */
#define RR_RDLENGTH_LENGTH  2U          /**< Length of the record data length field in bytes */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* NET_DNS_TYPES_H */
/** @} */

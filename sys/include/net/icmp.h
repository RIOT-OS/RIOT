/*
 * SPDX-FileCopyrightText: 2015 José Ignacio Alamos <jialamos@uc.cl>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    net_icmp  ICMPV4
 * @ingroup     net_ipv4
 * @brief       Provides types related to ICMPv4
 * @see         <a href="https://tools.ietf.org/html/rfc792">
 *                  RFC 792
 *              </a>
 * @{
 *
 * @file
 * @brief   ICMPv4 type and function definitions
 *
 * @author  José Ignacio Alamos <jialamos@uc.cl>
 */

#include "byteorder.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Echo request and response message format.
 *
 * @see <a href="https://tools.ietf.org/html/rfc792#page-14">
 *          RFC 792, page 14
 *      </a>
 */
typedef struct __attribute__((packed)){
    uint8_t type;           /**< message type */
    uint8_t code;           /**< message code */
    network_uint16_t csum;  /**< checksum */
    network_uint16_t id;    /**< identifier */
    network_uint16_t sn;    /**< sequence number */
} icmp_echo_t;

#ifdef __cplusplus
}
#endif

/** @} */

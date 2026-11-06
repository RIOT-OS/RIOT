/*
 * SPDX-FileCopyrightText: 2016 Cenk Gündoğan <cenk.guendogan@haw-hamburg.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup net_gnrc_rpl_p2p
 * @{
 *
 * @file
 * @brief       P2P-RPL data structs
 *
 * Header file, which defines all structs used by P2P-RPL.
 *
 * @author      Cenk Gündoğan <cenk.guendogan@haw-hamburg.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "net/ipv6/addr.h"
#include "net/gnrc/rpl/structs.h"

/**
 * @brief   Address vector length in RDO DIO options and maximal hop count for the P2P-DODAG
 */
#define GNRC_RPL_P2P_ADDR_VEC_NUMOF    (8)

/**
 * @brief P2P Route Discovery Object (RDO) Option
 * @see <a href="https://tools.ietf.org/html/rfc6997#section-7">
 *          RFC 6997, section 7, P2P Route Discovery Option (P2P-RDO)
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t type;                       /**< Option Type: 0x0a */
    uint8_t length;                     /**< length of option, not including first two bytes */
    uint8_t compr_flags;                /**< flags and number of elided prefix octets */
    uint8_t lmn;                        /**< lifetime, maxrank/nexthop */
    ipv6_addr_t target;                 /**< target address */
} gnrc_rpl_p2p_opt_rdo_t;

/**
 * @brief P2P Discovery Reply Object (P2P-DRO)
 * @see <a href="https://tools.ietf.org/html/rfc6997#section-8">
 *          RFC 6997, section 8, P2P Discovery Reply Object (P2P-DRO)
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t instance_id;        /**< id of the instance */
    uint8_t version_number;     /**< version number of the DODAG */
    network_uint16_t flags_rev; /**< flags and reserved */
    ipv6_addr_t dodag_id;       /**< id of the dodag */
} gnrc_rpl_p2p_dro_t;

/**
 * @brief P2P Discovery Reply Object Acknowledgement (P2P-DRO-ACK)
 * @see <a href="https://tools.ietf.org/html/rfc6997#section-10">
 *          RFC 6997, section 10, P2P Discovery Reply Object Acknowledgement (P2P-DRO-ACK)
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t instance_id;        /**< id of the instance */
    uint8_t version_number;     /**< version number of the DODAG */
    network_uint16_t seq_rev;   /**< sequence number and reserved */
    ipv6_addr_t dodag_id;       /**< id of the dodag */
} gnrc_rpl_p2p_dro_ack_t;

/**
 * @brief Extended DODAG information for P2P-RPL
 */
typedef struct {
    bool state;             /**< state: used / unused */
    gnrc_rpl_dodag_t *dodag;/**< DODAG, which owns this P2P extension */
    uint8_t compr;          /**< number of elided prefix octets */
    uint8_t routes_numof;   /**< number of requested routes */
    bool hop_by_hop;        /**< request hop-by-hop routes or source routes */
    bool stop;              /**< stop route discovery */
    bool reply;             /**< request P2P-DRO */
    bool dro_ack;           /**< request P2P-DRO-ACK */
    uint8_t lifetime_enc;   /**< encoded lifetime of the P2P-DODAG */
    int8_t lifetime_sec;    /**< lifetime of the P2P-DODAG in seconds */
    uint8_t maxrank;        /**< maximum rank the P2P-DODAG should span */
    uint8_t dro_seq;        /**< sequence number of the P2P-DRO */
    ipv6_addr_t target;     /**< target of the P2P route discovery */
    bool for_me;            /**< true if this node is the target */
    uint8_t addr_numof;     /**< number of addresses in the address vector */
    int8_t dro_delay;       /**< delay DRO after it was requested in seconds */
    ipv6_addr_t addr_vec[GNRC_RPL_P2P_ADDR_VEC_NUMOF];   /**< address vector */
} gnrc_rpl_p2p_ext_t;

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

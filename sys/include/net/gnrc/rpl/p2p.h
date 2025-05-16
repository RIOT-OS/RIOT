/*
 * Copyright (C) 2016 Cenk Gündoğan <mail@cgundogan.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    net_gnrc_rpl_p2p Reactive Discovery of P2P Routes in LLNs
 * @ingroup     net_gnrc_rpl
 * @brief       Implementation of P2P-RPL
 * @see <a href="https://tools.ietf.org/html/rfc6997">
 *          RFC 6997
 *      </a>
 * @{
 *
 * @file
 * @brief       Definititions for P2P-RPL
 *
 * @author  Cenk Gündoğan <mail@cgundogan.de>
 */

#include "net/ipv6/addr.h"
#include "net/gnrc.h"
#include "net/gnrc/rpl/structs.h"
#include "net/gnrc/rpl/p2p_structs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   P2P-RPL Mode of Operation
 */
#define GNRC_RPL_P2P_MOP        (0x04)

/**
 * @brief   Default lifetime of the P2P-RPL DODAG, encoded
 * @see <a href="https://tools.ietf.org/html/rfc6997#section-7">
 *          RFC 6997, section 7, P2P Route Discovery Option (P2P-RDO)
 *      </a>
 */
#define GNRC_RPL_P2P_LIFETIME   (0x02)

/**
 * @brief   Number of elided prefix octets from the target field and address vector
 * @see <a href="https://tools.ietf.org/html/rfc6997#section-7">
 *          RFC 6997, section 7, P2P Route Discovery Option (P2P-RDO)
 *      </a>
 */
#define GNRC_RPL_P2P_COMPR      (0)

/**
 * @brief   Maximum rank in the DODAG during the route discovery
 * @see <a href="https://tools.ietf.org/html/rfc6997#section-7">
 *          RFC 6997, section 7, P2P Route Discovery Option (P2P-RDO)
 *      </a>
 */
#define GNRC_RPL_P2P_MAX_RANK   (0)

/**
 * @name Trickle parameters
 * @see <a href="https://tools.ietf.org/html/rfc6997#section-6.1">
 *          RFC 6997, section 6.1, Setting a P2P Mode DIO
 *      </a>
 * @{
 */
#define GNRC_RPL_P2P_DEFAULT_DIO_INTERVAL_MIN           (6)
#define GNRC_RPL_P2P_DEFAULT_DIO_REDUNDANCY_CONSTANT    (1)
/** @} */

/**
 * @name Default parent and route entry lifetime
 * default lifetime will be multiplied by the lifetime unit to obtain the resulting lifetime
 * @{
 */
#define GNRC_RPL_P2P_DEFAULT_LIFETIME   (0xFF)
#define GNRC_RPL_P2P_LIFETIME_UNIT      (0xFFFF)
/** @} */

/**
 * @brief   P2P-RPL RDO DIO option type
 * @see     <a href="https://tools.ietf.org/html/rfc6997#section-7">
 *              RFC 6997, section 7, P2P Route Discovery Option (P2P-RDO)
 *          </a>
 */
#define GNRC_RPL_P2P_OPT_RDO    (0x0A)

/**
 *  @brief  DRO ICMPv6 code
 *  @see <a href="https://tools.ietf.org/html/rfc6997#section-8">
 *          RFC 6997, section 8, The P2P Discovery Reply Object (P2P-DRO)
 *      </a>
 */
#define GNRC_RPL_P2P_ICMPV6_CODE_DRO    (0x04)

/**
 *  @brief  DRO-ACK ICMPv6 code
 *  @see <a href="https://tools.ietf.org/html/rfc6997#section-10">
 *          RFC 6997, section 10, The P2P Discovery Reply Object Acknowledgement (P2P-DRO-ACK)
 *      </a>
 */
#define GNRC_RPL_P2P_ICMPV6_CODE_DRO_ACK    (0x05)

/**
 *  @brief  Time in seconds to wait before sending a DRO
 */
#define GNRC_RPL_P2P_DRO_DELAY  (4)

/**
 *  @brief  Message type for handling DRO sending
 */
#define GNRC_RPL_P2P_MSG_TYPE_DRO_HANDLE    (0x09A0)

/**
 *  @brief  Lookup table used to decode/encode the lifetime values
 */
extern const uint8_t gnrc_rpl_p2p_lifetime_lookup[4];

/**
 * @brief Initialization of a P2P-RPL Instance as root node.
 *
 * @param[in] instance_id       Id of the instance
 * @param[in] dodag_id          Id of the DODAG
 * @param[in] target            Target of the P2P-RPL routes discovery
 * @param[in] gen_inst_id       Flag indicating whether to generate a local instance id.
 *                              If true, @p instance_id will be ignored
 *
 * @return  Pointer to the new Instance, on success.
 * @return  NULL, otherwise.
 */
gnrc_rpl_instance_t *gnrc_rpl_p2p_root_init(uint8_t instance_id, ipv6_addr_t *dodag_id,
                                            ipv6_addr_t *target, bool gen_inst_id);

/**
 * @brief Build an RDO
 *
 * @param[in,out] pkt           The RDO will be added to the @p pkt
 * @param[in] p2p_ext           Pointer to the P2P-RPL DODAG extension
 */
gnrc_pktsnip_t *gnrc_rpl_p2p_rdo_build(gnrc_pktsnip_t *pkt, gnrc_rpl_p2p_ext_t *p2p_ext);

/**
 * @brief Parse an RDO
 *
 * @param[in] rdo               The RDO to parse from.
 * @param[in] p2p_ext           Pointer to the P2P-RPL DODAG extension
 */
void gnrc_rpl_p2p_rdo_parse(gnrc_rpl_p2p_opt_rdo_t *rdo, gnrc_rpl_p2p_ext_t *p2p_ext);

/**
 * @brief Send a DRO control message
 *
 * @param[in] pkt               The ICMPv6 packet to send. Can be NULL.
 * @param[in] p2p_ext           Pointer to the P2P-RPL DODAG extension
 */
void gnrc_rpl_p2p_send_DRO(gnrc_pktsnip_t *pkt, gnrc_rpl_p2p_ext_t *p2p_ext);

/**
 * @brief Receive and parse a DRO control message
 *
 * @param[in] pkt               The ICMPv6 packet to parse.
 * @param[in] src               The source address of the IPv6 packet.
 */
void gnrc_rpl_p2p_recv_DRO(gnrc_pktsnip_t *pkt, ipv6_addr_t *src);

/**
 * @brief Updates the lifetime of the P2P Dodag and the delay of the DRO
 */
void gnrc_rpl_p2p_update(void);

#ifdef __cplusplus
}
#endif

/** @} */

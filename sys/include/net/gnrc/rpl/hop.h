/*
 * Copyright (C) 2017 Martin Landsmann <martin.landsmann@haw-hamburg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_rpl_hop RPL Hop-by-Hop header extension
 * @ingroup     net_gnrc_rpl
 * @brief       Implementation of RPL Hop-by-Hop extension headers
 * @see <a href="https://tools.ietf.org/html/rfc6553">
 *          RFC 6553
 *      </a>
 * @{
 *
 * @file
 * @brief       Definitions for Carrying RPL Information in Data-Plane Datagrams
 *
 * @author  Martin Landsmann <martin.landsmann@haw-hamburg.de>
 */
#ifndef GNRC_RPL_HOP_H
#define GNRC_RPL_HOP_H

#include "net/ipv6/hdr.h"
#include "net/ipv6/addr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Type of the Hop-by-Hop Option.
 *
 * @see <a href="https://tools.ietf.org/html/rfc6553#section-6">
 *         RFC6553, section 6
 *      </a>
 */
#define GNRC_RPL_HOP_OPT_TYPE   (0x63)

/**
 * @name   Return types used in processing a hop-by-hop option
 *
 * @{
 */
#define HOP_OPT_SUCCESS                   (0)
#define HOP_OPT_ERR_HEADER_LENGTH         (-1)
#define HOP_OPT_ERR_INCONSISTENCY         (-2)
#define HOP_OPT_ERR_FLAG_R_SET            (-3)
#define HOP_OPT_ERR_FLAG_F_SET            (-4)
#define HOP_OPT_ERR_NOT_FOR_ME            (-5)
/** @} */

/**
 * @name Option flags
 * @see <a href="https://tools.ietf.org/html/rfc6553#section-3">
 *         RFC6553, section 3
 *     </a>
 * @{
 */
#define GNRC_RPL_HOP_OPT_FLAG_O  (1 << 0)
#define GNRC_RPL_HOP_OPT_FLAG_R  (1 << 1)
#define GNRC_RPL_HOP_OPT_FLAG_F  (1 << 2)
/** @} */

/**
 * @brief   The RPL Hop-by-Hop header extension.
 *
 * @see <a href="https://tools.ietf.org/html/rfc6553#section-3">
 *          RFC 6553
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t nh;           /**< next header identifier (0x63) */
    uint8_t len;          /**< length in 8 octets without first octet */
    uint8_t ORF_flags;    /**< O|R|F flags followed by 5 unused bits */
    uint8_t instance_id;  /**< id of the instance */
    uint16_t sender_rank; /**< rank of the sender */
} gnrc_rpl_hop_opt_t;


/**
 * @brief parse the given hop-by-hop option, check for inconsistencies,
 *        adjust the option for further processing and return the result.
 *
 * @param[in,out] hop pointer to the hop-by-hop header option
 *
 * @returns HOP_OPT_SUCCESS on success
 *          HOP_OPT_ERR_HEADER_LENGTH if there was a header length mismatch
 *          HOP_OPT_ERR_INCONSISTENCY if the F flag was already set
 *          HOP_OPT_ERR_FLAG_R_SET if we set the R flag
 *          HOP_OPT_ERR_FLAG_F_SET if we set the F flag
 *          HOP_OPT_ERR_NOT_FOR_ME if the content is not for us
 */
int gnrc_rpl_hop_opt_process(gnrc_rpl_hop_opt_t *hop);

#ifdef __cplusplus
}
#endif

#endif /* GNRC_RPL_HOP_H */
/** @} */

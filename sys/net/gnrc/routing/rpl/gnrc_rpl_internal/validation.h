/*
 * Copyright (C) 2016 Cenk Gündoğan <mail@cgundogan.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_gnrc_rpl
 * @{
 *
 * @file
 * @brief       RPL control message validation functions
 *
 * @author      Cenk Gündoğan <mail@cgundogan.de>
 */

#ifndef GNRC_RPL_VALIDATION_H
#define GNRC_RPL_VALIDATION_H

#ifdef __cplusplus
extern "C" {
#endif

#include "net/gnrc/rpl/structs.h"
#include "net/gnrc/icmpv6.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/**
 * @brief   Checks validity of DIS control messages
 *
 * @param[in]   dis     The DIS control message
 * @param[in]   len     Length of the DIS control message
 *
 * @return  true, if @p dis is valid
 * @return  false, otherwise
 */
static inline bool gnrc_rpl_validation_DIS(gnrc_rpl_dis_t *dis, uint16_t len)
{
    uint16_t expected_len = sizeof(*dis) + sizeof(icmpv6_hdr_t);

    if (expected_len <= len) {
        return true;
    }

    DEBUG("RPL: wrong DIS len: %d, expected: %d\n", len, expected_len);

    return false;
}

/**
 * @brief   Checks validity of control message options
 *
 * @param[in]   msg_type    Type of the control message
 * @param[in]   inst        The RPL instance
 * @param[in]   opt         Options of the control message
 * @param[in]   len         Length of the options
 *
 * @return  true, if @p opt is valid
 * @return  false, otherwise
 */
bool gnrc_rpl_validation_options(int msg_type, gnrc_rpl_instance_t *inst,
                                 gnrc_rpl_opt_t *opt, uint16_t len);

/**
 * @brief   Checks validity of DIO control messages
 *
 * @param[in]   dio         The DIO control message
 * @param[in]   len         Length of the DIO control message
 *
 * @return  true, if @p dio is valid
 * @return  false, otherwise
 */
static inline bool gnrc_rpl_validation_DIO(gnrc_rpl_dio_t *dio, uint16_t len)
{
    uint16_t expected_len = sizeof(*dio) + sizeof(icmpv6_hdr_t);

    if (expected_len <= len) {
        return true;
    }

    DEBUG("RPL: wrong DIO len: %d, expected: %d\n", len, expected_len);

    return false;
}

/**
 * @brief   Checks validity of DAO control messages
 *
 * @param[in]   dao         The DAO control message
 * @param[in]   len         Length of the DAO control message
 *
 * @return  true, if @p dao is valid
 * @return  false, otherwise
 */
static inline bool gnrc_rpl_validation_DAO(gnrc_rpl_dao_t *dao, uint16_t len)
{
    uint16_t expected_len = sizeof(*dao) + sizeof(icmpv6_hdr_t);

    if ((dao->k_d_flags & GNRC_RPL_DAO_D_BIT)) {
        expected_len += sizeof(ipv6_addr_t);
    }

    if (expected_len <= len) {
        return true;
    }

    DEBUG("RPL: wrong DAO len: %d, expected: %d\n", len, expected_len);

    return false;
}

/**
 * @brief   Checks validity of DAO-ACK control messages
 *
 * @param[in]   dao_ack     The DAO-ACK control message
 * @param[in]   len         Length of the DAO-ACK control message
 *
 * @return  true, if @p dao_ack is valid
 * @return  false, otherwise
 */
static inline bool gnrc_rpl_validation_DAO_ACK(gnrc_rpl_dao_ack_t *dao_ack, uint16_t len)
{
    uint16_t expected_len = sizeof(*dao_ack) + sizeof(icmpv6_hdr_t);

    if ((dao_ack->d_reserved & GNRC_RPL_DAO_ACK_D_BIT)) {
        expected_len += sizeof(ipv6_addr_t);
    }

    if (expected_len == len) {
        return true;
    }

    DEBUG("RPL: wrong DAO-ACK len: %d, expected: %d\n", len, expected_len);

    return false;
}

#ifdef __cplusplus
}
#endif

#endif /* GNRC_RPL_VALIDATION_H */
/** @} */

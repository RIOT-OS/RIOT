/*
 * Copyright (C) 2016 Cenk Gündoğan <cenk.guendogan@haw-hamburg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @{
 *
 * @file
 *
 * @author  Cenk Gündoğan <cenk.guendogan@haw-hamburg.de>
 */

#include "net/gnrc/icmpv6.h"
#include "net/gnrc/ipv6.h"

#include "net/gnrc/rpl.h"
#include "gnrc_rpl_internal/validation.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#if IS_ACTIVE(ENABLE_DEBUG)
static char addr_str[IPV6_ADDR_MAX_STR_LEN];
#endif

bool gnrc_rpl_validation_options(int msg_type, gnrc_rpl_instance_t *inst,
                                 gnrc_rpl_opt_t *opt, uint16_t len)
{
    uint16_t expected_len = 0;

    while(expected_len < len) {
        if (opt->type == GNRC_RPL_OPT_PAD1) {
            expected_len += 1;
            opt = (gnrc_rpl_opt_t *) (((uint8_t *) opt) + 1);
            continue;
        }

        if ((expected_len + sizeof(gnrc_rpl_opt_t)) > len) {
            break;
        }

        switch(opt->type) {
            case (GNRC_RPL_OPT_DODAG_CONF):
                if (msg_type != GNRC_RPL_ICMPV6_CODE_DIO) {
                    DEBUG("RPL: DODAG CONF DIO option not expected\n");
                    return false;
                }

                if (opt->length != GNRC_RPL_OPT_DODAG_CONF_LEN) {
                    DEBUG("RPL: wrong DIO option (DODAG CONF) len: %d, expected: %d\n",
                           opt->length, GNRC_RPL_OPT_DODAG_CONF_LEN);
                    return false;
                }
                break;

            case (GNRC_RPL_OPT_PREFIX_INFO):
                if (msg_type != GNRC_RPL_ICMPV6_CODE_DIO) {
                    DEBUG("RPL: PREFIX INFO DIO option not expected\n");
                    return false;
                }

                if (opt->length != GNRC_RPL_OPT_PREFIX_INFO_LEN) {
                    DEBUG("RPL: wrong DIO option (PREFIX INFO) len: %d, expected: %d\n",
                           opt->length, GNRC_RPL_OPT_PREFIX_INFO_LEN);
                    return false;
                }
                break;

            case (GNRC_RPL_OPT_TARGET):
                if (msg_type != GNRC_RPL_ICMPV6_CODE_DAO) {
                    DEBUG("RPL: RPL TARGET DAO option not expected\n");
                    return false;
                }

                if (opt->length > GNRC_RPL_OPT_TARGET_LEN) {
                    DEBUG("RPL: wrong DAO option (RPL TARGET) len: %d, expected (max): %d\n",
                           opt->length, GNRC_RPL_OPT_TARGET_LEN);
                    return false;
                }
                break;

            case (GNRC_RPL_OPT_TRANSIT):
                if (msg_type != GNRC_RPL_ICMPV6_CODE_DAO) {
                    DEBUG("RPL: RPL TRANSIT INFO DAO option not expected\n");
                    return false;
                }

                uint8_t parent_addr = 0;
                if (inst->mop == GNRC_RPL_MOP_NON_STORING_MODE) {
                    parent_addr = sizeof(ipv6_addr_t);
                }

                if (opt->length != (GNRC_RPL_OPT_TRANSIT_INFO_LEN + parent_addr)) {
                    DEBUG("RPL: wrong DAO option (TRANSIT INFO) len: %d, expected: %d\n",
                           opt->length, (GNRC_RPL_OPT_TRANSIT_INFO_LEN + parent_addr));
                    return false;
                }
                break;

            default:
                break;

        }
        expected_len += opt->length + sizeof(gnrc_rpl_opt_t);

        if (expected_len >= len) {
          break;
        }

        opt = (gnrc_rpl_opt_t *) (((uint8_t *) (opt + 1)) + opt->length);
    }

    if (expected_len == len) {
        return true;
    }

    DEBUG("RPL: wrong options len: %d, expected: %d\n", len, expected_len);

    return false;
}

/**
 * @}
 */

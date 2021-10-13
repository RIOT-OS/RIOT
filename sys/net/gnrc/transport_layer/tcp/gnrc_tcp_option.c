/*
 * Copyright (C) 2015-2017 Simon Brummer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_gnrc
 * @{
 *
 * @file
 * @brief       Implementation of internal/option.h
 *
 * @author      Simon Brummer <simon.brummer@posteo.de>
 * @}
 */
#include "include/gnrc_tcp_common.h"
#include "include/gnrc_tcp_option.h"

#define ENABLE_DEBUG 0
#include "debug.h"

int _gnrc_tcp_option_parse(gnrc_tcp_tcb_t *tcb, tcp_hdr_t *hdr)
{
    TCP_DEBUG_ENTER;
    /* Extract offset value. Return if no options are set */
    uint8_t offset = GET_OFFSET(byteorder_ntohs(hdr->off_ctl));
    if (offset <= TCP_HDR_OFFSET_MIN) {
        TCP_DEBUG_LEAVE;
        return 0;
    }

    /* Get pointer to option field and field size */
    uint8_t *opt_ptr = (uint8_t *) hdr + sizeof(tcp_hdr_t);
    uint8_t opt_left = (offset - TCP_HDR_OFFSET_MIN) * 4;

    /* Parse options via tcp_hdr_opt_t */
    while (opt_left > 0) {
        tcp_hdr_opt_t *option = (tcp_hdr_opt_t *) opt_ptr;

        /* Examine current option */
        switch (option->kind) {
            case TCP_OPTION_KIND_EOL:
                TCP_DEBUG_INFO("EOL option found.");
                TCP_DEBUG_LEAVE;
                return 0;

            case TCP_OPTION_KIND_NOP:
                TCP_DEBUG_INFO("NOP option found.");
                opt_ptr += 1;
                opt_left -= 1;
                continue;

            case TCP_OPTION_KIND_MSS:
                if (opt_left < TCP_OPTION_LENGTH_MIN || option->length > opt_left ||
                    option->length != TCP_OPTION_LENGTH_MSS) {
                    TCP_DEBUG_ERROR("Invalid MSS option length.");
                    TCP_DEBUG_LEAVE;
                    return -1;
                }
                TCP_DEBUG_INFO("MSS option found.");
                tcb->mss = (option->value[0] << 8) | option->value[1];
                break;

            default:
                if (opt_left >= TCP_OPTION_LENGTH_MIN) {
                    TCP_DEBUG_INFO("Valid, unsupported option found.");
                }
        }

        if ((opt_left < TCP_OPTION_LENGTH_MIN) || (option->length > opt_left) ||
            (option->length < TCP_OPTION_LENGTH_MIN)) {
            TCP_DEBUG_ERROR("Invalid option found.");
            TCP_DEBUG_LEAVE;
            return -1;
        }

        opt_ptr += option->length;
        opt_left -= option->length;
    }
    TCP_DEBUG_LEAVE;
    return 0;
}

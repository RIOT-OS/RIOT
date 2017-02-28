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
#include "internal/common.h"
#include "internal/option.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

int _option_parse(gnrc_tcp_tcb_t *tcb, tcp_hdr_t *hdr)
{
    /* Extract Offset value. Return if no options are set */
    uint8_t offset = GET_OFFSET(byteorder_ntohs(hdr->off_ctl));
    if (offset <= TCP_HDR_OFFSET_MIN) {
        return 0;
    }

    /* Get Pointer to option field and field-size */
    uint8_t *opt_ptr = (uint8_t *) hdr + sizeof(tcp_hdr_t);
    uint8_t opt_left = (offset - TCP_HDR_OFFSET_MIN) * 4;

    /* Parse Options via tcp_hdr_opt_t */
    while (opt_left > 0) {
        tcp_hdr_opt_t *option = (tcp_hdr_opt_t *) opt_ptr;

        /* Examine current option */
        switch (option->kind) {
            case TCP_OPTION_KIND_EOL:
                DEBUG("gnrc_tcp_option.c : _option_parse() : EOL option found\n");
                return 0;

            case TCP_OPTION_KIND_NOP:
                DEBUG("gnrc_tcp_option.c : _option_parse() : NOP option found\n");
                opt_ptr += 1;
                opt_left -= 1;
                continue;

            case TCP_OPTION_KIND_MSS:
                if (option->length != TCP_OPTION_LENGTH_MSS) {
                    DEBUG("gnrc_tcp_option.c : _option_parse() : invalid MSS Option length.\n");
                    return -1;
                }
                tcb->mss = (option->value[0] << 8) | option->value[1];
                DEBUG("gnrc_tcp_option.c : _option_parse() : MSS option found. MSS=%"PRIu16"\n",
                      tcb->mss);
                break;

            default:
                DEBUG("gnrc_tcp_option.c : _option_parse() : Unknown option found.\
                      KIND=%"PRIu8", LENGTH=%"PRIu8"\n", option->kind, option->length);
        }
        opt_ptr += option->length;
        opt_left -= option->length;
    }
    return 0;
}

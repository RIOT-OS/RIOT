/*
 * Copyright (C) 2017 Simon Brummer
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
 * @brief       GNRC's TCP option handling related functions
 *
 * @author      Simon Brummer <simon.brummer@posteo.de>
 * @}
 */

#include "internal/common.h"
#include "internal/option.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

uint32_t _option_build_mss(uint16_t mss)
{
    return (((uint32_t )OPT_KIND_MSS) << 24) | (((uint32_t) OPT_LENGTH_MSS) << 16) | mss;
}

uint16_t _option_build_offset_control(uint16_t nopts, uint16_t ctl)
{
    return (nopts << 12) | ctl;
}

int _option_parse(gnrc_tcp_tcb_t* tcb, tcp_hdr_t *hdr)
{
    /* Extract Offset value. Return if no options are set */
    uint8_t offset = GET_OFFSET(byteorder_ntohs(hdr->off_ctl));
    if (offset <= OPTION_OFFSET_BASE) {
        return 0;
    }

    /* Get Pointer to option field and field-size */
    uint8_t* opt_ptr = (uint8_t *) hdr + sizeof(tcp_hdr_t);
    uint8_t opt_left = (offset - OPTION_OFFSET_BASE) * sizeof(network_uint32_t);

    /* Parse Options via tcp_opt_t */
    while (opt_left > 0) {
        tcp_hdr_opt_t* opt = (tcp_hdr_opt_t*) opt_ptr;

        /* Examine current option */
        switch (opt->kind) {
            case OPT_KIND_EOL:
                DEBUG("gnrc_tcp_option.c : _option_parse() : Option eol received\n");
                return 0;

            case OPT_KIND_NOP:
                DEBUG("gnrc_tcp_option.c : _option_parse() : Option nop received\n");
                opt_ptr += 1;
                opt_left -= 1;
                continue;

            case OPT_KIND_MSS:
                DEBUG("gnrc_tcp_option.c : _option_parse() : Option mss received\n");
                if (opt->length != OPT_LENGTH_MSS)
                {
                    DEBUG("gnrc_tcp_option.c : _option_parse() : invalid MSS Option length.\n");
                    return -1;
                }
                opt->data = (opt_ptr + 2);
                tcb->mss = (opt->data[0] << 8) | opt->data[1];
                break;

            default:
                DEBUG("gnrc_tcp_option.c : _option_parse() : Unsupported option received\n");
        }
        opt_ptr += opt->length;
        opt_left -= opt->length;
    }
    return 0;
}

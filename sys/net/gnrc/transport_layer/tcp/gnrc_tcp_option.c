/*
 * Copyright (C) 2015 Simon Brummer
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
 * @author      Simon Brummer <brummer.simon@googlemail.com>
 * @}
 */
#include "assert.h"
#include "internal/option.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

uint32_t _option_build_mss(uint16_t mss)
{
    return (((uint32_t )OPT_KIND_MSS) << 24) | (((uint32_t) OPT_LENGTH_MSS) << 16) | mss;
}

uint16_t _option_build_offset_control(uint16_t nopts, uint16_t ctl)
{
    assert(OPTION_OFFSET_BASE <= nopts && nopts <= OPTION_OFFSET_MAX);
    return (nopts << 12) | ctl;
}

int _option_parse(gnrc_tcp_tcb_t* tcb, tcp_hdr_t *hdr)
{
    uint8_t word_idx = 0;
    uint8_t byte_idx = 0;
    uint8_t word_end = 0;
    uint16_t off_ctl = byteorder_ntohs(hdr->off_ctl);

    word_end = GET_OFFSET(off_ctl) - OPTION_OFFSET_BASE;

    while (word_idx < word_end) {
        uint32_t word = byteorder_ntohl(hdr->options[word_idx]);

        /* If byte index is not aligned to word index. Fill word with bytes from next word. */
        if (byte_idx) {
            word >>= (byte_idx * 8);
            word |= (byteorder_ntohl(hdr->options[word_idx + 1]) << ((sizeof(word) - byte_idx) * 8));
        }

        /* Option handling */
        switch (OPT_GET_KIND(word)) {
            case OPT_KIND_EOL:
                DEBUG("gnrc_tcp_option.c : _option_parse() : Option eol\n");
                return 0;

            case OPT_KIND_NOP:
                byte_idx += 1;
                DEBUG("gnrc_tcp_option.c : _option_parse() : Option nop\n");
                break;

            case OPT_KIND_MSS:
                DEBUG("gnrc_tcp_option.c : _option_parse() : Option mss\n");
                if (OPT_GET_LENGTH(word) == OPT_LENGTH_MSS) {
                    tcb->mss = OPT_GET_VAL_2B(word);
                    byte_idx += 4;
                }
                else {
                    DEBUG("gnrc_tcp_option.c : _option_parse() : invalid MSS Option length.\n");
                    return -1;
                }
                break;

            /* Add options support HERE */
            default:
                DEBUG("gnrc_tcp_option.c : _option_parse() : Unsupported option received\n");
                byte_idx += 1;
        }

        /* Update index */
        if (byte_idx >= 4) {
            word_idx += 1;
            byte_idx -= 4;
        }
    }
    return 0;
}

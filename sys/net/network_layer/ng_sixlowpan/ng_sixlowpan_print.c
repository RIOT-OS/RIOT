/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 */

#include <stdio.h>
#include <inttypes.h>

#include "od.h"
#include "net/ng_ipv6/hdr.h"
#include "net/ng_sixlowpan.h"

void ng_sixlowpan_print(uint8_t *data, size_t size)
{
    if (data[0] == NG_SIXLOWPAN_UNCOMPRESSED) {
        puts("Uncompressed IPv6 packet");

        /* might just be the dispatch (or fragmented) so better check */
        if (size > sizeof(ng_ipv6_hdr_t)) {
            ng_ipv6_hdr_print((ng_ipv6_hdr_t *)(data + 1));
            od_hex_dump(data + sizeof(ng_ipv6_hdr_t) + 1,
                        size - sizeof(ng_ipv6_hdr_t) - 1,
                        OD_WIDTH_DEFAULT);
        }
    }
    else if (ng_sixlowpan_nalp(data[0])) {
        puts("Not a LoWPAN (NALP) frame");
        od_hex_dump(data, size, OD_WIDTH_DEFAULT);
    }
    else if ((data[0] & NG_SIXLOWPAN_FRAG_DISP_MASK) == NG_SIXLOWPAN_FRAG_1_DISP) {
        ng_sixlowpan_frag_t *hdr = (ng_sixlowpan_frag_t *)data;

        puts("Fragmentation Header (first)");
        printf("datagram size: %" PRIu16 "\n",
               byteorder_ntohs(hdr->disp_size) & NG_SIXLOWPAN_FRAG_SIZE_MASK);
        printf("tag: 0x%" PRIu16 "\n", byteorder_ntohs(hdr->tag));

        /* Print next dispatch */
        ng_sixlowpan_print(data + sizeof(ng_sixlowpan_frag_t),
                           size - sizeof(ng_sixlowpan_frag_t));
    }
    else if ((data[0] & NG_SIXLOWPAN_FRAG_DISP_MASK) == NG_SIXLOWPAN_FRAG_N_DISP) {
        ng_sixlowpan_frag_n_t *hdr = (ng_sixlowpan_frag_n_t *)data;

        puts("Fragmentation Header (subsequent)");
        printf("datagram size: %" PRIu16 "\n",
               byteorder_ntohs(hdr->disp_size) & NG_SIXLOWPAN_FRAG_SIZE_MASK);
        printf("tag: 0x%" PRIu16 "\n", byteorder_ntohs(hdr->tag));
        printf("offset: 0x%" PRIu8 "\n", hdr->offset);

        od_hex_dump(data + sizeof(ng_sixlowpan_frag_n_t),
                    size - sizeof(ng_sixlowpan_frag_n_t),
                    OD_WIDTH_DEFAULT);
    }
    else if ((data[0] & NG_SIXLOWPAN_IPHC1_DISP_MASK) == NG_SIXLOWPAN_IPHC1_DISP) {
        uint8_t offset = NG_SIXLOWPAN_IPHC_HDR_LEN;
        puts("IPHC dispatch");

        switch (data[0] & NG_SIXLOWPAN_IPHC1_TF) {
            case 0x00:
                puts("TF: ECN + DSCP + Flow Label (4 bytes)");
                break;

            case 0x08:
                puts("TF: ECN + Flow Label (3 bytes)");
                break;

            case 0x10:
                puts("TF: ECN + DSCP (1 bytes)");
                break;

            case 0x18:
                puts("TF: traffic class and flow label elided");
                break;
        }

        switch (data[0] & NG_SIXLOWPAN_IPHC1_NH) {
            case 0x00:
                puts("NH: inline");
                break;

            case 0x04:
                puts("NH: LOWPAN_NHC");
                break;
        }

        switch (data[0] & NG_SIXLOWPAN_IPHC1_HL) {
            case 0x00:
                puts("HLIM: inline");
                break;

            case 0x01:
                puts("HLIM: 1");
                break;

            case 0x02:
                puts("HLIM: 64");
                break;

            case 0x03:
                puts("HLIM: 255");
                break;
        }

        if (data[1] & NG_SIXLOWPAN_IPHC2_SAC) {
            printf("Stateful source address compression: ");

            switch (data[1] & NG_SIXLOWPAN_IPHC2_SAM) {
                case 0x00:
                    puts("unspecified address (::)");
                    break;

                case 0x10:
                    puts("64 bits inline");
                    break;

                case 0x20:
                    puts("16 bits inline");
                    break;

                case 0x40:
                    puts("elided (use L2 address)");
                    break;
            }
        }
        else {
            printf("Stateless source address compression: ");

            switch (data[1] & NG_SIXLOWPAN_IPHC2_SAM) {
                case 0x00:
                    puts("128 bits inline");
                    break;

                case 0x10:
                    puts("64 bits inline");
                    break;

                case 0x20:
                    puts("16 bits inline");
                    break;

                case 0x40:
                    puts("elided (use L2 address)");
                    break;
            }
        }

        if (data[1] & NG_SIXLOWPAN_IPHC2_M) {
            if (data[1] & NG_SIXLOWPAN_IPHC2_DAC) {
                puts("Stateful destinaton multicast address compression:");

                switch (data[1] & NG_SIXLOWPAN_IPHC2_DAM) {
                    case 0x00:
                        puts("    48 bits carried inline (Unicast-Prefix-based)");
                        break;

                    case 0x01:
                    case 0x02:
                    case 0x03:
                        puts("    reserved");
                        break;
                }
            }
            else {
                puts("Stateless destinaton multicast address compression:");

                switch (data[1] & NG_SIXLOWPAN_IPHC2_DAM) {
                    case 0x00:
                        puts("    128 bits carried inline");
                        break;

                    case 0x01:
                        puts("    48 bits carried inline");
                        break;

                    case 0x02:
                        puts("    32 bits carried inline");
                        break;

                    case 0x03:
                        puts("    8 bits carried inline");
                        break;
                }
            }
        }
        else {
            if (data[1] & NG_SIXLOWPAN_IPHC2_DAC) {
                printf("Stateful destinaton address compression: ");

                switch (data[1] & NG_SIXLOWPAN_IPHC2_DAM) {
                    case 0x00:
                        puts("reserved");
                        break;

                    case 0x10:
                        puts("64 bits inline");
                        break;

                    case 0x20:
                        puts("16 bits inline");
                        break;

                    case 0x40:
                        puts("elided (use L2 address)");
                        break;
                }
            }
            else {
                printf("Stateless destinaton address compression: ");

                switch (data[1] & NG_SIXLOWPAN_IPHC2_DAM) {
                    case 0x00:
                        puts("128 bits inline");
                        break;

                    case 0x10:
                        puts("64 bits inline");
                        break;

                    case 0x20:
                        puts("16 bits inline");
                        break;

                    case 0x40:
                        puts("elided (use L2 address)");
                        break;
                }
            }
        }

        if (data[1] & NG_SIXLOWPAN_IPHC2_CID_EXT) {
            offset += NG_SIXLOWPAN_IPHC_CID_EXT_LEN;
            printf("SCI: 0x%" PRIx8 "; DCI: 0x%" PRIx8 "\n",
                   data[2] >> 4, data[2] & 0xf);
        }

        od_hex_dump(data + offset, size - offset, OD_WIDTH_DEFAULT);
    }
}

/** @} */

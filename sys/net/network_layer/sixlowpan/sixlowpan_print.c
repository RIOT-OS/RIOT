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

#include "od.h"
#include "net/ipv6/hdr.h"
#include "net/sixlowpan.h"

void sixlowpan_print(uint8_t *data, size_t size)
{
    if (data[0] == SIXLOWPAN_UNCOMP) {
        printf("Uncompressed IPv6 packet\n");

        /* might just be the dispatch (or fragmented) so better check */
        if (size > sizeof(ipv6_hdr_t)) {
            ipv6_hdr_print((ipv6_hdr_t *)(data + 1));
            od_hex_dump(data + sizeof(ipv6_hdr_t) + 1,
                        size - sizeof(ipv6_hdr_t) - 1,
                        OD_WIDTH_DEFAULT);
        }
    }
    else if (sixlowpan_nalp(data[0])) {
        printf("Not a LoWPAN (NALP) frame\n");
        od_hex_dump(data, size, OD_WIDTH_DEFAULT);
    }
    else if ((data[0] & SIXLOWPAN_FRAG_DISP_MASK) == SIXLOWPAN_FRAG_1_DISP) {
        sixlowpan_frag_t *hdr = (sixlowpan_frag_t *)data;

        printf("Fragmentation Header (first)\n");
        printf("datagram size: %u\n",
               (byteorder_ntohs(hdr->disp_size) & SIXLOWPAN_FRAG_SIZE_MASK));
        printf("tag: 0x%04x\n", byteorder_ntohs(hdr->tag));

        /* Print next dispatch */
        sixlowpan_print(data + sizeof(sixlowpan_frag_t),
                           size - sizeof(sixlowpan_frag_t));
    }
    else if ((data[0] & SIXLOWPAN_FRAG_DISP_MASK) == SIXLOWPAN_FRAG_N_DISP) {
        sixlowpan_frag_n_t *hdr = (sixlowpan_frag_n_t *)data;

        printf("Fragmentation Header (subsequent)\n");
        printf("datagram size: %u\n",
               (byteorder_ntohs(hdr->disp_size) & SIXLOWPAN_FRAG_SIZE_MASK));
        printf("tag: 0x%04x\n", byteorder_ntohs(hdr->tag));
        printf("offset: %u\n", (unsigned)hdr->offset);

        od_hex_dump(data + sizeof(sixlowpan_frag_n_t),
                    size - sizeof(sixlowpan_frag_n_t),
                    OD_WIDTH_DEFAULT);
    }
    else if ((data[0] & SIXLOWPAN_IPHC1_DISP_MASK) == SIXLOWPAN_IPHC1_DISP) {
        uint8_t offset = SIXLOWPAN_IPHC_HDR_LEN;
        printf("IPHC dispatch\n");

        switch (data[0] & SIXLOWPAN_IPHC1_TF) {
            case 0x00:
                printf("TF: ECN + DSCP + Flow Label (4 bytes)\n");
                break;

            case 0x08:
                printf("TF: ECN + Flow Label (3 bytes)\n");
                break;

            case 0x10:
                printf("TF: ECN + DSCP (1 bytes)\n");
                break;

            case 0x18:
                printf("TF: traffic class and flow label elided\n");
                break;
        }

        switch (data[0] & SIXLOWPAN_IPHC1_NH) {
            case 0x00:
                printf("NH: inline\n");
                break;

            case 0x04:
                printf("NH: LOWPAN_NHC\n");
                break;
        }

        switch (data[0] & SIXLOWPAN_IPHC1_HL) {
            case 0x00:
                printf("HLIM: inline\n");
                break;

            case 0x01:
                printf("HLIM: 1\n");
                break;

            case 0x02:
                printf("HLIM: 64\n");
                break;

            case 0x03:
                printf("HLIM: 255\n");
                break;
        }

        if (data[1] & SIXLOWPAN_IPHC2_SAC) {
            printf("Stateful source address compression: ");

            switch (data[1] & SIXLOWPAN_IPHC2_SAM) {
                case 0x00:
                    printf("unspecified address (::)\n");
                    break;

                case 0x10:
                    printf("64 bits inline\n");
                    break;

                case 0x20:
                    printf("16 bits inline\n");
                    break;

                case 0x30:
                    printf("elided (use L2 address)\n");
                    break;
            }
        }
        else {
            printf("Stateless source address compression: ");

            switch (data[1] & SIXLOWPAN_IPHC2_SAM) {
                case 0x00:
                    printf("128 bits inline\n");
                    break;

                case 0x10:
                    printf("64 bits inline\n");
                    break;

                case 0x20:
                    printf("16 bits inline\n");
                    break;

                case 0x30:
                    printf("elided (use L2 address)\n");
                    break;
            }
        }

        if (data[1] & SIXLOWPAN_IPHC2_M) {
            if (data[1] & SIXLOWPAN_IPHC2_DAC) {
                printf("Stateful destination multicast address compression:\n");

                switch (data[1] & SIXLOWPAN_IPHC2_DAM) {
                    case 0x00:
                        printf("    48 bits carried inline (Unicast-Prefix-based)\n");
                        break;

                    case 0x01:
                    case 0x02:
                    case 0x03:
                        printf("    reserved\n");
                        break;
                }
            }
            else {
                printf("Stateless destination multicast address compression:\n");

                switch (data[1] & SIXLOWPAN_IPHC2_DAM) {
                    case 0x00:
                        printf("    128 bits carried inline\n");
                        break;

                    case 0x01:
                        printf("    48 bits carried inline\n");
                        break;

                    case 0x02:
                        printf("    32 bits carried inline\n");
                        break;

                    case 0x03:
                        printf("    8 bits carried inline\n");
                        break;
                }
            }
        }
        else {
            if (data[1] & SIXLOWPAN_IPHC2_DAC) {
                printf("Stateful destination address compression: ");

                switch (data[1] & SIXLOWPAN_IPHC2_DAM) {
                    case 0x00:
                        printf("reserved\n");
                        break;

                    case 0x01:
                        printf("64 bits inline\n");
                        break;

                    case 0x02:
                        printf("16 bits inline\n");
                        break;

                    case 0x03:
                        printf("elided (use L2 address)\n");
                        break;
                }
            }
            else {
                printf("Stateless destination address compression: ");

                switch (data[1] & SIXLOWPAN_IPHC2_DAM) {
                    case 0x00:
                        printf("128 bits inline\n");
                        break;

                    case 0x01:
                        printf("64 bits inline\n");
                        break;

                    case 0x02:
                        printf("16 bits inline\n");
                        break;

                    case 0x03:
                        printf("elided (use L2 address)\n");
                        break;
                }
            }
        }

        if (data[1] & SIXLOWPAN_IPHC2_CID_EXT) {
            offset += SIXLOWPAN_IPHC_CID_EXT_LEN;
            printf("SCI: 0x%x, DCI: 0x%x\n", (unsigned)(data[2] >> 4),
                   (unsigned)(data[2] & 0xf));
        }

        od_hex_dump(data + offset, size - offset, OD_WIDTH_DEFAULT);
    }
}

/** @} */

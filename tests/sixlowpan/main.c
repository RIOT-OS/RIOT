/*
 * Copyright (C) 2014 Martin Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Tests for sixlowpan module
 *
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * @}
 */

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "byteorder.h"
#include "ipv6.h"
#include "pktbuf.h"
#include "netapi.h"
#include "sixlowpan.h"
#include "sixlowpan/iphc_cbuf.h"

#include "framework.h"

static uint32_t test_nalp_received(void)
{
    int res;
    uint16_t src = 0xcccc;
    uint16_t dest = 0xabcd;
    /* NALP (not a LoWPAN frame): any frame starting with 2 MSB = 0 */
    uint8_t data[] = {(uint8_t)(RANDOM_BYTE & (~((uint8_t)0xc0))), RANDOM_BYTE};
    netapi_rcv_pkt_t rcv = { NETAPI_CMD_RCV, NULL, &src, sizeof(uint16_t),
                             &dest, sizeof(uint16_t), data, sizeof(data)
                           };

    res = netapi_send_command(sixlowpan_test_sixlowpan_pid, (netapi_cmd_t *)&rcv);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return (res < 0) ? (uint32_t)(-res) : 0;
}

static uint32_t test_unsupported_address_family_uncompressed_received(void)
{
    int res;
    uint32_t src = 0x01234567;
    uint16_t dest = 0xabcd;
    uint8_t data[] = {SIXLOWPAN_IPV6_DISPATCH, RANDOM_BYTE};
    netapi_rcv_pkt_t rcv = { NETAPI_CMD_RCV, NULL, &src, sizeof(uint32_t),
                             &dest, sizeof(uint16_t), data, sizeof(data)
                           };

    res = netapi_send_command(sixlowpan_test_sixlowpan_pid, (netapi_cmd_t *)&rcv);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return (res < 0) ? (uint32_t)(-res) : 0;
}

static uint32_t test_uncompressed_unfragmented_received(void)
{
    uint32_t res;
    uint16_t src = 0xcccc;
    uint16_t dest = 0xabcd;
    uint8_t data[] = {SIXLOWPAN_IPV6_DISPATCH, RANDOM_BYTE};
    netapi_rcv_pkt_t rcv = { NETAPI_CMD_RCV, NULL, &src, sizeof(uint16_t),
                             &dest, sizeof(uint16_t), data, sizeof(data)
                           };
    sixlowpan_test_exp_rcv_t exp = { &src, sizeof(uint16_t), &dest, sizeof(uint16_t),
                                     &(data[1]), 1
                                   };

    res = sixlowpan_test_receiver_test(&rcv, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_unsupported_address_family_fragment_received(void)
{
    int res;
    uint32_t src = 0x01234567;
    uint16_t dest = 0xabcd;
    uint8_t data[] = {0, 0, 0, 0, SIXLOWPAN_IPV6_DISPATCH, RANDOM_BYTE};
    netapi_rcv_pkt_t rcv = { NETAPI_CMD_RCV, NULL, &src, sizeof(uint32_t),
                             &dest, sizeof(uint16_t), data, sizeof(data)
                           };

    sixlowpan_init_frag1_dispatch(data, 2, 1);

    res = netapi_send_command(sixlowpan_test_sixlowpan_pid, (netapi_cmd_t *)&rcv);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return (res < 0) ? (uint32_t)(-res) : 0;
}

static uint32_t test_first_fragment_received(void)
{
    uint32_t res;
    uint16_t src = 0xcccc;
    uint16_t dest = 0xabcd;
    uint8_t data[] = {0, 0, 0, 0, SIXLOWPAN_IPV6_DISPATCH, 0x70};
    netapi_rcv_pkt_t rcv = { NETAPI_CMD_RCV, NULL, &src, sizeof(uint16_t),
                             &dest, sizeof(uint16_t), data, sizeof(data)
                           };
    sixlowpan_test_exp_rcv_t exp = { &src, sizeof(uint16_t), &dest, sizeof(uint16_t),
                                     &(data[5]), 1
                                   };

    sixlowpan_init_frag1_dispatch(data, 2, 1);

    res = sixlowpan_test_receiver_test(&rcv, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_more_fragments_first_not_8_byte_grouped_received(void)
{
    int res;
    uint16_t src = 0xcccc;
    uint16_t dest = 0xabcd;
    uint8_t data1[] = {0, 0, 0, 0, SIXLOWPAN_IPV6_DISPATCH, RANDOM_BYTE, 0x01, 0x02, 0x03, 0x04, 0x05};

    sixlowpan_init_frag1_dispatch(data1, sizeof(data1) + 5, 1);

    netapi_rcv_pkt_t rcv1 = { NETAPI_CMD_RCV, NULL, &src, sizeof(uint16_t), &dest, sizeof(uint16_t), data1, sizeof(data1) };

    res = netapi_send_command(sixlowpan_test_sixlowpan_pid, (netapi_cmd_t *)&rcv1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return (res < 0) ? (uint32_t)(-res) : 0;
}

static uint32_t test_2_fragments_received_ordered(void)
{
    uint32_t res;
    uint16_t src = 0xcccc;
    uint64_t dest = 0xabcdef1234567890;
    uint8_t data1[] = {0, 0, 0, 0, SIXLOWPAN_IPV6_DISPATCH, RANDOM_BYTE, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
    uint8_t data2[] = {0, 0, 0, 0, 0, 0x07, 0x08, 0x09, 0x10, 0x0a, 0x0b, 0x0c, 0x0d};
    uint8_t data_exp[15];

    sixlowpan_init_frag1_dispatch(data1, 16, 1);
    sixlowpan_init_fragn_dispatch(data2, 16, 1, 1);

    memcpy(data_exp, &(data1[5]), 7);
    memcpy(&(data_exp[7]), &(data2[5]), 8);

    netapi_rcv_pkt_t rcv[] = {
        { NETAPI_CMD_RCV, NULL, &src, sizeof(uint16_t), &dest, sizeof(uint64_t), data1, sizeof(data1) },
        { NETAPI_CMD_RCV, NULL, &src, sizeof(uint16_t), &dest, sizeof(uint64_t), data2, sizeof(data2) },
    };

    sixlowpan_test_exp_rcv_t exp = { &src, sizeof(uint16_t), &dest, sizeof(uint64_t),
                                     data_exp, 15
                                   };

    res = sixlowpan_test_receiver_test(rcv, 2, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_2_fragments_received_unordered(void)
{
    uint32_t res;
    uint64_t src = 0xcccccccccccc;
    uint16_t dest = 0xabcd;
    uint8_t data1[] = {0, 0, 0, 0, SIXLOWPAN_IPV6_DISPATCH, RANDOM_BYTE, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
    uint8_t data2[] = {0, 0, 0, 0, 0, 0x07, 0x08, 0x09, 0x10, 0x0a, 0x0b, 0x0c, 0x0d};
    uint8_t data_exp[15];

    sixlowpan_init_frag1_dispatch(data1, 16, 1);
    sixlowpan_init_fragn_dispatch(data2, 16, 1, 1);

    memcpy(data_exp, &(data1[5]), 7);
    memcpy(&(data_exp[7]), &(data2[5]), 8);

    netapi_rcv_pkt_t rcv[] = {
        { NETAPI_CMD_RCV, NULL, &src, sizeof(uint64_t), &dest, sizeof(uint16_t), data2, sizeof(data2) },
        { NETAPI_CMD_RCV, NULL, &src, sizeof(uint64_t), &dest, sizeof(uint16_t), data1, sizeof(data1) },
    };

    sixlowpan_test_exp_rcv_t exp = { &src, sizeof(uint64_t), &dest, sizeof(uint16_t),
                                     data_exp, 15
                                   };

    res = sixlowpan_test_receiver_test(rcv, 2, &exp, 1);

    if (!pktbuf_is_empty()) {
        return 0xffff;
    }

    return res;
}

static uint32_t test_2_fragments_received_ordered_last_not_full(void)
{
    uint32_t res;
    uint16_t src = 0xcccc;
    uint16_t dest = 0xabcd;
    uint8_t data1[] = {0, 0, 0, 0, SIXLOWPAN_IPV6_DISPATCH, RANDOM_BYTE, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
    uint8_t data2[] = {0, 0, 0, 0, 0, 0x07, 0x08, 0x09, 0x10, 0x0a};
    uint8_t data_exp[16];

    sixlowpan_init_frag1_dispatch(data1, 13, 1);
    sixlowpan_init_fragn_dispatch(data2, 13, 1, 1);

    memcpy(data_exp, &(data1[5]), 7);
    memcpy(&(data_exp[7]), &(data2[5]), 5);

    netapi_rcv_pkt_t rcv[] = {
        { NETAPI_CMD_RCV, NULL, &src, sizeof(uint16_t), &dest, sizeof(uint16_t), data1, sizeof(data1) },
        { NETAPI_CMD_RCV, NULL, &src, sizeof(uint16_t), &dest, sizeof(uint16_t), data2, sizeof(data2) },
    };

    sixlowpan_test_exp_rcv_t exp = { &src, sizeof(uint16_t), &dest, sizeof(uint16_t),
                                     data_exp, 12
                                   };

    res = sixlowpan_test_receiver_test(rcv, 2, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_more_fragments_nth_not_8_byte_grouped_received(void)
{
    int res;
    uint16_t src = 0xcccc;
    uint16_t dest = 0xabcd;
    uint8_t data1[] = {0, 0, 0, 0, SIXLOWPAN_IPV6_DISPATCH, RANDOM_BYTE, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
    uint8_t data2[] = {0, 0, 0, 0, 0, 0x07, 0x08, 0x09, 0x10, 0x0a, 0x0b, 0x0c};

    sixlowpan_init_frag1_dispatch(data1, sizeof(data1) + sizeof(data2) + 3, 1);
    sixlowpan_init_fragn_dispatch(data2, sizeof(data1) + sizeof(data2) + 3, 1, 1);

    netapi_rcv_pkt_t rcv1 = { NETAPI_CMD_RCV, NULL, &src, sizeof(uint16_t), &dest, sizeof(uint16_t), data1, sizeof(data1) };
    netapi_rcv_pkt_t rcv2 = { NETAPI_CMD_RCV, NULL, &src, sizeof(uint16_t), &dest, sizeof(uint16_t), data2, sizeof(data2) };

    res = netapi_send_command(sixlowpan_test_sixlowpan_pid, (netapi_cmd_t *)&rcv1);

    if (res < 0) {
        return SIXLOWPAN_TEST_EWRONG_TEST_RESULT;
    }

    res = netapi_send_command(sixlowpan_test_sixlowpan_pid, (netapi_cmd_t *)&rcv2);

    return (res < 0) ? (uint32_t)(-res) : 0;
}

static uint32_t test_3_fragments_received_ordered(void)
{
    uint32_t res;
    uint64_t src = 0xcccccccccccc;
    uint64_t dest = 0xabcdef1234567890;
    uint8_t data1[] = {0, 0, 0, 0, SIXLOWPAN_IPV6_DISPATCH, RANDOM_BYTE, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
    uint8_t data2[] = {0, 0, 0, 0, 0, 0x07, 0x08, 0x09, 0x10, 0x0a, 0x0b, 0x0c, 0x0d};
    uint8_t data3[] = {0, 0, 0, 0, 0, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15};
    uint8_t data_exp[23];

    sixlowpan_init_frag1_dispatch(data1, 24, 1);
    sixlowpan_init_fragn_dispatch(data2, 24, 1, 1);
    sixlowpan_init_fragn_dispatch(data3, 24, 1, 2);

    memcpy(data_exp, &(data1[5]), 7);
    memcpy(&(data_exp[7]), &(data2[5]), 8);
    memcpy(&(data_exp[15]), &(data3[5]), 8);

    netapi_rcv_pkt_t rcv[] = {
        { NETAPI_CMD_RCV, NULL, &src, sizeof(uint64_t), &dest, sizeof(uint64_t), data1, sizeof(data1) },
        { NETAPI_CMD_RCV, NULL, &src, sizeof(uint64_t), &dest, sizeof(uint64_t), data2, sizeof(data2) },
        { NETAPI_CMD_RCV, NULL, &src, sizeof(uint64_t), &dest, sizeof(uint64_t), data3, sizeof(data3) },
    };

    sixlowpan_test_exp_rcv_t exp = { &src, sizeof(uint64_t), &dest, sizeof(uint64_t),
                                     data_exp, 23
                                   };

    res = sixlowpan_test_receiver_test(rcv, 3, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_3_fragments_received_last_2_unordered(void)
{
    uint32_t res;
    uint16_t src = 0xcccc;
    uint16_t dest = 0xabcd;
    uint8_t data1[] = {0, 0, 0, 0, SIXLOWPAN_IPV6_DISPATCH, RANDOM_BYTE, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
    uint8_t data2[] = {0, 0, 0, 0, 0, 0x07, 0x08, 0x09, 0x10, 0x0a, 0x0b, 0x0c, 0x0d};
    uint8_t data3[] = {0, 0, 0, 0, 0, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15};
    uint8_t data_exp[23];

    sixlowpan_init_frag1_dispatch(data1, 24, 1);
    sixlowpan_init_fragn_dispatch(data2, 24, 1, 1);
    sixlowpan_init_fragn_dispatch(data3, 24, 1, 2);

    memcpy(data_exp, &(data1[5]), 7);
    memcpy(&(data_exp[7]), &(data2[5]), 8);
    memcpy(&(data_exp[15]), &(data3[5]), 8);

    netapi_rcv_pkt_t rcv[] = {
        { NETAPI_CMD_RCV, NULL, &src, sizeof(uint16_t), &dest, sizeof(uint16_t), data1, sizeof(data1) },
        { NETAPI_CMD_RCV, NULL, &src, sizeof(uint16_t), &dest, sizeof(uint16_t), data3, sizeof(data3) },
        { NETAPI_CMD_RCV, NULL, &src, sizeof(uint16_t), &dest, sizeof(uint16_t), data2, sizeof(data2) },
    };

    sixlowpan_test_exp_rcv_t exp = { &src, sizeof(uint16_t), &dest, sizeof(uint16_t),
                                     data_exp, 23
                                   };

    res = sixlowpan_test_receiver_test(rcv, 3, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_4_fragments_received_2_datagrams_unordered(void)
{
    uint32_t res;
    uint16_t src = 0xcccc;
    uint16_t dest = 0xabcd;
    uint8_t data1[] = {0, 0, 0, 0, SIXLOWPAN_IPV6_DISPATCH, RANDOM_BYTE, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
    uint8_t data2[] = {0, 0, 0, 0, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16};
    uint8_t data3[] = {0, 0, 0, 0, 0, 0x07, 0x08, 0x09, 0x10, 0x0a, 0x0b, 0x0c, 0x0d};
    uint8_t data4[] = {0, 0, 0, 0, 0, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e};
    uint8_t data_exp1[15];
    uint8_t data_exp2[16];

    sixlowpan_init_frag1_dispatch(data1, 16, 1);
    sixlowpan_init_frag1_dispatch(data2, 16, RANDOM_SHORT);
    sixlowpan_init_fragn_dispatch(data3, 16, 1, 1);
    sixlowpan_init_fragn_dispatch(data4, 16, RANDOM_SHORT, 1);

    memcpy(data_exp1, &(data1[5]), 7);
    memcpy(data_exp2, &(data2[4]), 8);
    memcpy(&(data_exp1[7]), &(data3[5]), 8);
    memcpy(&(data_exp2[8]), &(data4[5]), 8);

    netapi_rcv_pkt_t rcv[] = {
        { NETAPI_CMD_RCV, NULL, &src, sizeof(uint16_t), &dest, sizeof(uint16_t), data1, sizeof(data1) },
        { NETAPI_CMD_RCV, NULL, &src, sizeof(uint16_t), &dest, sizeof(uint16_t), data2, sizeof(data2) },
        { NETAPI_CMD_RCV, NULL, &src, sizeof(uint16_t), &dest, sizeof(uint16_t), data3, sizeof(data3) },
        { NETAPI_CMD_RCV, NULL, &src, sizeof(uint16_t), &dest, sizeof(uint16_t), data4, sizeof(data4) },
    };

    sixlowpan_test_exp_rcv_t exp[] = {
        { &src, sizeof(uint16_t), &dest, sizeof(uint16_t), data_exp1, 15 },
        { &src, sizeof(uint16_t), &dest, sizeof(uint16_t), data_exp2, 16 },
    };

    res = sixlowpan_test_receiver_test(rcv, 4, exp, 2);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_unsupported_address_family_send(void)
{
    int res;
    uint32_t dest = 0xabcdcdef;
    uint8_t data[] = {RANDOM_BYTE};
    netapi_snd_pkt_t snd = { NETAPI_CMD_SND, NULL, NULL, &dest, sizeof(uint32_t),
                             data, sizeof(data)
                           };

    res = netapi_send_command(sixlowpan_test_sixlowpan_pid, (netapi_cmd_t *)&snd);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return (res < 0) ? (uint32_t)(-res) : 0;
}

static uint32_t test_uncompressed_unfragmented_send(void)
{
    uint32_t res;
    uint16_t dest = 0xabcd;
    uint8_t data[] = { RANDOM_BYTE };
    uint8_t dispatch[] = { SIXLOWPAN_IPV6_DISPATCH };
    netdev_hlist_t ulh = { NULL, NULL, NETDEV_PROTO_6LOWPAN, dispatch, sizeof(dispatch) };
    netapi_snd_pkt_t snd = { NETAPI_CMD_SND, NULL, NULL, &dest, sizeof(uint16_t),
                             data, sizeof(data)
                           };
    sixlowpan_test_exp_snd_t exp = { &ulh, &dest, sizeof(uint16_t), data, sizeof(data) };
    ulh.next = &ulh;
    ulh.prev = &ulh;

#ifdef MODULE_SIXLOWPAN_HC

    if (sixlowpan_hc_set_status(sixlowpan_test_sixlowpan_pid,
                                SIXLOWPAN_HC_DISABLE) < 0) {
        return SIXLOWPAN_TEST_SET_HC_STATUS_ERROR;
    }

#endif

    res = sixlowpan_test_send_test(&snd, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_2_fragments_send(void)
{
    uint32_t res;
    uint16_t dest = 0xabcd;
    char data[] = "\xc0\x91\0\0Agcdefbhijklmnopqrstuvwxyz0123456789" /* 0xc0: Frag1 dispatch */
                  "abcdefghijklmnopqrstuvwxyz0123456789"             /* 0x90 (144): total length of datagram */
                  "abcdefghijklmnopqrstuvwxyz0123456789"             /* \0\0 (0): 16-bit datagram tag */
                  "abcdefghijklmnopqrstuvwxyz0123456789";            /* A == SIXLOWPAN_IPV6_DISPATCH */
    /* cppcheck-suppress variableScope that would make it even more uglier */
    char data2[] = "\xe0\x91\0\0\x0fmnopqrstuvwxyz0123456789";       /* 0xe0: FragN dispatch */
                                                                     /* 0x0f (15): datagram offset / 8 */

    netapi_snd_pkt_t snd = { NETAPI_CMD_SND, NULL, NULL, &dest, sizeof(uint16_t),
                             &(data[5]), sizeof(data) - 5
                           };
    sixlowpan_test_exp_snd_t exp[] = {
        { NULL, &dest, sizeof(uint16_t), data, 124 },
        { NULL, &dest, sizeof(uint16_t), data2, sizeof(data2) }
    };

#ifdef MODULE_SIXLOWPAN_HC

    if (sixlowpan_hc_set_status(sixlowpan_test_sixlowpan_pid,
                                SIXLOWPAN_HC_DISABLE) < 0) {
        return SIXLOWPAN_TEST_SET_HC_STATUS_ERROR;
    }

#endif

    res = sixlowpan_test_send_test(&snd, 1, exp, 2);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_3_fragments_send(void)
{
    uint32_t res;
    uint16_t dest = 0xabcd;
    char data[] = "\xc0\xfd\0\0Agcdefbhijklmnopqrstuvwxyz0123456789" /* 0xc0: Frag1 dispatch */
                  "abcdefghijklmnopqrstuvwxyz0123456789"             /* 0xfd (253): total length of datagram */
                  "abcdefghijklmnopqrstuvwxyz0123456789"             /* \0\0 (0): 16-bit datagram tag */
                  "abcdefghijklmnopqrstuvwxyz0123456789"             /* A == SIXLOWPAN_IPV6_DISPATCH */
                  "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
                  "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
                  "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    /* cppcheck-suppress variableScope that would make it even more uglier */
    char data2[] = "\xe0\xfd\0\0\x0fmnopqrstuvwxyz0123456789"        /* 0xe0: FragN dispatch */
                   "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"            /* 0x0f (15): datagram offset / 8 */
                   "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
                   "ABCDEFGHIJKLMNOPQRSTUVWX";
    /* cppcheck-suppress variableScope that would make it even more uglier */
    char data3[] = "\xe0\xfd\0\0\x1eYZ0123456789";                   /* 0x1e (30): datagram offset / 8 */

    netapi_snd_pkt_t snd = { NETAPI_CMD_SND, NULL, NULL, &dest, sizeof(uint16_t),
                             &(data[5]), sizeof(data) - 5
                           };
    sixlowpan_test_exp_snd_t exp[] = {
        { NULL, &dest, sizeof(uint16_t), data, 124 },
        { NULL, &dest, sizeof(uint16_t), data2, 125 },
        { NULL, &dest, sizeof(uint16_t), data3, sizeof(data3) }
    };

#ifdef MODULE_SIXLOWPAN_HC

    if (sixlowpan_hc_set_status(sixlowpan_test_sixlowpan_pid,
                                SIXLOWPAN_HC_DISABLE) < 0) {
        return SIXLOWPAN_TEST_SET_HC_STATUS_ERROR;
    }

#endif

    res = sixlowpan_test_send_test(&snd, 1, exp, 3);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_big_header_send(void)
{
    uint32_t res;
    uint16_t dest = 0xabcd;
    char header_data[] = "mcdefbhijklgnopqrstuvwxyz0123456789"
                         "abcdefghijklmnopqrstuvwxyz0123456789"
                         "abcdefghijklmnopqrstuvwxyz0123456789"
                         "abcdefghijklmno";
    netdev_hlist_t hlist = { NULL, NULL, NETDEV_PROTO_UNKNOWN, header_data, sizeof(header_data) };
    char data[] = "pqrstuvwxyz012345678";
    /* cppcheck-suppress variableScope that would make it even more uglier */
    char exp_data1[] = "\xc0\x91\0\0Amcdefbhijklgnopqrstuvwxyz0123456789" /* 0xc0: Frag1 dispatch */
                       "abcdefghijklmnopqrstuvwxyz0123456789"                 /* 0x90 (144): total length of datagram */
                       "abcdefghijklmnopqrstuvwxyz0123456789abcdefghijkl";    /* \0\0 (0): 16-bit datagram tag */
    /* cppcheck-suppress variableScope that would make it even more uglier */
    char exp_data2[] = "\xe0\x91\0\0\x0fmno\0pqrstuvwxyz012345678";       /* 0xe0: FragN dispatch */
                                                                          /* 0x0f (15): datagram offset / 8 */
    hlist.next = &hlist;
    hlist.prev = &hlist;

#ifdef MODULE_SIXLOWPAN_HC

    if (sixlowpan_hc_set_status(sixlowpan_test_sixlowpan_pid,
                                SIXLOWPAN_HC_DISABLE) < 0) {
        return SIXLOWPAN_TEST_SET_HC_STATUS_ERROR;
    }

#endif

    netapi_snd_pkt_t snd = { NETAPI_CMD_SND, NULL, &hlist, &dest, sizeof(uint16_t),
                             data, sizeof(data)
                           };
    sixlowpan_test_exp_snd_t exp[] = {
        { NULL, &dest, sizeof(uint16_t), exp_data1, sizeof(exp_data1) - 1 },
        { NULL, &dest, sizeof(uint16_t), exp_data2, sizeof(exp_data2) }
    };

    res = sixlowpan_test_send_test(&snd, 1, exp, 2);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_2_big_headers_send(void)
{
    uint32_t res;
    uint16_t dest = 0xabcd;
    char header_data1[] = "abcdefghijklgnopqrstuvwxyz0123456789"
                          "abcdefghijklmnopqrstuvwxyz0123456789"
                          "abcdefghijklmnopqrstuvwxyz0123456789"
                          "abcdefghijklmnopqrstuvwxyz0123456789";
    netdev_hlist_t hlist1 = { NULL, NULL, NETDEV_PROTO_UNKNOWN, header_data1, sizeof(header_data1) };
    netdev_hlist_t hlist2 = { NULL, NULL, NETDEV_PROTO_UNKNOWN, header_data1, sizeof(header_data1) };
    char data[] = "abcdefghi";
    /* cppcheck-suppress variableScope that would make it even more uglier */
    char exp_data1[] = "\xc1\x2d\0\0Aabcdefghijklgnopqrstuvwxyz0123456789" /* 0xc0: Frag1 dispatch */
                       "abcdefghijklmnopqrstuvwxyz0123456789"              /* 0x12d (301): total length of datagram */
                       "abcdefghijklmnopqrstuvwxyz0123456789abcdefghijk";  /* \0\0 (0): 16-bit datagram tag */
    /* cppcheck-suppress variableScope that would make it even more uglier */
    char exp_data2[] = "\xe1\x2d\0\0\x0flmnopqrstuvwxyz0123456789\0"       /* 0xe0: FragN dispatch */
                       "abcdefghijklgnopqrstuvwxyz0123456789"              /* 0x0f (15): datagram offset / 8 */
                       "abcdefghijklmnopqrstuvwxyz0123456789"
                       "abcdefghijklmnopqrstuv";
    /* cppcheck-suppress variableScope that would make it even more uglier */
    char exp_data3[] = "\xe1\x2d\0\0\x1ewxyz0123456789"                    /* 0x1e (30): datagram_offset / 8 */
                       "abcdefghijklmnopqrstuvwxyz0123456789\0abcdefghi";

    hlist1.next = &hlist2;
    hlist1.prev = &hlist2;
    hlist2.next = &hlist1;
    hlist2.prev = &hlist1;

    netapi_snd_pkt_t snd = { NETAPI_CMD_SND, NULL, &hlist1, &dest, sizeof(uint16_t),
                             data, sizeof(data)
                           };
    sixlowpan_test_exp_snd_t exp[] = {
        { NULL, &dest, sizeof(uint16_t), exp_data1, sizeof(exp_data1) - 1 },
        { NULL, &dest, sizeof(uint16_t), exp_data2, sizeof(exp_data2) - 1 },
        { NULL, &dest, sizeof(uint16_t), exp_data3, sizeof(exp_data3) }
    };

#ifdef MODULE_SIXLOWPAN_HC

    if (sixlowpan_hc_set_status(sixlowpan_test_sixlowpan_pid,
                                SIXLOWPAN_HC_DISABLE) < 0) {
        return SIXLOWPAN_TEST_SET_HC_STATUS_ERROR;
    }

#endif

    res = sixlowpan_test_send_test(&snd, 1, exp, 2);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

#ifdef MODULE_SIXLOWPAN_HC
static uint32_t test_compressed_too_short_dispatch(void)
{
    int res;
    uint16_t src = 0xcccc;
    uint16_t dest = 0xabcd;
    uint8_t data[] = { SIXLOWPAN_IPHC1_DISPATCH };
    netapi_rcv_pkt_t rcv = { NETAPI_CMD_RCV, NULL, &src, sizeof(uint16_t),
                             &dest, sizeof(uint16_t), data, sizeof(data)
                           };

    res = netapi_send_command(sixlowpan_test_sixlowpan_pid, (netapi_cmd_t *)&rcv);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return (res < 0) ? (uint32_t)(-res) : 0;
}

static uint32_t test_compressed_all_zero_received(void)
{
    uint32_t res;
    uint16_t src = 0xcccc;
    uint16_t dest = 0xabcd;
    uint8_t data[] = { SIXLOWPAN_IPHC1_DISPATCH, 0, 0x43, 0x04, 0x56, 0x78,
                       IPV6_PROTO_NUM_NONE, 0x25, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
                       0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
                       0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
                       0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20
                     };
    ipv6_hdr_t data_exp;
    netapi_rcv_pkt_t rcv = { NETAPI_CMD_RCV, NULL, &src, sizeof(uint16_t),
                             &dest, sizeof(uint16_t), data, sizeof(data)
                           };
    sixlowpan_test_exp_rcv_t exp = { &src, sizeof(uint16_t), &dest, sizeof(uint16_t),
                                     &data_exp, sizeof(data_exp)
                                   };

    ipv6_hdr_set_version(&data_exp);
    ipv6_hdr_set_trafficclass_dscp(&data_exp, 3);
    ipv6_hdr_set_trafficclass_ecn(&data_exp, 1);
    ipv6_hdr_set_flowlabel(&data_exp, 0x45678);
    data_exp.length.u16 = 0;
    data_exp.nextheader = IPV6_PROTO_NUM_NONE;
    data_exp.hoplimit = 0x25;
    data_exp.srcaddr.u64[0] = byteorder_htonll(0x0102030405060708);
    data_exp.srcaddr.u64[1] = byteorder_htonll(0x090a0b0c0d0e0f10);
    data_exp.destaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    data_exp.destaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);

    res = sixlowpan_test_receiver_test(&rcv, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_dscp_elided_received(void)
{
    uint32_t res;
    uint16_t src = 0xcccc;
    uint16_t dest = 0xabcd;
    uint8_t data[] = { SIXLOWPAN_IPHC1_DISPATCH | 0x08, 0, 0x44, 0x56, 0x78,
                       IPV6_PROTO_NUM_NONE, 0x25, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
                       0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
                       0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
                       0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20
                     };
    ipv6_hdr_t data_exp;
    netapi_rcv_pkt_t rcv = { NETAPI_CMD_RCV, NULL, &src, sizeof(uint16_t),
                             &dest, sizeof(uint16_t), data, sizeof(data)
                           };
    sixlowpan_test_exp_rcv_t exp = { &src, sizeof(uint16_t), &dest, sizeof(uint16_t),
                                     &data_exp, sizeof(data_exp)
                                   };

    ipv6_hdr_set_version(&data_exp);
    ipv6_hdr_set_trafficclass_dscp(&data_exp, 0);
    ipv6_hdr_set_trafficclass_ecn(&data_exp, 1);
    ipv6_hdr_set_flowlabel(&data_exp, 0x45678);
    data_exp.length.u16 = 0;
    data_exp.nextheader = IPV6_PROTO_NUM_NONE;
    data_exp.hoplimit = 0x25;
    data_exp.srcaddr.u64[0] = byteorder_htonll(0x0102030405060708);
    data_exp.srcaddr.u64[1] = byteorder_htonll(0x090a0b0c0d0e0f10);
    data_exp.destaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    data_exp.destaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);

    res = sixlowpan_test_receiver_test(&rcv, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_fl_elided_received(void)
{
    uint32_t res;
    uint16_t src = 0xcccc;
    uint16_t dest = 0xabcd;
    uint8_t data[] = { SIXLOWPAN_IPHC1_DISPATCH | 0x10, 0, 0x43,
                       IPV6_PROTO_NUM_NONE, 0x25, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
                       0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
                       0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
                       0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20
                     };
    ipv6_hdr_t data_exp;
    netapi_rcv_pkt_t rcv = { NETAPI_CMD_RCV, NULL, &src, sizeof(uint16_t),
                             &dest, sizeof(uint16_t), data, sizeof(data)
                           };
    sixlowpan_test_exp_rcv_t exp = { &src, sizeof(uint16_t), &dest, sizeof(uint16_t),
                                     &data_exp, sizeof(data_exp)
                                   };

    ipv6_hdr_set_version(&data_exp);
    ipv6_hdr_set_trafficclass_dscp(&data_exp, 3);
    ipv6_hdr_set_trafficclass_ecn(&data_exp, 1);
    ipv6_hdr_set_flowlabel(&data_exp, 0);
    data_exp.length.u16 = 0;
    data_exp.nextheader = IPV6_PROTO_NUM_NONE;
    data_exp.hoplimit = 0x25;
    data_exp.srcaddr.u64[0] = byteorder_htonll(0x0102030405060708);
    data_exp.srcaddr.u64[1] = byteorder_htonll(0x090a0b0c0d0e0f10);
    data_exp.destaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    data_exp.destaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);

    res = sixlowpan_test_receiver_test(&rcv, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_tc_fl_elided_received(void)
{
    uint32_t res;
    uint16_t src = 0xcccc;
    uint16_t dest = 0xabcd;
    uint8_t data[] = { SIXLOWPAN_IPHC1_DISPATCH | SIXLOWPAN_IPHC1_TF, 0,
                       IPV6_PROTO_NUM_NONE, 0x25, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
                       0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
                       0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
                       0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20
                     };
    ipv6_hdr_t data_exp;
    netapi_rcv_pkt_t rcv = { NETAPI_CMD_RCV, NULL, &src, sizeof(uint16_t),
                             &dest, sizeof(uint16_t), data, sizeof(data)
                           };
    sixlowpan_test_exp_rcv_t exp = { &src, sizeof(uint16_t), &dest, sizeof(uint16_t),
                                     &data_exp, sizeof(data_exp)
                                   };

    ipv6_hdr_set_version(&data_exp);
    ipv6_hdr_set_trafficclass(&data_exp, 0);
    ipv6_hdr_set_flowlabel(&data_exp, 0);
    data_exp.length.u16 = 0;
    data_exp.nextheader = IPV6_PROTO_NUM_NONE;
    data_exp.hoplimit = 0x25;
    data_exp.srcaddr.u64[0] = byteorder_htonll(0x0102030405060708);
    data_exp.srcaddr.u64[1] = byteorder_htonll(0x090a0b0c0d0e0f10);
    data_exp.destaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    data_exp.destaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);

    res = sixlowpan_test_receiver_test(&rcv, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_tf_elided_hl_1_received(void)
{
    uint32_t res;
    uint16_t src = 0xcccc;
    uint16_t dest = 0xabcd;
    uint8_t data[] = { SIXLOWPAN_IPHC1_DISPATCH | SIXLOWPAN_IPHC1_TF | 0x1, 0,
                       IPV6_PROTO_NUM_NONE, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
                       0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
                       0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
                       0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20
                     };
    ipv6_hdr_t data_exp;
    netapi_rcv_pkt_t rcv = { NETAPI_CMD_RCV, NULL, &src, sizeof(uint16_t),
                             &dest, sizeof(uint16_t), data, sizeof(data)
                           };
    sixlowpan_test_exp_rcv_t exp = { &src, sizeof(uint16_t), &dest, sizeof(uint16_t),
                                     &data_exp, sizeof(data_exp)
                                   };

    ipv6_hdr_set_version(&data_exp);
    ipv6_hdr_set_trafficclass(&data_exp, 0);
    ipv6_hdr_set_flowlabel(&data_exp, 0);
    data_exp.length.u16 = 0;
    data_exp.nextheader = IPV6_PROTO_NUM_NONE;
    data_exp.hoplimit = 1;
    data_exp.srcaddr.u64[0] = byteorder_htonll(0x0102030405060708);
    data_exp.srcaddr.u64[1] = byteorder_htonll(0x090a0b0c0d0e0f10);
    data_exp.destaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    data_exp.destaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);

    res = sixlowpan_test_receiver_test(&rcv, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_tf_elided_hl_64_received(void)
{
    uint32_t res;
    uint16_t src = 0xcccc;
    uint16_t dest = 0xabcd;
    uint8_t data[] = { SIXLOWPAN_IPHC1_DISPATCH | SIXLOWPAN_IPHC1_TF | 0x2, 0,
                       IPV6_PROTO_NUM_NONE, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
                       0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
                       0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
                       0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20
                     };
    ipv6_hdr_t data_exp;
    netapi_rcv_pkt_t rcv = { NETAPI_CMD_RCV, NULL, &src, sizeof(uint16_t),
                             &dest, sizeof(uint16_t), data, sizeof(data)
                           };
    sixlowpan_test_exp_rcv_t exp = { &src, sizeof(uint16_t), &dest, sizeof(uint16_t),
                                     &data_exp, sizeof(data_exp)
                                   };

    ipv6_hdr_set_version(&data_exp);
    ipv6_hdr_set_trafficclass(&data_exp, 0);
    ipv6_hdr_set_flowlabel(&data_exp, 0);
    data_exp.length.u16 = 0;
    data_exp.nextheader = IPV6_PROTO_NUM_NONE;
    data_exp.hoplimit = 64;
    data_exp.srcaddr.u64[0] = byteorder_htonll(0x0102030405060708);
    data_exp.srcaddr.u64[1] = byteorder_htonll(0x090a0b0c0d0e0f10);
    data_exp.destaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    data_exp.destaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);

    res = sixlowpan_test_receiver_test(&rcv, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_tf_elided_hl_255_received(void)
{
    uint32_t res;
    uint16_t src = 0xcccc;
    uint16_t dest = 0xabcd;
    uint8_t data[] = { SIXLOWPAN_IPHC1_DISPATCH | SIXLOWPAN_IPHC1_TF | 0x3, 0,
                       IPV6_PROTO_NUM_NONE, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
                       0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
                       0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
                       0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20
                     };
    ipv6_hdr_t data_exp;
    netapi_rcv_pkt_t rcv = { NETAPI_CMD_RCV, NULL, &src, sizeof(uint16_t),
                             &dest, sizeof(uint16_t), data, sizeof(data)
                           };
    sixlowpan_test_exp_rcv_t exp = { &src, sizeof(uint16_t), &dest, sizeof(uint16_t),
                                     &data_exp, sizeof(data_exp)
                                   };

    ipv6_hdr_set_version(&data_exp);
    ipv6_hdr_set_trafficclass(&data_exp, 0);
    ipv6_hdr_set_flowlabel(&data_exp, 0);
    data_exp.length.u16 = 0;
    data_exp.nextheader = IPV6_PROTO_NUM_NONE;
    data_exp.hoplimit = 255;
    data_exp.srcaddr.u64[0] = byteorder_htonll(0x0102030405060708);
    data_exp.srcaddr.u64[1] = byteorder_htonll(0x090a0b0c0d0e0f10);
    data_exp.destaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    data_exp.destaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);

    res = sixlowpan_test_receiver_test(&rcv, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_iphc1_set_cid_ext_received(void)
{
    uint32_t res;
    uint16_t src = 0xcccc;
    uint16_t dest = 0xabcd;
    /* TODO: we leave out NHC out for now => 0x1b instead of 0x1f */
    uint8_t data[] = { SIXLOWPAN_IPHC1_DISPATCH | 0x1b, SIXLOWPAN_IPHC2_CID,
                       0x00, IPV6_PROTO_NUM_NONE, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
                       0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
                       0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
                       0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20
                     };
    ipv6_hdr_t data_exp;
    netapi_rcv_pkt_t rcv = { NETAPI_CMD_RCV, NULL, &src, sizeof(uint16_t),
                             &dest, sizeof(uint16_t), data, sizeof(data)
                           };
    sixlowpan_test_exp_rcv_t exp = { &src, sizeof(uint16_t), &dest, sizeof(uint16_t),
                                     &data_exp, sizeof(data_exp)
                                   };

    ipv6_hdr_set_version(&data_exp);
    ipv6_hdr_set_trafficclass(&data_exp, 0);
    ipv6_hdr_set_flowlabel(&data_exp, 0);
    data_exp.length.u16 = 0;
    data_exp.nextheader = IPV6_PROTO_NUM_NONE;
    data_exp.hoplimit = 255;
    data_exp.srcaddr.u64[0] = byteorder_htonll(0x0102030405060708);
    data_exp.srcaddr.u64[1] = byteorder_htonll(0x090a0b0c0d0e0f10);
    data_exp.destaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    data_exp.destaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);

    res = sixlowpan_test_receiver_test(&rcv, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_iphc1_set_sam_sac_set_received_no_ctx(void)
{
    int res;
    uint16_t src = 0xcccc;
    uint16_t dest = 0xabcd;
    /* TODO: we leave out NHC out for now => 0x1b instead of 0x1f */
    uint8_t data[] = { SIXLOWPAN_IPHC1_DISPATCH | 0x1b, 0x50 };
    netapi_rcv_pkt_t rcv = { NETAPI_CMD_RCV, NULL, &src, sizeof(uint16_t),
                             &dest, sizeof(uint16_t), data, sizeof(data)
                           };

    res = netapi_send_command(sixlowpan_test_sixlowpan_pid, (netapi_cmd_t *)&rcv);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return (res < 0) ? (uint32_t)(-res) : 0;
}

static uint32_t test_compressed_iphc1_set_sam_sac_1_received(void)
{
    uint32_t res;
    uint16_t src = 0xcccc;
    uint16_t dest = 0xabcd;
    /* TODO: we leave out NHC out for now => 0x1b instead of 0x1f */
    uint8_t data[] = { SIXLOWPAN_IPHC1_DISPATCH | 0x1b, 0x10, IPV6_PROTO_NUM_NONE, 0x01,
                       0x02, 0x03, 0x04, 0x0a, 0x0b, 0x0c, 0x0d, 0x11, 0x12,
                       0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b,
                       0x1c, 0x1d, 0x1e, 0x1f, 0x20
                     };
    ipv6_hdr_t data_exp;
    netapi_rcv_pkt_t rcv = { NETAPI_CMD_RCV, NULL, &src, sizeof(uint16_t),
                             &dest, sizeof(uint16_t), data, sizeof(data)
                           };
    sixlowpan_test_exp_rcv_t exp = { &src, sizeof(uint16_t), &dest, sizeof(uint16_t),
                                     &data_exp, sizeof(data_exp)
                                   };

    ipv6_hdr_set_version(&data_exp);
    ipv6_hdr_set_trafficclass(&data_exp, 0);
    ipv6_hdr_set_flowlabel(&data_exp, 0);
    data_exp.length.u16 = 0;
    data_exp.nextheader = IPV6_PROTO_NUM_NONE;
    data_exp.hoplimit = 255;
    data_exp.srcaddr.u64[0] = byteorder_htonll(0xfe80000000000000);
    data_exp.srcaddr.u64[1] = byteorder_htonll(0x010203040a0b0c0d);
    data_exp.destaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    data_exp.destaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);

    res = sixlowpan_test_receiver_test(&rcv, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_iphc1_set_sam_sac_2_received(void)
{
    uint32_t res;
    uint16_t src = 0xcccc;
    uint16_t dest = 0xabcd;
    /* TODO: we leave out NHC out for now => 0x1b instead of 0x1f */
    uint8_t data[] = { SIXLOWPAN_IPHC1_DISPATCH | 0x1b, 0x20, IPV6_PROTO_NUM_NONE, 0x07,
                       0x08, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
                       0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20
                     };
    ipv6_hdr_t data_exp;
    netapi_rcv_pkt_t rcv = { NETAPI_CMD_RCV, NULL, &src, sizeof(uint16_t),
                             &dest, sizeof(uint16_t), data, sizeof(data)
                           };
    sixlowpan_test_exp_rcv_t exp = { &src, sizeof(uint16_t), &dest, sizeof(uint16_t),
                                     &data_exp, sizeof(data_exp)
                                   };

    ipv6_hdr_set_version(&data_exp);
    ipv6_hdr_set_trafficclass(&data_exp, 0);
    ipv6_hdr_set_flowlabel(&data_exp, 0);
    data_exp.length.u16 = 0;
    data_exp.nextheader = IPV6_PROTO_NUM_NONE;
    data_exp.hoplimit = 255;
    data_exp.srcaddr.u64[0] = byteorder_htonll(0xfe80000000000000);
    data_exp.srcaddr.u64[1] = byteorder_htonll(0x000000fffe000708);
    data_exp.destaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    data_exp.destaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);

    res = sixlowpan_test_receiver_test(&rcv, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_iphc1_set_sam_sac_3_received(void)
{
    uint32_t res;
    uint16_t src = HTONS(0x0910);
    uint16_t dest = 0xabcd;
    /* TODO: we leave out NHC out for now => 0x1b instead of 0x1f */
    uint8_t data[] = { SIXLOWPAN_IPHC1_DISPATCH | 0x1b, 0x30, IPV6_PROTO_NUM_NONE,
                       0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
                       0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20
                     };
    ipv6_hdr_t data_exp;
    netapi_rcv_pkt_t rcv = { NETAPI_CMD_RCV, NULL, &src, sizeof(uint16_t),
                             &dest, sizeof(uint16_t), data, sizeof(data)
                           };
    sixlowpan_test_exp_rcv_t exp = { &src, sizeof(uint16_t), &dest, sizeof(uint16_t),
                                     &data_exp, sizeof(data_exp)
                                   };

    ipv6_hdr_set_version(&data_exp);
    ipv6_hdr_set_trafficclass(&data_exp, 0);
    ipv6_hdr_set_flowlabel(&data_exp, 0);
    data_exp.length.u16 = 0;
    data_exp.nextheader = IPV6_PROTO_NUM_NONE;
    data_exp.hoplimit = 255;
    data_exp.srcaddr.u64[0] = byteorder_htonll(0xfe80000000000000);
    data_exp.srcaddr.u64[1] = byteorder_htonll(0x000000fffe000910);
    data_exp.destaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    data_exp.destaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);

    res = sixlowpan_test_receiver_test(&rcv, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_iphc1_set_sam_sac_4_received(void)
{
    uint32_t res;
    uint16_t src = 0xcccc;
    uint16_t dest = 0xabcd;
    /* TODO: we leave out NHC out for now => 0x1b instead of 0x1f */
    uint8_t data[] = { SIXLOWPAN_IPHC1_DISPATCH | 0x1b, 0x40, IPV6_PROTO_NUM_NONE, 0x11,
                       0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a,
                       0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20
                     };
    ipv6_hdr_t data_exp;
    netapi_rcv_pkt_t rcv = { NETAPI_CMD_RCV, NULL, &src, sizeof(uint16_t),
                             &dest, sizeof(uint16_t), data, sizeof(data)
                           };
    sixlowpan_test_exp_rcv_t exp = { &src, sizeof(uint16_t), &dest, sizeof(uint16_t),
                                     &data_exp, sizeof(data_exp)
                                   };

    ipv6_hdr_set_version(&data_exp);
    ipv6_hdr_set_trafficclass(&data_exp, 0);
    ipv6_hdr_set_flowlabel(&data_exp, 0);
    data_exp.length.u16 = 0;
    data_exp.nextheader = IPV6_PROTO_NUM_NONE;
    data_exp.hoplimit = 255;
    data_exp.srcaddr.u64[0].u64 = 0;
    data_exp.srcaddr.u64[1].u64 = 0;
    data_exp.destaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    data_exp.destaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);

    res = sixlowpan_test_receiver_test(&rcv, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_iphc1_set_sam_sac_5_received(void)
{
    uint32_t res;
    uint16_t src = 0xcccc;
    uint16_t dest = 0xabcd;
    /* TODO: we leave out NHC out for now => 0x1b instead of 0x1f */
    uint8_t data[] = { SIXLOWPAN_IPHC1_DISPATCH | 0x1b, 0x50, IPV6_PROTO_NUM_NONE, 0x01,
                       0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12,
                       0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b,
                       0x1c, 0x1d, 0x1e, 0x1f, 0x20
                     };
    ipv6_hdr_t data_exp;
    netapi_rcv_pkt_t rcv = { NETAPI_CMD_RCV, NULL, &src, sizeof(uint16_t),
                             &dest, sizeof(uint16_t), data, sizeof(data)
                           };
    sixlowpan_test_exp_rcv_t exp = { &src, sizeof(uint16_t), &dest, sizeof(uint16_t),
                                     &data_exp, sizeof(data_exp)
                                   };

    ipv6_hdr_set_version(&data_exp);
    ipv6_hdr_set_trafficclass(&data_exp, 0);
    ipv6_hdr_set_flowlabel(&data_exp, 0);
    data_exp.length.u16 = 0;
    data_exp.nextheader = IPV6_PROTO_NUM_NONE;
    data_exp.hoplimit = 255;
    data_exp.srcaddr.u64[0] = byteorder_htonll(0x0102030405060708);
    data_exp.srcaddr.u64[1] = byteorder_htonll(0x090a0b0c0d0e0f10);
    data_exp.destaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    data_exp.destaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);

    sixlowpan_iphc_cbuf_update(0, &data_exp.srcaddr, 69, 120);

    res = sixlowpan_test_receiver_test(&rcv, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_iphc1_set_sam_sac_6_received(void)
{
    uint32_t res;
    uint16_t src = 0xcccc;
    uint16_t dest = 0xabcd;
    /* TODO: we leave out NHC out for now => 0x1b instead of 0x1f */
    uint8_t data[] = { SIXLOWPAN_IPHC1_DISPATCH | 0x1b, 0x60, IPV6_PROTO_NUM_NONE, 0xdd,
                       0xbb, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
                       0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20
                     };
    ipv6_hdr_t data_exp;
    netapi_rcv_pkt_t rcv = { NETAPI_CMD_RCV, NULL, &src, sizeof(uint16_t),
                             &dest, sizeof(uint16_t), data, sizeof(data)
                           };
    sixlowpan_test_exp_rcv_t exp = { &src, sizeof(uint16_t), &dest, sizeof(uint16_t),
                                     &data_exp, sizeof(data_exp)
                                   };

    ipv6_hdr_set_version(&data_exp);
    ipv6_hdr_set_trafficclass(&data_exp, 0);
    ipv6_hdr_set_flowlabel(&data_exp, 0);
    data_exp.length.u16 = 0;
    data_exp.nextheader = IPV6_PROTO_NUM_NONE;
    data_exp.hoplimit = 255;
    data_exp.srcaddr.u64[0] = byteorder_htonll(0x0102030405060000);
    data_exp.srcaddr.u64[1] = byteorder_htonll(0x000000fffe00ddbb);
    data_exp.destaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    data_exp.destaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);

    sixlowpan_iphc_cbuf_update(0, &data_exp.srcaddr, 47, 120);

    res = sixlowpan_test_receiver_test(&rcv, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_iphc1_set_sam_sac_7_received(void)
{
    uint32_t res;
    uint64_t src = HTONLL(0x123456789abcdef);
    uint16_t dest = 0xabcd;
    /* TODO: we leave out NHC out for now => 0x1b instead of 0x1f */
    uint8_t data[] = { SIXLOWPAN_IPHC1_DISPATCH | 0x1b, 0x70, IPV6_PROTO_NUM_NONE, 0x11,
                       0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a,
                       0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20
                     };
    ipv6_hdr_t data_exp;
    netapi_rcv_pkt_t rcv = { NETAPI_CMD_RCV, NULL, &src, sizeof(uint64_t),
                             &dest, sizeof(uint16_t), data, sizeof(data)
                           };
    sixlowpan_test_exp_rcv_t exp = { &src, sizeof(uint64_t), &dest, sizeof(uint16_t),
                                     &data_exp, sizeof(data_exp)
                                   };

    ipv6_hdr_set_version(&data_exp);
    ipv6_hdr_set_trafficclass(&data_exp, 0);
    ipv6_hdr_set_flowlabel(&data_exp, 0);
    data_exp.length.u16 = 0;
    data_exp.nextheader = IPV6_PROTO_NUM_NONE;
    data_exp.hoplimit = 255;
    data_exp.srcaddr.u64[0] = byteorder_htonll(0xa0b1c23000000000);
    data_exp.srcaddr.u64[1] = byteorder_htonll(0x0323456789abcdef);
    data_exp.destaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    data_exp.destaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);

    sixlowpan_iphc_cbuf_update(0, &data_exp.srcaddr, 28, 120);

    res = sixlowpan_test_receiver_test(&rcv, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_iphc1_set_sam_sac_7_cid_ext_received(void)
{
    uint32_t res;
    uint16_t src = HTONS(0x0112);
    uint16_t dest = 0xabcd;
    /* TODO: we leave out NHC out for now => 0x1b instead of 0x1f */
    uint8_t data[] = { SIXLOWPAN_IPHC1_DISPATCH | 0x1b, 0xf0, 0x40, IPV6_PROTO_NUM_NONE,
                       0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
                       0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20
                     };
    ipv6_hdr_t data_exp;
    netapi_rcv_pkt_t rcv = { NETAPI_CMD_RCV, NULL, &src, sizeof(uint16_t),
                             &dest, sizeof(uint16_t), data, sizeof(data)
                           };
    sixlowpan_test_exp_rcv_t exp = { &src, sizeof(uint16_t), &dest, sizeof(uint16_t),
                                     &data_exp, sizeof(data_exp)
                                   };

    ipv6_hdr_set_version(&data_exp);
    ipv6_hdr_set_trafficclass(&data_exp, 0);
    ipv6_hdr_set_flowlabel(&data_exp, 0);
    data_exp.length.u16 = 0;
    data_exp.nextheader = IPV6_PROTO_NUM_NONE;
    data_exp.hoplimit = 255;
    data_exp.srcaddr.u64[0] = byteorder_htonll(0xa0b1230000000000);
    data_exp.srcaddr.u64[1] = byteorder_htonll(0x000000fffe000112);
    data_exp.destaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    data_exp.destaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);

    sixlowpan_iphc_cbuf_update(4, &data_exp.srcaddr, 24, 120);

    res = sixlowpan_test_receiver_test(&rcv, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_iphc1_set_dam_dac_set_received_no_ctx(void)
{
    int res;
    uint16_t src = 0xcccc;
    uint16_t dest = 0xabcd;
    /* TODO: we leave out NHC out for now => 0x1b instead of 0x1f */
    uint8_t data[] = { SIXLOWPAN_IPHC1_DISPATCH | 0x1b, 0x05 };
    netapi_rcv_pkt_t rcv = { NETAPI_CMD_RCV, NULL, &src, sizeof(uint16_t),
                             &dest, sizeof(uint16_t), data, sizeof(data)
                           };

    res = netapi_send_command(sixlowpan_test_sixlowpan_pid, (netapi_cmd_t *)&rcv);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return (res < 0) ? (uint32_t)(-res) : 0;
}

static uint32_t test_compressed_iphc1_set_m_dam_dac_1_received(void)
{
    uint32_t res;
    uint16_t src = 0x1234;
    uint16_t dest = 0xcccc;
    /* TODO: we leave out NHC out for now => 0x1b instead of 0x1f */
    uint8_t data[] = { SIXLOWPAN_IPHC1_DISPATCH | 0x1b, 0x01, IPV6_PROTO_NUM_NONE, 0x11,
                       0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a,
                       0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23,
                       0x24, 0x25, 0x26, 0x27, 0x28
                     };
    ipv6_hdr_t data_exp;
    netapi_rcv_pkt_t rcv = { NETAPI_CMD_RCV, NULL, &src, sizeof(uint16_t),
                             &dest, sizeof(uint16_t), data, sizeof(data)
                           };
    sixlowpan_test_exp_rcv_t exp = { &src, sizeof(uint16_t), &dest, sizeof(uint16_t),
                                     &data_exp, sizeof(data_exp)
                                   };

    ipv6_hdr_set_version(&data_exp);
    ipv6_hdr_set_trafficclass(&data_exp, 0);
    ipv6_hdr_set_flowlabel(&data_exp, 0);
    data_exp.length.u16 = 0;
    data_exp.nextheader = IPV6_PROTO_NUM_NONE;
    data_exp.hoplimit = 255;
    data_exp.srcaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    data_exp.srcaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);
    data_exp.destaddr.u64[0] = byteorder_htonll(0xfe80000000000000);
    data_exp.destaddr.u64[1] = byteorder_htonll(0x2122232425262728);

    res = sixlowpan_test_receiver_test(&rcv, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_iphc1_set_m_dam_dac_2_received(void)
{
    uint32_t res;
    uint16_t src = 0x1234;
    uint16_t dest = 0xcccc;
    /* TODO: we leave out NHC out for now => 0x1b instead of 0x1f */
    uint8_t data[] = { SIXLOWPAN_IPHC1_DISPATCH | 0x1b, 0x02, IPV6_PROTO_NUM_NONE, 0x11,
                       0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a,
                       0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0xac, 0xdc
                     };
    ipv6_hdr_t data_exp;
    netapi_rcv_pkt_t rcv = { NETAPI_CMD_RCV, NULL, &src, sizeof(uint16_t),
                             &dest, sizeof(uint16_t), data, sizeof(data)
                           };
    sixlowpan_test_exp_rcv_t exp = { &src, sizeof(uint16_t), &dest, sizeof(uint16_t),
                                     &data_exp, sizeof(data_exp)
                                   };

    ipv6_hdr_set_version(&data_exp);
    ipv6_hdr_set_trafficclass(&data_exp, 0);
    ipv6_hdr_set_flowlabel(&data_exp, 0);
    data_exp.length.u16 = 0;
    data_exp.nextheader = IPV6_PROTO_NUM_NONE;
    data_exp.hoplimit = 255;
    data_exp.srcaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    data_exp.srcaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);
    data_exp.destaddr.u64[0] = byteorder_htonll(0xfe80000000000000);
    data_exp.destaddr.u64[1] = byteorder_htonll(0x000000fffe00acdc);

    res = sixlowpan_test_receiver_test(&rcv, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_iphc1_set_m_dam_dac_3_received(void)
{
    uint32_t res;
    uint16_t src = 0x1234;
    uint16_t dest = HTONS(0x5435);
    /* TODO: we leave out NHC out for now => 0x1b instead of 0x1f */
    uint8_t data[] = { SIXLOWPAN_IPHC1_DISPATCH | 0x1b, 0x03, IPV6_PROTO_NUM_NONE, 0x11,
                       0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a,
                       0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20
                     };
    ipv6_hdr_t data_exp;
    netapi_rcv_pkt_t rcv = { NETAPI_CMD_RCV, NULL, &src, sizeof(uint16_t),
                             &dest, sizeof(uint16_t), data, sizeof(data)
                           };
    sixlowpan_test_exp_rcv_t exp = { &src, sizeof(uint16_t), &dest, sizeof(uint16_t),
                                     &data_exp, sizeof(data_exp)
                                   };

    ipv6_hdr_set_version(&data_exp);
    ipv6_hdr_set_trafficclass(&data_exp, 0);
    ipv6_hdr_set_flowlabel(&data_exp, 0);
    data_exp.length.u16 = 0;
    data_exp.nextheader = IPV6_PROTO_NUM_NONE;
    data_exp.hoplimit = 255;
    data_exp.srcaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    data_exp.srcaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);
    data_exp.destaddr.u64[0] = byteorder_htonll(0xfe80000000000000);
    data_exp.destaddr.u64[1] = byteorder_htonll(0x000000fffe005435);

    res = sixlowpan_test_receiver_test(&rcv, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_iphc1_set_m_dam_dac_4_received(void)
{
    int res;
    uint16_t src = 0xcccc;
    uint16_t dest = 0xabcd;
    /* TODO: we leave out NHC out for now => 0x1b instead of 0x1f */
    uint8_t data[] = { SIXLOWPAN_IPHC1_DISPATCH | 0x1b, 0x04 };
    netapi_rcv_pkt_t rcv = { NETAPI_CMD_RCV, NULL, &src, sizeof(uint16_t),
                             &dest, sizeof(uint16_t), data, sizeof(data)
                           };

    res = netapi_send_command(sixlowpan_test_sixlowpan_pid, (netapi_cmd_t *)&rcv);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return (res < 0) ? (uint32_t)(-res) : 0;
}

static uint32_t test_compressed_iphc1_set_m_dam_dac_5_received(void)
{
    uint32_t res;
    uint16_t src = 0x1234;
    uint64_t dest = HTONLL(0x0a0b0c0d0e0f0001);
    /* TODO: we leave out NHC out for now => 0x1b instead of 0x1f */
    uint8_t data[] = { SIXLOWPAN_IPHC1_DISPATCH | 0x1b, 0x05, IPV6_PROTO_NUM_NONE, 0x11,
                       0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a,
                       0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x10, 0x11, 0x12,
                       0x13, 0x14, 0x15, 0x00, 0x01
                     };
    ipv6_hdr_t data_exp;
    netapi_rcv_pkt_t rcv = { NETAPI_CMD_RCV, NULL, &src, sizeof(uint16_t),
                             &dest, sizeof(uint64_t), data, sizeof(data)
                           };
    sixlowpan_test_exp_rcv_t exp = { &src, sizeof(uint16_t), &dest, sizeof(uint64_t),
                                     &data_exp, sizeof(data_exp)
                                   };

    ipv6_hdr_set_version(&data_exp);
    ipv6_hdr_set_trafficclass(&data_exp, 0);
    ipv6_hdr_set_flowlabel(&data_exp, 0);
    data_exp.length.u16 = 0;
    data_exp.nextheader = IPV6_PROTO_NUM_NONE;
    data_exp.hoplimit = 255;
    data_exp.srcaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    data_exp.srcaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);
    data_exp.destaddr.u64[0] = byteorder_htonll(0xabcde00000000000);
    data_exp.destaddr.u64[1] = byteorder_htonll(0x1011121314150001);

    sixlowpan_iphc_cbuf_update(0, &data_exp.destaddr, 20, 120);

    res = sixlowpan_test_receiver_test(&rcv, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_iphc1_set_m_dam_dac_6_received(void)
{
    uint32_t res;
    uint16_t src = 0x1234;
    uint64_t dest = HTONLL(0x0a0b0c0d0e0f0001);
    /* TODO: we leave out NHC out for now => 0x1b instead of 0x1f */
    uint8_t data[] = { SIXLOWPAN_IPHC1_DISPATCH | 0x1b, 0x06, IPV6_PROTO_NUM_NONE, 0x11,
                       0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a,
                       0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x14, 0x15
                     };
    ipv6_hdr_t data_exp;
    netapi_rcv_pkt_t rcv = { NETAPI_CMD_RCV, NULL, &src, sizeof(uint16_t),
                             &dest, sizeof(uint64_t), data, sizeof(data)
                           };
    sixlowpan_test_exp_rcv_t exp = { &src, sizeof(uint16_t), &dest, sizeof(uint64_t),
                                     &data_exp, sizeof(data_exp)
                                   };

    ipv6_hdr_set_version(&data_exp);
    ipv6_hdr_set_trafficclass(&data_exp, 0);
    ipv6_hdr_set_flowlabel(&data_exp, 0);
    data_exp.length.u16 = 0;
    data_exp.nextheader = IPV6_PROTO_NUM_NONE;
    data_exp.hoplimit = 255;
    data_exp.srcaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    data_exp.srcaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);
    data_exp.destaddr.u64[0] = byteorder_htonll(0x1234560000000000);
    data_exp.destaddr.u64[1] = byteorder_htonll(0x000000fffe001415);

    sixlowpan_iphc_cbuf_update(0, &data_exp.destaddr, 23, 120);

    res = sixlowpan_test_receiver_test(&rcv, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_iphc1_set_m_dam_dac_7_received(void)
{
    uint32_t res;
    uint16_t src = 0x1234;
    uint64_t dest = HTONLL(0x0a0b0c0d0e0f0001);
    /* TODO: we leave out NHC out for now => 0x1b instead of 0x1f */
    uint8_t data[] = { SIXLOWPAN_IPHC1_DISPATCH | 0x1b, 0x07, IPV6_PROTO_NUM_NONE, 0x11,
                       0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a,
                       0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20
                     };
    ipv6_hdr_t data_exp;
    netapi_rcv_pkt_t rcv = { NETAPI_CMD_RCV, NULL, &src, sizeof(uint16_t),
                             &dest, sizeof(uint64_t), data, sizeof(data)
                           };
    sixlowpan_test_exp_rcv_t exp = { &src, sizeof(uint16_t), &dest, sizeof(uint64_t),
                                     &data_exp, sizeof(data_exp)
                                   };

    ipv6_hdr_set_version(&data_exp);
    ipv6_hdr_set_trafficclass(&data_exp, 0);
    ipv6_hdr_set_flowlabel(&data_exp, 0);
    data_exp.length.u16 = 0;
    data_exp.nextheader = IPV6_PROTO_NUM_NONE;
    data_exp.hoplimit = 255;
    data_exp.srcaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    data_exp.srcaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);
    data_exp.destaddr.u64[0] = byteorder_htonll(0xabcdef1234567890);
    data_exp.destaddr.u64[1] = byteorder_htonll(0x1011121314150001);

    sixlowpan_iphc_cbuf_update(0, &data_exp.destaddr, 111, 120);

    res = sixlowpan_test_receiver_test(&rcv, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_iphc1_set_m_dam_dac_7_cid_ext_received(void)
{
    uint32_t res;
    uint16_t src = 0x1234;
    uint64_t dest = HTONLL(0xa3000d2fd815d39f);
    /* TODO: we leave out NHC out for now => 0x1b instead of 0x1f */
    uint8_t data[] = { SIXLOWPAN_IPHC1_DISPATCH | 0x1b, 0x87, 0x0c, IPV6_PROTO_NUM_NONE,
                       0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
                       0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20
                     };
    ipv6_hdr_t data_exp;
    netapi_rcv_pkt_t rcv = { NETAPI_CMD_RCV, NULL, &src, sizeof(uint16_t),
                             &dest, sizeof(uint64_t), data, sizeof(data)
                           };
    sixlowpan_test_exp_rcv_t exp = { &src, sizeof(uint16_t), &dest, sizeof(uint64_t),
                                     &data_exp, sizeof(data_exp)
                                   };

    ipv6_hdr_set_version(&data_exp);
    ipv6_hdr_set_trafficclass(&data_exp, 0);
    ipv6_hdr_set_flowlabel(&data_exp, 0);
    data_exp.length.u16 = 0;
    data_exp.nextheader = IPV6_PROTO_NUM_NONE;
    data_exp.hoplimit = 255;
    data_exp.srcaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    data_exp.srcaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);
    data_exp.destaddr.u64[0] = byteorder_htonll(0xcea893eb3df5dd7d);
    data_exp.destaddr.u64[1] = byteorder_htonll(0xa1000d2fd815d39f);

    sixlowpan_iphc_cbuf_update(12, &data_exp.destaddr, 64, 120);

    res = sixlowpan_test_receiver_test(&rcv, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_iphc1_set_m_dam_dac_8_received(void)
{
    uint32_t res;
    uint16_t src = 0x1234;
    uint16_t dest = 0xcccc;
    /* TODO: we leave out NHC out for now => 0x1b instead of 0x1f */
    uint8_t data[] = { SIXLOWPAN_IPHC1_DISPATCH | 0x1b, 0x08, IPV6_PROTO_NUM_NONE, 0x11,
                       0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a,
                       0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0xff, 0x01, 0x02,
                       0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
                       0x0c, 0x0d, 0x0e, 0x0f
                     };
    ipv6_hdr_t data_exp;
    netapi_rcv_pkt_t rcv = { NETAPI_CMD_RCV, NULL, &src, sizeof(uint16_t),
                             &dest, sizeof(uint16_t), data, sizeof(data)
                           };
    sixlowpan_test_exp_rcv_t exp = { &src, sizeof(uint16_t), &dest, sizeof(uint16_t),
                                     &data_exp, sizeof(data_exp)
                                   };

    ipv6_hdr_set_version(&data_exp);
    ipv6_hdr_set_trafficclass(&data_exp, 0);
    ipv6_hdr_set_flowlabel(&data_exp, 0);
    data_exp.length.u16 = 0;
    data_exp.nextheader = IPV6_PROTO_NUM_NONE;
    data_exp.hoplimit = 255;
    data_exp.srcaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    data_exp.srcaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);
    data_exp.destaddr.u64[0] = byteorder_htonll(0xff01020304050607);
    data_exp.destaddr.u64[1] = byteorder_htonll(0x08090a0b0c0d0e0f);

    res = sixlowpan_test_receiver_test(&rcv, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_iphc1_set_m_dam_dac_9_received(void)
{
    uint32_t res;
    uint16_t src = 0x1234;
    uint16_t dest = 0xcccc;
    /* TODO: we leave out NHC out for now => 0x1b instead of 0x1f */
    uint8_t data[] = { SIXLOWPAN_IPHC1_DISPATCH | 0x1b, 0x09, IPV6_PROTO_NUM_NONE, 0x11,
                       0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a,
                       0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x11, 0x22, 0x33,
                       0x44, 0x55, 0x66
                     };
    ipv6_hdr_t data_exp;
    netapi_rcv_pkt_t rcv = { NETAPI_CMD_RCV, NULL, &src, sizeof(uint16_t),
                             &dest, sizeof(uint16_t), data, sizeof(data)
                           };
    sixlowpan_test_exp_rcv_t exp = { &src, sizeof(uint16_t), &dest, sizeof(uint16_t),
                                     &data_exp, sizeof(data_exp)
                                   };

    ipv6_hdr_set_version(&data_exp);
    ipv6_hdr_set_trafficclass(&data_exp, 0);
    ipv6_hdr_set_flowlabel(&data_exp, 0);
    data_exp.length.u16 = 0;
    data_exp.nextheader = IPV6_PROTO_NUM_NONE;
    data_exp.hoplimit = 255;
    data_exp.srcaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    data_exp.srcaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);
    data_exp.destaddr.u64[0] = byteorder_htonll(0xff11000000000000);
    data_exp.destaddr.u64[1] = byteorder_htonll(0x0000002233445566);

    res = sixlowpan_test_receiver_test(&rcv, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_iphc1_set_m_dam_dac_10_received(void)
{
    uint32_t res;
    uint16_t src = 0x1234;
    uint16_t dest = 0xcccc;
    /* TODO: we leave out NHC out for now => 0x1b instead of 0x1f */
    uint8_t data[] = { SIXLOWPAN_IPHC1_DISPATCH | 0x1b, 0x0a, IPV6_PROTO_NUM_NONE, 0x11,
                       0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a,
                       0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0xaa, 0xcc, 0xee,
                       0x00
                     };
    ipv6_hdr_t data_exp;
    netapi_rcv_pkt_t rcv = { NETAPI_CMD_RCV, NULL, &src, sizeof(uint16_t),
                             &dest, sizeof(uint16_t), data, sizeof(data)
                           };
    sixlowpan_test_exp_rcv_t exp = { &src, sizeof(uint16_t), &dest, sizeof(uint16_t),
                                     &data_exp, sizeof(data_exp)
                                   };

    ipv6_hdr_set_version(&data_exp);
    ipv6_hdr_set_trafficclass(&data_exp, 0);
    ipv6_hdr_set_flowlabel(&data_exp, 0);
    data_exp.length.u16 = 0;
    data_exp.nextheader = IPV6_PROTO_NUM_NONE;
    data_exp.hoplimit = 255;
    data_exp.srcaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    data_exp.srcaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);
    data_exp.destaddr.u64[0] = byteorder_htonll(0xffaa000000000000);
    data_exp.destaddr.u64[1] = byteorder_htonll(0x0000000000ccee00);

    res = sixlowpan_test_receiver_test(&rcv, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_iphc1_set_m_dam_dac_11_received(void)
{
    uint32_t res;
    uint16_t src = 0x1234;
    uint16_t dest = 0xcccc;
    /* TODO: we leave out NHC out for now => 0x1b instead of 0x1f */
    uint8_t data[] = { SIXLOWPAN_IPHC1_DISPATCH | 0x1b, 0x0b, IPV6_PROTO_NUM_NONE, 0x11,
                       0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a,
                       0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0xdc
                     };
    ipv6_hdr_t data_exp;
    netapi_rcv_pkt_t rcv = { NETAPI_CMD_RCV, NULL, &src, sizeof(uint16_t),
                             &dest, sizeof(uint16_t), data, sizeof(data)
                           };
    sixlowpan_test_exp_rcv_t exp = { &src, sizeof(uint16_t), &dest, sizeof(uint16_t),
                                     &data_exp, sizeof(data_exp)
                                   };

    ipv6_hdr_set_version(&data_exp);
    ipv6_hdr_set_trafficclass(&data_exp, 0);
    ipv6_hdr_set_flowlabel(&data_exp, 0);
    data_exp.length.u16 = 0;
    data_exp.nextheader = IPV6_PROTO_NUM_NONE;
    data_exp.hoplimit = 255;
    data_exp.srcaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    data_exp.srcaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);
    data_exp.destaddr.u64[0] = byteorder_htonll(0xff02000000000000);
    data_exp.destaddr.u64[1] = byteorder_htonll(0x00000000000000dc);

    res = sixlowpan_test_receiver_test(&rcv, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_iphc1_set_m_dam_dac_12_received(void)
{
    uint32_t res;
    uint16_t src = 0x1234;
    uint16_t dest = 0xcccc;
    /* TODO: we leave out NHC out for now => 0x1b instead of 0x1f */
    uint8_t data[] = { SIXLOWPAN_IPHC1_DISPATCH | 0x1b, 0x0c, IPV6_PROTO_NUM_NONE, 0x11,
                       0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a,
                       0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0xff, 0xee, 0xdd,
                       0xcc, 0xbb, 0xaa
                     };
    ipv6_hdr_t data_exp;
    netapi_rcv_pkt_t rcv = { NETAPI_CMD_RCV, NULL, &src, sizeof(uint16_t),
                             &dest, sizeof(uint16_t), data, sizeof(data)
                           };
    sixlowpan_test_exp_rcv_t exp = { &src, sizeof(uint16_t), &dest, sizeof(uint16_t),
                                     &data_exp, sizeof(data_exp)
                                   };

    ipv6_hdr_set_version(&data_exp);
    ipv6_hdr_set_trafficclass(&data_exp, 0);
    ipv6_hdr_set_flowlabel(&data_exp, 0);
    data_exp.length.u16 = 0;
    data_exp.nextheader = IPV6_PROTO_NUM_NONE;
    data_exp.hoplimit = 255;
    data_exp.srcaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    data_exp.srcaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);
    data_exp.destaddr.u64[0] = byteorder_htonll(0xffffee40f6d1f993);
    data_exp.destaddr.u64[1] = byteorder_htonll(0xe19465b5ddccbbaa);

    sixlowpan_iphc_cbuf_update(0, (ipv6_addr_t *)(&(data_exp.destaddr.u16[2])), 64, 120);

    res = sixlowpan_test_receiver_test(&rcv, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_iphc1_set_m_dam_dac_13_received(void)
{
    int res;
    uint16_t src = 0xcccc;
    uint16_t dest = 0xabcd;
    /* TODO: we leave out NHC out for now => 0x1b instead of 0x1f */
    uint8_t data[] = { SIXLOWPAN_IPHC1_DISPATCH | 0x1b, 0x0d };
    netapi_rcv_pkt_t rcv = { NETAPI_CMD_RCV, NULL, &src, sizeof(uint16_t),
                             &dest, sizeof(uint16_t), data, sizeof(data)
                           };

    sixlowpan_iphc_cbuf_update(0, (ipv6_addr_t *) &src, 2, 120);

    res = netapi_send_command(sixlowpan_test_sixlowpan_pid, (netapi_cmd_t *)&rcv);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return (res < 0) ? (uint32_t)(-res) : 0;
}

static uint32_t test_compressed_iphc1_set_m_dam_dac_14_received(void)
{
    int res;
    uint16_t src = 0xcccc;
    uint16_t dest = 0xabcd;
    /* TODO: we leave out NHC out for now => 0x1b instead of 0x1f */
    uint8_t data[] = { SIXLOWPAN_IPHC1_DISPATCH | 0x1b, 0x0e };
    netapi_rcv_pkt_t rcv = { NETAPI_CMD_RCV, NULL, &src, sizeof(uint16_t),
                             &dest, sizeof(uint16_t), data, sizeof(data)
                           };

    sixlowpan_iphc_cbuf_update(0, (ipv6_addr_t *) &src, 2, 120);

    res = netapi_send_command(sixlowpan_test_sixlowpan_pid, (netapi_cmd_t *)&rcv);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return (res < 0) ? (uint32_t)(-res) : 0;
}

static uint32_t test_compressed_iphc1_set_m_dam_dac_15_received(void)
{
    int res;
    uint16_t src = 0xcccc;
    uint16_t dest = 0xabcd;
    /* TODO: we leave out NHC out for now => 0x1b instead of 0x1f */
    uint8_t data[] = { SIXLOWPAN_IPHC1_DISPATCH | 0x1b, 0x0f };
    netapi_rcv_pkt_t rcv = { NETAPI_CMD_RCV, NULL, &src, sizeof(uint16_t),
                             &dest, sizeof(uint16_t), data, sizeof(data)
                           };

    sixlowpan_iphc_cbuf_update(0, (ipv6_addr_t *) &src, 2, 120);

    res = netapi_send_command(sixlowpan_test_sixlowpan_pid, (netapi_cmd_t *)&rcv);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return (res < 0) ? (uint32_t)(-res) : 0;
}

static inline ipv6_hdr_t *_get_ipv6_hdr(uint8_t *data)
{
    return (ipv6_hdr_t *)data;
}

static uint32_t test_2_compressed_fragments_received(void)
{
    uint32_t res;
    uint64_t src = HTONLL(0x604bb9c965be46b1);
    uint64_t dest = HTONLL(0xf9ed52a68384de12);
    uint8_t data1[] = { 0, 0, 0, 0, SIXLOWPAN_IPHC1_DISPATCH | 0x1b, 0x33, IPV6_PROTO_NUM_NONE, 0x02, 0x03, 0x04, 0x05, 0x06 };
    uint8_t data2[] = { 0, 0, 0, 0, 0, 0x07, 0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14 };
    uint8_t data_exp[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                           0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10,
                           0x11, 0x12, 0x13, 0x14
                         };

    netapi_rcv_pkt_t rcv[] = {
        { NETAPI_CMD_RCV, NULL, &src, sizeof(uint64_t), &dest, sizeof(uint64_t), data1, sizeof(data1) },
        { NETAPI_CMD_RCV, NULL, &src, sizeof(uint64_t), &dest, sizeof(uint64_t), data2, sizeof(data2) },
    };

    sixlowpan_test_exp_rcv_t exp = { &src, sizeof(uint64_t), &dest, sizeof(uint64_t),
                                     data_exp, sizeof(data_exp)
                                   };

    sixlowpan_init_frag1_dispatch(data1, 16, 1);
    sixlowpan_init_fragn_dispatch(data2, 16, 1, 1);

    ipv6_hdr_set_version(_get_ipv6_hdr(data_exp));
    ipv6_hdr_set_trafficclass(_get_ipv6_hdr(data_exp), 0);
    ipv6_hdr_set_flowlabel(_get_ipv6_hdr(data_exp), 0);
    (_get_ipv6_hdr(data_exp))->length = byteorder_htons(13);
    (_get_ipv6_hdr(data_exp))->nextheader = IPV6_PROTO_NUM_NONE;
    (_get_ipv6_hdr(data_exp))->hoplimit = 255;
    (_get_ipv6_hdr(data_exp))->srcaddr.u64[0] = byteorder_htonll(0xfe80000000000000);
    (_get_ipv6_hdr(data_exp))->srcaddr.u64[1] = byteorder_htonll(0x624bb9c965be46b1);
    (_get_ipv6_hdr(data_exp))->destaddr.u64[0] = byteorder_htonll(0xfe80000000000000);
    (_get_ipv6_hdr(data_exp))->destaddr.u64[1] = byteorder_htonll(0xfbed52a68384de12);

    res = sixlowpan_test_receiver_test(rcv, 2, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_all_zero_send(void)
{
    uint32_t res;
    uint16_t dest = 0xabcd;
    ipv6_hdr_t ipv6_hdr;
    uint8_t dispatch[] = { SIXLOWPAN_IPHC1_DISPATCH, 0, 0x43, 0x04, 0x56, 0x78,
                           IPV6_PROTO_NUM_NONE, 0x25, 0x01, 0x02, 0x03, 0x04,
                           0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d,
                           0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
                           0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
                           0x20
                         };
    netdev_hlist_t ulh = { NULL, NULL, NETDEV_PROTO_IPV6, &ipv6_hdr, sizeof(ipv6_hdr_t) };
    netdev_hlist_t exp_ulh = { NULL, NULL, NETDEV_PROTO_6LOWPAN, dispatch, sizeof(dispatch) };
    netapi_snd_pkt_t snd = { NETAPI_CMD_SND, NULL, &ulh, &dest, sizeof(uint16_t),
                             NULL, 0
                           };
    sixlowpan_test_exp_snd_t exp = { &exp_ulh, &dest, sizeof(uint16_t), NULL, 0 };

    ulh.next = &ulh;
    ulh.prev = &ulh;
    exp_ulh.next = &exp_ulh;
    exp_ulh.prev = &exp_ulh;

    ipv6_hdr_set_version(&ipv6_hdr);
    ipv6_hdr_set_trafficclass_dscp(&ipv6_hdr, 3);
    ipv6_hdr_set_trafficclass_ecn(&ipv6_hdr, 1);
    ipv6_hdr_set_flowlabel(&ipv6_hdr, 0x45678);
    ipv6_hdr.length.u16 = 0;
    ipv6_hdr.nextheader = IPV6_PROTO_NUM_NONE;
    ipv6_hdr.hoplimit = 0x25;
    ipv6_hdr.srcaddr.u64[0] = byteorder_htonll(0x0102030405060708);
    ipv6_hdr.srcaddr.u64[1] = byteorder_htonll(0x090a0b0c0d0e0f10);
    ipv6_hdr.destaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    ipv6_hdr.destaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);

    if (sixlowpan_hc_set_status(sixlowpan_test_sixlowpan_pid,
                                SIXLOWPAN_HC_ENABLE) < 0) {
        return SIXLOWPAN_TEST_SET_HC_STATUS_ERROR;
    }

    res = sixlowpan_test_send_test(&snd, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_dscp_elided_send(void)
{
    uint32_t res;
    uint16_t dest = 0xabcd;
    ipv6_hdr_t ipv6_hdr;
    uint8_t dispatch[] = { SIXLOWPAN_IPHC1_DISPATCH | 0x08, 0, 0x44, 0x56, 0x78,
                           IPV6_PROTO_NUM_NONE, 0x25, 0x01, 0x02, 0x03, 0x04,
                           0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d,
                           0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
                           0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
                           0x20
                         };
    netdev_hlist_t ulh = { NULL, NULL, NETDEV_PROTO_IPV6, &ipv6_hdr, sizeof(ipv6_hdr_t) };
    netdev_hlist_t exp_ulh = { NULL, NULL, NETDEV_PROTO_6LOWPAN, dispatch, sizeof(dispatch) };
    netapi_snd_pkt_t snd = { NETAPI_CMD_SND, NULL, &ulh, &dest, sizeof(uint16_t),
                             NULL, 0
                           };
    sixlowpan_test_exp_snd_t exp = { &exp_ulh, &dest, sizeof(uint16_t), NULL, 0 };

    ulh.next = &ulh;
    ulh.prev = &ulh;
    exp_ulh.next = &exp_ulh;
    exp_ulh.prev = &exp_ulh;

    ipv6_hdr_set_version(&ipv6_hdr);
    ipv6_hdr_set_trafficclass_dscp(&ipv6_hdr, 0);
    ipv6_hdr_set_trafficclass_ecn(&ipv6_hdr, 1);
    ipv6_hdr_set_flowlabel(&ipv6_hdr, 0x45678);
    ipv6_hdr.length.u16 = 0;
    ipv6_hdr.nextheader = IPV6_PROTO_NUM_NONE;
    ipv6_hdr.hoplimit = 0x25;
    ipv6_hdr.srcaddr.u64[0] = byteorder_htonll(0x0102030405060708);
    ipv6_hdr.srcaddr.u64[1] = byteorder_htonll(0x090a0b0c0d0e0f10);
    ipv6_hdr.destaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    ipv6_hdr.destaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);

    if (sixlowpan_hc_set_status(sixlowpan_test_sixlowpan_pid,
                                SIXLOWPAN_HC_ENABLE) < 0) {
        return SIXLOWPAN_TEST_SET_HC_STATUS_ERROR;
    }

    res = sixlowpan_test_send_test(&snd, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_fl_elided_send(void)
{
    uint32_t res;
    uint16_t dest = 0xabcd;
    ipv6_hdr_t ipv6_hdr;
    uint8_t dispatch[] = { SIXLOWPAN_IPHC1_DISPATCH | 0x10, 0, 0x43,
                           IPV6_PROTO_NUM_NONE, 0x25, 0x01, 0x02, 0x03, 0x04,
                           0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d,
                           0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
                           0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
                           0x20
                         };
    netdev_hlist_t ulh = { NULL, NULL, NETDEV_PROTO_IPV6, &ipv6_hdr, sizeof(ipv6_hdr_t) };
    netdev_hlist_t exp_ulh = { NULL, NULL, NETDEV_PROTO_6LOWPAN, dispatch, sizeof(dispatch) };
    netapi_snd_pkt_t snd = { NETAPI_CMD_SND, NULL, &ulh, &dest, sizeof(uint16_t),
                             NULL, 0
                           };
    sixlowpan_test_exp_snd_t exp = { &exp_ulh, &dest, sizeof(uint16_t), NULL, 0 };

    ulh.next = &ulh;
    ulh.prev = &ulh;
    exp_ulh.next = &exp_ulh;
    exp_ulh.prev = &exp_ulh;

    ipv6_hdr_set_version(&ipv6_hdr);
    ipv6_hdr_set_trafficclass_dscp(&ipv6_hdr, 3);
    ipv6_hdr_set_trafficclass_ecn(&ipv6_hdr, 1);
    ipv6_hdr_set_flowlabel(&ipv6_hdr, 0);
    ipv6_hdr.length.u16 = 0;
    ipv6_hdr.nextheader = IPV6_PROTO_NUM_NONE;
    ipv6_hdr.hoplimit = 0x25;
    ipv6_hdr.srcaddr.u64[0] = byteorder_htonll(0x0102030405060708);
    ipv6_hdr.srcaddr.u64[1] = byteorder_htonll(0x090a0b0c0d0e0f10);
    ipv6_hdr.destaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    ipv6_hdr.destaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);

    if (sixlowpan_hc_set_status(sixlowpan_test_sixlowpan_pid,
                                SIXLOWPAN_HC_ENABLE) < 0) {
        return SIXLOWPAN_TEST_SET_HC_STATUS_ERROR;
    }

    res = sixlowpan_test_send_test(&snd, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_tc_fl_elided_send(void)
{
    uint32_t res;
    uint16_t dest = 0xabcd;
    ipv6_hdr_t ipv6_hdr;
    uint8_t dispatch[] = { SIXLOWPAN_IPHC1_DISPATCH | SIXLOWPAN_IPHC1_TF, 0,
                           IPV6_PROTO_NUM_NONE, 0x25, 0x01, 0x02, 0x03, 0x04,
                           0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d,
                           0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
                           0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
                           0x20
                         };
    netdev_hlist_t ulh = { NULL, NULL, NETDEV_PROTO_IPV6, &ipv6_hdr, sizeof(ipv6_hdr_t) };
    netdev_hlist_t exp_ulh = { NULL, NULL, NETDEV_PROTO_6LOWPAN, dispatch, sizeof(dispatch) };
    netapi_snd_pkt_t snd = { NETAPI_CMD_SND, NULL, &ulh, &dest, sizeof(uint16_t),
                             NULL, 0
                           };
    sixlowpan_test_exp_snd_t exp = { &exp_ulh, &dest, sizeof(uint16_t), NULL, 0 };

    ulh.next = &ulh;
    ulh.prev = &ulh;
    exp_ulh.next = &exp_ulh;
    exp_ulh.prev = &exp_ulh;

    ipv6_hdr_set_version(&ipv6_hdr);
    ipv6_hdr_set_trafficclass_dscp(&ipv6_hdr, 0);
    ipv6_hdr_set_trafficclass_ecn(&ipv6_hdr, 0);
    ipv6_hdr_set_flowlabel(&ipv6_hdr, 0);
    ipv6_hdr.length.u16 = 0;
    ipv6_hdr.nextheader = IPV6_PROTO_NUM_NONE;
    ipv6_hdr.hoplimit = 0x25;
    ipv6_hdr.srcaddr.u64[0] = byteorder_htonll(0x0102030405060708);
    ipv6_hdr.srcaddr.u64[1] = byteorder_htonll(0x090a0b0c0d0e0f10);
    ipv6_hdr.destaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    ipv6_hdr.destaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);

    if (sixlowpan_hc_set_status(sixlowpan_test_sixlowpan_pid,
                                SIXLOWPAN_HC_ENABLE) < 0) {
        return SIXLOWPAN_TEST_SET_HC_STATUS_ERROR;
    }

    res = sixlowpan_test_send_test(&snd, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_tf_elided_hl_1_send(void)
{
    uint32_t res;
    uint16_t dest = 0xabcd;
    ipv6_hdr_t ipv6_hdr;
    uint8_t dispatch[] = { SIXLOWPAN_IPHC1_DISPATCH | SIXLOWPAN_IPHC1_TF | 0x1, 0,
                           IPV6_PROTO_NUM_NONE, 0x01, 0x02, 0x03, 0x04, 0x05,
                           0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e,
                           0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                           0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20
                         };
    netdev_hlist_t ulh = { NULL, NULL, NETDEV_PROTO_IPV6, &ipv6_hdr, sizeof(ipv6_hdr_t) };
    netdev_hlist_t exp_ulh = { NULL, NULL, NETDEV_PROTO_6LOWPAN, dispatch, sizeof(dispatch) };
    netapi_snd_pkt_t snd = { NETAPI_CMD_SND, NULL, &ulh, &dest, sizeof(uint16_t),
                             NULL, 0
                           };
    sixlowpan_test_exp_snd_t exp = { &exp_ulh, &dest, sizeof(uint16_t), NULL, 0 };

    ulh.next = &ulh;
    ulh.prev = &ulh;
    exp_ulh.next = &exp_ulh;
    exp_ulh.prev = &exp_ulh;

    ipv6_hdr_set_version(&ipv6_hdr);
    ipv6_hdr_set_trafficclass_dscp(&ipv6_hdr, 0);
    ipv6_hdr_set_trafficclass_ecn(&ipv6_hdr, 0);
    ipv6_hdr_set_flowlabel(&ipv6_hdr, 0);
    ipv6_hdr.length.u16 = 0;
    ipv6_hdr.nextheader = IPV6_PROTO_NUM_NONE;
    ipv6_hdr.hoplimit = 1;
    ipv6_hdr.srcaddr.u64[0] = byteorder_htonll(0x0102030405060708);
    ipv6_hdr.srcaddr.u64[1] = byteorder_htonll(0x090a0b0c0d0e0f10);
    ipv6_hdr.destaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    ipv6_hdr.destaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);

    if (sixlowpan_hc_set_status(sixlowpan_test_sixlowpan_pid,
                                SIXLOWPAN_HC_ENABLE) < 0) {
        return SIXLOWPAN_TEST_SET_HC_STATUS_ERROR;
    }

    res = sixlowpan_test_send_test(&snd, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_tf_elided_hl_64_send(void)
{
    uint32_t res;
    uint16_t dest = 0xabcd;
    ipv6_hdr_t ipv6_hdr;
    uint8_t dispatch[] = { SIXLOWPAN_IPHC1_DISPATCH | SIXLOWPAN_IPHC1_TF | 0x2, 0,
                           IPV6_PROTO_NUM_NONE, 0x01, 0x02, 0x03, 0x04, 0x05,
                           0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e,
                           0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                           0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20
                         };
    netdev_hlist_t ulh = { NULL, NULL, NETDEV_PROTO_IPV6, &ipv6_hdr, sizeof(ipv6_hdr_t) };
    netdev_hlist_t exp_ulh = { NULL, NULL, NETDEV_PROTO_6LOWPAN, dispatch, sizeof(dispatch) };
    netapi_snd_pkt_t snd = { NETAPI_CMD_SND, NULL, &ulh, &dest, sizeof(uint16_t),
                             NULL, 0
                           };
    sixlowpan_test_exp_snd_t exp = { &exp_ulh, &dest, sizeof(uint16_t), NULL, 0 };

    ulh.next = &ulh;
    ulh.prev = &ulh;
    exp_ulh.next = &exp_ulh;
    exp_ulh.prev = &exp_ulh;

    ipv6_hdr_set_version(&ipv6_hdr);
    ipv6_hdr_set_trafficclass_dscp(&ipv6_hdr, 0);
    ipv6_hdr_set_trafficclass_ecn(&ipv6_hdr, 0);
    ipv6_hdr_set_flowlabel(&ipv6_hdr, 0);
    ipv6_hdr.length.u16 = 0;
    ipv6_hdr.nextheader = IPV6_PROTO_NUM_NONE;
    ipv6_hdr.hoplimit = 64;
    ipv6_hdr.srcaddr.u64[0] = byteorder_htonll(0x0102030405060708);
    ipv6_hdr.srcaddr.u64[1] = byteorder_htonll(0x090a0b0c0d0e0f10);
    ipv6_hdr.destaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    ipv6_hdr.destaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);

    if (sixlowpan_hc_set_status(sixlowpan_test_sixlowpan_pid,
                                SIXLOWPAN_HC_ENABLE) < 0) {
        return SIXLOWPAN_TEST_SET_HC_STATUS_ERROR;
    }

    res = sixlowpan_test_send_test(&snd, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_tf_elided_hl_255_send(void)
{
    uint32_t res;
    uint16_t dest = 0xabcd;
    ipv6_hdr_t ipv6_hdr;
    uint8_t dispatch[] = { SIXLOWPAN_IPHC1_DISPATCH | SIXLOWPAN_IPHC1_TF | 0x3, 0,
                           IPV6_PROTO_NUM_NONE, 0x01, 0x02, 0x03, 0x04, 0x05,
                           0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e,
                           0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                           0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20
                         };
    netdev_hlist_t ulh = { NULL, NULL, NETDEV_PROTO_IPV6, &ipv6_hdr, sizeof(ipv6_hdr_t) };
    netdev_hlist_t exp_ulh = { NULL, NULL, NETDEV_PROTO_6LOWPAN, dispatch, sizeof(dispatch) };
    netapi_snd_pkt_t snd = { NETAPI_CMD_SND, NULL, &ulh, &dest, sizeof(uint16_t),
                             NULL, 0
                           };
    sixlowpan_test_exp_snd_t exp = { &exp_ulh, &dest, sizeof(uint16_t), NULL, 0 };

    ulh.next = &ulh;
    ulh.prev = &ulh;
    exp_ulh.next = &exp_ulh;
    exp_ulh.prev = &exp_ulh;

    ipv6_hdr_set_version(&ipv6_hdr);
    ipv6_hdr_set_trafficclass_dscp(&ipv6_hdr, 0);
    ipv6_hdr_set_trafficclass_ecn(&ipv6_hdr, 0);
    ipv6_hdr_set_flowlabel(&ipv6_hdr, 0);
    ipv6_hdr.length.u16 = 0;
    ipv6_hdr.nextheader = IPV6_PROTO_NUM_NONE;
    ipv6_hdr.hoplimit = 255;
    ipv6_hdr.srcaddr.u64[0] = byteorder_htonll(0x0102030405060708);
    ipv6_hdr.srcaddr.u64[1] = byteorder_htonll(0x090a0b0c0d0e0f10);
    ipv6_hdr.destaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    ipv6_hdr.destaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);

    if (sixlowpan_hc_set_status(sixlowpan_test_sixlowpan_pid,
                                SIXLOWPAN_HC_ENABLE) < 0) {
        return SIXLOWPAN_TEST_SET_HC_STATUS_ERROR;
    }

    res = sixlowpan_test_send_test(&snd, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_iphc1_set_sam_sac_1_send(void)
{
    uint32_t res;
    uint16_t dest = 0xabcd;
    ipv6_hdr_t ipv6_hdr;
    uint8_t dispatch[] = { SIXLOWPAN_IPHC1_DISPATCH | 0x1b, 0x10,
                           IPV6_PROTO_NUM_NONE, 0x01, 0x02, 0x03, 0x04, 0x0a,
                           0x0b, 0x0c, 0x0d, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
                           0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20
                         };
    netdev_hlist_t ulh = { NULL, NULL, NETDEV_PROTO_IPV6, &ipv6_hdr, sizeof(ipv6_hdr_t) };
    netdev_hlist_t exp_ulh = { NULL, NULL, NETDEV_PROTO_6LOWPAN, dispatch, sizeof(dispatch) };
    netapi_snd_pkt_t snd = { NETAPI_CMD_SND, NULL, &ulh, &dest, sizeof(uint16_t),
                             NULL, 0
                           };
    sixlowpan_test_exp_snd_t exp = { &exp_ulh, &dest, sizeof(uint16_t), NULL, 0 };

    ulh.next = &ulh;
    ulh.prev = &ulh;
    exp_ulh.next = &exp_ulh;
    exp_ulh.prev = &exp_ulh;

    ipv6_hdr_set_version(&ipv6_hdr);
    ipv6_hdr_set_trafficclass_dscp(&ipv6_hdr, 0);
    ipv6_hdr_set_trafficclass_ecn(&ipv6_hdr, 0);
    ipv6_hdr_set_flowlabel(&ipv6_hdr, 0);
    ipv6_hdr.length.u16 = 0;
    ipv6_hdr.nextheader = IPV6_PROTO_NUM_NONE;
    ipv6_hdr.hoplimit = 255;
    ipv6_hdr.srcaddr.u64[0] = byteorder_htonll(0xfe80000000000000);
    ipv6_hdr.srcaddr.u64[1] = byteorder_htonll(0x010203040a0b0c0d);
    ipv6_hdr.destaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    ipv6_hdr.destaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);

    if (sixlowpan_hc_set_status(sixlowpan_test_sixlowpan_pid,
                                SIXLOWPAN_HC_ENABLE) < 0) {
        return SIXLOWPAN_TEST_SET_HC_STATUS_ERROR;
    }

    res = sixlowpan_test_send_test(&snd, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_iphc1_set_sam_sac_2_send(void)
{
    uint32_t res;
    uint16_t dest = 0xabcd;
    ipv6_hdr_t ipv6_hdr;
    uint8_t dispatch[] = { SIXLOWPAN_IPHC1_DISPATCH | 0x1b, 0x20,
                           IPV6_PROTO_NUM_NONE, 0x07, 0x08, 0x11, 0x12, 0x13,
                           0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c,
                           0x1d, 0x1e, 0x1f, 0x20
                         };
    netdev_hlist_t ulh = { NULL, NULL, NETDEV_PROTO_IPV6, &ipv6_hdr, sizeof(ipv6_hdr_t) };
    netdev_hlist_t exp_ulh = { NULL, NULL, NETDEV_PROTO_6LOWPAN, dispatch, sizeof(dispatch) };
    netapi_snd_pkt_t snd = { NETAPI_CMD_SND, NULL, &ulh, &dest, sizeof(uint16_t),
                             NULL, 0
                           };
    sixlowpan_test_exp_snd_t exp = { &exp_ulh, &dest, sizeof(uint16_t), NULL, 0 };

    ulh.next = &ulh;
    ulh.prev = &ulh;
    exp_ulh.next = &exp_ulh;
    exp_ulh.prev = &exp_ulh;

    ipv6_hdr_set_version(&ipv6_hdr);
    ipv6_hdr_set_trafficclass_dscp(&ipv6_hdr, 0);
    ipv6_hdr_set_trafficclass_ecn(&ipv6_hdr, 0);
    ipv6_hdr_set_flowlabel(&ipv6_hdr, 0);
    ipv6_hdr.length.u16 = 0;
    ipv6_hdr.nextheader = IPV6_PROTO_NUM_NONE;
    ipv6_hdr.hoplimit = 255;
    ipv6_hdr.srcaddr.u64[0] = byteorder_htonll(0xfe80000000000000);
    ipv6_hdr.srcaddr.u64[1] = byteorder_htonll(0x000000fffe000708);
    ipv6_hdr.destaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    ipv6_hdr.destaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);

    if (sixlowpan_hc_set_status(sixlowpan_test_sixlowpan_pid,
                                SIXLOWPAN_HC_ENABLE) < 0) {
        return SIXLOWPAN_TEST_SET_HC_STATUS_ERROR;
    }

    res = sixlowpan_test_send_test(&snd, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_iphc1_set_sam_sac_3_send(void)
{
    uint32_t res;
    size_t src_len_mode = 2;
    uint16_t src = HTONS(0x0910);
    uint16_t dest = 0xabcd;
    ipv6_hdr_t ipv6_hdr;
    uint8_t dispatch[] = { SIXLOWPAN_IPHC1_DISPATCH | 0x1b, 0x30,
                           IPV6_PROTO_NUM_NONE, 0x11, 0x12, 0x13, 0x14, 0x15,
                           0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e,
                           0x1f, 0x20
                         };
    netdev_hlist_t ulh = { NULL, NULL, NETDEV_PROTO_IPV6, &ipv6_hdr, sizeof(ipv6_hdr_t) };
    netdev_hlist_t exp_ulh = { NULL, NULL, NETDEV_PROTO_6LOWPAN, dispatch, sizeof(dispatch) };
    netapi_snd_pkt_t snd = { NETAPI_CMD_SND, NULL, &ulh, &dest, sizeof(uint16_t),
                             NULL, 0
                           };
    sixlowpan_test_exp_snd_t exp = { &exp_ulh, &dest, sizeof(uint16_t), NULL, 0 };

    ulh.next = &ulh;
    ulh.prev = &ulh;
    exp_ulh.next = &exp_ulh;
    exp_ulh.prev = &exp_ulh;

    ipv6_hdr_set_version(&ipv6_hdr);
    ipv6_hdr_set_trafficclass_dscp(&ipv6_hdr, 0);
    ipv6_hdr_set_trafficclass_ecn(&ipv6_hdr, 0);
    ipv6_hdr_set_flowlabel(&ipv6_hdr, 0);
    ipv6_hdr.length.u16 = 0;
    ipv6_hdr.nextheader = IPV6_PROTO_NUM_NONE;
    ipv6_hdr.hoplimit = 255;
    ipv6_hdr.srcaddr.u64[0] = byteorder_htonll(0xfe80000000000000);
    ipv6_hdr.srcaddr.u64[1] = byteorder_htonll(0x000000fffe000910);
    ipv6_hdr.destaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    ipv6_hdr.destaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);

    if (netapi_set_option(sixlowpan_test_sixlowpan_pid,
                          NETDEV_OPT_SRC_LEN, &src_len_mode, sizeof(size_t)) < 0) {
        return SIXLOWPAN_TEST_SET_OPTION_ERROR;
    }

    if (netapi_set_option(sixlowpan_test_sixlowpan_pid,
                          NETDEV_OPT_ADDRESS, &src, sizeof(uint16_t)) < 0) {
        return SIXLOWPAN_TEST_SET_OPTION_ERROR;
    }

    if (sixlowpan_hc_set_status(sixlowpan_test_sixlowpan_pid,
                                SIXLOWPAN_HC_ENABLE) < 0) {
        return SIXLOWPAN_TEST_SET_HC_STATUS_ERROR;
    }

    res = sixlowpan_test_send_test(&snd, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_iphc1_set_sam_sac_4_send(void)
{
    uint32_t res;
    uint16_t dest = 0xabcd;
    ipv6_hdr_t ipv6_hdr;
    uint8_t dispatch[] = { SIXLOWPAN_IPHC1_DISPATCH | 0x1b, 0x40,
                           IPV6_PROTO_NUM_NONE, 0x11, 0x12, 0x13, 0x14, 0x15,
                           0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e,
                           0x1f, 0x20
                         };
    netdev_hlist_t ulh = { NULL, NULL, NETDEV_PROTO_IPV6, &ipv6_hdr, sizeof(ipv6_hdr_t) };
    netdev_hlist_t exp_ulh = { NULL, NULL, NETDEV_PROTO_6LOWPAN, dispatch, sizeof(dispatch) };
    netapi_snd_pkt_t snd = { NETAPI_CMD_SND, NULL, &ulh, &dest, sizeof(uint16_t),
                             NULL, 0
                           };
    sixlowpan_test_exp_snd_t exp = { &exp_ulh, &dest, sizeof(uint16_t), NULL, 0 };

    ulh.next = &ulh;
    ulh.prev = &ulh;
    exp_ulh.next = &exp_ulh;
    exp_ulh.prev = &exp_ulh;

    ipv6_hdr_set_version(&ipv6_hdr);
    ipv6_hdr_set_trafficclass_dscp(&ipv6_hdr, 0);
    ipv6_hdr_set_trafficclass_ecn(&ipv6_hdr, 0);
    ipv6_hdr_set_flowlabel(&ipv6_hdr, 0);
    ipv6_hdr.length.u16 = 0;
    ipv6_hdr.nextheader = IPV6_PROTO_NUM_NONE;
    ipv6_hdr.hoplimit = 255;
    ipv6_hdr.srcaddr.u64[0].u64 = 0;
    ipv6_hdr.srcaddr.u64[1].u64 = 0;
    ipv6_hdr.destaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    ipv6_hdr.destaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);

    if (sixlowpan_hc_set_status(sixlowpan_test_sixlowpan_pid,
                                SIXLOWPAN_HC_ENABLE) < 0) {
        return SIXLOWPAN_TEST_SET_HC_STATUS_ERROR;
    }

    res = sixlowpan_test_send_test(&snd, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_iphc1_set_sam_sac_5_send(void)
{
    uint32_t res;
    uint16_t dest = 0xabcd;
    ipv6_hdr_t ipv6_hdr;
    uint8_t dispatch[] = { SIXLOWPAN_IPHC1_DISPATCH | 0x1b, 0x50,
                           IPV6_PROTO_NUM_NONE, 0x09, 0x0a, 0x0b, 0x0c, 0x0d,
                           0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
                           0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e,
                           0x1f, 0x20
                         };
    netdev_hlist_t ulh = { NULL, NULL, NETDEV_PROTO_IPV6, &ipv6_hdr, sizeof(ipv6_hdr_t) };
    netdev_hlist_t exp_ulh = { NULL, NULL, NETDEV_PROTO_6LOWPAN, dispatch, sizeof(dispatch) };
    netapi_snd_pkt_t snd = { NETAPI_CMD_SND, NULL, &ulh, &dest, sizeof(uint16_t),
                             NULL, 0
                           };
    sixlowpan_test_exp_snd_t exp = { &exp_ulh, &dest, sizeof(uint16_t), NULL, 0 };

    ulh.next = &ulh;
    ulh.prev = &ulh;
    exp_ulh.next = &exp_ulh;
    exp_ulh.prev = &exp_ulh;

    ipv6_hdr_set_version(&ipv6_hdr);
    ipv6_hdr_set_trafficclass_dscp(&ipv6_hdr, 0);
    ipv6_hdr_set_trafficclass_ecn(&ipv6_hdr, 0);
    ipv6_hdr_set_flowlabel(&ipv6_hdr, 0);
    ipv6_hdr.length.u16 = 0;
    ipv6_hdr.nextheader = IPV6_PROTO_NUM_NONE;
    ipv6_hdr.hoplimit = 255;
    ipv6_hdr.srcaddr.u64[0] = byteorder_htonll(0x0102030405060708);
    ipv6_hdr.srcaddr.u64[1] = byteorder_htonll(0x090a0b0c0d0e0f10);
    ipv6_hdr.destaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    ipv6_hdr.destaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);

    sixlowpan_iphc_cbuf_update(0, &ipv6_hdr.srcaddr, 69, 120);

    if (sixlowpan_hc_set_status(sixlowpan_test_sixlowpan_pid,
                                SIXLOWPAN_HC_ENABLE) < 0) {
        return SIXLOWPAN_TEST_SET_HC_STATUS_ERROR;
    }

    res = sixlowpan_test_send_test(&snd, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_iphc1_set_sam_sac_6_send(void)
{
    uint32_t res;
    uint16_t dest = 0xabcd;
    ipv6_hdr_t ipv6_hdr;
    uint8_t dispatch[] = { SIXLOWPAN_IPHC1_DISPATCH | 0x1b, 0x60,
                           IPV6_PROTO_NUM_NONE, 0xdd, 0xbb, 0x11, 0x12, 0x13,
                           0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c,
                           0x1d, 0x1e, 0x1f, 0x20
                         };
    netdev_hlist_t ulh = { NULL, NULL, NETDEV_PROTO_IPV6, &ipv6_hdr, sizeof(ipv6_hdr_t) };
    netdev_hlist_t exp_ulh = { NULL, NULL, NETDEV_PROTO_6LOWPAN, dispatch, sizeof(dispatch) };
    netapi_snd_pkt_t snd = { NETAPI_CMD_SND, NULL, &ulh, &dest, sizeof(uint16_t),
                             NULL, 0
                           };
    sixlowpan_test_exp_snd_t exp = { &exp_ulh, &dest, sizeof(uint16_t), NULL, 0 };

    ulh.next = &ulh;
    ulh.prev = &ulh;
    exp_ulh.next = &exp_ulh;
    exp_ulh.prev = &exp_ulh;

    ipv6_hdr_set_version(&ipv6_hdr);
    ipv6_hdr_set_trafficclass_dscp(&ipv6_hdr, 0);
    ipv6_hdr_set_trafficclass_ecn(&ipv6_hdr, 0);
    ipv6_hdr_set_flowlabel(&ipv6_hdr, 0);
    ipv6_hdr.length.u16 = 0;
    ipv6_hdr.nextheader = IPV6_PROTO_NUM_NONE;
    ipv6_hdr.hoplimit = 255;
    ipv6_hdr.srcaddr.u64[0] = byteorder_htonll(0x0102030405060000);
    ipv6_hdr.srcaddr.u64[1] = byteorder_htonll(0x000000fffe00ddbb);
    ipv6_hdr.destaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    ipv6_hdr.destaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);

    sixlowpan_iphc_cbuf_update(0, &ipv6_hdr.srcaddr, 47, 120);

    if (sixlowpan_hc_set_status(sixlowpan_test_sixlowpan_pid,
                                SIXLOWPAN_HC_ENABLE) < 0) {
        return SIXLOWPAN_TEST_SET_HC_STATUS_ERROR;
    }

    res = sixlowpan_test_send_test(&snd, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_iphc1_set_sam_sac_7_send(void)
{
    uint32_t res;
    size_t src_len_mode = 8;
    uint64_t src = HTONLL(0x123456789abcdef);
    uint16_t dest = 0xabcd;
    ipv6_hdr_t ipv6_hdr;
    uint8_t dispatch[] = { SIXLOWPAN_IPHC1_DISPATCH | 0x1b, 0x70,
                           IPV6_PROTO_NUM_NONE, 0x11, 0x12, 0x13, 0x14, 0x15,
                           0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d,
                           0x1e, 0x1f, 0x20
                         };
    netdev_hlist_t ulh = { NULL, NULL, NETDEV_PROTO_IPV6, &ipv6_hdr, sizeof(ipv6_hdr_t) };
    netdev_hlist_t exp_ulh = { NULL, NULL, NETDEV_PROTO_6LOWPAN, dispatch, sizeof(dispatch) };
    netapi_snd_pkt_t snd = { NETAPI_CMD_SND, NULL, &ulh, &dest, sizeof(uint16_t),
                             NULL, 0
                           };
    sixlowpan_test_exp_snd_t exp = { &exp_ulh, &dest, sizeof(uint16_t), NULL, 0 };

    ulh.next = &ulh;
    ulh.prev = &ulh;
    exp_ulh.next = &exp_ulh;
    exp_ulh.prev = &exp_ulh;

    ipv6_hdr_set_version(&ipv6_hdr);
    ipv6_hdr_set_trafficclass_dscp(&ipv6_hdr, 0);
    ipv6_hdr_set_trafficclass_ecn(&ipv6_hdr, 0);
    ipv6_hdr_set_flowlabel(&ipv6_hdr, 0);
    ipv6_hdr.length.u16 = 0;
    ipv6_hdr.nextheader = IPV6_PROTO_NUM_NONE;
    ipv6_hdr.hoplimit = 255;
    ipv6_hdr.srcaddr.u64[0] = byteorder_htonll(0xa0b1c23000000000);
    ipv6_hdr.srcaddr.u64[1] = byteorder_htonll(0x0323456789abcdef);
    ipv6_hdr.destaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    ipv6_hdr.destaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);

    sixlowpan_iphc_cbuf_update(0, &ipv6_hdr.srcaddr, 28, 120);

    if (netapi_set_option(sixlowpan_test_sixlowpan_pid,
                          NETDEV_OPT_SRC_LEN, &src_len_mode, sizeof(size_t)) < 0) {
        return SIXLOWPAN_TEST_SET_OPTION_ERROR;
    }

    if (netapi_set_option(sixlowpan_test_sixlowpan_pid,
                          NETDEV_OPT_ADDRESS_LONG, &src, sizeof(uint64_t)) < 0) {
        return SIXLOWPAN_TEST_SET_OPTION_ERROR;
    }

    if (sixlowpan_hc_set_status(sixlowpan_test_sixlowpan_pid,
                                SIXLOWPAN_HC_ENABLE) < 0) {
        return SIXLOWPAN_TEST_SET_HC_STATUS_ERROR;
    }

    res = sixlowpan_test_send_test(&snd, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_iphc1_set_sam_sac_7_cid_ext_send(void)
{
    uint32_t res;
    size_t src_len_mode = 8;
    uint64_t src = HTONLL(0x123456789abcdfe);
    uint16_t dest = 0xabcd;
    ipv6_hdr_t ipv6_hdr;
    uint8_t dispatch[] = { SIXLOWPAN_IPHC1_DISPATCH | 0x1b, 0xf0, 0x40,
                           IPV6_PROTO_NUM_NONE, 0x11, 0x12, 0x13, 0x14, 0x15,
                           0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d,
                           0x1e, 0x1f, 0x20
                         };
    netdev_hlist_t ulh = { NULL, NULL, NETDEV_PROTO_IPV6, &ipv6_hdr, sizeof(ipv6_hdr_t) };
    netdev_hlist_t exp_ulh = { NULL, NULL, NETDEV_PROTO_6LOWPAN, dispatch, sizeof(dispatch) };
    netapi_snd_pkt_t snd = { NETAPI_CMD_SND, NULL, &ulh, &dest, sizeof(uint16_t),
                             NULL, 0
                           };
    sixlowpan_test_exp_snd_t exp = { &exp_ulh, &dest, sizeof(uint16_t), NULL, 0 };

    ulh.next = &ulh;
    ulh.prev = &ulh;
    exp_ulh.next = &exp_ulh;
    exp_ulh.prev = &exp_ulh;

    ipv6_hdr_set_version(&ipv6_hdr);
    ipv6_hdr_set_trafficclass_dscp(&ipv6_hdr, 0);
    ipv6_hdr_set_trafficclass_ecn(&ipv6_hdr, 0);
    ipv6_hdr_set_flowlabel(&ipv6_hdr, 0);
    ipv6_hdr.length.u16 = 0;
    ipv6_hdr.nextheader = IPV6_PROTO_NUM_NONE;
    ipv6_hdr.hoplimit = 255;
    ipv6_hdr.srcaddr.u64[0] = byteorder_htonll(0xa0b1c23000000000);
    ipv6_hdr.srcaddr.u64[1] = byteorder_htonll(0x0323456789abcdfe);
    ipv6_hdr.destaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    ipv6_hdr.destaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);

    sixlowpan_iphc_cbuf_update(4, &ipv6_hdr.srcaddr, 24, 120);

    if (netapi_set_option(sixlowpan_test_sixlowpan_pid,
                          NETDEV_OPT_SRC_LEN, &src_len_mode, sizeof(size_t)) < 0) {
        return SIXLOWPAN_TEST_SET_OPTION_ERROR;
    }

    if (netapi_set_option(sixlowpan_test_sixlowpan_pid,
                          NETDEV_OPT_ADDRESS_LONG, &src, sizeof(uint64_t)) < 0) {
        return SIXLOWPAN_TEST_SET_OPTION_ERROR;
    }

    if (sixlowpan_hc_set_status(sixlowpan_test_sixlowpan_pid,
                                SIXLOWPAN_HC_ENABLE) < 0) {
        return SIXLOWPAN_TEST_SET_HC_STATUS_ERROR;
    }

    res = sixlowpan_test_send_test(&snd, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_iphc1_set_m_dam_dac_1_send(void)
{
    uint32_t res;
    uint16_t dest = 0xabcd;
    ipv6_hdr_t ipv6_hdr;
    uint8_t dispatch[] = { SIXLOWPAN_IPHC1_DISPATCH | 0x1b, 0x01,
                           IPV6_PROTO_NUM_NONE, 0x11, 0x12, 0x13, 0x14, 0x15,
                           0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e,
                           0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
                           0x28
                         };
    netdev_hlist_t ulh = { NULL, NULL, NETDEV_PROTO_IPV6, &ipv6_hdr, sizeof(ipv6_hdr_t) };
    netdev_hlist_t exp_ulh = { NULL, NULL, NETDEV_PROTO_6LOWPAN, dispatch, sizeof(dispatch) };
    netapi_snd_pkt_t snd = { NETAPI_CMD_SND, NULL, &ulh, &dest, sizeof(uint16_t),
                             NULL, 0
                           };
    sixlowpan_test_exp_snd_t exp = { &exp_ulh, &dest, sizeof(uint16_t), NULL, 0 };

    ulh.next = &ulh;
    ulh.prev = &ulh;
    exp_ulh.next = &exp_ulh;
    exp_ulh.prev = &exp_ulh;

    ipv6_hdr_set_version(&ipv6_hdr);
    ipv6_hdr_set_trafficclass_dscp(&ipv6_hdr, 0);
    ipv6_hdr_set_trafficclass_ecn(&ipv6_hdr, 0);
    ipv6_hdr_set_flowlabel(&ipv6_hdr, 0);
    ipv6_hdr.length.u16 = 0;
    ipv6_hdr.nextheader = IPV6_PROTO_NUM_NONE;
    ipv6_hdr.hoplimit = 255;
    ipv6_hdr.srcaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    ipv6_hdr.srcaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);
    ipv6_hdr.destaddr.u64[0] = byteorder_htonll(0xfe80000000000000);
    ipv6_hdr.destaddr.u64[1] = byteorder_htonll(0x2122232425262728);

    if (sixlowpan_hc_set_status(sixlowpan_test_sixlowpan_pid,
                                SIXLOWPAN_HC_ENABLE) < 0) {
        return SIXLOWPAN_TEST_SET_HC_STATUS_ERROR;
    }

    res = sixlowpan_test_send_test(&snd, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_iphc1_set_m_dam_dac_2_send(void)
{
    uint32_t res;
    uint16_t dest = 0xabcd;
    ipv6_hdr_t ipv6_hdr;
    uint8_t dispatch[] = { SIXLOWPAN_IPHC1_DISPATCH | 0x1b, 0x02,
                           IPV6_PROTO_NUM_NONE, 0x11, 0x12, 0x13, 0x14, 0x15,
                           0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e,
                           0x1f, 0x20, 0xac, 0xdc
                         };
    netdev_hlist_t ulh = { NULL, NULL, NETDEV_PROTO_IPV6, &ipv6_hdr, sizeof(ipv6_hdr_t) };
    netdev_hlist_t exp_ulh = { NULL, NULL, NETDEV_PROTO_6LOWPAN, dispatch, sizeof(dispatch) };
    netapi_snd_pkt_t snd = { NETAPI_CMD_SND, NULL, &ulh, &dest, sizeof(uint16_t),
                             NULL, 0
                           };
    sixlowpan_test_exp_snd_t exp = { &exp_ulh, &dest, sizeof(uint16_t), NULL, 0 };

    ulh.next = &ulh;
    ulh.prev = &ulh;
    exp_ulh.next = &exp_ulh;
    exp_ulh.prev = &exp_ulh;

    ipv6_hdr_set_version(&ipv6_hdr);
    ipv6_hdr_set_trafficclass_dscp(&ipv6_hdr, 0);
    ipv6_hdr_set_trafficclass_ecn(&ipv6_hdr, 0);
    ipv6_hdr_set_flowlabel(&ipv6_hdr, 0);
    ipv6_hdr.length.u16 = 0;
    ipv6_hdr.nextheader = IPV6_PROTO_NUM_NONE;
    ipv6_hdr.hoplimit = 255;
    ipv6_hdr.srcaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    ipv6_hdr.srcaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);
    ipv6_hdr.destaddr.u64[0] = byteorder_htonll(0xfe80000000000000);
    ipv6_hdr.destaddr.u64[1] = byteorder_htonll(0x000000fffe00acdc);

    if (sixlowpan_hc_set_status(sixlowpan_test_sixlowpan_pid,
                                SIXLOWPAN_HC_ENABLE) < 0) {
        return SIXLOWPAN_TEST_SET_HC_STATUS_ERROR;
    }

    res = sixlowpan_test_send_test(&snd, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_iphc1_set_m_dam_dac_3_send(void)
{
    uint32_t res;
    uint16_t dest = HTONS(0x5435);
    ipv6_hdr_t ipv6_hdr;
    uint8_t dispatch[] = { SIXLOWPAN_IPHC1_DISPATCH | 0x1b, 0x03,
                           IPV6_PROTO_NUM_NONE, 0x11, 0x12, 0x13, 0x14, 0x15,
                           0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e,
                           0x1f, 0x20
                         };
    netdev_hlist_t ulh = { NULL, NULL, NETDEV_PROTO_IPV6, &ipv6_hdr, sizeof(ipv6_hdr_t) };
    netdev_hlist_t exp_ulh = { NULL, NULL, NETDEV_PROTO_6LOWPAN, dispatch, sizeof(dispatch) };
    netapi_snd_pkt_t snd = { NETAPI_CMD_SND, NULL, &ulh, &dest, sizeof(uint16_t),
                             NULL, 0
                           };
    sixlowpan_test_exp_snd_t exp = { &exp_ulh, &dest, sizeof(uint16_t), NULL, 0 };

    ulh.next = &ulh;
    ulh.prev = &ulh;
    exp_ulh.next = &exp_ulh;
    exp_ulh.prev = &exp_ulh;

    ipv6_hdr_set_version(&ipv6_hdr);
    ipv6_hdr_set_trafficclass_dscp(&ipv6_hdr, 0);
    ipv6_hdr_set_trafficclass_ecn(&ipv6_hdr, 0);
    ipv6_hdr_set_flowlabel(&ipv6_hdr, 0);
    ipv6_hdr.length.u16 = 0;
    ipv6_hdr.nextheader = IPV6_PROTO_NUM_NONE;
    ipv6_hdr.hoplimit = 255;
    ipv6_hdr.srcaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    ipv6_hdr.srcaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);
    ipv6_hdr.destaddr.u64[0] = byteorder_htonll(0xfe80000000000000);
    ipv6_hdr.destaddr.u64[1] = byteorder_htonll(0x000000fffe005435);

    if (sixlowpan_hc_set_status(sixlowpan_test_sixlowpan_pid,
                                SIXLOWPAN_HC_ENABLE) < 0) {
        return SIXLOWPAN_TEST_SET_HC_STATUS_ERROR;
    }

    res = sixlowpan_test_send_test(&snd, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_iphc1_set_m_dam_dac_5_send(void)
{
    uint32_t res;
    uint64_t dest = HTONLL(0x0a0b0c0d0e0f0001);
    ipv6_hdr_t ipv6_hdr;
    uint8_t dispatch[] = { SIXLOWPAN_IPHC1_DISPATCH | 0x1b, 0x05,
                           IPV6_PROTO_NUM_NONE, 0x11, 0x12, 0x13, 0x14, 0x15,
                           0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e,
                           0x1f, 0x20, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x00,
                           0x01
                         };
    netdev_hlist_t ulh = { NULL, NULL, NETDEV_PROTO_IPV6, &ipv6_hdr, sizeof(ipv6_hdr_t) };
    netdev_hlist_t exp_ulh = { NULL, NULL, NETDEV_PROTO_6LOWPAN, dispatch, sizeof(dispatch) };
    netapi_snd_pkt_t snd = { NETAPI_CMD_SND, NULL, &ulh, &dest, sizeof(uint64_t),
                             NULL, 0
                           };
    sixlowpan_test_exp_snd_t exp = { &exp_ulh, &dest, sizeof(uint64_t), NULL, 0 };

    ulh.next = &ulh;
    ulh.prev = &ulh;
    exp_ulh.next = &exp_ulh;
    exp_ulh.prev = &exp_ulh;

    ipv6_hdr_set_version(&ipv6_hdr);
    ipv6_hdr_set_trafficclass_dscp(&ipv6_hdr, 0);
    ipv6_hdr_set_trafficclass_ecn(&ipv6_hdr, 0);
    ipv6_hdr_set_flowlabel(&ipv6_hdr, 0);
    ipv6_hdr.length.u16 = 0;
    ipv6_hdr.nextheader = IPV6_PROTO_NUM_NONE;
    ipv6_hdr.hoplimit = 255;
    ipv6_hdr.srcaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    ipv6_hdr.srcaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);
    ipv6_hdr.destaddr.u64[0] = byteorder_htonll(0xabcde00000000000);
    ipv6_hdr.destaddr.u64[1] = byteorder_htonll(0x1011121314150001);

    sixlowpan_iphc_cbuf_update(0, &ipv6_hdr.destaddr, 20, 120);

    if (sixlowpan_hc_set_status(sixlowpan_test_sixlowpan_pid,
                                SIXLOWPAN_HC_ENABLE) < 0) {
        return SIXLOWPAN_TEST_SET_HC_STATUS_ERROR;
    }

    res = sixlowpan_test_send_test(&snd, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_iphc1_set_m_dam_dac_6_send(void)
{
    uint32_t res;
    uint64_t dest = HTONLL(0x0a0b0c0d0e0f0001);
    ipv6_hdr_t ipv6_hdr;
    uint8_t dispatch[] = { SIXLOWPAN_IPHC1_DISPATCH | 0x1b, 0x06,
                           IPV6_PROTO_NUM_NONE, 0x11, 0x12, 0x13, 0x14, 0x15,
                           0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e,
                           0x1f, 0x20, 0x14, 0x15
                         };
    netdev_hlist_t ulh = { NULL, NULL, NETDEV_PROTO_IPV6, &ipv6_hdr, sizeof(ipv6_hdr_t) };
    netdev_hlist_t exp_ulh = { NULL, NULL, NETDEV_PROTO_6LOWPAN, dispatch, sizeof(dispatch) };
    netapi_snd_pkt_t snd = { NETAPI_CMD_SND, NULL, &ulh, &dest, sizeof(uint64_t),
                             NULL, 0
                           };
    sixlowpan_test_exp_snd_t exp = { &exp_ulh, &dest, sizeof(uint64_t), NULL, 0 };

    ulh.next = &ulh;
    ulh.prev = &ulh;
    exp_ulh.next = &exp_ulh;
    exp_ulh.prev = &exp_ulh;

    ipv6_hdr_set_version(&ipv6_hdr);
    ipv6_hdr_set_trafficclass_dscp(&ipv6_hdr, 0);
    ipv6_hdr_set_trafficclass_ecn(&ipv6_hdr, 0);
    ipv6_hdr_set_flowlabel(&ipv6_hdr, 0);
    ipv6_hdr.length.u16 = 0;
    ipv6_hdr.nextheader = IPV6_PROTO_NUM_NONE;
    ipv6_hdr.hoplimit = 255;
    ipv6_hdr.srcaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    ipv6_hdr.srcaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);
    ipv6_hdr.destaddr.u64[0] = byteorder_htonll(0xabcdef1234567890);
    ipv6_hdr.destaddr.u64[1] = byteorder_htonll(0x000000fffe001415);

    sixlowpan_iphc_cbuf_update(0, &ipv6_hdr.destaddr, 111, 120);

    if (sixlowpan_hc_set_status(sixlowpan_test_sixlowpan_pid,
                                SIXLOWPAN_HC_ENABLE) < 0) {
        return SIXLOWPAN_TEST_SET_HC_STATUS_ERROR;
    }

    res = sixlowpan_test_send_test(&snd, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_iphc1_set_m_dam_dac_7_send(void)
{
    uint32_t res;
    uint64_t dest = HTONLL(0x0a0b0c0d0e0f0001);
    ipv6_hdr_t ipv6_hdr;
    uint8_t dispatch[] = { SIXLOWPAN_IPHC1_DISPATCH | 0x1b, 0x07,
                           IPV6_PROTO_NUM_NONE, 0x11, 0x12, 0x13, 0x14, 0x15,
                           0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e,
                           0x1f, 0x20
                         };
    netdev_hlist_t ulh = { NULL, NULL, NETDEV_PROTO_IPV6, &ipv6_hdr, sizeof(ipv6_hdr_t) };
    netdev_hlist_t exp_ulh = { NULL, NULL, NETDEV_PROTO_6LOWPAN, dispatch, sizeof(dispatch) };
    netapi_snd_pkt_t snd = { NETAPI_CMD_SND, NULL, &ulh, &dest, sizeof(uint64_t),
                             NULL, 0
                           };
    sixlowpan_test_exp_snd_t exp = { &exp_ulh, &dest, sizeof(uint64_t), NULL, 0 };

    ulh.next = &ulh;
    ulh.prev = &ulh;
    exp_ulh.next = &exp_ulh;
    exp_ulh.prev = &exp_ulh;

    ipv6_hdr_set_version(&ipv6_hdr);
    ipv6_hdr_set_trafficclass_dscp(&ipv6_hdr, 0);
    ipv6_hdr_set_trafficclass_ecn(&ipv6_hdr, 0);
    ipv6_hdr_set_flowlabel(&ipv6_hdr, 0);
    ipv6_hdr.length.u16 = 0;
    ipv6_hdr.nextheader = IPV6_PROTO_NUM_NONE;
    ipv6_hdr.hoplimit = 255;
    ipv6_hdr.srcaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    ipv6_hdr.srcaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);
    ipv6_hdr.destaddr.u64[0] = byteorder_htonll(0xabcdef1234567890);
    ipv6_hdr.destaddr.u64[1] = byteorder_htonll(0x1011121314150001);

    sixlowpan_iphc_cbuf_update(0, &ipv6_hdr.destaddr, 111, 120);

    if (sixlowpan_hc_set_status(sixlowpan_test_sixlowpan_pid,
                                SIXLOWPAN_HC_ENABLE) < 0) {
        return SIXLOWPAN_TEST_SET_HC_STATUS_ERROR;
    }

    res = sixlowpan_test_send_test(&snd, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_iphc1_set_m_dam_dac_8_send(void)
{
    uint32_t res;
    uint16_t dest = 0xabcd;
    ipv6_hdr_t ipv6_hdr;
    uint8_t dispatch[] = { SIXLOWPAN_IPHC1_DISPATCH | 0x1b, 0x08,
                           IPV6_PROTO_NUM_NONE, 0x11, 0x12, 0x13, 0x14, 0x15,
                           0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e,
                           0x1f, 0x20, 0xff, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
                           0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
                         };
    netdev_hlist_t ulh = { NULL, NULL, NETDEV_PROTO_IPV6, &ipv6_hdr, sizeof(ipv6_hdr_t) };
    netdev_hlist_t exp_ulh = { NULL, NULL, NETDEV_PROTO_6LOWPAN, dispatch, sizeof(dispatch) };
    netapi_snd_pkt_t snd = { NETAPI_CMD_SND, NULL, &ulh, &dest, sizeof(uint16_t),
                             NULL, 0
                           };
    sixlowpan_test_exp_snd_t exp = { &exp_ulh, &dest, sizeof(uint16_t), NULL, 0 };

    ulh.next = &ulh;
    ulh.prev = &ulh;
    exp_ulh.next = &exp_ulh;
    exp_ulh.prev = &exp_ulh;

    ipv6_hdr_set_version(&ipv6_hdr);
    ipv6_hdr_set_trafficclass_dscp(&ipv6_hdr, 0);
    ipv6_hdr_set_trafficclass_ecn(&ipv6_hdr, 0);
    ipv6_hdr_set_flowlabel(&ipv6_hdr, 0);
    ipv6_hdr.length.u16 = 0;
    ipv6_hdr.nextheader = IPV6_PROTO_NUM_NONE;
    ipv6_hdr.hoplimit = 255;
    ipv6_hdr.srcaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    ipv6_hdr.srcaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);
    ipv6_hdr.destaddr.u64[0] = byteorder_htonll(0xff01020304050607);
    ipv6_hdr.destaddr.u64[1] = byteorder_htonll(0x08090a0b0c0d0e0f);

    if (sixlowpan_hc_set_status(sixlowpan_test_sixlowpan_pid,
                                SIXLOWPAN_HC_ENABLE) < 0) {
        return SIXLOWPAN_TEST_SET_HC_STATUS_ERROR;
    }

    res = sixlowpan_test_send_test(&snd, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_iphc1_set_m_dam_dac_9_send(void)
{
    uint32_t res;
    uint16_t dest = 0xabcd;
    ipv6_hdr_t ipv6_hdr;
    uint8_t dispatch[] = { SIXLOWPAN_IPHC1_DISPATCH | 0x1b, 0x09,
                           IPV6_PROTO_NUM_NONE, 0x11, 0x12, 0x13, 0x14, 0x15,
                           0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e,
                           0x1f, 0x20, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66
                         };
    netdev_hlist_t ulh = { NULL, NULL, NETDEV_PROTO_IPV6, &ipv6_hdr, sizeof(ipv6_hdr_t) };
    netdev_hlist_t exp_ulh = { NULL, NULL, NETDEV_PROTO_6LOWPAN, dispatch, sizeof(dispatch) };
    netapi_snd_pkt_t snd = { NETAPI_CMD_SND, NULL, &ulh, &dest, sizeof(uint16_t),
                             NULL, 0
                           };
    sixlowpan_test_exp_snd_t exp = { &exp_ulh, &dest, sizeof(uint16_t), NULL, 0 };

    ulh.next = &ulh;
    ulh.prev = &ulh;
    exp_ulh.next = &exp_ulh;
    exp_ulh.prev = &exp_ulh;

    ipv6_hdr_set_version(&ipv6_hdr);
    ipv6_hdr_set_trafficclass_dscp(&ipv6_hdr, 0);
    ipv6_hdr_set_trafficclass_ecn(&ipv6_hdr, 0);
    ipv6_hdr_set_flowlabel(&ipv6_hdr, 0);
    ipv6_hdr.length.u16 = 0;
    ipv6_hdr.nextheader = IPV6_PROTO_NUM_NONE;
    ipv6_hdr.hoplimit = 255;
    ipv6_hdr.srcaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    ipv6_hdr.srcaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);
    ipv6_hdr.destaddr.u64[0] = byteorder_htonll(0xff11000000000000);
    ipv6_hdr.destaddr.u64[1] = byteorder_htonll(0x0000002233445566);

    if (sixlowpan_hc_set_status(sixlowpan_test_sixlowpan_pid,
                                SIXLOWPAN_HC_ENABLE) < 0) {
        return SIXLOWPAN_TEST_SET_HC_STATUS_ERROR;
    }

    res = sixlowpan_test_send_test(&snd, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_iphc1_set_m_dam_dac_10_send(void)
{
    uint32_t res;
    uint16_t dest = 0xabcd;
    ipv6_hdr_t ipv6_hdr;
    uint8_t dispatch[] = { SIXLOWPAN_IPHC1_DISPATCH | 0x1b, 0x0a,
                           IPV6_PROTO_NUM_NONE, 0x11, 0x12, 0x13, 0x14, 0x15,
                           0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e,
                           0x1f, 0x20, 0xaa, 0xcc, 0xee, 0x00
                         };
    netdev_hlist_t ulh = { NULL, NULL, NETDEV_PROTO_IPV6, &ipv6_hdr, sizeof(ipv6_hdr_t) };
    netdev_hlist_t exp_ulh = { NULL, NULL, NETDEV_PROTO_6LOWPAN, dispatch, sizeof(dispatch) };
    netapi_snd_pkt_t snd = { NETAPI_CMD_SND, NULL, &ulh, &dest, sizeof(uint16_t),
                             NULL, 0
                           };
    sixlowpan_test_exp_snd_t exp = { &exp_ulh, &dest, sizeof(uint16_t), NULL, 0 };

    ulh.next = &ulh;
    ulh.prev = &ulh;
    exp_ulh.next = &exp_ulh;
    exp_ulh.prev = &exp_ulh;

    ipv6_hdr_set_version(&ipv6_hdr);
    ipv6_hdr_set_trafficclass_dscp(&ipv6_hdr, 0);
    ipv6_hdr_set_trafficclass_ecn(&ipv6_hdr, 0);
    ipv6_hdr_set_flowlabel(&ipv6_hdr, 0);
    ipv6_hdr.length.u16 = 0;
    ipv6_hdr.nextheader = IPV6_PROTO_NUM_NONE;
    ipv6_hdr.hoplimit = 255;
    ipv6_hdr.srcaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    ipv6_hdr.srcaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);
    ipv6_hdr.destaddr.u64[0] = byteorder_htonll(0xffaa000000000000);
    ipv6_hdr.destaddr.u64[1] = byteorder_htonll(0x0000000000ccee00);

    if (sixlowpan_hc_set_status(sixlowpan_test_sixlowpan_pid,
                                SIXLOWPAN_HC_ENABLE) < 0) {
        return SIXLOWPAN_TEST_SET_HC_STATUS_ERROR;
    }

    res = sixlowpan_test_send_test(&snd, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_iphc1_set_m_dam_dac_11_send(void)
{
    uint32_t res;
    uint16_t dest = 0xabcd;
    ipv6_hdr_t ipv6_hdr;
    uint8_t dispatch[] = { SIXLOWPAN_IPHC1_DISPATCH | 0x1b, 0x0b,
                           IPV6_PROTO_NUM_NONE, 0x11, 0x12, 0x13, 0x14, 0x15,
                           0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e,
                           0x1f, 0x20, 0xdc
                         };
    netdev_hlist_t ulh = { NULL, NULL, NETDEV_PROTO_IPV6, &ipv6_hdr, sizeof(ipv6_hdr_t) };
    netdev_hlist_t exp_ulh = { NULL, NULL, NETDEV_PROTO_6LOWPAN, dispatch, sizeof(dispatch) };
    netapi_snd_pkt_t snd = { NETAPI_CMD_SND, NULL, &ulh, &dest, sizeof(uint16_t),
                             NULL, 0
                           };
    sixlowpan_test_exp_snd_t exp = { &exp_ulh, &dest, sizeof(uint16_t), NULL, 0 };

    ulh.next = &ulh;
    ulh.prev = &ulh;
    exp_ulh.next = &exp_ulh;
    exp_ulh.prev = &exp_ulh;

    ipv6_hdr_set_version(&ipv6_hdr);
    ipv6_hdr_set_trafficclass_dscp(&ipv6_hdr, 0);
    ipv6_hdr_set_trafficclass_ecn(&ipv6_hdr, 0);
    ipv6_hdr_set_flowlabel(&ipv6_hdr, 0);
    ipv6_hdr.length.u16 = 0;
    ipv6_hdr.nextheader = IPV6_PROTO_NUM_NONE;
    ipv6_hdr.hoplimit = 255;
    ipv6_hdr.srcaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    ipv6_hdr.srcaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);
    ipv6_hdr.destaddr.u64[0] = byteorder_htonll(0xff02000000000000);
    ipv6_hdr.destaddr.u64[1] = byteorder_htonll(0x00000000000000dc);

    if (sixlowpan_hc_set_status(sixlowpan_test_sixlowpan_pid,
                                SIXLOWPAN_HC_ENABLE) < 0) {
        return SIXLOWPAN_TEST_SET_HC_STATUS_ERROR;
    }

    res = sixlowpan_test_send_test(&snd, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_iphc1_set_m_dam_dac_12_send(void)
{
    uint32_t res;
    uint16_t dest = 0xabcd;
    ipv6_hdr_t ipv6_hdr;
    uint8_t dispatch[] = { SIXLOWPAN_IPHC1_DISPATCH | 0x1b, 0x0c,
                           IPV6_PROTO_NUM_NONE, 0x11, 0x12, 0x13, 0x14, 0x15,
                           0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e,
                           0x1f, 0x20, 0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa
                         };
    netdev_hlist_t ulh = { NULL, NULL, NETDEV_PROTO_IPV6, &ipv6_hdr, sizeof(ipv6_hdr_t) };
    netdev_hlist_t exp_ulh = { NULL, NULL, NETDEV_PROTO_6LOWPAN, dispatch, sizeof(dispatch) };
    netapi_snd_pkt_t snd = { NETAPI_CMD_SND, NULL, &ulh, &dest, sizeof(uint16_t),
                             NULL, 0
                           };
    sixlowpan_test_exp_snd_t exp = { &exp_ulh, &dest, sizeof(uint16_t), NULL, 0 };

    ulh.next = &ulh;
    ulh.prev = &ulh;
    exp_ulh.next = &exp_ulh;
    exp_ulh.prev = &exp_ulh;

    ipv6_hdr_set_version(&ipv6_hdr);
    ipv6_hdr_set_trafficclass_dscp(&ipv6_hdr, 0);
    ipv6_hdr_set_trafficclass_ecn(&ipv6_hdr, 0);
    ipv6_hdr_set_flowlabel(&ipv6_hdr, 0);
    ipv6_hdr.length.u16 = 0;
    ipv6_hdr.nextheader = IPV6_PROTO_NUM_NONE;
    ipv6_hdr.hoplimit = 255;
    ipv6_hdr.srcaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    ipv6_hdr.srcaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);
    ipv6_hdr.destaddr.u64[0] = byteorder_htonll(0xffffee40f6d1f993);
    ipv6_hdr.destaddr.u64[1] = byteorder_htonll(0xe19465b5ddccbbaa);

    sixlowpan_iphc_cbuf_update(0, (ipv6_addr_t *)(&(ipv6_hdr.destaddr.u16[2])), 64, 120);

    if (sixlowpan_hc_set_status(sixlowpan_test_sixlowpan_pid,
                                SIXLOWPAN_HC_ENABLE) < 0) {
        return SIXLOWPAN_TEST_SET_HC_STATUS_ERROR;
    }

    res = sixlowpan_test_send_test(&snd, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_2_compressed_fragments_send(void)
{
    uint32_t res;
    uint16_t dest = 0xabcd;
    ipv6_hdr_t ipv6_hdr;
    char data[] = "abcdefghijklmnopqrstuvwxyz0123456789"
                  "abcdefghijklmnopqrstuvwxyz0123456789"
                  "abcdefghijklmnopqrstuvwxyz0123456789";
    uint8_t exp_data1[] = { 0xc0, 0x95, 0, 0, SIXLOWPAN_IPHC1_DISPATCH, 0, 0x43,
                            0x04, 0x56, 0x78, IPV6_PROTO_NUM_NONE, 0x25, 0x01,
                            0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a,
                            0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13,
                            0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c,
                            0x1d, 0x1e, 0x1f, 0x20, 'a', 'b', 'c', 'd', 'e', 'f',
                            'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q',
                            'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1',
                            '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c',
                            'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                            'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y',
                            'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                            'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'
                          };
    uint8_t exp_data2[] = "\xe0\x95\0\0\x0fijklmnopqrstuvwxyz0123456789";
    netdev_hlist_t ulh = { NULL, NULL, NETDEV_PROTO_IPV6, &ipv6_hdr, sizeof(ipv6_hdr_t) };
    netapi_snd_pkt_t snd = { NETAPI_CMD_SND, NULL, &ulh, &dest, sizeof(uint16_t),
                             data, sizeof(data)
                           };
    sixlowpan_test_exp_snd_t exp[] = { { NULL, &dest, sizeof(uint16_t), exp_data1, sizeof(exp_data1) },
                                       { NULL, &dest, sizeof(uint16_t), exp_data2, sizeof(exp_data2) }
                                     };

    ulh.next = &ulh;
    ulh.prev = &ulh;

    ipv6_hdr_set_version(&ipv6_hdr);
    ipv6_hdr_set_trafficclass_dscp(&ipv6_hdr, 3);
    ipv6_hdr_set_trafficclass_ecn(&ipv6_hdr, 1);
    ipv6_hdr_set_flowlabel(&ipv6_hdr, 0x45678);
    ipv6_hdr.length.u16 = 0;
    ipv6_hdr.nextheader = IPV6_PROTO_NUM_NONE;
    ipv6_hdr.hoplimit = 0x25;
    ipv6_hdr.srcaddr.u64[0] = byteorder_htonll(0x0102030405060708);
    ipv6_hdr.srcaddr.u64[1] = byteorder_htonll(0x090a0b0c0d0e0f10);
    ipv6_hdr.destaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    ipv6_hdr.destaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);

    sixlowpan_iphc_cbuf_update(0, (ipv6_addr_t *)(&(ipv6_hdr.destaddr.u16[2])), 64, 120);

    if (sixlowpan_hc_set_status(sixlowpan_test_sixlowpan_pid,
                                SIXLOWPAN_HC_ENABLE) < 0) {
        return SIXLOWPAN_TEST_SET_HC_STATUS_ERROR;
    }

    res = sixlowpan_test_send_test(&snd, 1, exp, 2);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

static uint32_t test_compressed_with_other_header(void)
{
    uint32_t res;
    uint16_t dest = 0xabcd;
    ipv6_hdr_t ipv6_hdr;
    uint8_t dispatch[] = { SIXLOWPAN_IPHC1_DISPATCH | 0x1b, 0x0c,
                           IPV6_PROTO_NUM_NONE, 0x11, 0x12, 0x13, 0x14, 0x15,
                           0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e,
                           0x1f, 0x20, 0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa
                         };
    netdev_hlist_t ulh1 = { NULL, NULL, NETDEV_PROTO_IPV6, &ipv6_hdr, sizeof(ipv6_hdr_t) };
    netdev_hlist_t ulh2 = { NULL, NULL, NETDEV_PROTO_UNKNOWN, "abcdef", sizeof("abcdef") };
    netdev_hlist_t exp_ulh1 = { NULL, NULL, NETDEV_PROTO_6LOWPAN, dispatch, sizeof(dispatch) };
    netdev_hlist_t exp_ulh2 = { NULL, NULL, NETDEV_PROTO_UNKNOWN, "abcdef", sizeof("abcdef") };
    netapi_snd_pkt_t snd = { NETAPI_CMD_SND, NULL, &ulh1, &dest, sizeof(uint16_t),
                             NULL, 0
                           };
    sixlowpan_test_exp_snd_t exp = { &exp_ulh1, &dest, sizeof(uint16_t), NULL, 0 };

    ulh1.next = &ulh2;
    ulh1.prev = &ulh2;
    ulh2.next = &ulh1;
    ulh2.prev = &ulh1;
    exp_ulh1.next = &exp_ulh2;
    exp_ulh1.prev = &exp_ulh2;
    exp_ulh2.next = &exp_ulh1;
    exp_ulh2.prev = &exp_ulh1;

    ipv6_hdr_set_version(&ipv6_hdr);
    ipv6_hdr_set_trafficclass_dscp(&ipv6_hdr, 0);
    ipv6_hdr_set_trafficclass_ecn(&ipv6_hdr, 0);
    ipv6_hdr_set_flowlabel(&ipv6_hdr, 0);
    ipv6_hdr.length.u16 = 0;
    ipv6_hdr.nextheader = IPV6_PROTO_NUM_NONE;
    ipv6_hdr.hoplimit = 255;
    ipv6_hdr.srcaddr.u64[0] = byteorder_htonll(0x1112131415161718);
    ipv6_hdr.srcaddr.u64[1] = byteorder_htonll(0x191a1b1c1d1e1f20);
    ipv6_hdr.destaddr.u64[0] = byteorder_htonll(0xffffee40f6d1f993);
    ipv6_hdr.destaddr.u64[1] = byteorder_htonll(0xe19465b5ddccbbaa);

    sixlowpan_iphc_cbuf_update(0, (ipv6_addr_t *)(&(ipv6_hdr.destaddr.u16[2])), 64, 120);

    if (sixlowpan_hc_set_status(sixlowpan_test_sixlowpan_pid,
                                SIXLOWPAN_HC_ENABLE) < 0) {
        return SIXLOWPAN_TEST_SET_HC_STATUS_ERROR;
    }

    res = sixlowpan_test_send_test(&snd, 1, &exp, 1);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

#endif

int main(void)
{
    sixlowpan_test_init();

    SIXLOWPAN_TEST_RUN(EINVAL,          test_nalp_received);
    SIXLOWPAN_TEST_RUN(EAFNOSUPPORT,    test_unsupported_address_family_uncompressed_received);
    SIXLOWPAN_TEST_RUN(0,               test_uncompressed_unfragmented_received);
    SIXLOWPAN_TEST_RUN(EAFNOSUPPORT,    test_unsupported_address_family_fragment_received);
    SIXLOWPAN_TEST_RUN(0,               test_first_fragment_received);
    SIXLOWPAN_TEST_RUN(EMSGSIZE,        test_more_fragments_first_not_8_byte_grouped_received);
    SIXLOWPAN_TEST_RUN(0,               test_2_fragments_received_ordered);
    SIXLOWPAN_TEST_RUN(0,               test_2_fragments_received_unordered);
    SIXLOWPAN_TEST_RUN(0,               test_2_fragments_received_ordered_last_not_full);
    SIXLOWPAN_TEST_RUN(EMSGSIZE,        test_more_fragments_nth_not_8_byte_grouped_received);
    SIXLOWPAN_TEST_RUN(0,               test_3_fragments_received_ordered);
    SIXLOWPAN_TEST_RUN(0,               test_3_fragments_received_last_2_unordered);
    SIXLOWPAN_TEST_RUN(0,               test_4_fragments_received_2_datagrams_unordered);
    SIXLOWPAN_TEST_RUN(EAFNOSUPPORT,    test_unsupported_address_family_send);
    SIXLOWPAN_TEST_RUN(0,               test_uncompressed_unfragmented_send);
    SIXLOWPAN_TEST_RUN(0,               test_2_fragments_send);
    SIXLOWPAN_TEST_RUN(0,               test_3_fragments_send);
    SIXLOWPAN_TEST_RUN(0,               test_big_header_send);
    SIXLOWPAN_TEST_RUN(0,               test_2_big_headers_send);
#ifdef MODULE_SIXLOWPAN_HC
    SIXLOWPAN_TEST_RUN(EMSGSIZE,        test_compressed_too_short_dispatch);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_all_zero_received);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_dscp_elided_received);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_fl_elided_received);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_tc_fl_elided_received);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_tf_elided_hl_1_received);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_tf_elided_hl_64_received);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_tf_elided_hl_255_received);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_iphc1_set_cid_ext_received);
    SIXLOWPAN_TEST_RUN(EADDRNOTAVAIL,   test_compressed_iphc1_set_sam_sac_set_received_no_ctx);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_iphc1_set_sam_sac_1_received);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_iphc1_set_sam_sac_2_received);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_iphc1_set_sam_sac_3_received);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_iphc1_set_sam_sac_4_received);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_iphc1_set_sam_sac_5_received);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_iphc1_set_sam_sac_6_received);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_iphc1_set_sam_sac_7_received);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_iphc1_set_sam_sac_7_cid_ext_received);
    SIXLOWPAN_TEST_RUN(EADDRNOTAVAIL,   test_compressed_iphc1_set_dam_dac_set_received_no_ctx);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_iphc1_set_m_dam_dac_1_received);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_iphc1_set_m_dam_dac_2_received);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_iphc1_set_m_dam_dac_3_received);
    SIXLOWPAN_TEST_RUN(EDESTADDRREQ,    test_compressed_iphc1_set_m_dam_dac_4_received);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_iphc1_set_m_dam_dac_5_received);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_iphc1_set_m_dam_dac_6_received);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_iphc1_set_m_dam_dac_7_received);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_iphc1_set_m_dam_dac_7_cid_ext_received);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_iphc1_set_m_dam_dac_8_received);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_iphc1_set_m_dam_dac_9_received);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_iphc1_set_m_dam_dac_10_received);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_iphc1_set_m_dam_dac_11_received);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_iphc1_set_m_dam_dac_12_received);
    SIXLOWPAN_TEST_RUN(EDESTADDRREQ,    test_compressed_iphc1_set_m_dam_dac_13_received);
    SIXLOWPAN_TEST_RUN(EDESTADDRREQ,    test_compressed_iphc1_set_m_dam_dac_14_received);
    SIXLOWPAN_TEST_RUN(EDESTADDRREQ,    test_compressed_iphc1_set_m_dam_dac_15_received);
    SIXLOWPAN_TEST_RUN(0,               test_2_compressed_fragments_received);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_all_zero_send);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_dscp_elided_send);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_fl_elided_send);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_tc_fl_elided_send);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_tf_elided_hl_1_send);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_tf_elided_hl_64_send);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_tf_elided_hl_255_send);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_iphc1_set_sam_sac_1_send);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_iphc1_set_sam_sac_2_send);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_iphc1_set_sam_sac_3_send);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_iphc1_set_sam_sac_4_send);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_iphc1_set_sam_sac_5_send);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_iphc1_set_sam_sac_6_send);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_iphc1_set_sam_sac_7_send);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_iphc1_set_sam_sac_7_cid_ext_send);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_iphc1_set_m_dam_dac_1_send);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_iphc1_set_m_dam_dac_2_send);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_iphc1_set_m_dam_dac_3_send);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_iphc1_set_m_dam_dac_5_send);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_iphc1_set_m_dam_dac_6_send);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_iphc1_set_m_dam_dac_7_send);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_iphc1_set_m_dam_dac_8_send);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_iphc1_set_m_dam_dac_9_send);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_iphc1_set_m_dam_dac_10_send);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_iphc1_set_m_dam_dac_11_send);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_iphc1_set_m_dam_dac_12_send);
    SIXLOWPAN_TEST_RUN(0,               test_2_compressed_fragments_send);
    SIXLOWPAN_TEST_RUN(0,               test_compressed_with_other_header);
#endif

    printf("\n\nAll tests successful.\n");

    return 0;
}

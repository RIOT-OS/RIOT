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

    res = sixlowpan_test_send_test(&snd, 1, exp, 2);

    if (!pktbuf_is_empty()) {
        return SIXLOWPAN_TEST_EPKTBUF_LEAK;
    }

    return res;
}

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

    printf("\n\nAll tests successful.\n");

    return 0;
}

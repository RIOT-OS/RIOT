/*
 * Copyright (C) 2014 Hochschule für Angewandte Wissenschaften Hamburg (HAW)
 * Copyright (C) 2015 Lotte Steenbrink <lotte.steenbrink@haw-hamburg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       constants and global variables for AODVv2 tests
 *
 * @author      Lotte Steenbrink <lotte.steenbrink@haw-hamburg.de>
 *
 * @}
 */

#ifndef AODV_TESTS_H_
#define AODV_TESTS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "udp.h"

#include "rfc5444/rfc5444_writer.h"

#define AODV_TEST_MSGBUF_MAX            (500)

typedef struct
{
    unsigned char buffer[AODV_TEST_MSGBUF_MAX];
    uint8_t length;
    struct netaddr* sender;
} aodvv2_test_msg;


extern radio_address_t aodvv2_iface_id;

extern aodvv2_test_msg aodv_test_plain_rreq;
extern aodvv2_test_msg aodv_test_more_recent_rreq;
extern aodvv2_test_msg aodv_test_plain_rrep;
extern aodvv2_test_msg aodv_test_more_recent_rrep;

/*
Messages will always be sent/stored along the following path:
OrigAddr -> sender_oa -> TESTNODE -> sender_ta -> TargAddr
OrigAddr <- sender_oa <- TESTNODE <- sender_ta <- TargAddr */
extern struct netaddr aodvv2_test_origaddr;
extern struct netaddr aodvv2_test_sender_oa;
extern struct netaddr aodvv2_test_sender_ta;
extern struct netaddr aodvv2_test_targaddr;

/* callback for the rfc5444 packet writer. can be set with aodv_packet_writer_init()
 * To enforce the dropping of all control packets (and reduce overhead) */
void aodv_test_drop_packet(struct rfc5444_writer *wr __attribute__ ((unused)),
                          struct rfc5444_writer_target *iface __attribute__((unused)),
                          void *buffer, size_t length);

#ifdef  __cplusplus
}
#endif

#endif /* AODV_FIB_TESTS_H_ */

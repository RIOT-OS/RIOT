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
 * @brief       tests for the AODVv2 writer
 *
 * @author      Lotte Steenbrink <lotte.steenbrink@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include "aodv_writer_tests.h"
#include "aodv_tests.h"

#include "aodv.h"
#include "aodvv2/aodvv2.h"

#include "common/autobuf.h"
#include "rfc5444/rfc5444_writer.h"
#include "rfc5444/rfc5444_print.h"
#include "writer.h"
#include "thread.h"

aodvv2_test_msg aodv_test_plain_rreq;
aodvv2_test_msg aodv_test_more_recent_rreq;
aodvv2_test_msg aodv_test_plain_rrep;
aodvv2_test_msg aodv_test_more_recent_rrep;

static aodvv2_test_msg* current_msg; /* Point to the buffer the current message should be written to.
                                    * Yes, this is awful, but I fear the oonf_apis
                                    * callback infrastructure leaves me no other choice...
                                    * Sorry. :( */

/* All the test data */
static timex_t now, validity_t;

static struct aodvv2_packet_data plain_rreq_msg, plain_rrep_msg;

static int aodv_test_writer_init_data(void)
{
    vtimer_now(&now);
    validity_t = timex_set(AODVV2_ACTIVE_INTERVAL + AODVV2_MAX_IDLETIME, 0);

    plain_rreq_msg = (struct aodvv2_packet_data) {
        .hoplimit = AODVV2_MAX_HOPCOUNT,
        .sender = aodvv2_test_sender_oa,
        .metricType = AODVV2_DEFAULT_METRIC_TYPE,
        .origNode = {
            .addr = aodvv2_test_origaddr,
            .metric = 2,
            .seqnum = 1,
        },
        .targNode = {
            .addr = aodvv2_test_targaddr,
            .metric = 12,
            .seqnum = 1,
        },
        .timestamp = now,
    };

    vtimer_now(&now);
    plain_rrep_msg = (struct aodvv2_packet_data) {
        .hoplimit = AODVV2_MAX_HOPCOUNT,
        .sender = aodvv2_test_sender_ta,
        .metricType = AODVV2_DEFAULT_METRIC_TYPE,
        .origNode = {
            .addr = aodvv2_test_origaddr,
            .metric = 4,
            .seqnum = 1,
        },
        .targNode = {
            .addr = aodvv2_test_targaddr,
            .metric = 2,
            .seqnum = 2,
        },
        .timestamp = now,
    };

    return 0;
}

static void aodv_test_write_packet(struct rfc5444_writer *wr __attribute__ ((unused)),
                          struct rfc5444_writer_target *iface __attribute__((unused)),
                          void *buffer, size_t length)
{
    printf("Writing message to buffer\n");
    /* make sure buffer is clear */
    memcpy(current_msg->buffer, buffer, length);
    current_msg->length = length;
    printf("Done.\n");
}

static void aodv_test_writer_write_new_rreq(void)
{
    current_msg = &aodv_test_plain_rreq;
    current_msg->sender = &aodvv2_test_sender_oa;
    aodv_send_rreq(&plain_rreq_msg);
}

static void aodv_test_writer_write_more_recent_rreq(void)
{
    plain_rreq_msg.origNode.seqnum += 1;
    current_msg = &aodv_test_more_recent_rreq;
    current_msg->sender = &aodvv2_test_sender_oa;
    aodv_send_rreq(&plain_rreq_msg);
}

static void aodv_test_writer_write_new_rrep(void)
{
    current_msg = &aodv_test_plain_rrep;
    current_msg->sender = &aodvv2_test_sender_ta;
    aodv_send_rrep(&plain_rrep_msg, &aodvv2_test_sender_ta);
}

static void aodv_test_writer_write_more_recent_rrep(void)
{
    plain_rrep_msg.targNode.seqnum += 1;
    current_msg = &aodv_test_more_recent_rrep;
    current_msg->sender = &aodvv2_test_sender_ta;
    aodv_send_rrep(&plain_rrep_msg, &aodvv2_test_sender_ta);
}

/* Store packets in buffers that we can use them for testing */
void write_packets_to_buf(void)
{
    printf("============= Preparing to write packets to buffers ==================\n");

    /* Make sure the threads are up and running */
    sleep(2);

    if (0 != aodv_test_writer_init_data()){
        printf ("FAILED: unable to init data!\n");
        return;
    }
    /* overwrite the aodvv2 packet writer */
    aodv_packet_writer_init(aodv_test_write_packet);

    aodv_test_writer_write_new_rreq();

    /* make sure sender_thread is done */
    sleep(2);
    aodv_test_writer_write_more_recent_rreq();

    /* make sure sender_thread is done */
    sleep(2);
    aodv_test_writer_write_new_rrep();

    /* make sure sender_thread is done */
    sleep(2);
    aodv_test_writer_write_more_recent_rrep();

    /* give current writer time to finish and init aodvv2 again cleanly
     * to undo the change to the aodv_packet_writer callback */
    sleep(2);
    aodv_init();
}

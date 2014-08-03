/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup olsr2
 * @{
 * @author  Benjamin Valentin <benpicco@zedat.fu-berlin.de>
 * @}
 */

#ifdef RIOT
#include "net_help.h"
#endif

#ifdef ENABLE_NAME
#include <string.h>
#endif

#include "common/avl.h"
#include "common/common_types.h"
#include "common/netaddr.h"
#include "rfc5444/rfc5444.h"
#include "rfc5444/rfc5444_iana.h"
#include "rfc5444/rfc5444_writer.h"

#include "constants.h"
#include "writer.h"
#include "nhdp.h"
#include "olsr.h"
#include "olsr_debug.h"
#include "routing.h"

uint8_t msg_buffer[256];
uint8_t msg_addrtlvs[512];
uint8_t packet_buffer[256];

uint16_t seq_no = 1;

static bool send_tc_messages;

static void _cb_add_nhdp_message_TLVs(struct rfc5444_writer *wr);
static void _cb_add_nhdp_addresses(struct rfc5444_writer *wr);

static void _cb_add_olsr_message_TLVs(struct rfc5444_writer *wr);
static void _cb_add_olsr_addresses(struct rfc5444_writer *wr);

/* HELLO message */
static struct rfc5444_writer_content_provider _nhdp_message_content_provider = {
    .msg_type = RFC5444_MSGTYPE_HELLO,
    .addMessageTLVs = _cb_add_nhdp_message_TLVs,
    .addAddresses = _cb_add_nhdp_addresses,
};

static struct rfc5444_writer_tlvtype _nhdp_addrtlvs[] = {
    [IDX_ADDRTLV_MPR] = { .type = RFC5444_ADDRTLV_MPR },
    [IDX_ADDRTLV_LINK_STATUS] = { .type = RFC5444_ADDRTLV_LINK_STATUS },
    [IDX_ADDRTLV_METRIC] = { .type = RFC5444_ADDRTLV_LINK_METRIC},
#ifdef ENABLE_NAME
    [IDX_ADDRTLV_NODE_NAME] = { .type = RFC5444_TLV_NODE_NAME },
#endif
};

/* TC message */
static struct rfc5444_writer_content_provider _olsr_message_content_provider = {
    .msg_type = RFC5444_MSGTYPE_TC,
    .addMessageTLVs = _cb_add_olsr_message_TLVs,
    .addAddresses = _cb_add_olsr_addresses,
};

static struct rfc5444_writer_tlvtype _olsr_addrtlvs[] = {
    [IDX_ADDRTLV_LINK_STATUS] = { .type = RFC5444_ADDRTLV_LINK_STATUS },
    [IDX_ADDRTLV_METRIC] = { .type = RFC5444_ADDRTLV_LINK_METRIC},
#ifdef ENABLE_NAME
    [IDX_ADDRTLV_NODE_NAME] = { .type = RFC5444_TLV_NODE_NAME },
#endif
};

/* add TLVs to HELLO message */
static void
_cb_add_nhdp_message_TLVs(struct rfc5444_writer *wr)
{
    uint8_t time_encoded = rfc5444_timetlv_encode(OLSR2_HELLO_REFRESH_INTERVAL_SECONDS);
    rfc5444_writer_add_messagetlv(wr, RFC5444_MSGTLV_VALIDITY_TIME, 0, &time_encoded, sizeof(time_encoded));

#ifdef ENABLE_NAME
    rfc5444_writer_add_messagetlv(wr, RFC5444_TLV_NODE_NAME, 0, local_name, strlen(local_name) + 1);
#endif
}

/* add addresses to HELLO message */
static void
_cb_add_nhdp_addresses(struct rfc5444_writer *wr)
{
    struct olsr_node *node, *safe;
    int value;
    uint8_t mpr;
    send_tc_messages = false;

    /* add all neighbors */
    avl_for_each_element_safe(get_olsr_head(), node, node, safe) {

        /* if the node was just removed entirely from the database, continue */
        if (remove_expired(node)) {
            continue;
        }

        if (node->type != NODE_TYPE_NHDP && !node->lost) {
            continue;
        }

        if (node->pending && !node->lost) {
            continue;
        }

        if (!node->pending && h1_deriv(node)->mpr_slctr_route) {
            send_tc_messages = true;
        }

        struct rfc5444_writer_address *address = rfc5444_writer_add_address(wr,
                _nhdp_message_content_provider.creator, node->addr, false);

        mpr = 0;

        if (h1_deriv(node)->mpr_neigh_flood > 0) {
            mpr |= RFC5444_MPR_FLOODING;
        }

        if (h1_deriv(node)->mpr_neigh_route > 0) {
            mpr |= RFC5444_MPR_ROUTING;
        }

        if (mpr > 0)
            rfc5444_writer_add_addrtlv(wr, address, &_nhdp_addrtlvs[IDX_ADDRTLV_MPR],
                                       &mpr, sizeof mpr, false);

        metric_t link_metric = get_link_metric(node, get_local_addr());

        if (link_metric > RFC5444_METRIC_MIN) {
            uint16_t mtrc = rfc5444_metric_encode(link_metric);
            rfc5444_writer_add_addrtlv(wr, address, &_nhdp_addrtlvs[IDX_ADDRTLV_METRIC],
                                       &mtrc, sizeof mtrc, false);
        }

        if (node->lost) {
            DEBUG("LINKSTATUS: neighbor %s lost (HELLO) [%d]", node->name, node->lost);
            value = RFC5444_LINKSTATUS_LOST;
            rfc5444_writer_add_addrtlv(wr, address, &_nhdp_addrtlvs[IDX_ADDRTLV_LINK_STATUS],
                                       &value, sizeof value, false);

            if (!send_tc_messages) {
                node->lost--;
            }
        }

#ifdef ENABLE_NAME

        if (node->name)
            rfc5444_writer_add_addrtlv(wr, address, &_nhdp_addrtlvs[IDX_ADDRTLV_NODE_NAME],
                                       node->name, strlen(node->name) + 1, false);

#endif
    }
}

/* add TLVs to TC message */
static void
_cb_add_olsr_message_TLVs(struct rfc5444_writer *wr)
{
    uint8_t time_encoded = rfc5444_timetlv_encode(OLSR2_TC_REFRESH_INTERVAL_SECONDS);
    rfc5444_writer_add_messagetlv(wr, RFC5444_MSGTLV_VALIDITY_TIME, 0, &time_encoded, sizeof(time_encoded));

#ifdef ENABLE_NAME
    rfc5444_writer_add_messagetlv(wr, RFC5444_TLV_NODE_NAME, 0, local_name, strlen(local_name) + 1);
#endif
}

/* add addresses to TC message */
static void
_cb_add_olsr_addresses(struct rfc5444_writer *wr)
{
    struct olsr_node *node;
    int value;

    /* add all neighbors */
    avl_for_each_element(get_olsr_head(), node, node) {
        if (h1_deriv(node) == NULL) {
            continue;
        }

        if (!h1_deriv(node)->mpr_slctr_route) {
            continue;
        }

        /* don't advertise neighbors routed over another hop */
        if (node->distance != 1 && !node->lost) {
            continue;
        }

        if (node->pending && !node->lost) {
            continue;
        }

        struct rfc5444_writer_address *address __attribute__((unused));

        address = rfc5444_writer_add_address(wr, _olsr_message_content_provider.creator, node->addr, false);

        if (node->link_metric > RFC5444_METRIC_MIN) {
            uint16_t mtrc = rfc5444_metric_encode(node->link_metric);
            rfc5444_writer_add_addrtlv(wr, address, &_nhdp_addrtlvs[IDX_ADDRTLV_METRIC],
                                       &mtrc, sizeof mtrc, false);
        }

        if (node->lost) {
            DEBUG("LINKSTATUS: neighbor %s lost (TC) [%d]", node->name, node->lost);
            value = RFC5444_LINKSTATUS_LOST;
            rfc5444_writer_add_addrtlv(wr, address, &_olsr_addrtlvs[IDX_ADDRTLV_LINK_STATUS],
                                       &value, sizeof value, false);

            node->lost--;
        }

#ifdef ENABLE_NAME

        if (node->name)
            rfc5444_writer_add_addrtlv(wr, address, &_olsr_addrtlvs[IDX_ADDRTLV_NODE_NAME],
                                       node->name, strlen(node->name) + 1, false);

#endif
    }
}

/* header for HELLO messages */
static void
_cb_add_hello_message_header(struct rfc5444_writer *wr, struct rfc5444_writer_message *message)
{
    /* no originator, no hopcount, no hoplimit, no sequence number */
    rfc5444_writer_set_msg_header(wr, message, false, false, false, false);
}

/* header for TC messages */
static void
_cb_add_tc_message_header(struct rfc5444_writer *wr, struct rfc5444_writer_message *message)
{
    /* originator, hopcount, hoplimit, sequence number */
    rfc5444_writer_set_msg_header(wr, message, true, true, true, true);
    rfc5444_writer_set_msg_seqno(wr, message, seq_no++);
    rfc5444_writer_set_msg_originator(wr, message, netaddr_get_binptr(get_local_addr()));

    message->hoplimit = OLSR2_TC_HOP_LIMIT;
}

/* reader has already decided whether to forward or not, just say ok to that */
bool
olsr_message_forwarding_selector(struct rfc5444_writer_target *rfc5444_target __attribute__((unused)))
{
    return true;
}

/**
 * Initialize RFC5444 writer
 * @param ptr pointer to "send_packet" function
 */
void
writer_init(write_packet_func_ptr ptr)
{
    struct rfc5444_writer_message *_hello_msg;
    struct rfc5444_writer_message *_tc_msg;

    writer.msg_buffer = msg_buffer;
    writer.msg_size = sizeof(msg_buffer);
    writer.addrtlv_buffer = msg_addrtlvs;
    writer.addrtlv_size	 = sizeof(msg_addrtlvs);

    interface.packet_buffer = packet_buffer;
    interface.packet_size	= sizeof(packet_buffer);
    interface.sendPacket	= ptr;

    /* initialize writer */
    rfc5444_writer_init(&writer);

    /* register a target (for sending messages to) in writer */
    rfc5444_writer_register_target(&writer, &interface);

    /* register a message content provider */
    rfc5444_writer_register_msgcontentprovider(&writer, &_nhdp_message_content_provider, _nhdp_addrtlvs, ARRAYSIZE(_nhdp_addrtlvs));
    rfc5444_writer_register_msgcontentprovider(&writer, &_olsr_message_content_provider, _olsr_addrtlvs, ARRAYSIZE(_olsr_addrtlvs));

    /* register message type 1 with 16 byte addresses */
    _hello_msg = rfc5444_writer_register_message(&writer, RFC5444_MSGTYPE_HELLO, false, RFC5444_MAX_ADDRLEN);
    _tc_msg = rfc5444_writer_register_message(&writer, RFC5444_MSGTYPE_TC, false, RFC5444_MAX_ADDRLEN);

    _hello_msg->addMessageHeader = _cb_add_hello_message_header;
    _tc_msg->addMessageHeader = _cb_add_tc_message_header;
    _tc_msg->forward_target_selector = olsr_message_forwarding_selector;
}

void writer_send_hello(void)
{
    DEBUG("[HELLO]");

    /* send message */
    rfc5444_writer_create_message_alltarget(&writer, RFC5444_MSGTYPE_HELLO);
    rfc5444_writer_flush(&writer, &interface, false);
}

void writer_send_tc(void)
{
    if (!send_tc_messages) {
        return;
    }

    DEBUG("[TC]");

    /* send message */
    rfc5444_writer_create_message_alltarget(&writer, RFC5444_MSGTYPE_TC);
    rfc5444_writer_flush(&writer, &interface, false);
}

/**
 * Cleanup RFC5444 writer
 */
void
writer_cleanup(void)
{
    rfc5444_writer_cleanup(&writer);
}

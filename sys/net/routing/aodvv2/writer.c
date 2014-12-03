/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2014 Lotte Steenbrink <lotte.steenbrink@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     aodvv2
 * @{
 *
 * @file        writer.c
 * @brief       writer to create RFC5444 aodvv2 messages
 *
 * @author      Lotte Steenbrink <lotte.steenbrink@fu-berlin.de>
 */

#ifdef RIOT
#include "net_help.h"
#endif

#include "writer.h"
#include "debug.h"

#include "aodv_debug.h"

#define ENABLE_DEBUG (0)

static void _cb_addMessageHeader(struct rfc5444_writer *wr,
                                 struct rfc5444_writer_message *message);

static void _cb_rreq_addAddresses(struct rfc5444_writer *wr);
static void _cb_rrep_addAddresses(struct rfc5444_writer *wr);
static void _cb_rerr_addAddresses(struct rfc5444_writer *wr);

static mutex_t writer_mutex;

struct rfc5444_writer writer;
static struct writer_target _target;

static struct unreachable_node *_unreachable_nodes;
static size_t _num_unreachable_nodes;

static uint8_t _msg_buffer[128];
static uint8_t _msg_addrtlvs[1000];
static uint8_t _packet_buffer[128];

static struct rfc5444_writer_message *_rreq_msg;
static struct rfc5444_writer_message *_rrep_msg;
static struct rfc5444_writer_message *_rerr_msg;

/*
 * message content provider that will add message TLVs,
 * addresses and address block TLVs to all messages of type RREQ.
 */
static struct rfc5444_writer_content_provider _rreq_message_content_provider =
{
    .msg_type = RFC5444_MSGTYPE_RREQ,
    .addAddresses = _cb_rreq_addAddresses,
};

/* declaration of all address TLVs added to the RREQ message */
static struct rfc5444_writer_tlvtype _rreq_addrtlvs[] =
{
    [RFC5444_MSGTLV_ORIGSEQNUM] = { .type = RFC5444_MSGTLV_ORIGSEQNUM },
    [RFC5444_MSGTLV_METRIC] = {
        .type = RFC5444_MSGTLV_METRIC,
        .exttype = AODVV2_DEFAULT_METRIC_TYPE
    },
};

/*
 * message content provider that will add message TLVs,
 * addresses and address block TLVs to all messages of type RREQ.
 */
static struct rfc5444_writer_content_provider _rrep_message_content_provider =
{
    .msg_type = RFC5444_MSGTYPE_RREP,
    .addAddresses = _cb_rrep_addAddresses,
};

/* declaration of all address TLVs added to the RREP message */
static struct rfc5444_writer_tlvtype _rrep_addrtlvs[] =
{
    [RFC5444_MSGTLV_ORIGSEQNUM] = { .type = RFC5444_MSGTLV_ORIGSEQNUM},
    [RFC5444_MSGTLV_TARGSEQNUM] = { .type = RFC5444_MSGTLV_TARGSEQNUM},
    [RFC5444_MSGTLV_METRIC] = {
        .type = RFC5444_MSGTLV_METRIC,
        .exttype = AODVV2_DEFAULT_METRIC_TYPE
    },
};

/*
 * message content provider that will add message TLVs,
 * addresses and address block TLVs to all messages of type RREQ.
 */
static struct rfc5444_writer_content_provider _rerr_message_content_provider =
{
    .msg_type = RFC5444_MSGTYPE_RERR,
    .addAddresses = _cb_rerr_addAddresses,
};

/* declaration of all address TLVs added to the RREP message */
static struct rfc5444_writer_tlvtype _rerr_addrtlvs[] =
{
    [RFC5444_MSGTLV_UNREACHABLE_NODE_SEQNUM] = { .type = RFC5444_MSGTLV_UNREACHABLE_NODE_SEQNUM},
};

/**
 * Callback to define the message header for a RFC5444 RREQ message
 * @param message
 */
static void
_cb_addMessageHeader(struct rfc5444_writer *wr, struct rfc5444_writer_message *message)
{
    AODV_DEBUG("%s()\n", __func__);

    /* no originator, no hopcount, has hoplimit, no seqno */
    rfc5444_writer_set_msg_header(wr, message, false, false, true, false);
    rfc5444_writer_set_msg_hoplimit(wr, message, _target.packet_data.hoplimit);
}

/**
 * Callback to add addresses and address TLVs to a RFC5444 RREQ message
 */
static void
_cb_rreq_addAddresses(struct rfc5444_writer *wr)
{
    AODV_DEBUG("%s()\n", __func__);

    struct rfc5444_writer_address *origNode_addr;

    /* add origNode address (has no address tlv); is mandatory address */
    origNode_addr = rfc5444_writer_add_address(wr, _rreq_message_content_provider.creator,
                    &_target.packet_data.origNode.addr, true);

    /* add targNode address (has no address tlv); is mandatory address */
    rfc5444_writer_add_address(wr, _rreq_message_content_provider.creator,
                               &_target.packet_data.targNode.addr, true);

    /* add SeqNum TLV and metric TLV to origNode */
    /* TODO: allow_dup true or false? */
    rfc5444_writer_add_addrtlv(wr, origNode_addr, &_rreq_addrtlvs[RFC5444_MSGTLV_ORIGSEQNUM],
                               &_target.packet_data.origNode.seqnum,
                               sizeof(_target.packet_data.origNode.seqnum), false);
    /* cppcheck: suppress false positive on non-trivially initialized arrays.
     *           this is a known bug: http://trac.cppcheck.net/ticket/5497 */
    /* cppcheck-suppress arrayIndexOutOfBounds */
    rfc5444_writer_add_addrtlv(wr, origNode_addr, &_rreq_addrtlvs[RFC5444_MSGTLV_METRIC],
                               &_target.packet_data.origNode.metric,
                               sizeof(_target.packet_data.origNode.metric), false);
}

/**
 * Callback to add addresses and address TLVs to a RFC5444 RREQ message
 */
static void
_cb_rrep_addAddresses(struct rfc5444_writer *wr)
{
    AODV_DEBUG("%s()\n", __func__);

    struct rfc5444_writer_address *origNode_addr, *targNode_addr;

    uint16_t origNode_seqnum = _target.packet_data.origNode.seqnum;

    uint16_t targNode_seqnum = seqnum_get();
    seqnum_inc();

    uint8_t targNode_hopCt = _target.packet_data.targNode.metric;

    /* add origNode address (has no address tlv); is mandatory address */
    origNode_addr = rfc5444_writer_add_address(wr, _rrep_message_content_provider.creator,
                                               &_target.packet_data.origNode.addr, true);

    /* add targNode address (has no address tlv); is mandatory address */
    targNode_addr = rfc5444_writer_add_address(wr, _rrep_message_content_provider.creator,
                                               &_target.packet_data.targNode.addr, true);

    /* add OrigNode and TargNode SeqNum TLVs */
    /* TODO: allow_dup true or false? */
    rfc5444_writer_add_addrtlv(wr, origNode_addr, &_rrep_addrtlvs[RFC5444_MSGTLV_ORIGSEQNUM],
                               &origNode_seqnum, sizeof(origNode_seqnum), false);
    rfc5444_writer_add_addrtlv(wr, targNode_addr, &_rrep_addrtlvs[RFC5444_MSGTLV_TARGSEQNUM],
                               &targNode_seqnum, sizeof(targNode_seqnum), false);

    /* Add Metric TLV to targNode Address */
    rfc5444_writer_add_addrtlv(wr, targNode_addr, &_rrep_addrtlvs[RFC5444_MSGTLV_METRIC],
                               &targNode_hopCt, sizeof(targNode_hopCt), false);
}

/**
 * Callback to add addresses and address TLVs to a RFC5444 RERR message
 */
static void
_cb_rerr_addAddresses(struct rfc5444_writer *wr)
{
    AODV_DEBUG("%s()\n", __func__);

    for (unsigned i = 0; i < _num_unreachable_nodes; i++) {
        /* add unreachableNode addresses (has no address tlv); is mandatory address */
        struct rfc5444_writer_address *unreachableNode_addr = rfc5444_writer_add_address(
                                    wr, _rerr_message_content_provider.creator,
                                    &_unreachable_nodes[i].addr, true);

        /* add SeqNum TLV to unreachableNode */
        /* TODO: allow_dup true or false? */
        /* cppcheck: suppress false positive on non-trivially initialized arrays.
         *           this is a known bug: http://trac.cppcheck.net/ticket/5497 */
        /* cppcheck-suppress arrayIndexOutOfBounds */
        rfc5444_writer_add_addrtlv(wr, unreachableNode_addr,
                                   &_rerr_addrtlvs[RFC5444_MSGTLV_UNREACHABLE_NODE_SEQNUM],
                                   &_unreachable_nodes[i].seqnum,
                                   sizeof(_unreachable_nodes[i].seqnum), false);
    }
}

void aodv_packet_writer_init(write_packet_func_ptr ptr)
{
    AODV_DEBUG("%s()\n", __func__);

    mutex_init(&writer_mutex);

    /* define interface for generating rfc5444 packets */
    _target.interface.packet_buffer = _packet_buffer;
    _target.interface.packet_size = sizeof(_packet_buffer);

    /* set function to send binary packet content */
    _target.interface.sendPacket = ptr;

    /* define the rfc5444 writer */
    writer.msg_buffer = _msg_buffer;
    writer.msg_size = sizeof(_msg_buffer);
    writer.addrtlv_buffer = _msg_addrtlvs;
    writer.addrtlv_size = sizeof(_msg_addrtlvs);

    /* initialize writer */
    rfc5444_writer_init(&writer);

    /* register a target (for sending messages to) in writer */
    rfc5444_writer_register_target(&writer, &_target.interface);

    /* register a message content providers for RREQ and RREP */
    rfc5444_writer_register_msgcontentprovider(&writer, &_rreq_message_content_provider,
                                               _rreq_addrtlvs, ARRAYSIZE(_rreq_addrtlvs));
    rfc5444_writer_register_msgcontentprovider(&writer, &_rrep_message_content_provider,
                                               _rrep_addrtlvs, ARRAYSIZE(_rrep_addrtlvs));
    rfc5444_writer_register_msgcontentprovider(&writer, &_rerr_message_content_provider,
                                               _rerr_addrtlvs, ARRAYSIZE(_rerr_addrtlvs));

    /* register rreq and rrep messages with 16 byte (ipv6) addresses.
     * AddPacketHeader & addMessageHeader callbacks are triggered here. */
    _rreq_msg = rfc5444_writer_register_message(&writer, RFC5444_MSGTYPE_RREQ,
                                                false, RFC5444_MAX_ADDRLEN);
    _rrep_msg = rfc5444_writer_register_message(&writer, RFC5444_MSGTYPE_RREP,
                                                false, RFC5444_MAX_ADDRLEN);
    _rerr_msg = rfc5444_writer_register_message(&writer, RFC5444_MSGTYPE_RERR,
                                                false, RFC5444_MAX_ADDRLEN);

    _rreq_msg->addMessageHeader = _cb_addMessageHeader;
    _rrep_msg->addMessageHeader = _cb_addMessageHeader;
    _rerr_msg->addMessageHeader = _cb_addMessageHeader;
}

/**
 * Send a RREQ. DO NOT use this function to dispatch packets from anything else
 * than the sender_thread. To send RREQs, use aodv_send_rreq().
 * @param packet_data parameters of the RREQ
 * @param next_hop Address the RREP is sent to
 */
void aodv_packet_writer_send_rreq(struct aodvv2_packet_data *packet_data, struct netaddr *next_hop)
{
    AODV_DEBUG("%s()\n", __func__);

    if ((packet_data == NULL) || (next_hop == NULL)) {
        return;
    }

    /* Make sure no other thread is using the writer right now */
    mutex_lock(&writer_mutex);
    memcpy(&_target.packet_data, packet_data, sizeof(struct aodvv2_packet_data));
    _target.type = RFC5444_MSGTYPE_RREQ;
    _target.packet_data.hoplimit = packet_data->hoplimit;

    /* set address to which the write_packet callback should send our RREQ */
    memcpy(&_target.target_addr, next_hop, sizeof (struct netaddr));

    rfc5444_writer_create_message_alltarget(&writer, RFC5444_MSGTYPE_RREQ);
    rfc5444_writer_flush(&writer, &_target.interface, false);
    mutex_unlock(&writer_mutex);
}


/**
 * Send a RREP. DO NOT use this function to dispatch packets from anything else
 * than the sender_thread. To send RREPs, use aodv_send_rrep().
 * @param packet_data parameters of the RREP
 * @param next_hop Address the RREP is sent to
 */
void aodv_packet_writer_send_rrep(struct aodvv2_packet_data *packet_data, struct netaddr *next_hop)
{
    AODV_DEBUG("%s()\n", __func__);

    if ((packet_data == NULL) || (next_hop == NULL)) {
        return;
    }

    /* Make sure no other thread is using the writer right now */
    mutex_lock(&writer_mutex);
    memcpy(&_target.packet_data, packet_data, sizeof(struct aodvv2_packet_data));
    _target.type = RFC5444_MSGTYPE_RREP;
    _target.packet_data.hoplimit = AODVV2_MAX_HOPCOUNT;

    /* set address to which the write_packet callback should send our RREQ */
    memcpy(&_target.target_addr, next_hop, sizeof (struct netaddr));

    rfc5444_writer_create_message_alltarget(&writer, RFC5444_MSGTYPE_RREP);
    rfc5444_writer_flush(&writer, &_target.interface, false);
    mutex_unlock(&writer_mutex);
}

/**
 * Send a RERR. DO NOT use this function to dispatch packets from anything else
 * than the sender_thread. To send RERRs, use aodv_send_rerr().
 * @param unreachable_nodes[] array containing all newly unreachable nodes. each
 *                            in a struct unreachable_node
 * @param len                 length of unreachable_nodes[]
 * @param hoplimit            the message's hop limit
 * @param next_hop            Address the RREP is sent to
 */
void aodv_packet_writer_send_rerr(struct unreachable_node unreachable_nodes[], size_t len,
                      int hoplimit, struct netaddr *next_hop)
{
    AODV_DEBUG("%s()\n", __func__);

    if ((unreachable_nodes == NULL) || (next_hop == NULL)) {
        return;
    }

    mutex_lock(&writer_mutex);
    _target.packet_data.hoplimit = hoplimit;
    _target.type = RFC5444_MSGTYPE_RERR;
    _unreachable_nodes = unreachable_nodes;
    _num_unreachable_nodes = len;

    /* set address to which the write_packet callback should send our RREQ */
    memcpy(&_target.target_addr, next_hop, sizeof (struct netaddr));

    rfc5444_writer_create_message_alltarget(&writer, RFC5444_MSGTYPE_RERR);
    rfc5444_writer_flush(&writer, &_target.interface, false);
    mutex_unlock(&writer_mutex);
}

void aodv_packet_writer_cleanup(void)
{
    AODV_DEBUG("%s()\n", __func__);
    rfc5444_writer_cleanup(&writer);
}

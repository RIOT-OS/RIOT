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
 * @file
 * @brief       reading and handling of RFC5444 aodvv2 messages
 *
 * @author      Lotte Steenbrink <lotte.steenbrink@fu-berlin.de>
 */

#ifdef RIOT
#include "net_help.h"
#endif

#include "reader.h"
#include "aodv_debug.h"
#include "ng_fib.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#if ENABLE_DEBUG
/* For PRIu16 etc. */
#include <inttypes.h>
#endif

static enum rfc5444_result _cb_rreq_blocktlv_addresstlvs_okay(
    struct rfc5444_reader_tlvblock_context *cont);
static enum rfc5444_result _cb_rreq_blocktlv_messagetlvs_okay(
    struct rfc5444_reader_tlvblock_context *cont);
static enum rfc5444_result _cb_rreq_end_callback(
    struct rfc5444_reader_tlvblock_context *cont, bool dropped);

static enum rfc5444_result _cb_rrep_blocktlv_addresstlvs_okay(
    struct rfc5444_reader_tlvblock_context *cont);
static enum rfc5444_result _cb_rrep_blocktlv_messagetlvs_okay(
    struct rfc5444_reader_tlvblock_context *cont);
static enum rfc5444_result _cb_rrep_end_callback(
    struct rfc5444_reader_tlvblock_context *cont, bool dropped);

static enum rfc5444_result _cb_rerr_blocktlv_addresstlvs_okay(
    struct rfc5444_reader_tlvblock_context *cont);
static enum rfc5444_result _cb_rerr_blocktlv_messagetlvs_okay(
    struct rfc5444_reader_tlvblock_context *cont);
static enum rfc5444_result _cb_rerr_end_callback(
    struct rfc5444_reader_tlvblock_context *cont, bool dropped);

/* helper functions */
static void print_json_received_rreq(void);
static void print_json_received_rrep(void);
static uint8_t _get_link_cost(aodvv2_metric_t metricType);
static uint8_t _get_max_metric(aodvv2_metric_t metricType);
static uint8_t _get_route_cost(aodvv2_metric_t metricType, uint8_t metric);

/* This is where we store data gathered from packets */
static struct aodvv2_packet_data packet_data;
static struct unreachable_node unreachable_nodes[AODVV2_MAX_UNREACHABLE_NODES];
static int num_unreachable_nodes;
static int aodvv2_validity_t = (AODVV2_ACTIVE_INTERVAL + AODVV2_MAX_IDLETIME) * 1000; /* milliseconds */

static struct rfc5444_reader reader;
#if AODV_DEBUG
static struct netaddr_str nbuf;
#endif

/*
 * Message consumer, will be called once for every message of
 * type RFC5444_MSGTYPE_RREQ that contains all the mandatory message TLVs
 */
static struct rfc5444_reader_tlvblock_consumer _rreq_consumer =
{
    .msg_id = RFC5444_MSGTYPE_RREQ,
    .block_callback = _cb_rreq_blocktlv_messagetlvs_okay,
    .end_callback = _cb_rreq_end_callback,
};

/*
 * Address consumer. Will be called once for every address in a message of
 * type RFC5444_MSGTYPE_RREQ.
 */
static struct rfc5444_reader_tlvblock_consumer _rreq_address_consumer =
{
    .msg_id = RFC5444_MSGTYPE_RREQ,
    .addrblock_consumer = true,
    .block_callback = _cb_rreq_blocktlv_addresstlvs_okay,
};

/*
 * Message consumer, will be called once for every message of
 * type RFC5444_MSGTYPE_RREP that contains all the mandatory message TLVs
 */
static struct rfc5444_reader_tlvblock_consumer _rrep_consumer =
{
    .msg_id = RFC5444_MSGTYPE_RREP,
    .block_callback = _cb_rrep_blocktlv_messagetlvs_okay,
    .end_callback = _cb_rrep_end_callback,
};

/*
 * Address consumer. Will be called once for every address in a message of
 * type RFC5444_MSGTYPE_RREP.
 */
static struct rfc5444_reader_tlvblock_consumer _rrep_address_consumer =
{
    .msg_id = RFC5444_MSGTYPE_RREP,
    .addrblock_consumer = true,
    .block_callback = _cb_rrep_blocktlv_addresstlvs_okay,
};

/*
 * Message consumer, will be called once for every message of
 * type RFC5444_MSGTYPE_RERR that contains all the mandatory message TLVs
 */
static struct rfc5444_reader_tlvblock_consumer _rerr_consumer =
{
    .msg_id = RFC5444_MSGTYPE_RERR,
    .block_callback = _cb_rerr_blocktlv_messagetlvs_okay,
    .end_callback = _cb_rerr_end_callback,
};

/*
 * Address consumer. Will be called once for every address in a message of
 * type RFC5444_MSGTYPE_RERR.
 */
static struct rfc5444_reader_tlvblock_consumer _rerr_address_consumer =
{
    .msg_id = RFC5444_MSGTYPE_RERR,
    .addrblock_consumer = true,
    .block_callback = _cb_rerr_blocktlv_addresstlvs_okay,
};


/*
 * Address consumer entries definition
 * TLV types RFC5444_MSGTLV__SEQNUM and RFC5444_MSGTLV_METRIC
 */
static struct rfc5444_reader_tlvblock_consumer_entry _rreq_rrep_address_consumer_entries[] =
{
    [RFC5444_MSGTLV_ORIGSEQNUM] = { .type = RFC5444_MSGTLV_ORIGSEQNUM},
    [RFC5444_MSGTLV_TARGSEQNUM] = { .type = RFC5444_MSGTLV_TARGSEQNUM},
    [RFC5444_MSGTLV_METRIC] = { .type = RFC5444_MSGTLV_METRIC }
};

/*
 * Address consumer entries definition
 * TLV types RFC5444_MSGTLV__SEQNUM and RFC5444_MSGTLV_METRIC
 */
static struct rfc5444_reader_tlvblock_consumer_entry _rerr_address_consumer_entries[] =
{
    [RFC5444_MSGTLV_UNREACHABLE_NODE_SEQNUM] = {
        .type = RFC5444_MSGTLV_UNREACHABLE_NODE_SEQNUM
    },
};

/**
 * This block callback is called for every address
 *
 * @param cont
 * @return
 */
static enum rfc5444_result _cb_rreq_blocktlv_messagetlvs_okay(struct rfc5444_reader_tlvblock_context *cont)
{
    if (!cont->has_hoplimit) {
        AODV_DEBUG("\tERROR: missing hop limit\n");
        return RFC5444_DROP_PACKET;
    }

    packet_data.hoplimit = cont->hoplimit;
    if (packet_data.hoplimit == 0) {
        AODV_DEBUG("\tERROR: Hoplimit is 0.\n");
        return RFC5444_DROP_PACKET;
    }
    packet_data.hoplimit--;

    return RFC5444_OKAY;
}

/**
 * This block callback is called for every address of a RREQ Message.
 *
 * @param cont
 * @return
 */
static enum rfc5444_result _cb_rreq_blocktlv_addresstlvs_okay(struct rfc5444_reader_tlvblock_context *cont)
{
#if AODV_DEBUG
    struct netaddr_str nbuf;
#endif
    struct rfc5444_reader_tlvblock_entry *tlv;
    bool is_origNode_addr = false;
    bool is_targNode_addr = false;

    /* handle OrigNode SeqNum TLV */
    tlv = _rreq_rrep_address_consumer_entries[RFC5444_MSGTLV_ORIGSEQNUM].tlv;
    if (tlv) {
        is_origNode_addr = true;
        packet_data.origNode.addr = cont->addr;
        packet_data.origNode.seqnum = *tlv->single_value;
    }

    /* handle TargNode SeqNum TLV */
    tlv = _rreq_rrep_address_consumer_entries[RFC5444_MSGTLV_TARGSEQNUM].tlv;
    if (tlv) {
        is_targNode_addr = true;
        packet_data.targNode.addr = cont->addr;
        packet_data.targNode.seqnum = *tlv->single_value;
    }
    if (!tlv && !is_origNode_addr) {
        /* assume that tlv missing => targNode Address */
        is_targNode_addr = true;
        packet_data.targNode.addr = cont->addr;
    }
    if (!is_origNode_addr && !is_targNode_addr) {
        AODV_DEBUG("\tERROR: mandatory RFC5444_MSGTLV_ORIGSEQNUM TLV missing.\n");
        return RFC5444_DROP_PACKET;
    }

    /* handle Metric TLV */
    /* cppcheck: suppress false positive on non-trivially initialized arrays.
     *           this is a known bug: http://trac.cppcheck.net/ticket/5497 */
    /* cppcheck-suppress arrayIndexOutOfBounds */
    tlv = _rreq_rrep_address_consumer_entries[RFC5444_MSGTLV_METRIC].tlv;
    if (!tlv && is_origNode_addr) {
        AODV_DEBUG("\tERROR: Missing or unknown metric TLV.\n");
        return RFC5444_DROP_PACKET;
    }
    if (tlv) {
        if (!is_origNode_addr) {
            AODV_DEBUG("\tERROR: Metric TLV belongs to wrong address.\n");
            return RFC5444_DROP_PACKET;
        }
        packet_data.metricType = tlv->type_ext;
        packet_data.origNode.metric = *tlv->single_value;
    }

    return RFC5444_OKAY;
}

/**
 * This callback is called every time the _rreq_consumer finishes reading a
 * packet.
 * @param cont
 * @param dropped indicates whether the packet has been dropped previously by
 *                another callback
 */
static enum rfc5444_result _cb_rreq_end_callback(
    struct rfc5444_reader_tlvblock_context *cont, bool dropped)
{
    (void) cont;

    struct aodvv2_routing_entry_t *rt_entry;
    timex_t now;

    /* We've received a valid RREQ, log this. */
    print_json_received_rreq();

    /* Check if packet contains the required information */
    if (dropped) {
        AODV_DEBUG("\t Dropping packet.\n");
        return RFC5444_DROP_PACKET;
    }
    if ((packet_data.origNode.addr._type == AF_UNSPEC) || !packet_data.origNode.seqnum) {
        AODV_DEBUG("\tERROR: missing OrigNode Address or SeqNum. Dropping packet.\n");
        return RFC5444_DROP_PACKET;
    }
    if (packet_data.targNode.addr._type == AF_UNSPEC) {
        AODV_DEBUG("\tERROR: missing TargNode Address. Dropping packet.\n");
        return RFC5444_DROP_PACKET;
    }
    if (packet_data.hoplimit == 0) {
        AODV_DEBUG("\tERROR: Hoplimit is 0. Dropping packet.\n");
        return RFC5444_DROP_PACKET;
    }
    if ((_get_max_metric(packet_data.metricType) - _get_link_cost(packet_data.metricType))
        <= packet_data.origNode.metric) {
        AODV_DEBUG("\tMetric Limit reached. Dropping packet.\n");
        return RFC5444_DROP_PACKET;
    }

    /*
      The incoming RREQ MUST be checked against previously received
      information from the RREQ Table Section 7.6.  If the information
      in the incoming RteMsg is redundant, then then no further action
      is taken.
    */
    if (rreqtable_is_redundant(&packet_data)) {
        AODV_DEBUG("\tPacket is redundant. Dropping Packet. %i\n", RFC5444_DROP_PACKET);
        return RFC5444_DROP_PACKET;
    }

    /* Update the cost of the route, since the packet has successfully traversed
     * one more hop. */
    packet_data.origNode.metric = _get_route_cost(packet_data.metricType,
                                                  packet_data.origNode.metric);
    vtimer_now(&now);
    packet_data.timestamp = now;

    /* for every relevant
     * address (RteMsg.Addr) in the RteMsg, HandlingRtr searches its route
     * table to see if there is a route table entry with the same MetricType
     * of the RteMsg, matching RteMsg.Addr.
     */

    rt_entry = routingtable_get_entry(&packet_data.origNode.addr, packet_data.metricType);

    if (!rt_entry || (rt_entry->metricType != packet_data.metricType)) {
        /* CAUTION SUPER HACKY FIX FIXME ASAP
        problem: sometimes we get broadcasted RREQs from 2 hop neighbors and then
        AODVv2 gets super confused when they're not in the routing table and starts a
        Route discovery to find them and all hell breaks loose. let's see if we can fix
        this (horribly).

        (another fix would be to stop bouncing the RREP back to the sender and asking
        the routing table for the next hop (or just send towards TargNode and let the
        network stack figure out the rest?))
        TODO evaluate that
        */

        ipv6_addr_t sender_tmp;
        netaddr_to_ipv6_addr_t(&packet_data.sender, &sender_tmp);
        ndp_neighbor_cache_t *ndp_nc_entry = ndp_neighbor_cache_search(&sender_tmp);

        if (ndp_nc_entry == NULL) {
            AODV_DEBUG("No bidirectional link to sender. Dropping packet.\n");
            return RFC5444_DROP_PACKET;
        }
        /* HACKY FIX ENDS HERE */

        struct aodvv2_routing_entry_t *tmp_rt_entry = (struct aodvv2_routing_entry_t *)
                                                       malloc(sizeof(struct aodvv2_routing_entry_t));
        memset(tmp_rt_entry, 0, sizeof(*tmp_rt_entry));

        routingtable_fill_routing_entry_t_rreq(&packet_data, tmp_rt_entry);
        routingtable_add_entry(tmp_rt_entry);

        /* add entry to FIB */
        fib_add_entry(aodvv2_if_id, tmp_rt_entry->addr._addr, sizeof(ipv6_addr_t), 0,
                      tmp_rt_entry->nextHopAddr._addr, sizeof(ipv6_addr_t), 0, aodvv2_validity_t);

        free(tmp_rt_entry);
    }
    else {
        if (!routingtable_offers_improvement(rt_entry, &packet_data.origNode)) {
            AODV_DEBUG("\tPacket offers no improvement over known route. Dropping Packet.\n");
            return RFC5444_DROP_PACKET;
        }
        /* The incoming routing information is better than existing routing
         * table information and SHOULD be used to improve the route table. */
        AODV_DEBUG("\tUpdating Routing Table entry...\n");
        routingtable_fill_routing_entry_t_rreq(&packet_data, rt_entry);

        /* update the FIB */
        fib_update_entry(rt_entry->addr._addr, sizeof(ipv6_addr_t), rt_entry->nextHopAddr._addr,
                         sizeof(ipv6_addr_t), 0, aodvv2_validity_t);
    }

    /*
     * If TargNode is a client of the router receiving the RREQ, then the
     * router generates a RREP message as specified in Section 7.4, and
     * subsequently processing for the RREQ is complete.  Otherwise,
     * processing continues as follows.
     */
    if (clienttable_is_client(&packet_data.targNode.addr)) {
        AODV_DEBUG("TargNode is in client list, sending RREP\n");

        /* make sure to start with a clean metric value */
        packet_data.targNode.metric = 0;
        aodv_send_rrep(&packet_data, &packet_data.sender);
    }
    else {
        AODV_DEBUG("I am not TargNode, forwarding RREQ\n");
        aodv_send_rreq(&packet_data);
    }
    return RFC5444_OKAY;
}

/**
 * This block callback is called for every address
 *
 * @param cont
 * @return
 */
static enum rfc5444_result _cb_rrep_blocktlv_messagetlvs_okay(struct rfc5444_reader_tlvblock_context *cont)
{
    if (!cont->has_hoplimit) {
        AODV_DEBUG("\tERROR: missing hop limit\n");
        return RFC5444_DROP_PACKET;
    }

    packet_data.hoplimit = cont->hoplimit;
    if (packet_data.hoplimit == 0) {
        AODV_DEBUG("\tERROR: Hoplimit is 0.\n");
        return RFC5444_DROP_PACKET;
    }

    packet_data.hoplimit--;

    return RFC5444_OKAY;
}

/**
 * This block callback is called for every address of a RREP Message.
 *
 * @param cont
 * @return
 */
static enum rfc5444_result _cb_rrep_blocktlv_addresstlvs_okay(struct rfc5444_reader_tlvblock_context *cont)
{
#if AODV_DEBUG
    /* cppcheck-suppress unusedVariable as nbuf is needed by AODV_DEBUG. */
    struct netaddr_str nbuf;
#endif
    struct rfc5444_reader_tlvblock_entry *tlv;
    bool is_targNode_addr = false;

    /* handle TargNode SeqNum TLV */
    tlv = _rreq_rrep_address_consumer_entries[RFC5444_MSGTLV_TARGSEQNUM].tlv;
    if (tlv) {
        is_targNode_addr = true;
        packet_data.targNode.addr = cont->addr;
        packet_data.targNode.seqnum = *tlv->single_value;
    }

    /* handle OrigNode SeqNum TLV */
    tlv = _rreq_rrep_address_consumer_entries[RFC5444_MSGTLV_ORIGSEQNUM].tlv;
    if (tlv) {
        is_targNode_addr = false;
        packet_data.origNode.addr = cont->addr;
        packet_data.origNode.seqnum = *tlv->single_value;
    }
    if (!tlv && !is_targNode_addr) {
        AODV_DEBUG("\tERROR: mandatory SeqNum TLV missing.\n");
        return RFC5444_DROP_PACKET;
    }

    /* handle Metric TLV */
    /* cppcheck: suppress false positive on non-trivially initialized arrays.
     *           this is a known bug: http://trac.cppcheck.net/ticket/5497 */
    /* cppcheck-suppress arrayIndexOutOfBounds */
    tlv = _rreq_rrep_address_consumer_entries[RFC5444_MSGTLV_METRIC].tlv;
    if (!tlv && is_targNode_addr) {
        AODV_DEBUG("\tERROR: Missing or unknown metric TLV.\n");
        return RFC5444_DROP_PACKET;
    }
    if (tlv) {
        if (!is_targNode_addr) {
            AODV_DEBUG("\tERROR: metric TLV belongs to wrong address.\n");
            return RFC5444_DROP_PACKET;
        }
        packet_data.metricType = tlv->type_ext;
        packet_data.targNode.metric = *tlv->single_value;
    }
    return RFC5444_OKAY;
}

/**
 * This callback is called every time the _rreq_consumer finishes reading a
 * packet.
 * @param cont
 * @param dropped indicates wehther the packet has been dropped previously by
 *                another callback
 */
static enum rfc5444_result _cb_rrep_end_callback(
    struct rfc5444_reader_tlvblock_context *cont, bool dropped)
{
    (void) cont;

    struct aodvv2_routing_entry_t *rt_entry;
#if AODV_DEBUG
    struct netaddr_str nbuf;
#endif
    timex_t now;

    /* We've received a valid RREP, log this. */
    print_json_received_rrep();

    /* Check if packet contains the required information */
    if (dropped) {
        AODV_DEBUG("\t Dropping packet.\n");
        return RFC5444_DROP_PACKET;
    }
    if ((packet_data.origNode.addr._type == AF_UNSPEC)
        || !packet_data.origNode.seqnum) {
        AODV_DEBUG("\tERROR: missing OrigNode Address or SeqNum. Dropping packet.\n");
        return RFC5444_DROP_PACKET;
    }
    if ((packet_data.targNode.addr._type == AF_UNSPEC)
        || !packet_data.targNode.seqnum) {
        AODV_DEBUG("\tERROR: missing TargNode Address or SeqNum. Dropping packet.\n");
        return RFC5444_DROP_PACKET;
    }
    if ((_get_max_metric(packet_data.metricType) - _get_link_cost(packet_data.metricType))
        <= packet_data.targNode.metric) {
        AODV_DEBUG("\tMetric Limit reached. Dropping packet.\n");
        return RFC5444_DROP_PACKET;
    }

    /* Update the cost of the route, since the packet has successfully traversed
     * one more hop. */
    packet_data.targNode.metric = _get_route_cost(packet_data.metricType,
                                                  packet_data.targNode.metric);
    vtimer_now(&now);
    packet_data.timestamp = now;

    /* for every relevant address (RteMsg.Addr) in the RteMsg, HandlingRtr
    searches its route table to see if there is a route table entry with the
    same MetricType of the RteMsg, matching RteMsg.Addr. */

    rt_entry = routingtable_get_entry(&packet_data.targNode.addr, packet_data.metricType);

    if (!rt_entry || (rt_entry->metricType != packet_data.metricType)) {
        /* CAUTION SUPER HACKY FIX FIXME ASAP
        problem: sometimes we get broadcasted RREQs from 2 hop neighbors and then
        AODVv2 gets super confused when they're not in the routing table and starts a
        Route discovery to find them and all hell breaks loose. let's see if we can fix
        this (horribly).

        (another fix would be to stop bouncing the RREP back to the sender and asking
        the routing table for the next hop (or just send towards TargNode and let the network stack figure out the rest?))
        TODO evaluate that
        */

        ipv6_addr_t sender_tmp;
        netaddr_to_ipv6_addr_t(&packet_data.sender, &sender_tmp);
        ndp_neighbor_cache_t *ndp_nc_entry = ndp_neighbor_cache_search(&sender_tmp);

        if (ndp_nc_entry == NULL) {
            AODV_DEBUG("No bidirectional link to sender. Dropping packet.\n");
            return RFC5444_DROP_PACKET;
        }
        /* HACKY FIX ENDS HERE */

        struct aodvv2_routing_entry_t *tmp_rt_entry = (struct aodvv2_routing_entry_t *)
                                                       malloc(sizeof(struct aodvv2_routing_entry_t));
        memset(tmp_rt_entry, 0, sizeof(*tmp_rt_entry));

        routingtable_fill_routing_entry_t_rrep(&packet_data, tmp_rt_entry);
        routingtable_add_entry(tmp_rt_entry);

        /* add entry to FIB */
        fib_add_entry(aodvv2_if_id, tmp_rt_entry->addr._addr, sizeof(ipv6_addr_t), 0,
                      tmp_rt_entry->nextHopAddr._addr, sizeof(ipv6_addr_t), 0, aodvv2_validity_t);

        free(tmp_rt_entry);
    }
    else {
        if (!routingtable_offers_improvement(rt_entry, &packet_data.targNode)) {
            AODV_DEBUG("\tPacket offers no improvement over known route. Dropping Packet.\n");
            return RFC5444_DROP_PACKET;
        }
        /* The incoming routing information is better than existing routing
         * table information and SHOULD be used to improve the route table. */
        AODV_DEBUG("\tUpdating Routing Table entry...\n");
        routingtable_fill_routing_entry_t_rrep(&packet_data, rt_entry);

        /* update the FIB */
        fib_update_entry(rt_entry->addr._addr, sizeof(ipv6_addr_t), rt_entry->nextHopAddr._addr,
                         sizeof(ipv6_addr_t), 0, aodvv2_validity_t);
    }

    /* If HandlingRtr is RREQ_Gen then the RREP satisfies RREQ_Gen's
    earlier RREQ, and RREP processing is completed.  Any packets
    buffered for OrigNode should be transmitted. */
    if (clienttable_is_client(&packet_data.origNode.addr)) {
        AODV_DEBUG("\t%s:  This is my RREP. We are done here!\n",
              netaddr_to_string(&nbuf, &packet_data.origNode.addr));
    }

    else {
        /* If HandlingRtr is not RREQ_Gen then the outgoing RREP is sent to the
         * Route.NextHopAddress for the RREP.AddrBlk[OrigNodeNdx]. */
        AODV_DEBUG("Not my RREP, passing it on to the next hop\n");
        aodv_send_rrep(&packet_data,
                       routingtable_get_next_hop(&packet_data.origNode.addr,packet_data.metricType));
    }
    return RFC5444_OKAY;
}

static enum rfc5444_result _cb_rerr_blocktlv_messagetlvs_okay(struct rfc5444_reader_tlvblock_context *cont)
{
    if (!cont->has_hoplimit) {
        AODV_DEBUG("\tERROR: missing hop limit\n");
        return RFC5444_DROP_PACKET;
    }

    packet_data.hoplimit = cont->hoplimit;
    if (packet_data.hoplimit == 0) {
        AODV_DEBUG("\tERROR: Hoplimit is 0.\n");
        return RFC5444_DROP_PACKET;
    }

    packet_data.hoplimit--;

    /* prepare buffer for unreachable nodes */
    num_unreachable_nodes = 0;
    for (unsigned i = 0; i < AODVV2_MAX_UNREACHABLE_NODES; i++) {
        memset(&unreachable_nodes[i], 0, sizeof(unreachable_nodes[i]));
    }
    return RFC5444_OKAY;
}

static enum rfc5444_result _cb_rerr_blocktlv_addresstlvs_okay(struct rfc5444_reader_tlvblock_context *cont)
{
#if AODV_DEBUG
    /* cppcheck-suppress unusedVariable as nbuf is needed by AODV_DEBUG. */
    struct netaddr_str nbuf;
#endif
    struct aodvv2_routing_entry_t *unreachable_entry;
    struct rfc5444_reader_tlvblock_entry *tlv;

    AODV_DEBUG("%s()\n", __func__);
    AODV_DEBUG("\tmessage type: %d\n", cont->type);
    AODV_DEBUG("\taddr: %s\n", netaddr_to_string(&nbuf, &cont->addr));

    /* Out of buffer size for more unreachable nodes. We're screwed, basically. */
    if (num_unreachable_nodes == AODVV2_MAX_UNREACHABLE_NODES) {
        return RFC5444_OKAY;
    }

    /* gather packet data */
    packet_data.origNode.addr = cont->addr;

    /* handle this unreachable node's SeqNum TLV */
    /* cppcheck: suppress false positive on non-trivially initialized arrays.
     *           this is a known bug: http://trac.cppcheck.net/ticket/5497 */
    /* cppcheck-suppress arrayIndexOutOfBounds */
    tlv = _rerr_address_consumer_entries[RFC5444_MSGTLV_UNREACHABLE_NODE_SEQNUM].tlv;
    if (tlv) {
        AODV_DEBUG("\ttlv RFC5444_MSGTLV_UNREACHABLE_NODE_SEQNUM: %d\n", *tlv->single_value);
        packet_data.origNode.seqnum = *tlv->single_value;
    }

    /* Check if there is an entry for unreachable node in our routing table */
    unreachable_entry = routingtable_get_entry(&packet_data.origNode.addr, packet_data.metricType);
    if (unreachable_entry) {
        AODV_DEBUG("\t found possibly unreachable entry.\n");

        /* check if route to unreachable node has to be marked as broken and RERR has to be forwarded */
        if (netaddr_cmp(&unreachable_entry->nextHopAddr, &packet_data.sender) == 0
                && (!tlv || seqnum_cmp(unreachable_entry->seqnum, packet_data.origNode.seqnum) == 0)) {
            unreachable_entry->state = ROUTE_STATE_INVALID;
            unreachable_nodes[num_unreachable_nodes].addr = packet_data.origNode.addr;
            unreachable_nodes[num_unreachable_nodes].seqnum = packet_data.origNode.seqnum;
            num_unreachable_nodes++;
        }

        /* remove entry from FIB */
        fib_remove_entry(packet_data.origNode.addr._addr, sizeof(ipv6_addr_t));
    }

    return RFC5444_OKAY;
}

static enum rfc5444_result _cb_rerr_end_callback(struct rfc5444_reader_tlvblock_context *cont, bool dropped)
{
    (void) cont;

    if (dropped) {
        AODV_DEBUG("\tDropping packet.\n");
        return RFC5444_DROP_PACKET;
    }

    if (num_unreachable_nodes == 0) {
        AODV_DEBUG("\tNo unreachable nodes from my routing table. Dropping Packet.\n");
        return RFC5444_DROP_PACKET;
    }
    /* gather all unreachable nodes and put them into a RERR */
    aodv_send_rerr(unreachable_nodes, num_unreachable_nodes, &na_mcast);
    return RFC5444_OKAY;
}

void aodv_packet_reader_init(void)
{
    AODV_DEBUG("%s()\n", __func__);

    /* initialize reader */
    rfc5444_reader_init(&reader);

    /* register message consumers. We have no message TLVs, so we can leave the
     * rfc5444_reader_tlvblock_consumer_entry empty */
    rfc5444_reader_add_message_consumer(&reader, &_rreq_consumer,
                                        NULL, 0);
    rfc5444_reader_add_message_consumer(&reader, &_rrep_consumer,
                                        NULL, 0);
    rfc5444_reader_add_message_consumer(&reader, &_rerr_consumer,
                                        NULL, 0);

    /* register address consumer */
    rfc5444_reader_add_message_consumer(&reader, &_rreq_address_consumer,
                                        _rreq_rrep_address_consumer_entries,
                                        ARRAYSIZE(_rreq_rrep_address_consumer_entries));
    rfc5444_reader_add_message_consumer(&reader, &_rrep_address_consumer,
                                        _rreq_rrep_address_consumer_entries,
                                        ARRAYSIZE(_rreq_rrep_address_consumer_entries));
    rfc5444_reader_add_message_consumer(&reader, &_rerr_address_consumer,
                                        _rerr_address_consumer_entries,
                                        ARRAYSIZE(_rerr_address_consumer_entries));
}

void aodv_packet_reader_cleanup(void)
{
    rfc5444_reader_cleanup(&reader);
}

int aodv_packet_reader_handle_packet(void *buffer, size_t length, struct netaddr *sender)
{
    memcpy(&packet_data.sender, sender, sizeof(*sender));

    return rfc5444_reader_handle_packet(&reader, buffer, length);
}

/*============= HELPER FUNCTIONS =============================================*/

static void print_json_received_rreq(void)
{
#if TEST_SETUP
    static struct netaddr_str nbuf_origaddr, nbuf_targaddr, nbuf_send;

    printf("{\"log_type\": \"received_rreq\", "
        "\"log_data\":{ \"last_hop\": \"%s\", \"orig_addr\": \"%s\", "
        "\"orig_seqnum\": %d, \"targ_addr\": \"%s\", \"metric\": %d}}\n",
        netaddr_to_string(&nbuf_send, &packet_data.sender),
        netaddr_to_string(&nbuf_origaddr, &packet_data.origNode.addr),
        packet_data.origNode.seqnum,
        netaddr_to_string(&nbuf_targaddr, &packet_data.targNode.addr),
        packet_data.origNode.metric);
#endif
}

static void print_json_received_rrep(void)
{
#if TEST_SETUP
    static struct netaddr_str nbuf_origaddr, nbuf_targaddr, nbuf_send;

    printf("{\"log_type\": \"received_rrep\", "
        "\"log_data\":{ \"last_hop\": \"%s\", \"orig_addr\": \"%s\", "
        "\"orig_seqnum\": %d, \"targ_addr\": \"%s\", \"targ_seqnum\":%d}}\n",
        netaddr_to_string(&nbuf_send, &packet_data.sender),
        netaddr_to_string(&nbuf_origaddr, &packet_data.origNode.addr),
        packet_data.origNode.seqnum,
        netaddr_to_string(&nbuf_targaddr, &packet_data.targNode.addr),
        packet_data.targNode.seqnum);
#endif
}

/*
 * Cost(L): Get Cost of a Link regarding the specified metric.
 * (currently only AODVV2_DEFAULT_METRIC_TYPE (HopCount) implemented)
 * returns cost if metric is known, 0 otherwise
 */
static uint8_t _get_link_cost(aodvv2_metric_t metricType)
{
    if (metricType == AODVV2_DEFAULT_METRIC_TYPE) {
        return 1;
    }
    return 0;
}

/*
 * MAX_METRIC[MetricType]:
 * returns maximum value of the given metric if metric is known, 0 otherwise.
 */
static uint8_t _get_max_metric(aodvv2_metric_t metricType)
{
    if (metricType == AODVV2_DEFAULT_METRIC_TYPE) {
        return AODVV2_MAX_HOPCOUNT;
    }
    return 0;
}

/*
 * Cost(R): Get Cost of a Route regarding the specified metric, based on the
 * earlier metric value of the Route.
 * (currently only AODVV2_DEFAULT_METRIC_TYPE (HopCount) implemented)
 * returns cost if metric is known, 0 otherwise
 */
static uint8_t _get_route_cost(aodvv2_metric_t metricType, uint8_t metric)
{
    if (metricType == AODVV2_DEFAULT_METRIC_TYPE){
        return metric + _get_link_cost(AODVV2_DEFAULT_METRIC_TYPE);
    }
    return 0;
}

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
 * @brief       aodvv2 routing protocol
 *
 * @author      Lotte Steenbrink <lotte.steenbrink@fu-berlin.de>
 * @author      Martin Landsmann <Martin.Landsmann@HAW-Hamburg.de>
 */

#include "aodv.h"
#include "net/gnrc/aodvv2.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define AODVV2_PREFIX {{ 0xbe, 0xe2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }}
#define AODV_MSG_Q_SIZE      (32)  /**< TODO: check if smaller values work, too */
#define UDP_BUFFER_SIZE     (128)  /**< with respect to IEEE 802.15.4's MTU */

static void aodv_init_addresses(void);
static void *aodv_thread(void *arg);
static void aodv_send_packet(struct rfc5444_writer *wr __attribute__ ((unused)),
                struct rfc5444_writer_target *iface __attribute__((unused)),
                void *buffer, size_t length);
static void print_json_pkt_sent(struct writer_target *wt);

#if TEST_SETUP
static struct netaddr_str nbuf_origaddr, nbuf_targaddr, nbuf_nexthop;
#endif

kernel_pid_t aodvv2_if_id; /* The interface AODVv2 operates on & finds routes for */

/* The metric type AODVv2 uses. (In case a different type is ever added, consider
 * adding getter & setter functions and moving them to a metric.c, along with
 * loopFree(), cost() etc functions) */
static aodvv2_metric_t aodv_metric_type;

static ipv6_addr_t aodvv2_prefix = AODVV2_PREFIX;
static int aodvv2_prefix_len;
static int aodvv2_address_type_size;

static ipv6_addr_t aodv_ipv6_addr_local; /* The IP address of this router */
static ipv6_addr_t aodv_ipv6_addr_mcast;

/* netaddr representation of this router's addresses (for oonf api interaction) */
static struct netaddr aodvv2_netaddr_local;
struct netaddr aodvv2_netaddr_mcast;

static char aodv_stack_buf[THREAD_STACKSIZE_MAIN];

/*
    TODO:
    - old_aodv durchgehen um sicher zu gehen dass ich nix übersehen hab
*/

void aodv_init(kernel_pid_t interface)
{
    DEBUG("%s()\n", __func__);

    if (!interface) {
        DEBUG("Error: AODVv2 interface pid is invalid.\n");
        return;
    }

    /* initialize AODVv2 internals */
    aodvv2_if_id = interface;
    aodv_metric_type = AODVV2_DEFAULT_METRIC_TYPE;
    seqnum_init();
    routingtable_init();
    clienttable_init();
    rreqtable_init();

    /* initialize addresses */
    aodvv2_address_type_size = sizeof(ipv6_addr_t);
    aodvv2_prefix_len = 64;
    aodv_init_addresses();

    /* every node is its own client. */
    clienttable_add_client(&aodvv2_netaddr_local);

    /* init RFC 5444 reader and writer */
    aodv_packet_reader_init();
    aodv_packet_writer_init(aodv_send_packet);

    /* start listening & sending packets */
    thread_create(aodv_stack_buf, sizeof(aodv_stack_buf), THREAD_PRIORITY_MAIN - 1,
                  THREAD_CREATE_STACKTEST, aodv_thread, NULL, "aodv_thread");
}

/*
 * init the multicast address all RREQ and RERRS are sent to
 * and the local address (source address) of this node
 */
static void aodv_init_addresses(void)
{
    eui64_t iid;

    /* init multicast address: set to to a link-local all nodes multicast address */
    ipv6_addr_set_all_nodes_multicast(&aodv_ipv6_addr_mcast, IPV6_ADDR_MCAST_SCP_LINK_LOCAL);

    /* get id (of type eui46_t) of the interface we're sending on */
    int err_code = gnrc_netapi_get(aodvv2_if_id, NETOPT_IPV6_IID, 0, &iid,
                       sizeof(eui64_t));
    if (err_code < 0) {
        DEBUG("Error: failed to get iid, error code: %d\n", err_code);
        return;
    }

    /* Set addr according to our interface id */
    ipv6_addr_set_aiid(&aodv_ipv6_addr_local, &iid.uint8[0]);
    /* Set (global!) prefix */
    ipv6_addr_init_prefix(&aodv_ipv6_addr_local, &aodvv2_prefix, aodvv2_prefix_len);
    /* Add our homemade address to our interface (aodvv2_if_id) */
    gnrc_ipv6_netif_add_addr(aodvv2_if_id, &aodv_ipv6_addr_local, aodvv2_prefix_len, 0);

    /* store src & multicast address as netaddr as well for easy interaction
     * with oonf api (i.e. reader & writer code) */
    ipv6_addr_t_to_netaddr(&aodv_ipv6_addr_local, &aodvv2_netaddr_local);
    ipv6_addr_t_to_netaddr(&aodv_ipv6_addr_mcast, &aodvv2_netaddr_mcast);
}

static void *aodv_thread(void *arg)
{
    (void) arg;

    /* general message handling stuff */
    gnrc_netreg_entry_t server = {
        .next = NULL,
        .demux_ctx = GNRC_NETREG_DEMUX_CTX_ALL,
        .pid = KERNEL_PID_UNDEF
    };

    msg_t aodv_msgq[AODV_MSG_Q_SIZE];
    msg_init_queue(aodv_msgq, AODV_MSG_Q_SIZE);

    msg_t msg, reply;
    reply.content.value = (uint32_t)(-ENOTSUP);
    reply.type = GNRC_NETAPI_MSG_TYPE_ACK;

    /* start server & register AODVv2 receiver for MANET_PORT */
    server.pid = sched_active_pid; /* sched_active_pid is currently our pid ;) */
    server.demux_ctx = (uint32_t) MANET_PORT;
    gnrc_netreg_register(GNRC_NETTYPE_UDP, &server);

    /* GNRC_NETAPI_MSG_TYPE_RCV message stuff */
    char pkt_rcv_buf[UDP_BUFFER_SIZE];
    struct netaddr pkt_sender;

    /* FIB_MSG_RP_SIGNAL messages stuff*/
    ipv6_addr_t ipv6_dest;
    struct netaddr netaddr_dest;

    int err = fib_register_rp(&gnrc_ipv6_fib_table, &aodvv2_prefix.u8[0],
                              aodvv2_address_type_size);
    if ( err != 0) {
        DEBUG("[aodvv2] ERROR: cannot register at fib, error code:\n");
        exit(1);
    }

    /* start listening for incoming AODVv2 packets and FIB control messages */
    while (1) {
        msg_receive(&msg);
        switch (msg.type) {
            case GNRC_NETAPI_MSG_TYPE_RCV:
            {
                gnrc_pktsnip_t *pkt = (gnrc_pktsnip_t *) msg.content.ptr;
                if (pkt->size <= UDP_BUFFER_SIZE) {
                    memcpy(pkt_rcv_buf, pkt->data, pkt->size);

                    if(pkt->next->next->type == GNRC_NETTYPE_IPV6) {
                        ipv6_addr_t_to_netaddr(&(((ipv6_hdr_t*)
                                                  (pkt->next->next->data))->src),
                                               &pkt_sender);
                    }
                    if (!netaddr_cmp(&pkt_sender, &aodvv2_netaddr_local) == 0) {
                        /* didn't receive our own packet: handle, don't drop */
                        aodv_packet_reader_handle_packet((void *) pkt_rcv_buf,
                                                         pkt->size, &pkt_sender);
                    }
                }
                break;
            }
            case GNRC_NETAPI_MSG_TYPE_SND:
                break;
            case GNRC_NETAPI_MSG_TYPE_GET:
            case GNRC_NETAPI_MSG_TYPE_SET:
                msg_reply(&msg, &reply);
                break;
            case FIB_MSG_RP_SIGNAL_UNREACHABLE_DESTINATION:
            {
                rp_address_msg_t* rp_msg = (rp_address_msg_t*)msg.content.ptr;
                if (rp_msg->address_size == sizeof(ipv6_addr_t)) {
                    /* We currently only support IPv6*/
                    memcpy(&ipv6_dest, rp_msg->address, rp_msg->address_size);
                    /* Reply to the FIB so that it can stop blocking */
                    msg_reply(&msg, &msg);

                    /* perform/initiate a rreq for dst here*/
                    ipv6_addr_t_to_netaddr(&ipv6_dest, &netaddr_dest);

                    aodvv2_seqnum_t seqnum = seqnum_get();
                    seqnum_inc();

                    /* Build new RREQ */
                    struct aodvv2_packet_data new_rreq_data = (struct aodvv2_packet_data) {
                        .hoplimit = AODVV2_MAX_HOPCOUNT,
                        .metricType = aodv_metric_type,
                        .origNode = (struct node_data) {
                            .addr = aodvv2_netaddr_local,
                            .metric = 0,
                            .seqnum = seqnum,
                        },
                        .targNode = (struct node_data) {
                            .addr = netaddr_dest,
                        },
                        .timestamp = (timex_t) {0,0} /* never used, exists merely
                                                      * to silence compiler*/
                    };

                    /* since we're originating the RREQ, add it to the RREQ
                     * table or add its predecessor (if any) */
                    rreqtable_is_redundant(&new_rreq_data);

                    aodv_packet_writer_send_rreq(&new_rreq_data,
                                                 &aodvv2_netaddr_mcast);
                }
                else {
                    /* Reply to the FIB so that it can stop blocking */
                    msg_reply(&msg, &msg);
                }
                break;
            }
            default:
                DEBUG("[aodvv2] received msg of unexpected type: %d\n", msg.type);
                break;
        }
    }

    return NULL;
}

/**
 * Send finalized RFC 5444 packets. This callback is called by every
 * aodv_packet_writer_send_* function.
 */
static void aodv_send_packet(struct rfc5444_writer *wr __attribute__ ((unused)),
                          struct rfc5444_writer_target *iface __attribute__((unused)),
                          void *buffer, size_t length)
{
    ipv6_addr_t addr_send;
    gnrc_pktsnip_t *payload, *pkt_with_udp, *pkt_with_ip;

    /* fetch the address the packet is supposed to be sent to (i.e. to a
     * specific node or the multicast address) from the writer_target struct
     * iface* is stored in. This is a bit hacky, but it does the trick. */
    struct writer_target *wt = container_of(iface, struct writer_target, interface);

    print_json_pkt_sent(wt);
    netaddr_to_ipv6_addr_t(&wt->target_addr, &addr_send);

    /* allocate payload (i.e. the RFC5444 packet)*/
    payload = gnrc_pktbuf_add(NULL, buffer, length, GNRC_NETTYPE_UNDEF);
    if (payload == NULL) {
        DEBUG("Error: unable to copy data to packet buffer\n");
        return;
    }
    /* allocate UDP header, set payload, set source port := destination port */
    pkt_with_udp = gnrc_udp_hdr_build(payload, MANET_PORT, MANET_PORT);
    if (pkt_with_udp == NULL) {
        DEBUG("Error: unable to allocate UDP header\n");
        gnrc_pktbuf_release(payload);
        return;
    }
    /* allocate IPv6 header, set pkt_with_udp as payload */
    pkt_with_ip = gnrc_ipv6_hdr_build(pkt_with_udp, &aodv_ipv6_addr_local,
                                      &addr_send);
    if (pkt_with_ip == NULL) {
        DEBUG("Error: unable to allocate IPv6 header\n");
        gnrc_pktbuf_release(pkt_with_udp);
        return;
    }
    /* all headers are set, send packet */
    if(!gnrc_netapi_dispatch_send(GNRC_NETTYPE_UDP, GNRC_NETREG_DEMUX_CTX_ALL,
                                 pkt_with_ip)) {
        DEBUG("Error sending packet\n");
        gnrc_pktbuf_release(pkt_with_ip);
    }
}


/* Print the json representation of a sent packet to stdout for automated
 * testing and debugging */
static void print_json_pkt_sent(struct writer_target *wt)
{
#if TEST_SETUP
    // note: what if the content at wt has changed until this is printed? memcpy the entire thing?
    int msg_type = wt->type;
    if (msg_type == RFC5444_MSGTYPE_RREQ) {
    printf("{\"log_type\": \"sent_rreq\", \"log_data\": {"
        "\"orig_addr\": \"%s\", \"targ_addr\": \"%s\", \"orig_seqnum\": %d, \"metric\": %d}}\n",
        netaddr_to_string(&nbuf_origaddr, &wt->packet_data.origNode.addr),
        netaddr_to_string(&nbuf_targaddr, &wt->packet_data.targNode.addr),
        wt->packet_data.origNode.seqnum, wt->packet_data.origNode.metric);
    }
    if (msg_type == RFC5444_MSGTYPE_RREP) {
    printf("{\"log_type\": \"sent_rrep\", \"log_data\": {"
        "\"next_hop\": \"%s\",\"orig_addr\": \"%s\", \"orig_seqnum\": %d,"
        " \"targ_addr\": \"%s\"}}\n",
                netaddr_to_string(&nbuf_nexthop, &wt->target_addr),
                netaddr_to_string(&nbuf_origaddr, &wt->packet_data.origNode.addr),
                wt->packet_data.origNode.seqnum,
                netaddr_to_string(&nbuf_targaddr, &wt->packet_data.targNode.addr));
    }
    if (msg_type == RFC5444_MSGTYPE_RERR) {
        /* TODO */
    }
#else
    (void) wt; /* silence compiler */
#endif
}

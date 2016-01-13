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
#include "aodvv2/aodvv2.h"
#include "aodv_debug.h"
#include "ng_fib.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define UDP_BUFFER_SIZE     (128) /** with respect to IEEE 802.15.4's MTU */
#define RCV_MSG_Q_SIZE      (32)  /* TODO: check if smaller values work, too */
#define AODVV2_PREFIX {{ 0xbe, 0xe2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }}

static void _init_addresses(void);
static void *_aodv_receiver_thread(void *arg);
static void *_aodv_sender_thread(void *arg);
static void *fib_signal_handler_thread(void *arg);

static void _deep_free_msg_container(struct msg_container *msg_container);
static void _write_packet(struct rfc5444_writer *wr __attribute__ ((unused)),
                          struct rfc5444_writer_target *iface __attribute__((unused)),
                          void *buffer, size_t length);
static void print_json_pkt_sent(struct writer_target *wt);

#if AODV_DEBUG
char addr_str[IPV6_MAX_ADDR_STR_LEN];
static struct netaddr_str nbuf;
static char addr_str[IPV6_MAX_ADDR_STR_LEN];
#endif
#if TEST_SETUP
static struct netaddr_str nbuf_origaddr, nbuf_targaddr, nbuf_nexthop;
#endif

static char aodv_rcv_stack_buf[THREAD_STACKSIZE_MAIN];
static char aodv_snd_stack_buf[THREAD_STACKSIZE_MAIN];
static char aodv_fib_stack_buf[THREAD_STACKSIZE_MAIN];

static aodvv2_metric_t _metric_type;
static int sender_thread;
static struct autobuf _hexbuf;
static ng_ipv6_addr_t _v6_addr_mcast, _v6_addr_loopback;
static ng_ipv6_addr_t _v6_addr_local;
static struct netaddr na_local; /* the same as _v6_addr_local, but to save us
                                 * constant calls to ipv6_addr_t_to_netaddr()... */
static struct writer_target *wt;
static mutex_t rreq_mutex;
static mutex_t rrep_mutex;
static mutex_t rerr_mutex;

static ng_ipv6_addr_t aodvv2_prefix = AODVV2_PREFIX;
static int aodvv2_prefix_len;
static int aodvv2_address_type_size;

struct netaddr na_mcast;
kernel_pid_t aodvv2_if_id;

void aodv_init(kernel_pid_t interface)
{
    DEBUG("%s()\n", __func__);

    /* init this thread's IPC msg queue */
    msg_t msgq[RCV_MSG_Q_SIZE];
    msg_init_queue(msgq, sizeof msgq);

    if (!interface) {
        DEBUG("Error: AODVv2 interface pid is invalid.\n");
        return;
    }
    aodvv2_if_id = interface;

    aodvv2_address_type_size = sizeof(ng_ipv6_addr_t);
    aodvv2_prefix_len = 64;

    mutex_init(&rreq_mutex);
    mutex_init(&rrep_mutex);
    mutex_init(&rerr_mutex);

    aodv_set_metric_type(AODVV2_DEFAULT_METRIC_TYPE);
    _init_addresses();

    /* init ALL the things! \o, */
    seqnum_init();
    routingtable_init();
    clienttable_init();

    /* every node is its own client. */
    clienttable_add_client(&na_local);
    rreqtable_init();

    /* init reader and writer */
    aodv_packet_reader_init();
    aodv_packet_writer_init(_write_packet);

    /* start listening & enable sending */
    thread_create(aodv_rcv_stack_buf, sizeof(aodv_rcv_stack_buf), THREAD_PRIORITY_MAIN,
                  CREATE_STACKTEST, _aodv_receiver_thread, NULL, "_aodv_receiver_thread");
    AODV_DEBUG("listening on port %d\n", MANET_PORT);
    sender_thread = thread_create(aodv_snd_stack_buf, sizeof(aodv_snd_stack_buf),
                                  THREAD_PRIORITY_MAIN, CREATE_STACKTEST, _aodv_sender_thread,
                                  NULL, "_aodv_sender_thread");
    thread_create(aodv_fib_stack_buf, sizeof(aodv_fib_stack_buf),
                                  THREAD_PRIORITY_MAIN, CREATE_STACKTEST, fib_signal_handler_thread,
                                  NULL, "fib_signal_handler_thread");
}

void aodv_set_metric_type(aodvv2_metric_t metric_type)
{
    if (metric_type != AODVV2_DEFAULT_METRIC_TYPE) {
        return;
    }
    _metric_type = metric_type;
}
/*
 * @brief handles callbacks from the FIB when it needs a route
 */
void *fib_signal_handler_thread(void *arg)
{
    (void) arg;
    ng_ipv6_addr_t dest;
    struct netaddr na_dest;

    int err = fib_register_rp(&aodvv2_prefix.u8[0], aodvv2_address_type_size);
    if ( err != 0) {
        DEBUG("ERROR: cannot register at fib, error code:\n");
        exit(1);
    }

    while (true) {
        msg_t msg;
        msg_receive(&msg);

        if (msg.type == FIB_MSG_RP_SIGNAL) {
            rp_address_msg_t* rp_msg = (rp_address_msg_t*)msg.content.ptr;
            if (rp_msg->address_size == sizeof(ng_ipv6_addr_t)) {
                /* We currently only support IPv6*/
                memcpy(&dest, rp_msg->address, rp_msg->address_size);
                /* Reply to the FIB so that it can stop blocking */
                msg_reply(&msg, &msg);

                /* perform/initiate a rreq for dst here*/
                ipv6_addr_t_to_netaddr(&dest, &na_dest);

                aodvv2_seqnum_t seqnum = seqnum_get();
                seqnum_inc();

                /* Build new RREQ */
                struct aodvv2_packet_data rreq_data = (struct aodvv2_packet_data) {
                    .hoplimit = AODVV2_MAX_HOPCOUNT,
                    .metricType = _metric_type,
                    .origNode = (struct node_data) {
                        .addr = na_local,
                        .metric = 0,
                        .seqnum = seqnum,
                    },
                    .targNode = (struct node_data) {
                        .addr = na_dest,
                    },
                    .timestamp = (timex_t) {0,0} /* this timestamp is never used, it exists
                                                  * merely to make the compiler shut up */
                };

                AODV_DEBUG("\tstarting route discovery towards %s... \n",
                      ng_ipv6_addr_to_str(addr_str, &dest, NG_IPV6_ADDR_MAX_STR_LEN));
                aodv_send_rreq(&rreq_data);
            }
            else {
                /* Reply to the FIB so that it can stop blocking */
                msg_reply(&msg, &msg);
            }
        }
    }
    return NULL;
}

void aodv_send_rreq(struct aodvv2_packet_data *packet_data)
{
    /* Make sure only one thread is dispatching a RREQ at a time */
    mutex_lock(&rreq_mutex);

    struct aodvv2_packet_data *pd = malloc(sizeof(struct aodvv2_packet_data));
    memcpy(pd, packet_data, sizeof(struct aodvv2_packet_data));

    struct rreq_rrep_data *rd = malloc(sizeof(struct rreq_rrep_data));
    *rd = (struct rreq_rrep_data) {
        .next_hop = &na_mcast,
        .packet_data = pd,
    };

    struct msg_container *mc = malloc(sizeof(struct msg_container));
    *mc = (struct msg_container) {
        .type = RFC5444_MSGTYPE_RREQ,
        .data = rd
    };

    msg_t msg;
    msg.content.ptr = (char *) mc;

    msg_try_send(&msg, sender_thread);
    mutex_unlock(&rreq_mutex);
}

void aodv_send_rrep(struct aodvv2_packet_data *packet_data, struct netaddr *next_hop)
{
    /* Make sure only one thread is dispatching a RREP at a time */
    mutex_lock(&rrep_mutex);

    struct aodvv2_packet_data *pd = malloc(sizeof(struct aodvv2_packet_data));
    memcpy(pd, packet_data, sizeof(struct aodvv2_packet_data));

    struct netaddr *nh = malloc(sizeof(struct netaddr));
    memcpy(nh, next_hop, sizeof(struct netaddr));

    struct rreq_rrep_data *rd = malloc(sizeof(struct rreq_rrep_data));
    *rd = (struct rreq_rrep_data) {
        .next_hop = nh,
        .packet_data = pd,
    };

    struct msg_container *mc = malloc(sizeof(struct msg_container));
    *mc = (struct msg_container) {
        .type = RFC5444_MSGTYPE_RREP,
        .data = rd
    };

    msg_t msg;
    msg.content.ptr = (char *) mc;

    msg_try_send(&msg, sender_thread);
    mutex_unlock(&rrep_mutex);
}

void aodv_send_rerr(struct unreachable_node unreachable_nodes[], size_t len, struct netaddr *next_hop)
{
    /* Make sure only one thread is dispatching a RERR at a time */
    mutex_lock(&rerr_mutex);

    struct rerr_data *rerrd = malloc(sizeof(struct rerr_data));
    *rerrd = (struct rerr_data) {
        .unreachable_nodes = unreachable_nodes,
        .len = len,
        .hoplimit = AODVV2_MAX_HOPCOUNT,
        .next_hop = next_hop
    };

    struct msg_container *mc2 = malloc(sizeof(struct msg_container));
    *mc2 = (struct msg_container) {
        .type = RFC5444_MSGTYPE_RERR,
        .data = rerrd
    };

    msg_t msg2;
    msg2.content.ptr = (char *) mc2;

    msg_try_send(&msg2, sender_thread);
    mutex_unlock(&rerr_mutex);
}

/*
 * init the multicast address all RREQ and RERRS are sent to
 * and the local address (source address) of this node
 */
static void _init_addresses(void)
{
    eui64_t iid;

    /* init multicast address: set to to a link-local all nodes multicast address */
    ng_ipv6_addr_set_all_nodes_multicast(&_v6_addr_mcast, NG_IPV6_ADDR_MCAST_SCP_LINK_LOCAL);
    DEBUG("my multicast address is: %s\n",
        ng_ipv6_addr_to_str(addr_str, &_v6_addr_mcast, NG_IPV6_ADDR_MAX_STR_LEN));

    /* get id (of type eui46_t) of the interface we're sending on */
    int err_code = ng_netapi_get(aodvv2_if_id, NETCONF_OPT_IPV6_IID, 0, &iid,
                       sizeof(eui64_t));
    if (err_code < 0) {
        DEBUG("Error: failed to get iid, error code: %d\n", err_code);
        return;
    }

    /* Set addr according to our interface id */
    ng_ipv6_addr_set_aiid(&_v6_addr_local, &iid.uint8[0]);
    /* Set (global!) prefix */
    ng_ipv6_addr_init_prefix(&_v6_addr_local, &aodvv2_prefix, aodvv2_prefix_len);
    /* Add our homemade address to our interface (aodvv2_if_id) */
    ng_ipv6_netif_add_addr(aodvv2_if_id, &_v6_addr_local, aodvv2_prefix_len, 0);

    DEBUG("my src address is:       %s\n",
        ng_ipv6_addr_to_str(addr_str, &_v6_addr_local, NG_IPV6_ADDR_MAX_STR_LEN));

    /* store src & multicast address as netaddr as well for easy interaction
     * with oonf based stuff */
    ipv6_addr_t_to_netaddr(&_v6_addr_local, &na_local);
    ipv6_addr_t_to_netaddr(&_v6_addr_mcast, &na_mcast);

    /* TODO: do I need this?*/
    ng_ipv6_addr_set_loopback(&_v6_addr_loopback);
}

/* Build RREQs, RREPs and RERRs from the information contained in the thread's
 * message queue and send them */
static void *_aodv_sender_thread(void *arg)
{
    (void) arg;

    msg_t msgq[RCV_MSG_Q_SIZE];
    msg_init_queue(msgq, RCV_MSG_Q_SIZE);

    while (true) {
        msg_t msg;
        msg_receive(&msg);
        struct msg_container *mc = (struct msg_container *) msg.content.ptr;

        if (mc->type == RFC5444_MSGTYPE_RREQ) {
            struct rreq_rrep_data *rreq_data = (struct rreq_rrep_data *) mc->data;
            aodv_packet_writer_send_rreq(rreq_data->packet_data, rreq_data->next_hop);
        }
        else if (mc->type == RFC5444_MSGTYPE_RREP) {
            struct rreq_rrep_data *rrep_data = (struct rreq_rrep_data *) mc->data;
            aodv_packet_writer_send_rrep(rrep_data->packet_data, rrep_data->next_hop);
        }
        else if (mc->type == RFC5444_MSGTYPE_RERR) {
            struct rerr_data *rerr_data = (struct rerr_data *) mc->data;
            aodv_packet_writer_send_rerr(rerr_data->unreachable_nodes, rerr_data->len,
                             rerr_data->hoplimit, rerr_data->next_hop);
        }
        else {
            AODV_DEBUG("ERROR: Couldn't identify Message\n");
        }
        _deep_free_msg_container(mc);
    }

    return NULL;
}

/* receive RREQs, RREPs and RERRs and handle them */
static void *_aodv_receiver_thread(void *arg)
{
    (void) arg;

    ng_netreg_entry_t server = {
        .next = NULL,
        .demux_ctx = NG_NETREG_DEMUX_CTX_ALL,
        .pid = KERNEL_PID_UNDEF };

    AODV_DEBUG("%s()\n", __func__);
    char buf_rcv[UDP_BUFFER_SIZE];
    struct netaddr _sender;

    msg_t msg, reply;
    msg_t msg_q[RCV_MSG_Q_SIZE];

    msg_init_queue(msg_q, RCV_MSG_Q_SIZE);
    reply.content.value = (uint32_t)(-ENOTSUP);
    reply.type = NG_NETAPI_MSG_TYPE_ACK;

    /* start server (which means registering AODVv2 receiver for the chosen port) */
    server.pid = sched_active_pid; /* sched_active_pid is our pid, since we are currently act */
    server.demux_ctx = (uint32_t) MANET_PORT;
    ng_netreg_register(NG_NETTYPE_UDP, &server);

    while (1) {
        msg_receive(&msg);
        switch (msg.type) {
            case NG_NETAPI_MSG_TYPE_RCV:
                AODV_DEBUG("received data:\n");
                ng_pktsnip_t *pkt = ((ng_pktsnip_t *)msg.content.ptr);
                if (pkt->size <= UDP_BUFFER_SIZE) {
                    memcpy(buf_rcv, pkt->data, pkt->size);

                    if(pkt->next->next->type == NG_NETTYPE_IPV6) {
                        ipv6_addr_t_to_netaddr(&(((ng_ipv6_hdr_t*)(pkt->next->next->data))->src), &_sender);
                    }

                    if (netaddr_cmp(&_sender, &na_local) == 0) {
                        AODV_DEBUG("received our own packet, dropping it.\n");
                    }
                    else {
                        aodv_packet_reader_handle_packet((void *) buf_rcv, pkt->size, &_sender);
                    }
                }
                ng_pktbuf_release(pkt);
                break;
            case NG_NETAPI_MSG_TYPE_SND:
                break;
            case NG_NETAPI_MSG_TYPE_GET:
            case NG_NETAPI_MSG_TYPE_SET:
                msg_reply(&msg, &reply);
                break;
            default:
                AODV_DEBUG("received something unexpected\n");
                break;
        }
    }

    return NULL;
}

/**
    The code o this function was borrowed from:
    examples/ng_networking/udp.c:34 - :90
*/
static void send(ng_ipv6_addr_t addr, uint16_t port, void *data, size_t data_length)
{
    ng_pktsnip_t *payload, *pkt_with_udp, *pkt_with_ip;

    /* allocate payload */
    payload = ng_pktbuf_add(NULL, data, data_length, NG_NETTYPE_UNDEF);
    if (payload == NULL) {
        DEBUG("Error: unable to copy data to packet buffer\n");
        return;
    }
    /* allocate UDP header, set payload, set source port := destination port */
    pkt_with_udp = ng_udp_hdr_build(payload, (uint8_t*)&port, 2, (uint8_t*)&port, 2);
    if (pkt_with_udp == NULL) {
        DEBUG("Error: unable to allocate UDP header\n");
        ng_pktbuf_release(payload);
        return;
    }
    /* allocate IPv6 header, set pkt_with_udp as payload */
    pkt_with_ip = ng_ipv6_hdr_build(pkt_with_udp, (uint8_t*)&_v6_addr_local, aodvv2_address_type_size,
                                    (uint8_t *)&addr, sizeof(addr));
    if (pkt_with_ip == NULL) {
        DEBUG("Error: unable to allocate IPv6 header\n");
        ng_pktbuf_release(pkt_with_udp);
        return;
    }
    /* all headers are set, send packet */
    if(!ng_netapi_dispatch_send(NG_NETTYPE_UDP, NG_NETREG_DEMUX_CTX_ALL, pkt_with_ip)) {
        DEBUG("Error sending packet\n");
    }
}

/**
 * Handle the output of the RFC5444 packet creation process. This callback is
 * called by every writer_send_* function.
 */
static void _write_packet(struct rfc5444_writer *wr __attribute__ ((unused)),
                          struct rfc5444_writer_target *iface __attribute__((unused)),
                          void *buffer, size_t length)
{
    AODV_DEBUG("%s()\n", __func__);
    ng_ipv6_addr_t addr_send;

    /* generate hexdump and human readable representation of packet
     * and print to console */
    abuf_hexdump(&_hexbuf, "\t", buffer, length);
    rfc5444_print_direct(&_hexbuf, buffer, length);
    /* DEBUG("%s", abuf_getptr(&_hexbuf)); */
    abuf_clear(&_hexbuf);

    /* fetch the address the packet is supposed to be sent to (i.e. to a
     * specific node or the multicast address) from the writer_target struct
     * iface* is stored in. This is a bit hacky, but it does the trick. */
    wt = container_of(iface, struct writer_target, interface);

    print_json_pkt_sent(wt);
    netaddr_to_ipv6_addr_t(&wt->target_addr, &addr_send);

    /* When originating a RREQ, add it to our RREQ table/update its predecessor */
    if (wt->type == RFC5444_MSGTYPE_RREQ
        && netaddr_cmp(&wt->packet_data.origNode.addr, &na_local) == 0) {
        DEBUG("originating RREQ with SeqNum %d towards %s via %s; updating RREQ table...\n",
            wt->packet_data.origNode.seqnum,
            netaddr_to_string(&nbuf, &wt->packet_data.targNode.addr),
            ng_ipv6_addr_to_str(addr_str, &addr_send, IPV6_MAX_ADDR_STR_LEN));
        rreqtable_is_redundant(&wt->packet_data);
    }

    send(addr_send, (uint16_t) MANET_PORT, buffer, length);
}

/* Print the json representation of a sent packet to stdout for debugging */
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

/* free the matryoshka doll of cobbled-together structs that the sender_thread receives */
static void _deep_free_msg_container(struct msg_container *mc)
{
    int type = mc->type;
    if ((type == RFC5444_MSGTYPE_RREQ) || (type == RFC5444_MSGTYPE_RREP)) {
        struct rreq_rrep_data *rreq_rrep_data = (struct rreq_rrep_data *) mc->data;
        free(rreq_rrep_data->packet_data);
        if (netaddr_cmp(rreq_rrep_data->next_hop, &na_mcast) != 0) {
            free(rreq_rrep_data->next_hop);
        }
    }
    else if (type == RFC5444_MSGTYPE_RERR) {
        struct rerr_data *rerr_data = (struct rerr_data *) mc->data;
        if (netaddr_cmp(rerr_data->next_hop, &na_mcast) != 0) {
            free(rerr_data->next_hop);
        }
    }
    free(mc->data);
    free(mc);
}

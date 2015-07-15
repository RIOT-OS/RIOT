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
 */

#include "aodv.h"
#include "aodvv2/aodvv2.h"
#include "aodv_debug.h"
#include "ng_fib.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define UDP_BUFFER_SIZE     (128) /** with respect to IEEE 802.15.4's MTU */
#define RCV_MSG_Q_SIZE      (32)  /* TODO: check if smaller values work, too */

static void _init_addresses(void);
static void _init_sock_snd(void);
static void *_aodv_receiver_thread(void *arg);
static void *_aodv_sender_thread(void *arg);
static void *fib_signal_handler_thread(void *arg);

static void _deep_free_msg_container(struct msg_container *msg_container);
static void _write_packet(struct rfc5444_writer *wr __attribute__ ((unused)),
                          struct rfc5444_writer_target *iface __attribute__((unused)),
                          void *buffer, size_t length);

#if ENABLE_DEBUG
char addr_str[IPV6_MAX_ADDR_STR_LEN];
static struct netaddr_str nbuf;
#endif

static char aodv_rcv_stack_buf[THREAD_STACKSIZE_MAIN];
static char aodv_snd_stack_buf[THREAD_STACKSIZE_MAIN];
static char aodv_fib_stack_buf[THREAD_STACKSIZE_MAIN];

static aodvv2_metric_t _metric_type;
static int sender_thread;
static int _sock_snd;
static struct autobuf _hexbuf;
static sockaddr6_t sa_wp;
static ipv6_addr_t _v6_addr_local, _v6_addr_mcast, _v6_addr_loopback;
static struct netaddr na_local; /* the same as _v6_addr_local, but to save us
                                 * constant calls to ipv6_addr_t_to_netaddr()... */
static struct writer_target *wt;
static mutex_t rreq_mutex;
static mutex_t rrep_mutex;
static mutex_t rerr_mutex;

struct netaddr na_mcast;
kernel_pid_t aodvv2_if_id;
ipv6_addr_t aodvv2_prefix;
int aodvv2_prefix_len;

void aodv_init(void)
{
    AODV_DEBUG("%s()\n", __func__);

    /* init this thread's IPC msg queue (TODO: do I need to do this?) */
    msg_t msgq[RCV_MSG_Q_SIZE];
    msg_init_queue(msgq, sizeof msgq);

    /* TODO: set & handle prefix and prefix_len properly (consider AODVV2_RIOT_PREFIXLEN!) */
    aodvv2_prefix_len = 0;
    /* TODO: set if_id properly (as param of aodv_init) */
    aodvv2_if_id = 0;
    net_if_set_src_address_mode(aodvv2_if_id, NET_IF_TRANS_ADDR_M_SHORT);

    mutex_init(&rreq_mutex);
    mutex_init(&rrep_mutex);
    mutex_init(&rerr_mutex);

    aodv_set_metric_type(AODVV2_DEFAULT_METRIC_TYPE);
    _init_addresses();
    _init_sock_snd();

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
    AODV_DEBUG("listening on port %d\n", HTONS(MANET_PORT));
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
    ipv6_addr_t dest;
    struct netaddr na_dest;

    fib_register_rp((uint8_t*) &aodvv2_prefix, aodvv2_prefix_len);

    while (true) {
        AODV_DEBUG("%s()\n", __func__);
        msg_t msg;
        msg_receive(&msg);

        if (msg.type == FIB_MSG_RP_SIGNAL) {
            rp_address_msg_t* rp_msg = (rp_address_msg_t*)msg.content.ptr;
            if (rp_msg->address_size == sizeof(ipv6_addr_t)) {
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

                DEBUG("\tstarting route discovery towards %s... \n",
                      ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, &dest));
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

    AODV_DEBUG("%s()\n", __func__);

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

    AODV_DEBUG("%s()\n", __func__);

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

    AODV_DEBUG("%s()\n", __func__);

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
    /* init multicast address: set to to a link-local all nodes multicast address */
    ipv6_addr_set_all_nodes_addr(&_v6_addr_mcast);
    AODV_DEBUG("my multicast address is: %s\n",
          ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, &_v6_addr_mcast));

    /* get best IP for sending */
    ipv6_net_if_get_best_src_addr(&_v6_addr_local, &_v6_addr_mcast);
    AODV_DEBUG("my src address is:       %s\n",
          ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, &_v6_addr_local));

    /* store src & multicast address as netaddr as well for easy interaction
     * with oonf based stuff */
    ipv6_addr_t_to_netaddr(&_v6_addr_local, &na_local);
    ipv6_addr_t_to_netaddr(&_v6_addr_mcast, &na_mcast);
    ipv6_addr_set_loopback_addr(&_v6_addr_loopback);

    /* init sockaddr that write_packet will use to send data */
    sa_wp.sin6_family = AF_INET6;
    sa_wp.sin6_port = HTONS(MANET_PORT);
}

/* init socket communication for sender */
static void _init_sock_snd(void)
{
    _sock_snd = socket_base_socket(PF_INET6, SOCK_DGRAM, IPPROTO_UDP);

    if (-1 == _sock_snd) {
        AODV_DEBUG("Error Creating Socket!\n");
    }
}

/* Build RREQs, RREPs and RERRs from the information contained in the thread's
 * message queue and send them */
static void *_aodv_sender_thread(void *arg)
{
    (void) arg;

    msg_t msgq[RCV_MSG_Q_SIZE];
    msg_init_queue(msgq, RCV_MSG_Q_SIZE);
    AODV_DEBUG("_aodv_sender_thread initialized.\n");

    while (true) {
        AODV_DEBUG("%s()\n", __func__);
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
            DEBUG("ERROR: Couldn't identify Message\n");
        }
        _deep_free_msg_container(mc);
    }

    return NULL;
}

/* receive RREQs, RREPs and RERRs and handle them */
static void *_aodv_receiver_thread(void *arg)
{
    (void) arg;

    AODV_DEBUG("%s()\n", __func__);
    uint32_t fromlen;
    char buf_rcv[UDP_BUFFER_SIZE];
    msg_t msg_q[RCV_MSG_Q_SIZE];

    msg_init_queue(msg_q, RCV_MSG_Q_SIZE);

    sockaddr6_t sa_rcv = { .sin6_family = AF_INET6,
                           .sin6_port = HTONS(MANET_PORT)
                         };

    int sock_rcv = socket_base_socket(PF_INET6, SOCK_DGRAM, IPPROTO_UDP);

    if (-1 == socket_base_bind(sock_rcv, &sa_rcv, sizeof(sa_rcv))) {
        DEBUG("Error: bind to receive socket failed!\n");
        socket_base_close(sock_rcv);
        return NULL;
    }

    AODV_DEBUG("ready to receive data\n");
    while (true) {
        int32_t rcv_size = socket_base_recvfrom(sock_rcv, (void *)buf_rcv, UDP_BUFFER_SIZE, 0,
                                        &sa_rcv, &fromlen);

        if (rcv_size < 0) {
            AODV_DEBUG("ERROR receiving data!\n");
        }

        AODV_DEBUG("_aodv_receiver_thread() %s:",
              ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, &_v6_addr_local));
        DEBUG(" UDP packet received from %s\n",
              ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, &sa_rcv.sin6_addr));

        struct netaddr _sender;
        ipv6_addr_t_to_netaddr(&sa_rcv.sin6_addr, &_sender);

        /* We sometimes get passed our own packets. Drop them. */
        if (netaddr_cmp(&_sender, &na_local) == 0) {
            AODV_DEBUG("received our own packet, dropping it.\n");
        }
        else {
            aodv_packet_reader_handle_packet((void *) buf_rcv, rcv_size, &_sender);
        }
    }

    socket_base_close(sock_rcv);

    return NULL;
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
    /* generate hexdump and human readable representation of packet
     * and print to console */
    abuf_hexdump(&_hexbuf, "\t", buffer, length);
    rfc5444_print_direct(&_hexbuf, buffer, length);
    DEBUG("%s", abuf_getptr(&_hexbuf));
    abuf_clear(&_hexbuf);

    /* fetch the address the packet is supposed to be sent to (i.e. to a
     * specific node or the multicast address) from the writer_target struct
     * iface* is stored in. This is a bit hacky, but it does the trick. */
    wt = container_of(iface, struct writer_target, interface);
    netaddr_to_ipv6_addr_t(&wt->target_addr, &sa_wp.sin6_addr);

    /* When originating a RREQ, add it to our RREQ table/update its predecessor */
    if (wt->type == RFC5444_MSGTYPE_RREQ
        && netaddr_cmp(&wt->packet_data.origNode.addr, &na_local) == 0) {
        AODV_DEBUG("originating RREQ with SeqNum %d towards %s via %s; updating RREQ table...\n",
              wt->packet_data.origNode.seqnum,
              netaddr_to_string(&nbuf, &wt->packet_data.targNode.addr),
              ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, &sa_wp.sin6_addr));
        rreqtable_is_redundant(&wt->packet_data);
    }

    int bytes_sent = socket_base_sendto(_sock_snd, buffer, length,
                                        0, &sa_wp, sizeof sa_wp);

    (void) bytes_sent;
    AODV_DEBUG("%d bytes sent.\n", bytes_sent);
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

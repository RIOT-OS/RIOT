/*
 * Copyright (C) 2014 Martin Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @addtogroup  net_ipv6
 * @{
 *
 * @file        ipv6.c
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#include <stdbool.h>

#include "byteorder.h"
#include "kernel.h"
#include "kernel_types.h"
#include "pktqueue.h"
#include "net_help.h"
#include "vtimer.h"

#include "ipv6.h"
#include "ipv6/addr.h"
#include "ipv6/if.h"

#define ENABLE_DEBUG    (0)
#if ENABLE_DEBUG
#define DEBUG_ENABLED
#ifndef IPV6_MAX_ADDR_STR_LEN
#define IPV6_MAX_ADDR_STR_LEN   (40)

char addr_str[IPV6_MAX_ADDR_STR_LEN];
#endif
#include "debug.h"

#define _IPV6_STACKSIZE         (KERNEL_CONF_STACKSIZE_DEFAULT)
#define _IPV6_REGISTRY_SIZE         (4)
#define _IPV6_MSG_QUEUE_SIZE        (8)
#define _IPV6_PKTQUEUE_SIZE         (8)

#define _IPV6_MSG_HANDLE_PKTQUEUE       (0x020a)

#define _IPV6_PKTQUEUE_PRIORITY_HIGH    (0)
#define _IPV6_PKTQUEUE_PRIORITY_NORMAL  (1)
#define _IPV6_PKTQUEUE_PRIORITY_PENDING (2)
#define _IPV6_PKTQUEUE_MAX_TRIES        (4)
#define _IPV6_PKTQUEUE_DEFAULT_INTERVAL (500)

typedef struct {
    uint16_t next_header;   /* 16-bit to include ANY */
    kernel_pid_t pid;
} _registry_t;  /* type for checksum receiver registry */

typedef struct {
    netdev_hlist_t *next_headers;
    void *payload;
    size_t payload_len;
    ipv6_addr_t *next_hop;
    uint8_t tries;
} _ipv6_pkt_t;  /* type to store ipv6 packet in queue */

kernel_pid_t ipv6_pid = KERNEL_PID_UNDEF;

static _registry_t _registry[_IPV6_REGISTRY_SIZE];
static char _ipv6_stack[_IPV6_STACKSIZE];
static uint8_t _ipv6_default_hop_limit = IPV6_MULTIHOP_HOP_LIMIT;
static pktqueue_t pktqueue = PKTQUEUE_INIT;
static pktqueue_node_t pktqueue_nodes[_IPV6_PKTQUEUE_SIZE];
static msg_t handle_pktqueue_signal = { 0, _IPV6_MSG_HANDLE_PKTQUEUE, 0 };
static vtimer_t pktqueue_timer;

_ipv6_pkt_t pktqueue_nodes_data[_IPV6_PKTQUEUE_SIZE];

#ifdef MODULE_IPV6_ROUTER

static uint8_t _is_router = false;
static ipv6_routing_provider_t routing_provider = NULL;

#endif /* MODULE_IPV6_ROUTER */

static void *_control(void *args);  /* IPv6 control thread */

static int _ipv6_init(bool as_router)
{
    for (int if_id = 0; if_id < IPV6_IF_NUM; if_id++) {
        ipv6_addr_t tmp = IPV6_ADDR_ALL_NODES_LINK_LOCAL;
        int res;

        if ((res = ipv6_if_add_addr(if_id, &tmp, false)) < 0) {
            DEBUG("Error on setting all nodes multicast address to interface %d: "
                  "error code %d\n", if_id, res);
            return res;
        }

        DEBUG("Add all nodes multicast address to interface %d: %s\n",
              if_id, inet_pton(AF_INET6, tmp, addr_str, IPV6_MAX_ADDR_STR_LEN));

#ifdef MODULE_IPV6_ROUTER

        if (as_router) {
            ipv6_addr_set_all_routers_multicast(&tmp, IPV6_ADDR_MCAST_SCP_LINK_LOCAL);

            if ((res = ipv6_if_add_addr(if_id, &tmp, false)) < 0) {
                DEBUG("Error on setting all routers multicast address to "
                      "interface %d: error code %d\n", if_id, res);
                return res;
            }

            DEBUG("Add all routers multicast address to interface %d: %s\n",
                  if_id, inet_pton(AF_INET6, tmp, addr_str, IPV6_MAX_ADDR_STR_LEN));
        }

#endif /* MODULE_IPV6_ROUTER */

    }

    if (ipv6_pid != KERNEL_PID_UNDEF && (ipv6_pid = thread_create(_ipv6_stack,
                                         _IPV6_STACKSIZE, PRIORITY_MAIN - 1,
                                         CREATE_STACKTEST, _control, NULL,
                                         "ipv6")) < 0) {
        return -ECANCELED;
    }

    return 0;
}

int ipv6_init(void)
{
    return _ipv6_init(false);
}

#ifdef MODULE_IPV6_ROUTER

int ipv6_init_as_router(void)
{
    _is_router = true;
    return _ipv6_init(true);
}

bool ipv6_is_router(void)
{
    return _is_router;
}

int ipv6_register_routing_provider(ipv6_routing_provider_t rp)
{
    if (routing_provider != NULL) {
        return -ENOBUFS;
    }

    rp = routing_provider;
    return 0;
}

#endif /* MODULE_IPV6_ROUTER */

uint16_t ipv6_pseudo_hdr_csum(const ipv6_hdr_t *ipv6_hdr, uint32_t ul_packet_len,
                              uint8_t next_header, uint16_t sum)
{
    network_uint32_t length, nh;
    length = byteorder_htonl(ul_packet_len);
    nh.u32 = 0;
    nh.u8[3] = next_header;

    sum = net_help_csum(sum, (uint8_t *)(&ipv6_hdr->srcaddr),
                        2 * sizeof(ipv6_addr_t));
    sum = net_help_csum(sum, (uint8_t *)(&length), sizeof(length));
    sum = net_help_csum(sum, (uint8_t *)(&nh), sizeof(nh));

    return sum;
}

static pktqueue_node_t *_init_queue_node(const ipv6_hdr_t *ipv6_hdr,
        netdev_hlist_t *next_header,
        void *payload, size_t payload_len,
        ipv6_addr_t *next_hop, uint32_t priority)
{
    int i;

    for (i = 0; i < _IPV6_PKTQUEUE_SIZE; i++) {
        netdev_hlist_t *ipv6_hdr_node;

        if (pktqueue_nodes[i].data != NULL) {
            continue;
        }

        ipv6_hdr_node = pktbuf_alloc(sizeof(netdev_hlist_t));

        ipv6_hdr_node->protocol = NETDEV_PROTO_IPV6;
        ipv6_hdr_node->header = ipv6_hdr;
        ipv6_hdr_node->header_len = sizeof(ipv6_hdr_t);

        netdev_hlist_add(&next_headers, ipv6_hdr_node);

        pktqueue_nodes_data[i].next_headers = next_headers;
        pktqueue_nodes_data[i].payload = payload;
        pktqueue_nodes_data[i].payload_len = payload_len;
        pktqueue_nodes_data[i].next_hop = next_hop;
        pktqueue_nodes_data[i].tries = tries;

        pktqueue_nodes[i].priority = priority;
        pktqueue_nodes[i].data = &(pktqueue_nodes_data[i]);

        return &(pktqueue_nodes[i]);
    }

    return NULL;
}

static int _queue_packet(const ipv6_hdr_t *ipv6_hdr, netdev_hlist_t *next_headers,
                         void *payload, size_t payload_len,
                         ipv6_addr_t *next_hop, uint32_t priority)
{
    pktqueue_node_t *node;

    if (thread_getpid() != ipv6_pid) {
        return -EACCES;
    }

    node = _init_queue_node(ipv6_hdr, next_headers, payload, payload_len,
                            next_hop, priority);

    if (node == NULL) {
        return -ENOBUFS;
    }

    pktqueue_add(&pktqueue, node);

    msg_send_to_self(&_handle_pktqueue_signal);

    return netdev_get_hlist_len(next_headers) + payload_len;
}

int ipv6_send_packet_over_next_hop(const ipv6_hdr_t *ipv6_hdr,
                                   netdev_hlist_t *next_headers,
                                   void *payload, size_t payload_len,
                                   ipv6_addr_t *next_hop)
{
    return _queue_packet(ipv6_hdr, next_headers, payload, payload_len,
                         next_hop, _IPV6_PKTQUEUE_PRIORITY_NORMAL);
}

int ipv6_send_packet_high_priority_over_next_hop(const ipv6_hdr_t *ipv6_hdr,
        netdev_hlist_t *next_headers,
        void *payload, size_t payload_len,
        ipv6_addr_t *next_hop)
{
    return _queue_packet(ipv6_hdr, next_headers, payload, payload_len,
                         next_hop, _IPV6_PKTQUEUE_PRIORITY_HIGH);
}

static void _transfer_snd(_ipv6_pkt_t *ipv6_pkt)
{

}

static void _schedule_next_send(void)
{
    timex_t interval = { 0, _IPV6_PKTQUEUE_DEFAULT_INTERVAL };

    vtimer_set_msg(&pktqueue_timer, interval, ipv6_pid, &handle_pktqueue_signal)
}

static void _handle_pktqueue(void)
{
    pktqueue_node_t *next = pktqueue_remove_head(&pktqueue);
    _ipv6_pkt_t *ipv6_pkt = (_ipv6_pkt_t)next->data;

    _transfer_snd(ipv6_pkt);

    if (pktqueue_get_head(&pktqueue) != NULL) {
        _schedule_next_send();
    }
}

static int _get_option(netapi_conf_t *conf)
{
    switch ((ipv6_conf_t)conf->param) {
        case IPV6_CONF_PROTO:
            if (conf->data_len < sizeof(netdev_proto_t)) {
                return -EOVERFLOW;
            }
            else {
                netdev_proto_t *proto = conf->data;
                *proto = NETDEV_PROTO_IPV6;
                conf->data_len = sizeof(netdev_proto_t);
                return conf->data_len;
            }

        case IPV6_CONF_REGISTRY:
            if (conf->data_len >= sizeof(kernel_pid_t)) {
                kernel_pid_t *data = conf->data;
                int size = 0;

                for (size = 0; size < _IPV6_REGISTRY_SIZE; size++) {
                    if (i < (conf->data_len / sizeof(kernel_pid_t))) {
                        data[i] = _registry[i].pid;
                    }
                    else {
                        break;
                    }
                }

                conf->data_len = sizeof(kernel_pid_t) * size;

                return conf->data_len;
            }
            else {
                conf->data_len = 0;
                return 0;
            }

            break;

        case IPV6_CONF_DEFAULT_HOP_LIMIT:
            if (conf->data_len == 0) {
                return -EOVERFLOW;
            }
            else {
                uint8_t *data = conf->data;

                *data = _ipv6_default_hop_limit;
                conf->data_len = sizeof(uint8_t);
                return conf->data_len;
            }

        default:
            return -ENOTSUP;
    }
}

static int _set_option(netapi_conf_t *conf)
{
    switch ((ipv6_conf_t)conf->param) {
        case IPV6_CONF_DEFAULT_HOP_LIMIT:
            if (conf->data_len > sizeof(uint8_t)) {
                return -EOVERFLOW;
            }
            else if (conf->data_len == 0) {
                return -EINVAL;
            }
            else {
                uint8_t *data = conf->data;

                _ipv6_default_hop_limit = *data;
                conf->data_len = sizeof(uint8_t);
                return conf->data_len;
            }

        default:
            return -ENOTSUP;
    }
}

static void *_control(void *args)
{
    msg_t msg_q[_IPV6_MSG_QUEUE_SIZE];

    for (int i = 0; i < _IPV6_REGISTRY_SIZE; i++) {
        _registry[i].next_header = 0;
        _registry[i].pid = KERNEL_PID_UNDEF;
    }

    msg_init_queue(msg_q, _IPV6_MSG_QUEUE_SIZE);

    while (1) {
        msg_t msg_cmd, msg_ack;
        netapi_cmd_t *cmd;
        netapi_ack_t *ack;
        uint8_t replied = 0;

        msg_receive(&msg_cmd);

        switch (msg_cmd.type) {
            case MSG_TIMER:
                msg_send_to_self((msg_t *)msg.content.ptr);

            case _IPV6_MSG_HANDLE_PKTQUEUE:
                _handle_pktqueue();

            case NETAPI_MSG_TYPE:
                msg_ack.type = NETAPI_MSG_TYPE;

                cmd = (netapi_cmd_t *)msg_cmd.content.ptr;

                ack = cmd->ack;
                ack->type = NETAPI_CMD_ACK;
                ack->orig = cmd->type;

                msg_ack.content.ptr = (char *)ack;

                switch (cmd->type) {
                    case NETAPI_CMD_RCV:
                        break;

                    case NETAPI_CMD_SND:
                        break;

                    case NETAPI_CMD_GET:
                        ack->result = _get_option((netapi_conf_t *)cmd);

                        break;

                    case NETAPI_CMD_SET:
                        ack->result = _set_option((netapi_conf_t *)cmd);

                        break;

                    case NETAPI_CMD_REG:
                        DEBUG("Received registration command from %d\n" msg_cmd.sender_pid);

                        ack->result = -ENOBUFS;

                        for (unsigned int i = 0; i < _IPV6_REGISTRY_SIZE; i++) {
                            if (registry[i].pid = KERNEL_PID_UNDEF) {
                                registry[i].next_header = ((netapi_reg_t *)cmd)->demux_ctx;
                                registry[i].pid = ((netapi_reg_t *)cmd)->reg_pid;
                                ack->result = NETAPI_STATUS_OK;
                            }
                        }

                        break;

                    case NETAPI_CMD_UNREG:
                        DEBUG("Received unregistration command from %d\n", msg_cmd.sender_pid);

                        ack->result = NETAPI_STATUS_OK;

                        for (unsigned int i = 0; i < _REGISTRY_SIZE; i++) {
                            if (registry[i] == ((netapi_reg_t *)cmd)->reg_pid) {
                                registry[i].pid = KERNEL_PID_UNDEF;

                                break;
                            }
                        }

                        break;

                    default:
                        DEBUG("Received unknown command from %d\n", msg_cmd.sender_pid);

                        ack->result = -ENOTSUP;

                        break;
                }

                if (!replied) {
                    msg_reply(&msg_cmd, &msg_ack);
                }
                else {
                    replied = 0;
                }

                break;

            default:
                msg_ack.type = 0;
                msg_reply(&msg_cmd, &msg_ack);

                break;
        }

    }

    return NULL;
}

/**
 * @}
 */

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
#include "net_help.h"

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

#define _IPV6_STACKSIZE     (KERNEL_CONF_STACKSIZE_DEFAULT)
#define _IPV6_REGISTRY_SIZE (4)

typedef struct {
    uint16_t next_header;   /* 16-bit to include ANY */
    kernel_pid_t pid;
} _registry_t;  /* type for checksum receiver registry */

kernel_pid_t ipv6_pid = KERNEL_PID_UNDEF;

static _registry_t _rcv_registry[_IPV6_REGISTRY_SIZE];
static char _ipv6_stack[_IPV6_STACKSIZE];

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

static void *_control(void *args)
{
    /* TODO: send router solicitation */

    return NULL;
}

/**
 * @}
 */

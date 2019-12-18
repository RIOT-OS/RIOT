/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 */
#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>

#include "byteorder.h"
#include "cpu_conf.h"
#include "kernel_types.h"
#include "net/gnrc/icmpv4.h"
#include "net/protnum.h"
#include "thread.h"
#include "utlist.h"

#include "net/arp.h"
#include "net/netopt.h"
#include "net/gnrc.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/netif/internal.h"

#include "net/gnrc/ipv4.h"
#include "net/gnrc/ipv4/arp/arp.h"

#define ENABLE_DEBUG    (1)
#include "debug.h"
#if ENABLE_DEBUG == 1
char ipv4_addr1[IPV4_ADDR_MAX_STR_LEN];
char ipv4_addr2[IPV4_ADDR_MAX_STR_LEN];
char ipv4_addr3[IPV4_ADDR_MAX_STR_LEN];
#endif

gnrc_ipv4_route_t gnrc_ipv4_route[GNRC_IPV4_ROUTE_TABLE_SIZE];

static bool gnrc_ipv4_route_is_empty(gnrc_ipv4_route_t *route)
{
  return ipv4_addr_is_unspecified(&route->gateway);
}

static void gnrc_ipv4_route_set_empty(gnrc_ipv4_route_t *route)
{
  ipv4_addr_set_unspecified(&route->gateway);
}

static bool gnrc_ipv4_route_equals(gnrc_ipv4_route_t *route1, gnrc_ipv4_route_t *route2)
{
  return memcmp(route1, route2, sizeof(gnrc_ipv4_route_t)) == 0;
}

void gnrc_ipv4_route_delete_all(void)
{
  for (size_t i=0; i<GNRC_IPV4_ROUTE_TABLE_SIZE; i++) {
    gnrc_ipv4_route_set_empty(&gnrc_ipv4_route[i]);
  }
}

bool gnrc_ipv4_route_add(gnrc_ipv4_route_t *route)
{
  DEBUG("gnrc_ipv4_route: add destination=%s mask=%s gateway=%s\n",
    ipv4_addr_to_str(ipv4_addr1, &route->destination, IPV4_ADDR_MAX_STR_LEN),
    ipv4_addr_to_str(ipv4_addr2, &route->mask, IPV4_ADDR_MAX_STR_LEN),
    ipv4_addr_to_str(ipv4_addr3, &route->gateway, IPV4_ADDR_MAX_STR_LEN)
  );

  /* Search the available space in the table */
  for (size_t i=0; i<GNRC_IPV4_ROUTE_TABLE_SIZE; i++) {
    if (gnrc_ipv4_route_is_empty(&gnrc_ipv4_route[i])) {
      memcpy(&gnrc_ipv4_route[i], route, sizeof(gnrc_ipv4_route_t));
      DEBUG("gnrc_ipv4_route : gnrc_ipv4_route_add sucess\n");
      return true;
    }
  }

  /* Route not found */
  return false;
}

bool gnrc_ipv4_route_delete(gnrc_ipv4_route_t *route)
{
  DEBUG("gnrc_ipv4_route: delete destination=%s mask=%s gateway=%s\n",
    ipv4_addr_to_str(ipv4_addr1, &route->destination, IPV4_ADDR_MAX_STR_LEN),
    ipv4_addr_to_str(ipv4_addr2, &route->mask, IPV4_ADDR_MAX_STR_LEN),
    ipv4_addr_to_str(ipv4_addr3, &route->gateway, IPV4_ADDR_MAX_STR_LEN)
  );

  /* Search the requested route in the table */
  for (size_t i=0; i<GNRC_IPV4_ROUTE_TABLE_SIZE; i++) {
    if (gnrc_ipv4_route_equals(route, &gnrc_ipv4_route[i])) {
      gnrc_ipv4_route_set_empty(&gnrc_ipv4_route[i]);
      return true;
    }
  }

  /* Route not found */
  return false;
}

/*
 *  Check if the destination is in a IPv4 network attached to this iface
 */
static bool gnrc_ipv4_route_same_network(const ipv4_addr_t *dst, gnrc_netif_t *netif, ipv4_addr_t *src)
{
  int res;
  ipv4_addr_t addrs[GNRC_NETIF_IPV4_ADDRS_NUMOF];
  res = gnrc_netapi_get(netif->pid, NETOPT_IPV4_ADDR, 0, addrs, sizeof(addrs));
  if (res <= 0) {
    return false;
  }

  ipv4_addr_t masks[GNRC_NETIF_IPV4_ADDRS_NUMOF];
  res = gnrc_netapi_get(netif->pid, NETOPT_IPV4_MASK, 0, masks, sizeof(masks));
  if (res <= 0) {
    return false;
  }

  for (size_t i=0; i < (size_t)(res / sizeof(ipv4_addr_t)); i++) {
    if (ipv4_addr_is_unspecified(&addrs[i])) {
      continue;
    }

    if (ipv4_addr_match_prefix(&addrs[i], &masks[i], dst)) {
      *src = addrs[i];
      return true;
    }
  }

  return false;
}

void gnrc_ipv4_route_get_next_hop_l2addr(const ipv4_addr_t *dst, gnrc_netif_t **netif, gnrc_pktsnip_t *pkt, ipv4_addr_t *hop)
{
  ipv4_hdr_t *hdr = pkt->data;

  ipv4_addr_t src;
  ipv4_addr_set_unspecified(&src);

  /* Search if the destination can be contacted directly */
  DEBUG("gnrc_ipv4_route: Search in the known netowrks\n");
  if (*netif != NULL) {
    if (gnrc_ipv4_route_same_network(dst, *netif, &src)) {
      *hop = *dst;
      memcpy(&hdr->src, &src, sizeof(ipv4_addr_t));
      return;
    }
  } else {
    gnrc_netif_t *it = NULL;
    while ((it = gnrc_netif_iter(it))) {
      if (gnrc_ipv4_route_same_network(dst, it, &src)) {
        *netif = it;
        *hop = *dst;
        memcpy(&hdr->src, &src, sizeof(ipv4_addr_t));
        return;
      }
    }
  }

  /* Search in the routing table */
  DEBUG("gnrc_ipv4_route: Search in the routing table\n");
  for (size_t i=0; i<GNRC_IPV4_ROUTE_TABLE_SIZE; i++) {
    if (gnrc_ipv4_route_is_empty(&gnrc_ipv4_route[i])) {
      continue;
    }

    DEBUG("gnrc_ipv4_route: checking route destination=%s mask=%s gateway=%s\n",
      ipv4_addr_to_str(ipv4_addr1, &gnrc_ipv4_route[i].destination, IPV4_ADDR_MAX_STR_LEN),
      ipv4_addr_to_str(ipv4_addr2, &gnrc_ipv4_route[i].mask, IPV4_ADDR_MAX_STR_LEN),
      ipv4_addr_to_str(ipv4_addr3, &gnrc_ipv4_route[i].gateway, IPV4_ADDR_MAX_STR_LEN)
    );

    if (ipv4_addr_match_prefix(&gnrc_ipv4_route[i].destination, &gnrc_ipv4_route[i].mask, dst)) {
      *netif = gnrc_netif_get_by_pid(gnrc_ipv4_route[i].iface);
      *hop = gnrc_ipv4_route[i].gateway;

      // Find best source addr
      if(gnrc_ipv4_route_same_network(hop, *netif, &src)) {
        memcpy(&hdr->src, &src, sizeof(ipv4_addr_t));
      }
      return;
      
    }
  }

  /* Not found */
  *netif = NULL;
}

/*
 * Copyright (C) 2019 William MARTIN <william.martin@power-lan.com>
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
#include "utlist.h"

#include "net/gnrc.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/netif/internal.h"
#include "net/gnrc/ipv4/arp/arp.h"
#include "net/arp.h"

#define ENABLE_DEBUG    (1)
#include "debug.h"

#if ENABLE_DEBUG
static char _stack[GNRC_IPV4_ARP_STACK_SIZE + THREAD_EXTRA_STACKSIZE_PRINTF];
#else
static char _stack[GNRC_IPV4_ARP_STACK_SIZE];
#endif

static char ipv4_addr[IPV4_ADDR_MAX_STR_LEN];
kernel_pid_t gnrc_ipv4_arp_pid = KERNEL_PID_UNDEF;

#define ARP_TABLE_SIZE  (8)
static arp_t arp_table[ARP_TABLE_SIZE];

void gnrc_ipv4_arp_print_table(void)
{
  for (int i=0; i<ARP_TABLE_SIZE; i++) {
    arp_t *payload = &arp_table[i];
    DEBUG("ipv4_arp: ip=%s mac=%02X:%02X:%02X:%02X:%02X:%02X iface=%d flags=%d\n",
      ipv4_addr_to_str(ipv4_addr, &payload->ipv4, IPV4_ADDR_MAX_STR_LEN),
      payload->mac[0],
      payload->mac[1],
      payload->mac[2],
      payload->mac[3],
      payload->mac[4],
      payload->mac[5],
      payload->iface,
      payload->flags);
  }
}

static bool havePendingRequests(void)
{
  bool ret = false;

  for (int i=0; i<ARP_TABLE_SIZE; i++) {
    if (arp_table[i].flags == 0) {
      continue;
    }

    if (arp_table[i].flags == ARP_FLAG_COMPLETE) {
      continue;
    }

    if (arp_table[i].flags & ARP_FLAG_KNOWN) {
      arp_table[i].retryCount--;
      if (arp_table[i].retryCount == 0) {
        arp_table[i].flags = 0;
        continue;
      }

      ret = true;
    }
  }

  return ret;
}

static void _send_response(arp_payload_t *request, gnrc_netif_t *netif)
{
  // Build ARP response
  arp_payload_t reponse;
  reponse.hw_type = byteorder_htons(1);
  reponse.protocol_type = byteorder_htons(ETHERTYPE_IPV4);
  reponse.hw_size = 6;
  reponse.protocol_size = 4;
  reponse.opcode = byteorder_htons(2);
  memcpy(&reponse.sender_hw_addr, &netif->l2addr, sizeof(reponse.sender_hw_addr));
  memcpy(&reponse.sender_protocol_addr, &request->target_protocol_addr, sizeof(ipv4_addr_t));
  memcpy(&reponse.target_hw_addr, &request->sender_hw_addr, sizeof(reponse.target_hw_addr));
  memcpy(&reponse.target_protocol_addr, &request->sender_protocol_addr, sizeof(ipv4_addr_t));

  gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(NULL, &reponse, sizeof(arp_payload_t), GNRC_NETTYPE_ARP);
  assert(pkt != NULL);

  // L2 headers
  gnrc_pktsnip_t *netif_hdr = gnrc_netif_hdr_build(NULL, 0, (uint8_t *) &request->sender_hw_addr, 6);
  assert(netif_hdr != NULL);

  gnrc_netif_hdr_t *hdr = netif_hdr->data;
  hdr->if_pid = netif->pid;
  LL_PREPEND(pkt, netif_hdr);

  // Publish on network interface
  if (gnrc_netapi_send(netif->pid, pkt) < 1) {
      DEBUG("ipv4_arp: unable to send packet\n");
      gnrc_pktbuf_release(pkt);
  }
}

static void _send_request(ipv4_addr_t *ipv4, gnrc_netif_t *netif)
{
  // Find the best source IP addr (not implemented)
  ipv4_addr_t ipv4src = ipv4_addr_unspecified;
  for (int i=0; i<GNRC_NETIF_IPV4_ADDRS_NUMOF; i++) {
    if (netif->ipv4.addrs_flags[i] & 0x01) {
      ipv4src = netif->ipv4.addrs[i];
    }
  }

  if (ipv4_addr_equal(&ipv4src, &ipv4_addr_unspecified)) {
    DEBUG("ipv4_arp: no IPv4 on interface ? abord...\n");
    return;
  }

  ipv4_addr_t ipv4_addrs[GNRC_NETIF_IPV4_ADDRS_NUMOF];
  int res = gnrc_netapi_get(netif->pid, NETOPT_IPV4_ADDR, 0, ipv4_addrs, sizeof(ipv4_addrs));
  if (res < 0) {
    DEBUG("ipv4_arp: Failed to list IPs on interface %d\n", netif->pid);
    return;
  }
  for (unsigned i = 0; i < (unsigned)(res / sizeof(ipv4_addr_t)); i++) {
    ipv4src = ipv4_addrs[i];
    break;
  }

  // Build ARP response
  arp_payload_t request;
  request.hw_type = byteorder_htons(1);
  request.protocol_type = byteorder_htons(ETHERTYPE_IPV4);
  request.hw_size = 6;
  request.protocol_size = 4;
  request.opcode = byteorder_htons(1);
  memcpy(&request.sender_hw_addr, &netif->l2addr, sizeof(request.sender_hw_addr));
  memcpy(&request.sender_protocol_addr, &ipv4src, sizeof(ipv4_addr_t));
  memset(&request.target_hw_addr, 0, sizeof(request.target_hw_addr));
  memcpy(&request.target_protocol_addr, ipv4, sizeof(ipv4_addr_t));

  gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(NULL, &request, sizeof(arp_payload_t), GNRC_NETTYPE_ARP);
  assert(pkt != NULL);

  // L2 headers
  uint8_t broadcast[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  gnrc_pktsnip_t *netif_hdr = gnrc_netif_hdr_build(NULL, 0, broadcast, sizeof(broadcast));
  assert(netif_hdr != NULL);

  gnrc_netif_hdr_t *hdr = netif_hdr->data;
  hdr->if_pid = netif->pid;
  LL_PREPEND(pkt, netif_hdr);

  // Publish on network interface
  if (gnrc_netapi_send(netif->pid, pkt) < 1) {
      DEBUG("ipv4_arp: unable to send packet\n");
      gnrc_pktbuf_release(pkt);
  }
}

static void advertisePendingRequests(void)
{
  for (int i=0; i<ARP_TABLE_SIZE; i++) {
    // Empty entries
    if (arp_table[i].flags == 0) {
      continue;
    }

    // Complete entry
    if (arp_table[i].flags == ARP_FLAG_COMPLETE) {
      continue;
    }

    // Send ARP
    _send_request(&arp_table[i].ipv4, gnrc_netif_get_by_pid(arp_table[i].iface));
  }
}

static void _receive_response(msg_t *msg, arp_payload_t *payload)
{
    DEBUG("ipv4_arp: _receive_response msg->sender_pid:%d\n", msg->sender_pid);
    DEBUG("ipv4_arp: _receive_response payload->sender_protocol_addr:%s\n", ipv4_addr_to_str(ipv4_addr, &payload->sender_protocol_addr, IPV4_ADDR_MAX_STR_LEN));

  for (int i=0; i<ARP_TABLE_SIZE; i++) {
    if (ipv4_addr_equal(&arp_table[i].ipv4, &payload->sender_protocol_addr) && arp_table[i].iface == msg->sender_pid) {
      memcpy(&arp_table[i].mac, payload->sender_hw_addr, ARP_MAC_SIZE);
      arp_table[i].flags = ARP_FLAG_COMPLETE;
      DEBUG("ipv4_arp: %s is now %02X:%02X:%02X:%02X:%02X:%02X\n",
        ipv4_addr_to_str(ipv4_addr, &payload->sender_protocol_addr, IPV4_ADDR_MAX_STR_LEN),
        payload->sender_hw_addr[0],
        payload->sender_hw_addr[1],
        payload->sender_hw_addr[2],
        payload->sender_hw_addr[3],
        payload->sender_hw_addr[4],
        payload->sender_hw_addr[5]);

      break;
    }
  }
}

static void _receive(msg_t *msg)
{
  assert(msg != NULL);

  // Ensure pkt is ARP
  gnrc_pktsnip_t *pkt = msg->content.ptr;
  if (pkt->type != GNRC_NETTYPE_ARP) {
      DEBUG("ipv4_arp: unexpected packet type\n");
      gnrc_pktbuf_release_error(pkt, EINVAL);
      return;
  }
  arp_payload_t *payload = (arp_payload_t *)pkt->data;

  // TODO: pkt->size is too big ~50 bytes
  if (pkt->size < sizeof(arp_payload_t)) {
    DEBUG("ipv4_arp: wrong packet size %d instead of %d\n", pkt->size, sizeof(arp_payload_t));
    gnrc_pktbuf_release_error(pkt, EINVAL);
    return;
  }

  // Check protocol type
  if (byteorder_ntohs(payload->protocol_type) != ETHERTYPE_IPV4) {
      DEBUG("ipv4_arp: wrong protocol_type\n");
      gnrc_pktbuf_release_error(pkt, EINVAL);
      return;
  }

  // Check hw/protocol length
  if (payload->hw_size != 6 || payload->protocol_size != 4) {
      DEBUG("ipv4_arp: unexpected hw_size or protocol_size\n");
      gnrc_pktbuf_release_error(pkt, EINVAL);
      return;
  }

  if (byteorder_ntohs(payload->opcode) == 2) {
      DEBUG("ipv4_arp: opcode reply\n");
      _receive_response(msg, payload);
      gnrc_pktbuf_release(pkt);
      return;
  }

  // Ensure this is a request
  if (byteorder_ntohs(payload->opcode) != 1) {
      DEBUG("ipv4_arp: not an arp request\n");
      gnrc_pktbuf_release_error(pkt, EINVAL);
      return;
  }

  // TODO: Ensure payload->sender_hw_addr is the same as rcv in L2

  // Get network interface
  gnrc_netif_t *netif = NULL;
  netif = gnrc_netif_get_by_pid(msg->sender_pid);
  assert(netif != NULL);

  // We love debugs
  DEBUG("ipv4_arp: opcode = %d\n", byteorder_ntohs(payload->opcode));
  DEBUG("ipv4_arp: sender_hw_addr = %02X:%02X:%02X:%02X:%02X:%02X\n",
    payload->sender_hw_addr[0],
    payload->sender_hw_addr[1],
    payload->sender_hw_addr[2],
    payload->sender_hw_addr[3],
    payload->sender_hw_addr[4],
    payload->sender_hw_addr[5]);
  DEBUG("ipv4_arp: sender_protocol_addr = %s\n", ipv4_addr_to_str(ipv4_addr, &payload->sender_protocol_addr, IPV4_ADDR_MAX_STR_LEN));
  DEBUG("ipv4_arp: target_hw_addr = %02X:%02X:%02X:%02X:%02X:%02X\n",
    payload->target_hw_addr[0],
    payload->target_hw_addr[1],
    payload->target_hw_addr[2],
    payload->target_hw_addr[3],
    payload->target_hw_addr[4],
    payload->target_hw_addr[5]);
  DEBUG("ipv4_arp: target_protocol_addr = %s\n", ipv4_addr_to_str(ipv4_addr, &payload->target_protocol_addr, IPV4_ADDR_MAX_STR_LEN));

  // List IP
  ipv4_addr_t ipv4_addrs[GNRC_NETIF_IPV4_ADDRS_NUMOF];
  int res = gnrc_netapi_get(netif->pid, NETOPT_IPV4_ADDR, 0, ipv4_addrs, sizeof(ipv4_addrs));
  if (res < 0) {
    DEBUG("ipv4_arp: Failed to list IPs on interface %d\n", netif->pid);
    gnrc_pktbuf_release(pkt);
    return;
  }

  for (unsigned i = 0; i < (unsigned)(res / sizeof(ipv4_addr_t)); i++) {
    if (ipv4_addr_equal(&ipv4_addrs[i], &payload->target_protocol_addr)) {
      DEBUG("ipv4_arp: It's me ! Mario\n");
      _send_response(payload, netif);
      break;
    }
  }

  gnrc_pktbuf_release(pkt);
}

static void _get(msg_t *msg, msg_t *reply)
{
  gnrc_netapi_opt_t *opt = msg->content.ptr;
  arp_netapi_get_t *request = opt->data;

  for (int i=0; i<ARP_TABLE_SIZE; i++) {
    if (ipv4_addr_equal(&arp_table[i].ipv4, &request->ipv4) && arp_table[i].iface == request->iface) {
      if (arp_table[i].flags & ARP_FLAG_COMPLETE) {
        // IP and MAC known
        memcpy(&request->mac, &arp_table[i].mac, ARP_MAC_SIZE);

        DEBUG("ipv4_arp: %s iface=%d known as %02X:%02X:%02X:%02X:%02X:%02X\n",
          ipv4_addr_to_str(ipv4_addr, &request->ipv4, IPV4_ADDR_MAX_STR_LEN),
          request->iface,
          request->mac[0],
          request->mac[1],
          request->mac[2],
          request->mac[3],
          request->mac[4],
          request->mac[5]);

        reply->content.value = 0;
        msg_reply(msg, reply);
        return;
      } else {
        DEBUG("ipv4_arp: %s iface=%d have partial informations\n",
          ipv4_addr_to_str(ipv4_addr, &request->ipv4, IPV4_ADDR_MAX_STR_LEN),
          request->iface);
        // IP known, but not the MAC
        reply->content.value = -EAGAIN;
        msg_reply(msg, reply);
        return;
      }
    }
  }

  // Unknown IP
          DEBUG("ipv4_arp: %s is unknown IP\n",
          ipv4_addr_to_str(ipv4_addr, &request->ipv4, IPV4_ADDR_MAX_STR_LEN));
  for (int i=0; i<ARP_TABLE_SIZE; i++) {
    if (arp_table[i].flags == 0) {
      // Reserve sapce in ARP table
      arp_table[i].ipv4 = request->ipv4;
      arp_table[i].iface = request->iface;
      arp_table[i].flags = ARP_FLAG_KNOWN;
      arp_table[i].retryCount = ARP_RETRY_COUNT;

      DEBUG("ipv4_arp: adding %s for iface %d in table\n",
        ipv4_addr_to_str(ipv4_addr, &arp_table[i].ipv4, IPV4_ADDR_MAX_STR_LEN), arp_table[i].iface);

      // Send ARP
      _send_request(&request->ipv4, gnrc_netif_get_by_pid(request->iface));

      // Response to GET
      reply->content.value = -EAGAIN;
      msg_reply(msg, reply);
      return;
    }
  }

  reply->content.value = -ENOMEM;
  msg_reply(msg, reply);
}

static void *_event_loop(void *args)
{
    msg_t msg, reply, msg_q[GNRC_IPV4_ARP_MSG_QUEUE_SIZE];
    gnrc_netreg_entry_t me_reg = GNRC_NETREG_ENTRY_INIT_PID(GNRC_NETREG_DEMUX_CTX_ALL,
                                                            sched_active_pid);

    (void)args;
    msg_init_queue(msg_q, GNRC_IPV4_ARP_MSG_QUEUE_SIZE);

    /* register interest in all IPv4 packets */
    gnrc_netreg_register(GNRC_NETTYPE_ARP, &me_reg);

    /* preinitialize ACK */
    reply.type = GNRC_NETAPI_MSG_TYPE_ACK;

    /* start event loop */
    while (1) {
        DEBUG("ipv4_arp: waiting for incoming message.\n");
        int timeout = xtimer_msg_receive_timeout(&msg, XTIMER_HZ_BASE);
        if (timeout < 0) {
          if (havePendingRequests()) {
            advertisePendingRequests();
            continue;
          } else {
            msg_receive(&msg);
          }
        }

        switch (msg.type) {
            case GNRC_NETAPI_MSG_TYPE_RCV:
                DEBUG("ipv4_arp: GNRC_NETAPI_MSG_TYPE_RCV received\n");
                _receive(&msg);
                break;

            case GNRC_NETAPI_MSG_TYPE_SND:
                DEBUG("ipv4_arp: GNRC_NETAPI_MSG_TYPE_SND received\n");
                break;

            case GNRC_NETAPI_MSG_TYPE_GET:
                _get(&msg, &reply);
                break;

            case GNRC_NETAPI_MSG_TYPE_SET:
                DEBUG("ipv4: reply to unsupported get/set\n");
                reply.content.value = -ENOTSUP;
                msg_reply(&msg, &reply);
                break;

            default:
                break;
        }
    }

    return NULL;
}

kernel_pid_t gnrc_ipv4_arp_init(void)
{
    // Init ARP table
    for (int i=0; i<ARP_TABLE_SIZE; i++) {
      arp_table[i].flags = 0;
    }

    // Create ARP thread
    if (gnrc_ipv4_arp_pid == KERNEL_PID_UNDEF) {
        gnrc_ipv4_arp_pid = thread_create(_stack, sizeof(_stack), GNRC_IPV4_ARP_PRIO,
                                          THREAD_CREATE_STACKTEST,
                                          _event_loop, NULL, "ipv4_arp");
    }

    return gnrc_ipv4_arp_pid;
}

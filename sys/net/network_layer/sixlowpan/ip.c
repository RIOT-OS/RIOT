/*
 * IPv6 implementation
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup sixlowpan
 * @{
 * @file    sixlowip.c
 * @brief   6lowpan IP layer functions
 * @author  Stephan Zeisberg <zeisberg@mi.fu-berlin.de>
 * @author  Martin Lenders <mlenders@inf.fu-berlin.de>
 * @author  Eric Engel <eric.engel@fu-berlin.de>
 * @author  Oliver Gesch <oliver.gesch@googlemail.com>
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "vtimer.h"
#include "mutex.h"
#include "msg.h"
#include "net_if.h"
#include "sixlowpan/mac.h"

#include "ip.h"
#include "icmp.h"
#include "lowpan.h"

#include "net_help.h"

#define ENABLE_DEBUG    (0)
#if ENABLE_DEBUG
char addr_str[IPV6_MAX_ADDR_STR_LEN];
#endif
#include "debug.h"

#define IP_PKT_RECV_BUF_SIZE        (64)
#define LLHDR_IPV6HDR_LEN           (LL_HDR_LEN + IPV6_HDR_LEN)
#define IPV6_NET_IF_ADDR_BUFFER_LEN (NET_IF_MAX * IPV6_NET_IF_ADDR_LIST_LEN)

uint8_t ip_send_buffer[BUFFER_SIZE];
uint8_t buffer[BUFFER_SIZE];
msg_t ip_msg_queue[IP_PKT_RECV_BUF_SIZE];
ipv6_hdr_t *ipv6_buf;
icmpv6_hdr_t *icmp_buf;
uint8_t *nextheader;

kernel_pid_t udp_packet_handler_pid = KERNEL_PID_UNDEF;
kernel_pid_t tcp_packet_handler_pid = KERNEL_PID_UNDEF;

static volatile kernel_pid_t _rpl_process_pid = KERNEL_PID_UNDEF;
ipv6_addr_t *(*ip_get_next_hop)(ipv6_addr_t *);
uint8_t (*ip_srh_indicator)(void);

static ipv6_net_if_ext_t ipv6_net_if_ext[NET_IF_MAX];
static ipv6_net_if_addr_t ipv6_net_if_addr_buffer[IPV6_NET_IF_ADDR_BUFFER_LEN];
static ipv6_addr_t ipv6_addr_buffer[IPV6_NET_IF_ADDR_BUFFER_LEN];
static uint8_t ipv6_net_if_addr_buffer_count = 0;

static uint8_t default_hop_limit = MULTIHOP_HOPLIMIT;

/* registered upper layer threads */
kernel_pid_t sixlowip_reg[SIXLOWIP_MAX_REGISTERED];

int ipv6_send_packet(ipv6_hdr_t *packet, ipv6_addr_t *next_hop)
{
    uint16_t length = IPV6_HDR_LEN + NTOHS(packet->length);
    ndp_neighbor_cache_t *nce;

    if (next_hop == NULL) {
        DEBUGF("Got a packet to send to %s\n", ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, &packet->destaddr));
        ipv6_net_if_get_best_src_addr(&packet->srcaddr, &packet->destaddr);
    }

    if (!ipv6_addr_is_multicast(&packet->destaddr)
        && ndp_addr_is_on_link(&packet->destaddr)) {
        /* not multicast, on-link */
        nce = ndp_get_ll_address(&packet->destaddr);
        DEBUG("ipv6 send packet: after get ll adr\n");
        if (nce == NULL
            || sixlowpan_lowpan_sendto(nce->if_id, &nce->lladdr,
                                       nce->lladdr_len, (uint8_t *) packet, length) < 0) {
            DEBUG("ipv6 send packet: before lowpan_sendto\n");
            /* XXX: this is wrong, but until ND does work correctly,
             *      this is the only way (aka the old way)*/

            uint16_t raddr = NTOHS(packet->destaddr.uint16[7]);
            sixlowpan_lowpan_sendto(0, &raddr, 2, (uint8_t *) packet, length);

            /* return -1; */
        }
        DEBUG("ipv6 send packet: before return length\n");
        return length;
    }
    else {
        /* see if dest should be routed to a different next hop */
        if (ipv6_addr_is_multicast(&packet->destaddr)) {
            /* if_id will be ignored */
            uint16_t addr = 0xffff;
            return sixlowpan_lowpan_sendto(0, &addr, 2, (uint8_t *) packet,
                                           length);
        }

        if (next_hop == NULL) {
            DEBUG("Trying to find the next hop for %s\n", ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, &packet->destaddr));

            if (ip_get_next_hop == NULL) {
                return -1;
            }

            ipv6_addr_t *dest = ip_get_next_hop(&packet->destaddr);

            if (dest == NULL) {
                return -1;
            }

            nce = ndp_get_ll_address(dest);

            if (nce == NULL
                || sixlowpan_lowpan_sendto(nce->if_id, &nce->lladdr,
                                           nce->lladdr_len, (uint8_t *) packet, length) < 0) {
                /* XXX: this is wrong, but until ND does work correctly,
                 *      this is the only way (aka the old way)*/
                uint16_t raddr = dest->uint16[7];
                sixlowpan_lowpan_sendto(0, &raddr, 2, (uint8_t *) packet,
                                        length);
                /* return -1; */
            }
        }
        else {
            DEBUGF("Set destination based on next-hop: %s\n", ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, next_hop));
            nce = ndp_get_ll_address(next_hop);

            if (nce == NULL
                || sixlowpan_lowpan_sendto(nce->if_id, &nce->lladdr,
                                           nce->lladdr_len, (uint8_t *) packet, length) < 0) {
                /* XXX: this is wrong, but until ND does work correctly,
                 *      this is the only way (aka the old way)*/
                uint16_t raddr = next_hop->uint16[7];
                sixlowpan_lowpan_sendto(0, &raddr, 2, (uint8_t *) packet,
                                        length);
                /* return -1; */
            }
        }
        return length;
    }
}

ipv6_hdr_t *ipv6_get_buf_send(void)
{
    return ((ipv6_hdr_t *) &ip_send_buffer[LL_HDR_LEN]);
}

uint8_t *get_payload_buf_send(uint8_t ext_len)
{
    return &(ip_send_buffer[LLHDR_IPV6HDR_LEN + ext_len]);
}

ipv6_hdr_t *ipv6_get_buf(void)
{
    return ((ipv6_hdr_t *) &buffer[LL_HDR_LEN]);
}

icmpv6_hdr_t *get_icmpv6_buf(uint8_t ext_len)
{
    return ((icmpv6_hdr_t *) &buffer[LLHDR_IPV6HDR_LEN + ext_len]);
}

uint8_t *get_payload_buf(uint8_t ext_len)
{
    return &(buffer[LLHDR_IPV6HDR_LEN + ext_len]);
}

int ipv6_sendto(const ipv6_addr_t *dest, uint8_t next_header,
                const uint8_t *payload, uint16_t payload_length, ipv6_addr_t *next_hop)
{
    uint8_t *p_ptr;

    if (next_header == IPV6_PROTO_NUM_TCP) {
        p_ptr = get_payload_buf_send(ipv6_ext_hdr_len);
        ipv6_buf = ipv6_get_buf_send();
    }
    else {
        ipv6_buf = ipv6_get_buf();
        p_ptr = get_payload_buf(ipv6_ext_hdr_len);
    }

    ipv6_buf->version_trafficclass = IPV6_VER;
    ipv6_buf->trafficclass_flowlabel = 0;
    ipv6_buf->flowlabel = 0;
    ipv6_buf->nextheader = next_header;
    ipv6_buf->hoplimit = MULTIHOP_HOPLIMIT;
    ipv6_buf->length = HTONS(payload_length);
    printf("IPV6_sendto payload length: %u\n", payload_length);
    memcpy(&(ipv6_buf->destaddr), dest, 16);
    memcpy(p_ptr, payload, payload_length);
    printf("IPV6_sendto  before");
    return ipv6_send_packet(ipv6_buf, next_hop);
}

void ipv6_set_default_hop_limit(uint8_t hop_limit)
{
    default_hop_limit = hop_limit;
}

uint8_t ipv6_get_default_hop_limit(void)
{
    return default_hop_limit;
}

/* Register an upper layer thread */
uint8_t ipv6_register_packet_handler(kernel_pid_t pid)
{
    uint8_t i;

    for (i = 0;
         ((i < SIXLOWIP_MAX_REGISTERED) && (sixlowip_reg[i] != pid)
          && (sixlowip_reg[i] != 0)); i++) {
        ;
    }

    if (i >= SIXLOWIP_MAX_REGISTERED) {
        return ENOMEM;
    }
    else {
        sixlowip_reg[i] = pid;
        return 1;
    }
}

int icmpv6_demultiplex(const icmpv6_hdr_t *hdr)
{
    switch (hdr->type) {
        case (ICMPV6_TYPE_ECHO_REQUEST): {
            DEBUG("INFO: packet type: icmp echo request\n");
            /* processing echo request */
            recv_echo_req();
            break;
        }

        case (ICMPV6_TYPE_ECHO_REPLY): {
            DEBUG("INFO: packet type: icmp echo reply\n");
            /* processing echo reply */
            recv_echo_repl();
            break;
        }

        case (ICMPV6_TYPE_ROUTER_SOL): {
            DEBUG("INFO: packet type: icmp router solicitation\n");
            /* processing router solicitation */
            recv_rtr_sol();
            /* init solicited router advertisment*/
            break;
        }

        case (ICMPV6_TYPE_ROUTER_ADV): {
            DEBUG("INFO: packet type: icmp router advertisment\n");
            /* processing router advertisment */
            recv_rtr_adv();
            /* init neighbor solicitation */
            break;
        }

        case (ICMPV6_TYPE_NEIGHBOR_SOL): {
            DEBUG("INFO: packet type: icmp neighbor solicitation\n");
            recv_nbr_sol();
            break;
        }

        case (ICMPV6_TYPE_NEIGHBOR_ADV): {
            DEBUG("INFO: packet type: icmp neighbor advertisment\n");
            recv_nbr_adv();
            break;
        }

        case (ICMPV6_TYPE_RPL_CONTROL): {
            DEBUG("INFO: packet type: RPL message\n");

            if (_rpl_process_pid != KERNEL_PID_UNDEF) {
                msg_t m_send;
                m_send.content.ptr = (char *) ipv6_buf;
                m_send.type = ((icmpv6_hdr_t *)(m_send.content.ptr + IPV6_HDR_LEN))->code;
                msg_send(&m_send, _rpl_process_pid);
            }
            else {
                DEBUG("INFO: no RPL handler registered\n");
            }

            break;
        }

        default:
            return -1;
    }

    return 0;
}

uint8_t ipv6_get_addr_match(const ipv6_addr_t *src, const ipv6_addr_t *dst)
{
    uint8_t val = 0, xor;

    for (int i = 0; i < 16; i++) {
        /* if bytes are equal add 8 */
        if (src->uint8[i] == dst->uint8[i]) {
            val += 8;
        }
        else {
            xor = src->uint8[i] ^ dst->uint8[i];

            /* while bits from byte equal add 1 */
            for (int j = 0; j < 8; j++) {
                if ((xor & 0x80) == 0) {
                    val++;
                    xor = xor << 1;
                }
                else {
                    break;
                }
            }

            break;
        }
    }

    return val;
}

/**
 * @brief Check if the given IPv6 address is assigned to any configured
 *        interface
 *
 * @param[in] addr  The IPv6 address to check
 *
 * @return 1    If *addr* is assigned to at least one interface
 * @return 0    If *addr* is not assigned to any interface
 * @return -1   If no IPv6 address is configured to any interface
 */
static int is_our_address(ipv6_addr_t *addr)
{
    int if_id = -1;
    int if_counter = -1;

    DEBUGF("Is this my address: %s?\n", ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, addr));

    while ((if_id = net_if_iter_interfaces(if_id)) >= 0) {
        ipv6_net_if_ext_t *net_if_ext = ipv6_net_if_get_ext(if_id);
        ipv6_net_if_addr_t *myaddr = NULL;
        uint8_t prefix = net_if_ext->prefix / 8;
        uint8_t suffix = IPV6_ADDR_LEN - prefix;

        while ((myaddr = (ipv6_net_if_addr_t *) net_if_iter_addresses(if_id,
                         (net_if_addr_t **) &myaddr)) != NULL) {
            if_counter++;
            DEBUGF("\tCompare with: %s?\n", ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, (ipv6_addr_t *) myaddr->addr_data));

            if ((ipv6_get_addr_match(myaddr->addr_data, addr)
                 >= net_if_ext->prefix)
                && (memcmp(&addr->uint8[prefix],
                           &myaddr->addr_data->uint8[prefix], suffix) == 0)) {
                DEBUGF("Found my address\n");
                return 1;
            }
        }
    }

    /* return negative value if no address is configured so far */
    if (if_counter >= 0) {
        return 0;
    }

    return -1;
}

void *ipv6_process(void *arg)
{
    (void) arg;

    msg_t m_recv_lowpan, m_send_lowpan;
    msg_t m_recv, m_send;
    uint8_t i;
    uint16_t packet_length;

    msg_init_queue(ip_msg_queue, IP_PKT_RECV_BUF_SIZE);

    while (1) {
        msg_receive(&m_recv_lowpan);

        ipv6_buf = (ipv6_hdr_t *) m_recv_lowpan.content.ptr;

        /* identifiy packet */
        nextheader = &ipv6_buf->nextheader;

        for (i = 0; i < SIXLOWIP_MAX_REGISTERED; i++) {
            if (sixlowip_reg[i]) {
                msg_t m_send;
                m_send.type = IPV6_PACKET_RECEIVED;
                m_send.content.ptr = (char *) ipv6_buf;
                msg_send(&m_send, sixlowip_reg[i]);
            }
        }

        int addr_match = is_our_address(&ipv6_buf->destaddr);
        DEBUGF("Did it match? %d\n", addr_match);

        /* no address configured for this node so far, exit early */
        if (addr_match < 0) {
            msg_reply(&m_recv_lowpan, &m_send_lowpan);
            continue;
        }
        /* destination is our address */
        else if (addr_match) {
            switch (*nextheader) {
                case (IPV6_PROTO_NUM_ICMPV6): {
                    icmp_buf = get_icmpv6_buf(ipv6_ext_hdr_len);

                    /* checksum test*/
                    if (ipv6_csum(ipv6_buf, (uint8_t *) icmp_buf,
                                  NTOHS(ipv6_buf->length), IPV6_PROTO_NUM_ICMPV6)
                        != 0xffff) {
                        DEBUG("ERROR: wrong checksum\n");
                    }

                    icmpv6_demultiplex(icmp_buf);
                    break;
                }

                case (IPV6_PROTO_NUM_TCP): {
                    if (tcp_packet_handler_pid != KERNEL_PID_UNDEF) {
                        m_send.content.ptr = (char *) ipv6_buf;
                        msg_send_receive(&m_send, &m_recv, tcp_packet_handler_pid);
                    }
                    else {
                        DEBUG("INFO: No TCP handler registered.\n");
                    }

                    break;
                }

                case (IPV6_PROTO_NUM_UDP): {
                    if (udp_packet_handler_pid != KERNEL_PID_UNDEF) {
                        m_send.content.ptr = (char *) ipv6_buf;
                        msg_send_receive(&m_send, &m_recv, udp_packet_handler_pid);
                    }
                    else {
                        DEBUG("INFO: No UDP handler registered.\n");
                    }

                    break;
                }

                case (IPV6_PROTO_NUM_SRH): {
                    if (_rpl_process_pid != KERNEL_PID_UNDEF) {
                        m_send.content.ptr = (char *) ipv6_buf;
                        msg_send(&m_send, _rpl_process_pid);
                    }
                    else {
                        DEBUG("INFO: no RPL handler registered\n");
                    }

                    break;
                }

                case (IPV6_PROTO_NUM_NONE): {
                    DEBUG("INFO: Packet with no Header following the IPv6 Header received.\n");
                    break;
                }

                default:
                    DEBUG("INFO: Unknown next header\n");
                    break;
            }
        }
        /* destination is foreign address */
        else {
            DEBUG("That's not for me, destination is %s\n", ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN, &ipv6_buf->destaddr));

            if ((ip_srh_indicator != NULL)
                && (ipv6_buf->nextheader != IPV6_PROTO_NUM_ICMPV6)
                && (ip_srh_indicator() == 1)) {
                if (_rpl_process_pid != KERNEL_PID_UNDEF) {
                    m_send.content.ptr = (char *) ipv6_buf;
                    msg_send(&m_send, _rpl_process_pid);
                }
                else {
                    DEBUG("INFO: no RPL handler registered\n");
                }
            }
            else {

                packet_length = IPV6_HDR_LEN + NTOHS(ipv6_buf->length);
                ndp_neighbor_cache_t *nce;

                ipv6_addr_t *dest;

                if (ip_get_next_hop == NULL) {
                    dest = &ipv6_buf->destaddr;
                }
                else {
                    dest = ip_get_next_hop(&ipv6_buf->destaddr);
                }

                if ((dest == NULL) || ((--ipv6_buf->hoplimit) == 0)) {
                    DEBUG("!!! Packet not for me, routing handler is set, but I "
                          " have no idea where to send or the hop limit is exceeded.\n");
                    msg_reply(&m_recv_lowpan, &m_send_lowpan);
                    continue;
                }

                nce = ndp_get_ll_address(dest);

                /* copy received packet to send buffer */
                memcpy(ipv6_get_buf_send(), ipv6_get_buf(), packet_length);

                /* send packet to node ID derived from dest IP */
                if (nce != NULL) {
                    sixlowpan_lowpan_sendto(nce->if_id, &nce->lladdr,
                                            nce->lladdr_len, (uint8_t *) ipv6_get_buf_send(),
                                            packet_length);
                }
                else {
                    /* XXX: this is wrong, but until ND does work correctly,
                     *      this is the only way (aka the old way)*/
                    uint16_t raddr = dest->uint16[7];
                    sixlowpan_lowpan_sendto(0, &raddr, 2,
                                            (uint8_t *) ipv6_get_buf_send(), packet_length);
                }
            }
        }

        msg_reply(&m_recv_lowpan, &m_send_lowpan);
    }
}

ipv6_net_if_ext_t *ipv6_net_if_get_ext(int if_id)
{
    if (net_if_get_interface(if_id)) {
        return &ipv6_net_if_ext[if_id];
    }
    else {
        return NULL;
    }
}

int ipv6_net_if_add_addr(int if_id, const ipv6_addr_t *addr,
                         ndp_addr_state_t state, uint32_t val_ltime, uint32_t pref_ltime,
                         uint8_t is_anycast)
{
    ipv6_net_if_hit_t hit;

    if (ipv6_addr_is_unspecified(addr) == 128) {
        DEBUG("ERROR: unspecified address (::) can't be assigned to interface.\n");
        return 0;
    }

    if (ipv6_addr_is_multicast(addr) && is_anycast) {
        DEBUG("ERROR: anycast addresses must not be multicast addresses "
              "(i.e. start with ff::/2)\n");
        return 0;
    }

    if (ipv6_net_if_addr_match(&hit, addr)) {
        return 1;
    }

    if (ipv6_net_if_addr_buffer_count < IPV6_NET_IF_ADDR_BUFFER_LEN) {
        timex_t valtime = { val_ltime, 0 };
        timex_t preftime = { pref_ltime, 0 };
        timex_t now;

        vtimer_now(&now);

        ipv6_addr_t *addr_data =
            &ipv6_addr_buffer[ipv6_net_if_addr_buffer_count];
        memcpy(addr_data, addr, sizeof(ipv6_addr_t));

        ipv6_net_if_addr_t *addr_entry =
            &ipv6_net_if_addr_buffer[ipv6_net_if_addr_buffer_count];
        addr_entry->addr_data = addr_data;
        addr_entry->addr_len = 128;

        if (is_anycast) {
            addr_entry->addr_protocol = NET_IF_L3P_IPV6_ANYCAST;
        }
        else if (ipv6_addr_is_multicast(addr_data)) {
            addr_entry->addr_protocol = NET_IF_L3P_IPV6_MULTICAST;
        }
        else {
            addr_entry->addr_protocol = NET_IF_L3P_IPV6_UNICAST;
        }

        addr_entry->ndp_state = state;
        addr_entry->valid_lifetime = timex_add(now, valtime);
        addr_entry->preferred_lifetime = timex_add(now, preftime);
        addr_entry->is_anycast = is_anycast;

        ipv6_net_if_addr_buffer_count++;

        net_if_add_address(if_id, (net_if_addr_t *) addr_entry);

        /* Register to Solicited-Node multicast address according to RFC 4291 */
        if (is_anycast || !ipv6_addr_is_multicast(addr)) {
            ipv6_addr_t sol_node_mcast_addr;
            ipv6_addr_set_solicited_node_addr(&sol_node_mcast_addr, addr);

            if (ipv6_net_if_addr_match(&hit, &sol_node_mcast_addr) == NULL) {
                ipv6_net_if_add_addr(if_id, &sol_node_mcast_addr, state,
                                     val_ltime, pref_ltime, 0);
            }
        }

        return 1;
    }

    return 0;
}

ipv6_net_if_hit_t *ipv6_net_if_addr_match(ipv6_net_if_hit_t *hit,
        const ipv6_addr_t *addr)
{
    int if_id = -1;
    ipv6_net_if_addr_t *addr_entry = NULL;

    while ((if_id = net_if_iter_interfaces(if_id)) >= 0) {
        while (net_if_iter_addresses(if_id, (net_if_addr_t **) &addr_entry)
               != NULL) {
            if (addr_entry->addr_protocol & NET_IF_L3P_IPV6) {
                uint8_t byte_al = addr_entry->addr_len / 8;
                uint8_t mask[] = { 0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc,
                                   0xfe
                                 };

                if (memcmp(addr_entry->addr_data, addr, byte_al) == 0
                    && (addr_entry->addr_len % 8 == 0
                        || ((addr_entry->addr_data->uint8[byte_al]
                             - addr->uint8[byte_al])
                            & mask[addr_entry->addr_len
                                   - (byte_al * 8)]))) {
                    hit->if_id = if_id;
                    hit->addr = addr_entry;
                    return hit;
                }
            }
        }
    }

    return NULL;
}

ipv6_net_if_hit_t *ipv6_net_if_addr_prefix_eq(ipv6_net_if_hit_t *hit,
        ipv6_addr_t *addr)
{
    int if_id = -1;
    ipv6_net_if_addr_t *addr_entry = NULL;

    while ((if_id = net_if_iter_interfaces(if_id)) >= 0) {
        while (net_if_iter_addresses(if_id, (net_if_addr_t **) &addr_entry)
               != NULL) {
            if (addr_entry->addr_protocol & NET_IF_L3P_IPV6) {
                if (memcmp(addr_entry->addr_data, &addr, 8) == 0) {
                    hit->if_id = if_id;
                    hit->addr = addr_entry;
                    return hit;
                }
            }
        }
    }

    return NULL;
}

/* TODO ipv6_net_if_hit_t returning function similar wrapping
 *      ipv6_net_if_get_best_src_addr() to search on all interfaces */

ipv6_addr_t *ipv6_addr_set_by_eui64(ipv6_addr_t *out, int if_id,
                                    const ipv6_addr_t *prefix)
{
    uint8_t force_generation = 0;
    out->uint16[0] = prefix->uint16[0];
    out->uint16[1] = prefix->uint16[1];
    out->uint16[2] = prefix->uint16[2];
    out->uint16[3] = prefix->uint16[3];

    if (net_if_get_src_address_mode(if_id) == NET_IF_TRANS_ADDR_M_SHORT) {
        force_generation = 1;
    }

    if (net_if_get_eui64((net_if_eui64_t *) &out->uint8[8], if_id,
                         force_generation)) {
#ifdef MODULE_SIXLOWPAN

        if (!sixlowpan_lowpan_eui64_to_short_addr((net_if_eui64_t *)&out->uint8[8])) {
            out->uint8[8] ^= 0x02;
        }

#else
        out->uint8[8] ^= 0x02;
#endif        
        return out;
    }
    else {
        return NULL;
    }
}

void ipv6_addr_init_prefix(ipv6_addr_t *out, const ipv6_addr_t *prefix,
                           uint8_t bits)
{
    if (bits > 128) {
        bits = 128;
    }

    uint8_t bytes = bits / 8, mask;

    if (bits % 8) {
        mask = 0xff << (8 - (bits - (bytes * 8)));
    }
    else {
        mask = 0x00;
    }

    memcpy(out, prefix, bytes);

    if (bytes < 16) {
        out->uint8[bytes] = prefix->uint8[bytes] & mask;
    }

    memset(&(out[bytes + 1]), 0, 15 - bytes);
}

void ipv6_net_if_get_best_src_addr(ipv6_addr_t *src, const ipv6_addr_t *dest)
{
    /* try to find best match if dest is not mcast or link local */
    int if_id = 0; // TODO: get this somehow
    ipv6_net_if_addr_t *addr = NULL;
    ipv6_net_if_addr_t *tmp_addr = NULL;

    if (!(ipv6_addr_is_link_local(dest)) && !(ipv6_addr_is_multicast(dest))) {
        while ((addr = (ipv6_net_if_addr_t *) net_if_iter_addresses(if_id,
                       (net_if_addr_t **) &addr))) {
            if (addr->ndp_state == NDP_ADDR_STATE_PREFERRED) {
                if (!ipv6_addr_is_link_local(addr->addr_data)
                    && !ipv6_addr_is_multicast(addr->addr_data)
                    && !ipv6_addr_is_unique_local_unicast(
                        addr->addr_data)) {

                    uint8_t bmatch = 0;
                    uint8_t tmp = ipv6_get_addr_match(dest, addr->addr_data);

                    if (tmp >= bmatch) {
                        tmp_addr = addr;
                    }
                }
            }
        }
    }
    else {
        while ((addr = (ipv6_net_if_addr_t *) net_if_iter_addresses(if_id,
                       (net_if_addr_t **) &addr))) {
            if (addr->ndp_state == NDP_ADDR_STATE_PREFERRED
                && ipv6_addr_is_link_local(addr->addr_data)
                && !ipv6_addr_is_multicast(addr->addr_data)) {
                tmp_addr = addr;
            }
        }
    }

    if (tmp_addr == NULL) {
        memset(src, 0, 16);
    }
    else {
        memcpy(src, tmp_addr->addr_data, 16);
    }
}

void ipv6_addr_init(ipv6_addr_t *out, uint16_t addr0, uint16_t addr1,
                    uint16_t addr2, uint16_t addr3, uint16_t addr4, uint16_t addr5,
                    uint16_t addr6, uint16_t addr7)
{
    out->uint16[0] = HTONS(addr0);
    out->uint16[1] = HTONS(addr1);
    out->uint16[2] = HTONS(addr2);
    out->uint16[3] = HTONS(addr3);
    out->uint16[4] = HTONS(addr4);
    out->uint16[5] = HTONS(addr5);
    out->uint16[6] = HTONS(addr6);
    out->uint16[7] = HTONS(addr7);
}

uint32_t get_remaining_time(timex_t *t)
{
    timex_t now;
    vtimer_now(&now);

    return (timex_sub(*t, now).seconds);
}

void set_remaining_time(timex_t *t, uint32_t time)
{
    timex_t tmp = { time, 0 };

    timex_t now;
    vtimer_now(&now);
    *t = timex_add(now, tmp);
}

int ipv6_init_as_router(void)

{
    ipv6_addr_t addr;
    int if_id = -1;

    ipv6_addr_set_all_routers_addr(&addr);

    while ((if_id = net_if_iter_interfaces(if_id)) >= 0) {
        if (!ipv6_net_if_add_addr(if_id, &addr, NDP_ADDR_STATE_PREFERRED, 0, 0,
                                  0)) {
            return 0;
        }
    }

    return 1;
}

uint8_t ipv6_is_router(void)
{
    ipv6_addr_t addr;
    ipv6_net_if_hit_t hit;

    ipv6_addr_set_all_routers_addr(&addr);

    if (ipv6_net_if_addr_match(&hit, &addr) != NULL) {
        return 1;
    }

    return 0;
}

void set_tcp_packet_handler_pid(kernel_pid_t pid)
{
    tcp_packet_handler_pid = pid;
}

void set_udp_packet_handler_pid(kernel_pid_t pid)
{
    udp_packet_handler_pid = pid;
}

void ipv6_register_next_header_handler(uint8_t next_header, kernel_pid_t pid)
{
    switch (next_header) {
        case (IPV6_PROTO_NUM_TCP):
            set_tcp_packet_handler_pid(pid);
            break;

        case (IPV6_PROTO_NUM_UDP):
            set_udp_packet_handler_pid(pid);
            break;

        default:
            /* TODO */
            break;
    }
}

/* register routing function */
void ipv6_iface_set_routing_provider(ipv6_addr_t *(*next_hop)(ipv6_addr_t *dest))
{
    ip_get_next_hop = next_hop;
}

#ifdef MODULE_RPL
/* register source-routing indicator function */
void ipv6_iface_set_srh_indicator(uint8_t (*srh_indi)(void))
{
    ip_srh_indicator = srh_indi;
}

/* register rpl-process function */
void ipv6_register_rpl_handler(kernel_pid_t pid)
{
    _rpl_process_pid = pid;
}

#endif

uint16_t ipv6_csum(ipv6_hdr_t *ipv6_header, uint8_t *buf, uint16_t len,
                   uint8_t proto)
{
    uint16_t sum = 0;
    DEBUG("Calculate checksum over src: %s, dst: %s, len: %04X, buf: %p, proto: %u\n",
          ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN,
                           &ipv6_header->srcaddr),
          ipv6_addr_to_str(addr_str, IPV6_MAX_ADDR_STR_LEN,
                           &ipv6_header->destaddr),
          len, buf, proto);
    sum = len + proto;
    sum = net_help_csum(sum, (uint8_t *) &ipv6_header->srcaddr,
                        2 * sizeof(ipv6_addr_t));
    sum = net_help_csum(sum, buf, len);
    return (sum == 0) ? 0xffff : HTONS(sum);
}

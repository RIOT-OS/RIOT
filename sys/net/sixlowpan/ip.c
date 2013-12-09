/**
 * IPv6 implementation
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
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
#include "sixlowpan/mac.h"

#include "ip.h"
#include "icmp.h"
#include "lowpan.h"

#include "destiny/socket.h"
#include "net_help.h"

#define ENABLE_DEBUG    (0)
#if ENABLE_DEBUG
char addr_str[IPV6_MAX_ADDR_STR_LEN];
#endif
#include "debug.h"

#define IP_PKT_RECV_BUF_SIZE        (64)
#define LLHDR_IPV6HDR_LEN           (LL_HDR_LEN + IPV6_HDR_LEN)

uint8_t ip_send_buffer[BUFFER_SIZE];
uint8_t buffer[BUFFER_SIZE];
msg_t msg_queue[IP_PKT_RECV_BUF_SIZE];
ipv6_hdr_t *ipv6_buf;
icmpv6_hdr_t *icmp_buf;
uint8_t *nextheader;

uint8_t iface_addr_list_count = 0;
int udp_packet_handler_pid = 0;
int tcp_packet_handler_pid = 0;
int rpl_process_pid = 0;
ipv6_addr_t *(*ip_get_next_hop)(ipv6_addr_t*) = 0;

/* registered upper layer threads */
int sixlowip_reg[SIXLOWIP_MAX_REGISTERED];

void ipv6_send_bytes(ipv6_hdr_t *bytes)
{
    uint16_t offset = IPV6_HDR_LEN + HTONS(bytes->length);

    bytes->flowlabel = HTONS(bytes->flowlabel);
    bytes->length = HTONS(bytes->length);

    memset(bytes, 0, BUFFER_SIZE);
    memcpy(bytes + LL_HDR_LEN, bytes, offset);

    sixlowpan_lowpan_sendto((ieee_802154_long_t *) &bytes->destaddr.uint16[4],
                            (uint8_t *)bytes,
                            offset);
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

void ipv6_sendto(const ipv6_addr_t *dest, uint8_t next_header,
                 const uint8_t *payload, uint16_t payload_length)
{
    uint8_t *p_ptr;
    uint16_t packet_length;

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
    ipv6_buf->length = payload_length;

    memcpy(&(ipv6_buf->destaddr), dest, 16);
    ipv6_iface_get_best_src_addr(&(ipv6_buf->srcaddr), &(ipv6_buf->destaddr));

    memcpy(p_ptr, payload, payload_length);

    packet_length = IPV6_HDR_LEN + payload_length;

    /* see if dest should be routed to a different next hop */
    if (ip_get_next_hop == NULL || ipv6_addr_is_multicast(&ipv6_buf->destaddr)) {
        dest = &ipv6_buf->destaddr;
    }
    else {
        dest = ip_get_next_hop(&ipv6_buf->destaddr);
    }

    if (dest == NULL) {
        return;
    }

    sixlowpan_lowpan_sendto((ieee_802154_long_t *) &dest->uint16[4],
                            (uint8_t *)ipv6_buf, packet_length);
}

/* Register an upper layer thread */
uint8_t ipv6_register_packet_handler(int pid)
{
    uint8_t i;

    for (i = 0; ((i < SIXLOWIP_MAX_REGISTERED) && (sixlowip_reg[i] != pid) && 
                 (sixlowip_reg[i] != 0)); i++) {
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

            if (rpl_process_pid != 0) {
                msg_t m_send;
                m_send.content.ptr = (char *) &hdr->code;
                msg_send(&m_send, rpl_process_pid, 1);
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

uint8_t ipv6_get_addr_match(const ipv6_addr_t *src,
                            const ipv6_addr_t *dst)
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
        }
    }

    return val;
}

void ipv6_process(void)
{
    msg_t m_recv_lowpan, m_send_lowpan;
    msg_t m_recv, m_send;
    ipv6_addr_t myaddr;
    uint8_t i;
    uint16_t packet_length;

    ipv6_addr_init(&myaddr, 0xabcd, 0x0, 0x0, 0x0, 0x3612, 0x00ff, 0xfe00,
                   sixlowpan_mac_get_radio_address());

    while (1) {
        msg_receive(&m_recv_lowpan);

        ipv6_buf = (ipv6_hdr_t *)m_recv_lowpan.content.ptr;

        /* identifiy packet */
        nextheader = &ipv6_buf->nextheader;

        /* destination is foreign address */
        if ((ipv6_get_addr_match(&myaddr, &ipv6_buf->destaddr) >= 112) &&
            (ipv6_buf->destaddr.uint8[15] != myaddr.uint8[15])) {
            packet_length = IPV6_HDR_LEN + ipv6_buf->length;

            ipv6_addr_t* dest;
            if (ip_get_next_hop == NULL) {
                dest = &ipv6_buf->destaddr;
            }
            else {
                dest = ip_get_next_hop(&ipv6_buf->destaddr);
            }

            if (dest == NULL || --ipv6_buf->hoplimit == 0) {
                continue;
            }

            /* copy received packet to send buffer */
            memcpy(ipv6_get_buf_send(), ipv6_get_buf(), packet_length);
            /* send packet to node ID derived from dest IP */
            sixlowpan_lowpan_sendto((ieee_802154_long_t *) &dest->uint16[4],
                                    (uint8_t *)ipv6_get_buf_send(),
                                    packet_length);
        }
        /* destination is our address */
        else {
            for (i = 0; i < SIXLOWIP_MAX_REGISTERED; i++) {
                if (sixlowip_reg[i]) {
                    msg_t m_send;
                    m_send.content.ptr = (char *) &ipv6_buf;
                    msg_send(&m_send, sixlowip_reg[i], 1);
                }
            }

            switch (*nextheader) {
                case (IPV6_PROTO_NUM_ICMPV6): {
                    icmp_buf = get_icmpv6_buf(ipv6_ext_hdr_len);
                    /* checksum test*/
                    if (ipv6_csum(ipv6_buf, (uint8_t*) icmp_buf, ipv6_buf->length, IPV6_PROTO_NUM_ICMPV6) != 0xffff) {
                        printf("ERROR: wrong checksum\n");
                    }
                    icmpv6_demultiplex(icmp_buf);
                    break;
                }

                case (IPV6_PROTO_NUM_TCP): {
                    if (tcp_packet_handler_pid != 0) {
                        m_send.content.ptr = (char *) ipv6_buf;
                        msg_send_receive(&m_send, &m_recv, tcp_packet_handler_pid);
                    }
                    else {
                        printf("INFO: No TCP handler registered.\n");
                    }

                    break;
                }

                case (IPV6_PROTO_NUM_UDP): {
                    if (udp_packet_handler_pid != 0) {
                        m_send.content.ptr = (char *) ipv6_buf;
                        msg_send_receive(&m_send, &m_recv, udp_packet_handler_pid);
                    }
                    else {
                        printf("INFO: No UDP handler registered.\n");
                    }

                    break;
                }

                case (IPV6_PROTO_NUM_NONE): {
                    printf("INFO: Packet with no Header following the IPv6 Header received.\n");
                    break;
                }

                default:
                    break;
            }
        }

        msg_reply(&m_recv_lowpan, &m_send_lowpan);
    }
}

void ipv6_iface_add_addr(const ipv6_addr_t *addr, ipv6_addr_type_t type,
                         ndp_addr_state_t state, uint32_t val_ltime,
                         uint32_t pref_ltime)
{
    if (ipv6_addr_is_unspecified(addr) == 128) {
        printf("ERROR: unspecified address (::) can't be assigned to interface.\n");
        return;
    }

    if (ipv6_iface_addr_match(addr) != 0) {
        return;
    }

    if (iface_addr_list_count < IFACE_ADDR_LIST_LEN) {
        memcpy(&(iface.addr_list[iface_addr_list_count].addr.uint8[0]),
               &(addr->uint8[0]), 16);
        iface.addr_list[iface_addr_list_count].state = state;
        timex_t valtime = {val_ltime, 0};
        timex_t preftime = {pref_ltime, 0};
        timex_t now;
        vtimer_now(&now);
        iface.addr_list[iface_addr_list_count].val_ltime = timex_add(now, valtime);
        iface.addr_list[iface_addr_list_count].pref_ltime = timex_add(now, preftime);
        iface.addr_list[iface_addr_list_count].type = type;
        iface_addr_list_count++;

        /* Register to Solicited-Node multicast address according to RFC 4291 */
        if (type == IPV6_ADDR_TYPE_ANYCAST || type == IPV6_ADDR_TYPE_LINK_LOCAL ||
            type == IPV6_ADDR_TYPE_GLOBAL || type == IPV6_ADDR_TYPE_UNICAST) {
            ipv6_addr_t sol_node_mcast_addr;
            ipv6_addr_set_solicited_node_addr(&sol_node_mcast_addr, addr);

            if (ipv6_iface_addr_match(&sol_node_mcast_addr) == NULL) {
                ipv6_iface_add_addr(&sol_node_mcast_addr,
                                    IPV6_ADDR_TYPE_SOLICITED_NODE,
                                    state, val_ltime, pref_ltime);
            }
        }
    }
}

addr_list_t *ipv6_iface_addr_match(const ipv6_addr_t *addr)
{
    int i;

    for (i = 0; i < iface_addr_list_count; i++) {
        if (memcmp(&(iface.addr_list[i].addr.uint8[0]),
                   &(addr->uint8[0]), 16) == 0) {
            return &(iface.addr_list[i]);
        }
    }

    return NULL;
}

addr_list_t *ipv6_iface_addr_prefix_eq(ipv6_addr_t *addr)
{
    int i;

    for (i = 0; i < iface_addr_list_count; i++) {
        if (memcmp(&(iface.addr_list[i].addr.uint8[0]),
                   &(addr->uint8[0]), 8) == 0) {
            return &(iface.addr_list[i]);
        }
    }

    return NULL;
}

void ipv6_iface_print_addrs(void)
{
    for (int i = 0; i < iface_addr_list_count; i++) {
        char addr_str[IPV6_MAX_ADDR_STR_LEN];
        printf("%s\n", ipv6_addr_to_str(addr_str,
                                        &(iface.addr_list[i].addr)));
    }
}

void ipv6_addr_set_by_eui64(ipv6_addr_t *out, const ipv6_addr_t *prefix)
{
    out->uint16[0] = prefix->uint16[0];
    out->uint16[1] = prefix->uint16[1];
    out->uint16[2] = prefix->uint16[2];
    out->uint16[3] = prefix->uint16[3];

    memcpy(&(out->uint8[8]), &(iface.laddr.uint8[0]), 8);
}

void ipv6_addr_init_prefix(ipv6_addr_t *out, const ipv6_addr_t *prefix,
                           uint8_t bits)
{
    if (bits > 128) {
        bits = 128;
    }

    uint8_t bytes = bits / 8, mask;

    if (bits % 8) {
        mask = 0xff << (bits - (bytes * 8));
    }
    else {
        mask = 0x00;
    }

    bytes++;
    memset(out, 0, 16);
    memcpy(out, prefix, bytes);
    out->uint8[bytes] = prefix->uint8[bytes] & mask;
}

void ipv6_addr_set_all_routers_addr(ipv6_addr_t *ipv6_addr)
{
    ipv6_addr->uint16[0] = HTONS(0xff02);
    ipv6_addr->uint16[1] = 0;
    ipv6_addr->uint16[2] = 0;
    ipv6_addr->uint16[3] = 0;
    ipv6_addr->uint16[4] = 0;
    ipv6_addr->uint16[5] = 0;
    ipv6_addr->uint16[6] = 0;
    ipv6_addr->uint16[7] = HTONS(0x0002);
}

void ipv6_addr_set_all_nodes_addr(ipv6_addr_t *ipv6_addr)
{
    ipv6_addr->uint16[0] = HTONS(0xff02);
    ipv6_addr->uint16[1] = 0;
    ipv6_addr->uint16[2] = 0;
    ipv6_addr->uint16[3] = 0;
    ipv6_addr->uint16[4] = 0;
    ipv6_addr->uint16[5] = 0;
    ipv6_addr->uint16[6] = 0;
    ipv6_addr->uint16[7] = HTONS(0x0001);
}

void ipv6_addr_set_loopback_addr(ipv6_addr_t *ipv6_addr)
{
    ipv6_addr->uint16[0] = 0;
    ipv6_addr->uint16[1] = 0;
    ipv6_addr->uint16[2] = 0;
    ipv6_addr->uint16[3] = 0;
    ipv6_addr->uint16[4] = 0;
    ipv6_addr->uint16[5] = 0;
    ipv6_addr->uint16[6] = 0;
    ipv6_addr->uint16[7] = HTONS(0x0001);
}

void ipv6_iface_get_best_src_addr(ipv6_addr_t *src, const ipv6_addr_t *dest)
{
    /* try to find best match if dest is not mcast or link local */
    int8_t itmp = -1;
    uint8_t tmp = 0;
    uint8_t bmatch = 0;

    if (!(ipv6_addr_is_link_local(dest)) && !(ipv6_addr_is_multicast(dest))) {
        for (int i = 0; i < IFACE_ADDR_LIST_LEN; i++) {
            if (iface.addr_list[i].state == NDP_ADDR_STATE_PREFERRED) {
                if (!ipv6_addr_is_link_local(&(iface.addr_list[i].addr)) &&
                    !ipv6_addr_is_multicast(&(iface.addr_list[i].addr)) &&
                    !ipv6_addr_is_unique_local_unicast(&(iface.addr_list[i].addr))) {
                    tmp = ipv6_get_addr_match(dest, &(iface.addr_list[i].addr));

                    if (tmp >= bmatch) {
                        bmatch = tmp;
                        itmp = i;
                    }
                }
            }
        }
    }
    else {
        for (int j = 0; j < IFACE_ADDR_LIST_LEN; j++) {
            if ((iface.addr_list[j].state == NDP_ADDR_STATE_PREFERRED) &&
                ipv6_addr_is_link_local(&(iface.addr_list[j].addr)) &&
                !ipv6_addr_is_multicast(&(iface.addr_list[j].addr))) {
                itmp = j;
            }
        }
    }

    if (itmp == -1) {
        memset(src, 0, 16);
    }
    else {
        memcpy(src, &(iface.addr_list[itmp].addr), 16);
    }
}

int ipv6_addr_is_equal(const ipv6_addr_t *a, const ipv6_addr_t *b)
{
    return (ipv6_get_addr_match(a, b) == 128);
}

void ipv6_addr_set_link_local_prefix(ipv6_addr_t *ipv6_addr)
{
    ipv6_addr->uint16[0] = HTONS(0xfe80);
    ipv6_addr->uint16[1] = 0;
    ipv6_addr->uint16[2] = 0;
    ipv6_addr->uint16[3] = 0;
}

void ipv6_addr_init(ipv6_addr_t *out, uint16_t addr0, uint16_t addr1,
                    uint16_t addr2, uint16_t addr3, uint16_t addr4,
                    uint16_t addr5, uint16_t addr6, uint16_t addr7)
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

int ipv6_addr_is_link_local(const ipv6_addr_t *addr)
{
    return (addr->uint16[0] == HTONS(0xfe80));
}

int ipv6_addr_is_unique_local_unicast(const ipv6_addr_t *addr)
{
    return (addr->uint8[0] == 0xfc || addr->uint8[0] == 0xfd);
}

int ipv6_addr_is_multicast(const ipv6_addr_t *addr)
{
    return (addr->uint8[0] == 0xff);
}

int ipv6_addr_is_unspecified(const ipv6_addr_t *ipv6_addr)
{
    return (ipv6_addr->uint32[0] == 0) && (ipv6_addr->uint32[1] == 0) &&
           (ipv6_addr->uint32[2] == 0) && (ipv6_addr->uint32[3] == 0);
}

int ipv6_addr_is_solicited_node(const ipv6_addr_t *ipv6_addr)
{
    return (ipv6_addr->uint8[0] == 0xFF) &&
           (ipv6_addr->uint8[1] == 0x02) &&
           (ipv6_addr->uint16[1] == 0x00) &&
           (ipv6_addr->uint16[2] == 0x00) &&
           (ipv6_addr->uint16[3] == 0x00) &&
           (ipv6_addr->uint16[4] == 0x00) &&
           (ipv6_addr->uint8[10] == 0x00) &&
           (ipv6_addr->uint8[11] == 0x01) &&
           (ipv6_addr->uint8[12] == 0xFF);
}

void ipv6_addr_set_solicited_node_addr(ipv6_addr_t *ipv6_addr_out,
                                       const ipv6_addr_t *ipv6_addr_in)
{
    /* copy only the last 24-bit of the ip-address that is beeing resolved */
    ipv6_addr_out->uint16[0] = HTONS(0xff02);
    ipv6_addr_out->uint16[1] = 0;
    ipv6_addr_out->uint16[2] = 0;
    ipv6_addr_out->uint16[3] = 0;
    ipv6_addr_out->uint16[4] = 0;
    ipv6_addr_out->uint16[5] = HTONS(0x0001);
    ipv6_addr_out->uint8[12] = 0xff;
    ipv6_addr_out->uint8[13] = ipv6_addr_in->uint8[13];
    ipv6_addr_out->uint16[7] = ipv6_addr_in->uint16[7];
}

char *ipv6_addr_to_str(char *addr_str, const ipv6_addr_t *ipv6_addr)
{
    sprintf(addr_str,
            "%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x",
            NTOHS(ipv6_addr->uint16[0]), NTOHS(ipv6_addr->uint16[1]),
            NTOHS(ipv6_addr->uint16[2]), NTOHS(ipv6_addr->uint16[3]),
            NTOHS(ipv6_addr->uint16[4]), NTOHS(ipv6_addr->uint16[5]),
            NTOHS(ipv6_addr->uint16[6]), NTOHS(ipv6_addr->uint16[7]));
    return addr_str;
}

uint32_t get_remaining_time(timex_t *t)
{
    timex_t now;
    vtimer_now(&now);

    return (timex_sub(*t, now).seconds);
}

void set_remaining_time(timex_t *t, uint32_t time)
{
    timex_t tmp = {time, 0};

    timex_t now;
    vtimer_now(&now);
    *t = timex_add(now, tmp);
}

void ipv6_init_iface_as_router(void)
{
    ipv6_addr_t addr;

    ipv6_addr_set_all_routers_addr(&addr);
    ipv6_iface_add_addr(&addr, NDP_ADDR_STATE_PREFERRED, 0, 0, IPV6_ADDR_TYPE_MULTICAST);
}


uint8_t ipv6_is_router(void)
{
    ipv6_addr_t addr;

    ipv6_addr_set_all_routers_addr(&addr);

    if (ipv6_iface_addr_match(&addr) != NULL) {
        return 1;
    }

    return 0;
}

void set_tcp_packet_handler_pid(int pid)
{
    tcp_packet_handler_pid = pid;
}

void set_udp_packet_handler_pid(int pid)
{
    udp_packet_handler_pid = pid;
}

void ipv6_register_next_header_handler(uint8_t next_header, int pid)
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
void ipv6_iface_set_routing_provider(ipv6_addr_t *(*next_hop)(ipv6_addr_t* dest)) {
    ip_get_next_hop = next_hop;
}

void ipv6_register_rpl_handler(int pid)
{
    rpl_process_pid = pid;
}

uint16_t ipv6_csum(ipv6_hdr_t *ipv6_header, uint8_t *buf, uint16_t len, uint8_t proto)
{
    uint16_t sum = 0;
    DEBUG("Calculate checksum over src: %s, dst: %s, len: %04X, buf: %p, proto: %u\n",
            ipv6_addr_to_str(addr_str, &ipv6_header->srcaddr),
            ipv6_addr_to_str(addr_str, &ipv6_header->destaddr),
            len, buf, proto);
    sum = len + proto;
    sum = csum(sum, (uint8_t *)&ipv6_header->srcaddr, 2 * sizeof(ipv6_addr_t));
    sum = csum(sum, buf, len);
    return (sum == 0) ? 0xffff : HTONS(sum);
}

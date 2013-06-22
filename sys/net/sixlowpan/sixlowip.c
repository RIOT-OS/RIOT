/**
 * IPv6 implementation 
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file subject to the terms and conditions of the GNU Lesser General
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
#include <vtimer.h>
#include <mutex.h>
#include "msg.h"
#include "sixlowip.h"
#include "sixlowmac.h"
#include "sixlownd.h"
#include "sixlowpan.h"
#include "sys/net/destiny/in.h"
#include "sys/net/destiny/socket.h"
#include "sys/net/net_help/net_help.h"
#include "sys/net/net_help/msg_help.h"
#include "sys/net/sixlowpan/rpl/rpl.h"

uint8_t ip_send_buffer[BUFFER_SIZE];
uint8_t buffer[BUFFER_SIZE];
msg_t msg_queue[IP_PKT_RECV_BUF_SIZE];
struct ipv6_hdr_t *ipv6_buf;
struct icmpv6_hdr_t *icmp_buf;
uint8_t ipv6_ext_hdr_len;
uint8_t *nextheader;
iface_t iface;
uint8_t iface_addr_list_count = 0;
int udp_packet_handler_pid = 0;
int tcp_packet_handler_pid = 0;
int rpl_process_pid = 0;

struct ipv6_hdr_t *get_ipv6_buf_send(void)
{
    return ((struct ipv6_hdr_t *) & (ip_send_buffer[LL_HDR_LEN]));
}

uint8_t *get_payload_buf_send(uint8_t ext_len)
{
    return &(ip_send_buffer[LLHDR_IPV6HDR_LEN + ext_len]);
}

struct ipv6_hdr_t *get_ipv6_buf(void)
{
    return ((struct ipv6_hdr_t *) & (buffer[LL_HDR_LEN]));
}

struct icmpv6_hdr_t *get_icmpv6_buf(uint8_t ext_len)
{
    return ((struct icmpv6_hdr_t *) & (buffer[LLHDR_IPV6HDR_LEN + ext_len]));
}

uint8_t *get_payload_buf(uint8_t ext_len)
{
    return &(buffer[LLHDR_IPV6HDR_LEN + ext_len]);
}

void sixlowpan_bootstrapping(void)
{

    init_rtr_sol(OPT_SLLAO);
}

void sixlowpan_send(ipv6_addr_t *addr, uint8_t *payload, uint16_t p_len,
                    uint8_t next_header)
{
    uint8_t *p_ptr;

    if(next_header == IPPROTO_TCP) {
        p_ptr = get_payload_buf_send(ipv6_ext_hdr_len);
        ipv6_buf = get_ipv6_buf_send();
    }
    else {
        ipv6_buf = get_ipv6_buf();
        p_ptr = get_payload_buf(ipv6_ext_hdr_len);
    }

    icmp_buf = get_icmpv6_buf(ipv6_ext_hdr_len);
    packet_length = 0;

    ipv6_buf->version_trafficclass = IPV6_VER;
    ipv6_buf->trafficclass_flowlabel = 0;
    ipv6_buf->flowlabel = 0;
    ipv6_buf->nextheader = next_header;
    ipv6_buf->hoplimit = MULTIHOP_HOPLIMIT;
    ipv6_buf->length = p_len;

    memcpy(&(ipv6_buf->destaddr), addr, 16);
    ipv6_get_saddr(&(ipv6_buf->srcaddr), &(ipv6_buf->destaddr));

    memcpy(p_ptr, payload, p_len);

    packet_length = IPV6_HDR_LEN + p_len;

    lowpan_init((ieee_802154_long_t *)&(ipv6_buf->destaddr.uint16[4]), 
                (uint8_t *)ipv6_buf);
}

int icmpv6_demultiplex(const struct icmpv6_hdr_t *hdr)
{
    switch(hdr->type) {
        case(ICMP_RTR_SOL): {
            puts("INFO: packet type: icmp router solicitation");
            /* processing router solicitation */
            recv_rtr_sol();
            /* init solicited router advertisment*/
            break;
        }

        case(ICMP_RTR_ADV): {
            puts("INFO: packet type: icmp router advertisment");
            /* processing router advertisment */
            recv_rtr_adv();
            /* init neighbor solicitation */
            break;
        }

        case(ICMP_NBR_SOL): {
            puts("INFO: packet type: icmp neighbor solicitation");
            recv_nbr_sol();
            break;
        }

        case(ICMP_NBR_ADV): {
            puts("INFO: packet type: icmp neighbor advertisment");
            recv_nbr_adv();
            break;
        }

        case(ICMP_RPL_CONTROL): {
            puts("INFO: packet type: RPL message");

            if(rpl_process_pid != 0) {
                msg_t m_send;
                m_send.content.ptr = (char *) &hdr->code;
                msg_send(&m_send, rpl_process_pid, 1);
            }
            else {
                puts("INFO: no RPL handler registered");
            }

            break;
        }

        default:
            return -1;
    }

    return 0;
}

void ipv6_process(void)
{
    msg_t m_recv_lowpan, m_send_lowpan;
    msg_t m_recv, m_send;
    ipv6_addr_t myaddr;
    ipv6_init_address(&myaddr, 0xabcd, 0x0, 0x0, 0x0, 0x3612, 0x00ff, 0xfe00,
                      get_radio_address());

    while(1) {
        msg_receive(&m_recv_lowpan);

        ipv6_buf = (struct ipv6_hdr_t *) m_recv_lowpan.content.ptr;

        /* identifiy packet */
        nextheader = &ipv6_buf->nextheader;

        if((ipv6_get_addr_match(&myaddr, &ipv6_buf->destaddr) >= 112) &&
           (ipv6_buf->destaddr.uint8[15] != myaddr.uint8[15])) {
            memcpy(get_ipv6_buf_send(), get_ipv6_buf(),
                   IPV6_HDR_LEN + ipv6_buf->length);
            lowpan_init((ieee_802154_long_t *)&(ipv6_buf->destaddr.uint16[4]),
                        (uint8_t *)get_ipv6_buf_send());
        }
        else {
            switch(*nextheader) {
                case(PROTO_NUM_ICMPV6): {
                    /* checksum test*/
                    if(icmpv6_csum(PROTO_NUM_ICMPV6) != 0xffff) {
                        printf("ERROR: wrong checksum\n");
                    }

                    icmp_buf = get_icmpv6_buf(ipv6_ext_hdr_len);
                    icmpv6_demultiplex(icmp_buf);
                    break;
                }

                case(IPPROTO_TCP): {
                    if(tcp_packet_handler_pid != 0) {
                        m_send.content.ptr = (char *) ipv6_buf;
                        msg_send_receive(&m_send, &m_recv, tcp_packet_handler_pid);
                    }
                    else {
                        printf("INFO: No TCP handler registered.\n");
                    }

                    break;
                }

                case(IPPROTO_UDP): {
                    if(udp_packet_handler_pid != 0) {
                        m_send.content.ptr = (char *) ipv6_buf;
                        msg_send_receive(&m_send, &m_recv, udp_packet_handler_pid);
                    }
                    else {
                        printf("INFO: No UDP handler registered.\n");
                    }

                    break;
                }

                case(PROTO_NUM_NONE): {
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

void ipv6_iface_add_addr(ipv6_addr_t *addr, uint8_t state, uint32_t val_ltime,
                         uint32_t pref_ltime, uint8_t type)
{
    if(ipv6_addr_unspec_match(addr) == 128) {
        printf("ERROR: unspecified address (::) can't be assigned to interface.\n");
        return;
    }

    if(ipv6_iface_addr_match(addr) != 0) {
        return;
    }

    if(iface_addr_list_count < IFACE_ADDR_LIST_LEN) {
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
        if(type == ADDR_TYPE_ANYCAST || type == ADDR_TYPE_LINK_LOCAL ||
           type == ADDR_TYPE_GLOBAL || type == ADDR_TYPE_UNICAST) {
            ipv6_addr_t sol_node_mcast_addr;
            ipv6_set_sol_node_mcast_addr(addr, &sol_node_mcast_addr);

            if(ipv6_iface_addr_match(&sol_node_mcast_addr) == NULL) {
                ipv6_iface_add_addr(&sol_node_mcast_addr, state, val_ltime, pref_ltime, ADDR_TYPE_SOL_NODE_MCAST);
            }
        }
    }
}

addr_list_t *ipv6_iface_addr_match(ipv6_addr_t *addr)
{
    int i;

    for(i = 0; i < iface_addr_list_count; i++) {
        if(memcmp(&(iface.addr_list[i].addr.uint8[0]),
                  &(addr->uint8[0]), 16) == 0) {
            return &(iface.addr_list[i]);
        }
    }

    return NULL;
}

addr_list_t *ipv6_iface_addr_prefix_eq(ipv6_addr_t *addr)
{
    int i;

    for(i = 0; i < iface_addr_list_count; i++) {
        if(memcmp(&(iface.addr_list[i].addr.uint8[0]),
                  &(addr->uint8[0]), 8) == 0) {
            return &(iface.addr_list[i]);
        }
    }

    return NULL;
}

void ipv6_iface_print_addrs(void)
{
    for(int i = 0; i < iface_addr_list_count; i++) {
        ipv6_print_addr(&(iface.addr_list[i].addr));
    }
}

void ipv6_init_addr_prefix(ipv6_addr_t *inout, ipv6_addr_t *prefix)
{
    inout->uint16[0] = prefix->uint16[0];
    inout->uint16[1] = prefix->uint16[1];
    inout->uint16[2] = prefix->uint16[2];
    inout->uint16[3] = prefix->uint16[3];

    memcpy(&(inout->uint8[8]), &(iface.laddr.uint8[0]), 8);
}

void ipv6_set_prefix(ipv6_addr_t *inout, ipv6_addr_t *prefix)
{
    inout->uint16[0] = prefix->uint16[0];
    inout->uint16[1] = prefix->uint16[1];
    inout->uint16[2] = prefix->uint16[2];
    inout->uint16[3] = prefix->uint16[3];
    inout->uint16[4] = 0;
    inout->uint16[5] = 0;
    inout->uint16[6] = 0;
    inout->uint16[7] = 0;
}

void ipv6_set_all_rtrs_mcast_addr(ipv6_addr_t *ipaddr)
{
    ipaddr->uint16[0] = HTONS(0xff02);
    ipaddr->uint16[1] = 0;
    ipaddr->uint16[2] = 0;
    ipaddr->uint16[3] = 0;
    ipaddr->uint16[4] = 0;
    ipaddr->uint16[5] = 0;
    ipaddr->uint16[6] = 0;
    ipaddr->uint16[7] = HTONS(0x0002);
}

void ipv6_set_all_nds_mcast_addr(ipv6_addr_t *ipaddr)
{
    ipaddr->uint16[0] = HTONS(0xff02);
    ipaddr->uint16[1] = 0;
    ipaddr->uint16[2] = 0;
    ipaddr->uint16[3] = 0;
    ipaddr->uint16[4] = 0;
    ipaddr->uint16[5] = 0;
    ipaddr->uint16[6] = 0;
    ipaddr->uint16[7] = HTONS(0x0001);
}

void ipv6_set_loaddr(ipv6_addr_t *ipaddr)
{
    ipaddr->uint16[0] = 0;
    ipaddr->uint16[1] = 0;
    ipaddr->uint16[2] = 0;
    ipaddr->uint16[3] = 0;
    ipaddr->uint16[4] = 0;
    ipaddr->uint16[5] = 0;
    ipaddr->uint16[6] = 0;
    ipaddr->uint16[7] = HTONS(0x0001);
}

void ipv6_get_saddr(ipv6_addr_t *src, ipv6_addr_t *dst)
{
    /* try to find best match if dest is not mcast or link local */
    int8_t itmp = -1;
    uint8_t tmp = 0;
    uint8_t bmatch = 0;

    if(!(ipv6_prefix_ll_match(dst)) && !(ipv6_prefix_mcast_match(dst))) {
        for(int i = 0; i < IFACE_ADDR_LIST_LEN; i++) {
            if(iface.addr_list[i].state == ADDR_STATE_PREFERRED) {
                if(!(ipv6_prefix_ll_match(&(iface.addr_list[i].addr)))) {
                    tmp = ipv6_get_addr_match(dst, &(iface.addr_list[i].addr));

                    if(tmp >= bmatch) {
                        bmatch = tmp;
                        itmp = i;
                    }
                }
            }
        }
    }
    else {
        for(int j = 0; j < IFACE_ADDR_LIST_LEN; j++) {
            if((iface.addr_list[j].state == ADDR_STATE_PREFERRED) &&
               ipv6_prefix_ll_match(&(iface.addr_list[j].addr))) {
                itmp = j;
            }
        }
    }

    if(itmp == -1) {
        memset(src, 0, 16);
    }
    else {
        memcpy(src, &(iface.addr_list[itmp].addr), 16);
    }
}

uint8_t ipv6_get_addr_match(ipv6_addr_t *src, ipv6_addr_t *dst)
{
    uint8_t val = 0, xor;

    for(int i = 0; i < 16; i++) {
        /* if bytes are equal add 8 */
        if(src->uint8[i] == dst->uint8[i]) {
            val += 8;
        }
        else {
            xor = src->uint8[i] ^ dst->uint8[i];

            /* while bits from byte equal add 1 */
            for(int j = 0; j < 8; j++) {
                if((xor & 0x80) == 0) {
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

void ipv6_set_ll_prefix(ipv6_addr_t *ipaddr)
{
    ipaddr->uint16[0] = HTONS(0xfe80);
    ipaddr->uint16[1] = 0;
    ipaddr->uint16[2] = 0;
    ipaddr->uint16[3] = 0;
}

void ipv6_init_address(ipv6_addr_t *addr, uint16_t addr0, uint16_t addr1,
                       uint16_t addr2, uint16_t addr3, uint16_t addr4,
                       uint16_t addr5, uint16_t addr6, uint16_t addr7)
{
    addr->uint16[0] = HTONS(addr0);
    addr->uint16[1] = HTONS(addr1);
    addr->uint16[2] = HTONS(addr2);
    addr->uint16[3] = HTONS(addr3);
    addr->uint16[4] = HTONS(addr4);
    addr->uint16[5] = HTONS(addr5);
    addr->uint16[6] = HTONS(addr6);
    addr->uint16[7] = HTONS(addr7);
}

uint8_t ipv6_prefix_ll_match(ipv6_addr_t *addr)
{
    if(addr->uint8[0] == 0xfe && addr->uint8[1] == 0x80) {
        return 1;
    }

    return 0;
}

uint8_t ipv6_prefix_mcast_match(ipv6_addr_t *addr)
{
    if(addr->uint8[0] == 0xff && addr->uint8[1] == 0x02) {
        return 1;
    }

    return 0;
}

uint8_t ipv6_addr_unspec_match(ipv6_addr_t *addr)
{
    if((addr->uint16[0] == 0) && (addr->uint16[1] == 0) &&
       (addr->uint16[2] == 0) && (addr->uint16[3] == 0) &&
       (addr->uint16[4] == 0) && (addr->uint16[5] == 0) &&
       (addr->uint16[6] == 0) && (addr->uint16[7] == 0)) {
        return 1;
    }

    return 0;
}

uint8_t ipv6_addr_sol_node_mcast_match(ipv6_addr_t *addr)
{
    /* note: cool if-condition*/
    if((addr->uint8[0] == 0xFF) && (addr->uint8[1] == 0x02) &&
       (addr->uint16[1] == 0x00) && (addr->uint16[2] == 0x00) &&
       (addr->uint16[3] == 0x00) && (addr->uint16[4] == 0x00) &&
       (addr->uint8[10] == 0x00) && (addr->uint8[11] == 0x01) &&
       (addr->uint8[12] == 0xFF)) {
        return 1;
    }

    return 0;
}

void ipv6_set_sol_node_mcast_addr(ipv6_addr_t *addr_in, ipv6_addr_t *addr_out)
{
    /* copy only the last 24-bit of the ip-address that is beeing resolved */
    addr_out->uint16[0] = HTONS(0xff02);
    addr_out->uint16[1] = 0;
    addr_out->uint16[2] = 0;
    addr_out->uint16[3] = 0;
    addr_out->uint16[4] = 0;
    addr_out->uint16[5] = HTONS(0x0001);
    addr_out->uint8[12] = 0xff;
    addr_out->uint8[13] = addr_in->uint8[13];
    addr_out->uint16[7] = addr_in->uint16[7];
}

void ipv6_print_addr(ipv6_addr_t *ipaddr)
{
    printf("%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x\n",
           ((uint8_t *)ipaddr)[0], ((uint8_t *)ipaddr)[1], ((uint8_t *)ipaddr)[2],
           ((uint8_t *)ipaddr)[3], ((uint8_t *)ipaddr)[4], ((uint8_t *)ipaddr)[5],
           ((uint8_t *)ipaddr)[6], ((uint8_t *)ipaddr)[7], ((uint8_t *)ipaddr)[8],
           ((uint8_t *)ipaddr)[9], ((uint8_t *)ipaddr)[10], ((uint8_t *)ipaddr)[11],
           ((uint8_t *)ipaddr)[12], ((uint8_t *)ipaddr)[13], ((uint8_t *)ipaddr)[14],
           ((uint8_t *)ipaddr)[15]);
}

uint8_t ipv6_next_hdr_unknown(uint8_t next_hdr)
{
    return  next_hdr == PROTO_NUM_ICMPV6 ||
            next_hdr == PROTO_NUM_NONE;
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

    ipv6_set_all_rtrs_mcast_addr(&addr);
    ipv6_iface_add_addr(&addr, ADDR_STATE_PREFERRED, 0, 0, ADDR_TYPE_MULTICAST);
}


uint8_t ipv6_is_router(void)
{
    ipv6_addr_t addr;

    ipv6_set_all_rtrs_mcast_addr(&addr);

    if(ipv6_iface_addr_match(&addr) != NULL) {
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

void set_rpl_process_pid(int pid)
{
    rpl_process_pid = pid;
}

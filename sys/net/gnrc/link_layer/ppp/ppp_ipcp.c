/*
 * Copyright (C) 2015 José Ignacio Alaos
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @ingroup     net_gnrc_ppp
 * @file
 * @brief       Implementation of PPP's IPCP protocol
 *
 * @author      José Ignacio Alamos <jialamos@uc.cl>
 * @}
 */

#include "net/gnrc/ppp/ipcp.h"
#include "net/gnrc/ppp/ppp.h"

#include "net/ppp/hdr.h"

#include "net/gnrc/ppp/opt.h"
#include "net/gnrc/ppp/fsm.h"
#include "net/gnrc/pkt.h"
#include "net/gnrc/pktbuf.h"
#include "net/gnrc/nettype.h"
#include "net/ipv4/hdr.h"
#include "net/udp.h"
#include "net/icmp.h"
#include "net/inet_csum.h"
#include <errno.h>

#define ENABLE_DEBUG    (0)
#include "debug.h"

#if ENABLE_DEBUG
/* For PRIu16 etc. */
#include <inttypes.h>
#endif

#define IPCP_OPT_SIZE_ADDRESS (4)   /**< size of Address option */
#define IPV4_DEFAULT_ID (1234)      /**< initial ID for IP packet */
#define DEFAULT_TUNNEL_PORT (0)     /**< default tunnel port */
#define DEFAULT_TUNNEL_ADDRESS (0)  /**< default tunnel IPv4 address */
#define PPP_ADDRESS_LEN (6)         /**< length of PPP address */

typedef struct __attribute__((packed)){
    ipv4_addr_t src;
    ipv4_addr_t dst;
    uint8_t zero;
    uint8_t protocol;
    network_uint16_t udp_len;
} udp_phdr_t;

static gnrc_ppp_fsm_conf_t *ipcp_get_conf_by_code(gnrc_ppp_fsm_t *cp, uint8_t code)
{
    switch (code) {
        case GNRC_PPP_IPCP_OPT_IP_ADDRESS:
            return &cp->conf[IPCP_IPADDRESS];
        default:
            return NULL;
    }
}

uint8_t ipcp_ipaddress_is_valid(gnrc_ppp_option_t *opt)
{
    return true;
}

uint8_t ipcp_ipaddress_build_nak_opts(uint8_t *buf)
{
    return 0;
}
void ipcp_ipaddress_set(gnrc_ppp_fsm_t *ipcp, gnrc_ppp_option_t *opt, uint8_t peer)
{
    uint8_t *payload;

    ppp_opt_get_payload(opt, (void **) &payload);
    if (!peer) {
        ((gnrc_ppp_ipcp_t *) ipcp)->ip = *((ipv4_addr_t *) payload);
    }
    else {
        ((gnrc_ppp_ipcp_t *) ipcp)->local_ip = *((ipv4_addr_t *) payload);
    }
}

static void ipcp_config_init(gnrc_ppp_fsm_t *ipcp)
{
    ipcp->conf = IPCP_NUMOPTS ? ((gnrc_ppp_ipcp_t *) ipcp)->ipcp_opts : NULL;

    ipcp->conf[IPCP_IPADDRESS].type = GNRC_PPP_IPCP_OPT_IP_ADDRESS;
    ipcp->conf[IPCP_IPADDRESS].default_value = byteorder_htonl(0);
    ipcp->conf[IPCP_IPADDRESS].size = IPCP_OPT_SIZE_ADDRESS;
    ipcp->conf[IPCP_IPADDRESS].flags = GNRC_PPP_OPT_ENABLED;
    ipcp->conf[IPCP_IPADDRESS].next = NULL;
    ipcp->conf[IPCP_IPADDRESS].is_valid = &ipcp_ipaddress_is_valid;
    ipcp->conf[IPCP_IPADDRESS].build_nak_opts = &ipcp_ipaddress_build_nak_opts;
    ipcp->conf[IPCP_IPADDRESS].set = &ipcp_ipaddress_set;
}

int ipcp_init(gnrc_netdev2_t *ppp_dev)
{
    netdev2_ppp_t *pppdev = (netdev2_ppp_t*) ppp_dev->dev;
	gnrc_ppp_protocol_t *prot_ipcp = (gnrc_ppp_protocol_t*) &pppdev->ipcp;
    ppp_protocol_init(prot_ipcp, ppp_dev, fsm_handle_ppp_msg, PROT_IPCP);
    fsm_init(ppp_dev, (gnrc_ppp_fsm_t *) prot_ipcp);
    ipcp_config_init((gnrc_ppp_fsm_t *) prot_ipcp);

    gnrc_ppp_ipcp_t *ipcp = (gnrc_ppp_ipcp_t *) prot_ipcp;
    gnrc_ppp_fsm_t *ipcp_fsm = (gnrc_ppp_fsm_t *) prot_ipcp;

    ipcp_fsm->supported_codes = FLAG_CONF_REQ | FLAG_CONF_ACK | FLAG_CONF_NAK | FLAG_CONF_REJ | FLAG_TERM_REQ | FLAG_TERM_ACK | FLAG_CODE_REJ;
    ipcp_fsm->prottype = GNRC_NETTYPE_IPCP;
    ipcp_fsm->restart_timer = GNRC_PPP_IPCP_RESTART_TIMER;
    ipcp_fsm->get_conf_by_code = &ipcp_get_conf_by_code;
    prot_ipcp->lower_layer = PROT_LCP;
    prot_ipcp->upper_layer = PROT_IPV4;
    ipcp->ip_id = IPV4_DEFAULT_ID;
    return 0;
}

static uint16_t checksum(gnrc_pktsnip_t *pkt)
{
    uint16_t acc = 0;
    uint16_t chksum = 0;

    while (pkt) {
        chksum = inet_csum_slice(chksum, pkt->data, pkt->size, acc);
        acc += pkt->size;
        pkt = pkt->next;
    }
    return chksum == 0xFFFF ? 0xFFFF : ~chksum;
}

static uint16_t udp_checksum(ipv4_addr_t src, ipv4_addr_t dst, uint8_t protocol, uint16_t udp_length, gnrc_pktsnip_t *pkt)
{
    uint8_t pseudo_hdr[12];
    udp_phdr_t *phdr = (udp_phdr_t *) pseudo_hdr;

    phdr->src = src;
    phdr->dst = dst;
    phdr->zero = 0;
    phdr->protocol = protocol;
    phdr->udp_len = byteorder_htons(udp_length);


    gnrc_pktsnip_t *chk = gnrc_pktbuf_add(pkt, pseudo_hdr, sizeof(pseudo_hdr), GNRC_NETTYPE_UNDEF);
    uint16_t chksum = checksum(chk);
    chk = gnrc_pktbuf_remove_snip(chk, chk);
    return chksum;
}

gnrc_pktsnip_t *gen_ip_pkt(gnrc_ppp_ipv4_t *ipv4, gnrc_pktsnip_t *payload, uint8_t protocol)
{
    gnrc_pktsnip_t *pkt = gnrc_pktbuf_add(payload, NULL, sizeof(ipv4_hdr_t), GNRC_NETTYPE_IPV4);
    ipv4_hdr_t *hdr = pkt->data;

    ipv4_addr_t dst = ipv4->tunnel_addr;
    gnrc_netdev2_t *gnrc_pppdev = ((gnrc_ppp_protocol_t *) ipv4)->pppdev;
    netdev2_ppp_t *pppdev = (netdev2_ppp_t*) gnrc_pppdev->dev;
    gnrc_ppp_ipcp_t *ipcp = (gnrc_ppp_ipcp_t *) (&pppdev->ipcp);
    ipv4_addr_t src = ipcp->ip;

    ipv4_hdr_set_version(hdr);
    ipv4_hdr_set_ihl(hdr, 5);
    hdr->ts = 0;
    hdr->tl = byteorder_htons(gnrc_pkt_len(pkt));
    hdr->id = byteorder_htons(++(ipcp->ip_id));
    ipv4_hdr_set_flags(hdr, 0);
    ipv4_hdr_set_fo(hdr, 0);
    hdr->ttl = 64;
    hdr->protocol = protocol;
    hdr->csum = byteorder_htons(0);
    hdr->src = src;
    hdr->dst = dst;

    /*Calculate checkshum*/
    hdr->csum = byteorder_htons(~inet_csum_slice(0, pkt->data, pkt->size, 0));

    return pkt;
}

gnrc_pktsnip_t *_build_udp(gnrc_ppp_ipv4_t *ipv4, gnrc_pktsnip_t *pkt)
{
    /* Add UDP header */
    gnrc_pktsnip_t *udp = gnrc_pktbuf_add(pkt, NULL, sizeof(udp_hdr_t), GNRC_NETTYPE_UNDEF);

    ipv4_addr_t dst = ipv4->tunnel_addr;
    gnrc_netdev2_t *gnrc_pppdev = ((gnrc_ppp_protocol_t *) ipv4)->pppdev;
    netdev2_ppp_t *pppdev = (netdev2_ppp_t*) gnrc_pppdev->dev;
    gnrc_ppp_ipcp_t *ipcp = (gnrc_ppp_ipcp_t *) (&pppdev->ipcp);
    ipv4_addr_t src = ipcp->ip;

    udp_hdr_t *udp_hdr = (udp_hdr_t *) udp->data;

    udp_hdr->src_port = byteorder_htons(53209);
    udp_hdr->dst_port = byteorder_htons(ipv4->tunnel_port);
    udp_hdr->length = byteorder_htons(gnrc_pkt_len(udp));
    udp_hdr->checksum = byteorder_htons(0);

    udp_hdr->checksum = byteorder_htons(udp_checksum(src, dst, 17, byteorder_ntohs(udp_hdr->length), udp));
    return udp;
}

int handle_ipv4(gnrc_ppp_protocol_t *protocol, uint8_t ppp_event, void *args)
{
    gnrc_netdev2_t *gnrc_pppdev = protocol->pppdev;
    netdev2_ppp_t *pppdev = (netdev2_ppp_t*) gnrc_pppdev->dev;
    gnrc_ppp_ipcp_t *ipcp = (gnrc_ppp_ipcp_t *) &pppdev->ipcp;

    (void) ipcp;

    gnrc_pktsnip_t *recv_pkt = (gnrc_pktsnip_t *) args;
    (void) recv_pkt;

    switch (ppp_event) {
        case PPP_LINKUP:
            DEBUG("gnrc_ppp: Obtained IP address! \n");
            DEBUG("Ip address is %i.%i.%i.%i\n", ipcp->ip.u8[0], ipcp->ip.u8[1], ipcp->ip.u8[2], ipcp->ip.u8[3]);
            protocol->state = PROTOCOL_UP;
            break;
        case PPP_LINKDOWN:
            DEBUG("gnrc_ppp: IPv4 down\n");
            protocol->state = PROTOCOL_DOWN;
            break;
        default:
            break;

    }
    return 0;
}


int ppp_ipv4_init(gnrc_netdev2_t *ppp_dev)
{
    netdev2_ppp_t *pppdev = (netdev2_ppp_t*) ppp_dev->dev;
    gnrc_ppp_ipv4_t *ipv4 = (gnrc_ppp_ipv4_t *) &pppdev->ipv4;

    ppp_protocol_init((gnrc_ppp_protocol_t*) ipv4, ppp_dev, handle_ipv4, PROT_IPV4);
    ipv4->tunnel_addr.u32 = byteorder_htonl(DEFAULT_TUNNEL_ADDRESS);
    ipv4->tunnel_port = DEFAULT_TUNNEL_PORT;
    return 0;
}

static gnrc_pktsnip_t *_encapsulate_pkt(gnrc_ppp_ipv4_t *ipv4, gnrc_pktsnip_t *pkt)
{
    gnrc_pktsnip_t *sent_pkt;

    sent_pkt = _build_udp(ipv4, pkt);
    sent_pkt = gen_ip_pkt(ipv4, sent_pkt, PROTNUM_UDP);
    return sent_pkt;
}

int _tunnel_is_set(gnrc_ppp_ipv4_t *ipv4)
{
    return byteorder_ntohl(ipv4->tunnel_addr.u32) != DEFAULT_TUNNEL_ADDRESS && ipv4->tunnel_port != DEFAULT_TUNNEL_PORT;
}
int ppp_ipv4_send(gnrc_netdev2_t *ppp_dev, gnrc_pktsnip_t *pkt)
{
    netdev2_ppp_t *pppdev = (netdev2_ppp_t*) ppp_dev->dev;
    int ipv4_ready = ((gnrc_ppp_protocol_t *) &pppdev->ipv4)->state == PROTOCOL_UP;

    if (!ipv4_ready) {
        DEBUG("gnrc_ppp: IPCP down. Dropping packet.\n");
        gnrc_pktbuf_release(pkt);
        return -1;
    }

    if (!_tunnel_is_set((gnrc_ppp_ipv4_t *) &pppdev->ipv4)) {
        printf("please add tunnel address and port\n");
        gnrc_pktbuf_release(pkt);
        return -1;
    }


    /* Remove netif*/
    pkt = gnrc_pktbuf_remove_snip(pkt, pkt);

    gnrc_pktsnip_t *send_pkt = _encapsulate_pkt((gnrc_ppp_ipv4_t *) &pppdev->ipv4, pkt);
    gnrc_ppp_send(ppp_dev, send_pkt);
    return 0;
}

gnrc_pktsnip_t *ppp_ipv4_recv(gnrc_netdev2_t *ppp_dev, gnrc_pktsnip_t *pkt)
{
    gnrc_pktsnip_t *ipv4_hdr = gnrc_pktbuf_mark(pkt, sizeof(ipv4_hdr_t), GNRC_NETTYPE_UNDEF);
    gnrc_pktsnip_t *udp_hdr = gnrc_pktbuf_mark(pkt, sizeof(udp_hdr_t), GNRC_NETTYPE_UNDEF);

    gnrc_pktbuf_remove_snip(pkt, ipv4_hdr);
    gnrc_pktbuf_remove_snip(pkt, udp_hdr);

    /* create netif header */
    gnrc_pktsnip_t *netif_hdr;
    netif_hdr = gnrc_pktbuf_add(NULL, NULL,
                                sizeof(gnrc_netif_hdr_t) + (2 * PPP_ADDRESS_LEN),
                                GNRC_NETTYPE_NETIF);

    if (netif_hdr == NULL) {
        goto safe_out;
    }

    gnrc_netif_hdr_init(netif_hdr->data, PPP_ADDRESS_LEN, PPP_ADDRESS_LEN);
    ((gnrc_netif_hdr_t *)netif_hdr->data)->if_pid = thread_getpid();

    LL_APPEND(pkt, netif_hdr);

    return pkt;
safe_out:
    gnrc_pktbuf_release(pkt);
    return NULL;
}

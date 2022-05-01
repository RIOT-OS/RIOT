#include "net/gnrc/ipv6/ipsec/ipsec_ts.h"
#include "utlist.h"
#include "net/gnrc/nettype.h"
#include "net/gnrc/ipv6/hdr.h"
#include "net/gnrc/ipv6/ext.h"
#include "net/gnrc/ipv6.h"
#include "net/udp.h"
#include "net/tcp.h"

int ipsec_ts_from_pkt(gnrc_pktsnip_t *pkt, ipsec_ts_t *ts)
{
    ipv6_hdr_t *ipv6_hdr = gnrc_ipv6_get_header(pkt);
    ipsec_ts_t tmp_ts = (ipsec_ts_t){ 0 };


    if (ipv6_hdr == NULL) {
        return -1;
    }
    tmp_ts.src = ipv6_hdr->src;
    tmp_ts.dst = ipv6_hdr->dst;
    tmp_ts.proto = ipv6_hdr->nh;
#if IS_USED(MODULE_GNRC_NETTYPE_UDP)
    udp_hdr_t *udp_hdr;
    gnrc_pktsnip_t *udp;
    if ((udp = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_UDP))) {
        udp_hdr = (udp_hdr_t *)udp->data;
        tmp_ts.src_port = ntohs(udp_hdr->src_port.u16);
        tmp_ts.dst_port = ntohs(udp_hdr->dst_port.u16);
    }
#endif
#if IS_USED(MODULE_GNRC_NETTYPE_TCP)
    tcp_hdr_t *tcp_hdr;
    gnrc_pktsnip_t *tcp;
    if ((tcp = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_TCP))) {
        tcp_hdr = (tcp_hdr_t *)tcp->data;
        tmp_ts.src_port = ntohs(tcp_hdr->src_port.u16);
        tmp_ts.dst_port = ntohs(tcp_hdr->dst_port.u16);
    }
#endif
    *ts = tmp_ts;

    return 0;
}

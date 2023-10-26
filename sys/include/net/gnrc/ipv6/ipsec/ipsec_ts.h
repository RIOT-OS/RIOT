/*
 * Copyright (C) 2022 Juliusz Neuman
 */


#ifndef NET_GNRC_IPV6_IPSEC_TS_H
#define NET_GNRC_IPV6_IPSEC_TS_H


#include "net/gnrc/pkt.h"
#include "net/gnrc/ipv6/ipsec/ipsec_enums.h"
#include "net/ipv6/addr.h"
#ifdef __cplusplus
extern "C" {
#endif


typedef struct __attribute__((__packed__)) {
    ipv6_addr_t dst;
    ipv6_addr_t src;
    uint16_t dst_port; //NULL when -1
    uint16_t src_port; //NULL when -1
    uint8_t proto;
} ipsec_ts_t;


int ipsec_ts_from_pkt(gnrc_pktsnip_t *pkt, ipsec_ts_t *ts);

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_IPV6_IPSEC_TS_H */

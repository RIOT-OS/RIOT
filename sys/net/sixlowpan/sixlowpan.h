#ifndef SIXLOWPAN_H
#define SIXLOWPAN_H

#define IP_PROCESS_STACKSIZE    2048
#define NC_STACKSIZE            512

/* fragment size in bytes*/
#define FRAG_PART_ONE_HDR_LEN   4
#define FRAG_PART_N_HDR_LEN     5

#define LOWPAN_IPHC_DISPATCH    0x60
#define LOWPAN_IPHC_FL_C        0x10
#define LOWPAN_IPHC_TC_C        0x08
#define LOWPAN_IPHC_CID         0x80
#define LOWPAN_IPHC_SAC         0x40
#define LOWPAN_IPHC_SAM         0x30
#define LOWPAN_IPHC_DAC         0x04
#define LOWPAN_IPHC_DAM         0x03
#define LOWPAN_IPHC_M           0x08
#define LOWPAN_IPHC_NH          0x04
#define LOWPAN_IPV6_DISPATCH    0x41
#define LOWPAN_CONTEXT_MAX      16

#include "transceiver.h"
#include "sixlowip.h"
#include <vtimer.h>

typedef struct lowpan_context_t {
    uint8_t num;
    ipv6_addr_t prefix;
    uint8_t length;
    uint8_t comp;
    vtimer_t lifetime;
} lowpan_context_t;

void sixlowpan_init(transceiver_type_t trans, uint8_t r_addr);
void sixlowpan_adhoc_init(transceiver_type_t trans, ipv6_addr_t *prefix, uint8_t r_addr);
void lowpan_init(ieee_802154_long_t *addr, uint8_t *data);
void lowpan_read(uint8_t *data, uint8_t length, ieee_802154_long_t *s_laddr,
           ieee_802154_long_t *d_laddr);
void lowpan_iphc_encoding(ieee_802154_long_t *dest);
void lowpan_iphc_decoding(uint8_t *data, uint8_t length,
                          ieee_802154_long_t *s_laddr,
                          ieee_802154_long_t *d_laddr);
uint8_t lowpan_context_len();
lowpan_context_t * lowpan_context_update(
                        uint8_t num, const ipv6_addr_t *prefix, 
                        uint8_t length, uint8_t comp,
                        uint16_t lifetime);
lowpan_context_t * lowpan_context_get();
lowpan_context_t * lowpan_context_lookup(ipv6_addr_t *addr);
lowpan_context_t * lowpan_context_num_lookup(uint8_t num);
void lowpan_ipv6_set_dispatch(uint8_t *data);
#endif

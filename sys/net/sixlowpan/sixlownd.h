/*
 * 6lowpan neighbor discovery
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup sixlowpan
 * @{
 * @file    sixlownd.h
 * @brief   6lowpan neighbor discovery constants, data structs, and prototypes
 * @author  Stephan Zeisberg <zeisberg@mi.fu-berlin.de>
 * @author  Martin Lenders <mlenders@inf.fu-berlin.de>
 * @author  Oliver Gesch <oliver.gesch@googlemail.com>
 * @}
 */

#ifndef SIXLOWND_H
#define SIXLOWND_H

#include <stdint.h>
#include "vtimer.h"
#include "sixlowpan.h"
#include "sixlowip.h"

/* parameter problem [rfc4443] */
#define PARA_PROB_LEN                   8
#define PARA_PROB_CODE_ERR              0   /* Erroneous header field encountered */
#define PARA_PROB_NXT_HDR_UNREC         1   /* Unrecognized Next Header type encountered */
#define PARA_PROB_IPV6_OPT_UNREC        2   /* Unrecognized IPv6 option encountered */
/* router solicitation */
#define RTR_SOL_LEN                     4
#define RTR_SOL_INTERVAL                4
#define RTR_SOL_MAX                     3
/* router advertisment */
#define RTR_ADV_M_FLAG                  0
#define RTR_ADV_O_FLAG                  0
#define RTR_ADV_MAX                     3
#define RTR_ADV_MAX_INTERVAL            600
#define RTR_ADV_LEN                     12
/* neighbour solicitation */
#define NBR_SOL_LEN                     20
/* neighbour advertisement */
#define NBR_ADV_LEN                     20
#define NBR_ADV_FLAG_R                  0x80
#define NBR_ADV_FLAG_S                  0x40
#define NBR_ADV_FLAG_O                  0x20
/* icmp message types rfc4443 */
#define ICMP_PARA_PROB                  4
/* icmp message types rfc4861 4.*/
#define ICMP_RTR_ADV                    134
#define ICMP_RTR_SOL                    133
#define ICMP_NBR_ADV                    136
#define ICMP_NBR_SOL                    135
#define ICMP_REDIRECT                   137	/* will be filtered out by the border router */
/* stllao option rfc4861 4.6.1 */
#define OPT_STLLAO_MIN_LEN              8
#define OPT_STLLAO_MAX_LEN              16
#define OPT_SLLAO_TYPE                  1
#define OPT_TLLAO_TYPE                  2
/* prefix info option rfc 4.6.2 */
#define OPT_PI_LIST_LEN                 5 //TODO: initalwert suchen        
#define OPT_PI_TYPE                     3
#define OPT_PI_LEN                      4
#define OPT_PI_HDR_LEN                  32
#define OPT_PI_FLAG_A                   0x40
#define OPT_PI_FLAG_L                   0x80
#define OPT_PI_VLIFETIME_INFINITE       0xffffffff
/* mtu option rfc4861 4.6.4 */
#define OPT_MTU_TYPE                    5
#define OPT_MTU_LEN                     1
#define OPT_MTU_HDR_LEN                 8
/* aro - address registration option draft-ietf-6lowpan-nd-14 4.2 */
#define OPT_ARO_TYPE                    31 /* TBD1  */
#define OPT_ARO_LEN                     2
#define OPT_ARO_HDR_LEN                 16
#define OPT_ARO_LTIME                   300 /* geeigneten wert finden */
#define OPT_ARO_STATE_SUCCESS           0
#define OPT_ARO_STATE_DUP_ADDR          1
#define OPT_ARO_STATE_NBR_CACHE_FULL    2
/* 6lowpan context option */
#define OPT_6CO_TYPE                    32
#define OPT_6CO_MIN_LEN                 2
#define OPT_6CO_MAX_LEN                 3
#define OPT_6CO_HDR_LEN                 8
#define OPT_6CO_LTIME                   5   /* geeigneten Wert finden */
#define OPT_6CO_FLAG_C                  0x10
#define OPT_6CO_FLAG_CID                0x0F
#define OPT_6CO_FLAG_C_VALUE_SET        1
#define OPT_6CO_FLAG_C_VALUE_UNSET      0
/* authoritative border router option */
#define OPT_ABRO_TYPE                   33
#define OPT_ABRO_LEN                    3
#define OPT_ABRO_HDR_LEN                24
/* authoritive border router cache size */
#define ABR_CACHE_SIZE                  2
/* neighbor cache size */
#define NBR_CACHE_SIZE                  8
#define NBR_CACHE_TYPE_GC               1
#define NBR_CACHE_TYPE_REG              2
#define NBR_CACHE_TYPE_TEN              3
#define NBR_CACHE_LTIME_TEN             20
/* neighbor status values */
#define NBR_STATUS_INCOMPLETE           0
#define NBR_STATUS_REACHABLE            1
#define NBR_STATUS_STALE                2
#define NBR_STATUS_DELAY                3
#define NBR_STATUS_PROBE                4
/* default router list size */
#define DEF_RTR_LST_SIZE                    3 /* geeigneten wert finden */

extern unsigned int nd_nbr_cache_rem_pid;

enum option_types_t {
    OPT_SLLAO = 1,
    OPT_TLLAO,
    OPT_PI,
    OPT_MTU,
    OPT_ARO,
    OPT_6CO,
    OPT_ABRO,
    OPT_DAR,
    OPT_DAC,
};

typedef struct __attribute__((packed)) {
    uint8_t type;
    uint8_t length;
} opt_buf_t;

typedef struct __attribute__((packed)) {
    uint8_t type;
    uint8_t length;
} opt_stllao_t;

typedef struct __attribute__((packed)) {
    uint8_t type;
    uint8_t length;
    uint16_t reserved;
    uint32_t mtu;
} opt_mtu_t;

typedef struct __attribute__((packed)) {
    uint8_t type;
    uint8_t length;
    uint8_t prefix_length;
    uint8_t l_a_reserved1;
    uint32_t val_ltime;
    uint32_t pref_ltime;
    uint32_t reserved2;
    ipv6_addr_t addr;
} opt_pi_t;

typedef struct __attribute__((packed)) {
    uint8_t type;
    uint8_t length;
    uint8_t status;
    uint8_t reserved1;
    uint16_t reserved2;
    uint16_t reg_ltime;
    ieee_802154_long_t eui64;
} opt_aro_t;

typedef struct __attribute__((packed)) {
    uint8_t type;
    uint8_t length;
    uint8_t c_length;
    uint8_t c_flags;
    uint16_t reserved;
    uint16_t val_ltime;
} opt_6co_hdr_t;

typedef struct __attribute__((packed)) {
    uint8_t type;
    uint8_t length;
    uint16_t version;
    uint32_t reserved;
    ipv6_addr_t addr;
} opt_abro_t;

typedef struct __attribute__((packed)) {
    uint8_t inuse;
    uint8_t adv;
    ipv6_addr_t addr;
    uint8_t length;
    uint8_t l_a_reserved1;
    uint32_t val_ltime;
    uint32_t pref_ltime;
    uint8_t infinite;
} plist_t;

struct __attribute__((packed)) rtr_adv_t {
    uint8_t hoplimit;
    uint8_t autoconfig_flags;
    uint16_t router_lifetime;
    uint32_t reachable_time;
    uint32_t retrans_timer;
};

struct __attribute__((packed)) nbr_sol_t {
    uint32_t reserved;
    ipv6_addr_t tgtaddr;
};

struct __attribute__((packed)) nbr_adv_t {
    uint8_t rso;
    uint8_t reserved[3];
    ipv6_addr_t tgtaddr;
};

struct __attribute__((packed)) para_prob_t {
    uint8_t pointer;
};

/* authoritive border router cache - draft-draft-ietf-6lowpan-nd-17 */
typedef struct __attribute__((packed)) abr_cache_t {
    uint16_t version;
    ipv6_addr_t abr_addr;
    uint8_t cids[LOWPAN_CONTEXT_MAX];
} abr_cache_t;

/* neighbor cache - rfc4861 5.1. */
typedef struct __attribute__((packed)) {
    uint8_t type;
    uint8_t state;
    uint8_t isrouter;
    ipv6_addr_t addr;
    ieee_802154_long_t laddr;
    ieee_802154_short_t saddr;
    timex_t ltime;
} nbr_cache_t;

/* default router list - rfc4861 5.1. */
typedef struct __attribute__((packed)) {
    ipv6_addr_t addr;
    timex_t inval_time;
} def_rtr_lst_t;

void init_rtr_sol(uint8_t sllao);
void recv_rtr_sol(void);
void recv_rtr_adv(void);
void init_rtr_adv(ipv6_addr_t *addr, uint8_t sllao, uint8_t mtu, uint8_t pi,
                  uint8_t sixco, uint8_t abro);
uint8_t plist_search(ipv6_addr_t *addr);
uint8_t plist_cmp(ipv6_addr_t *addr1, ipv6_addr_t *addr2);
int8_t plist_add(ipv6_addr_t *addr, uint8_t size, uint32_t val_ltime,
                 uint32_t pref_ltime, uint8_t adv_opt, uint8_t l_a_reserved1);
void set_llao(opt_stllao_t *sllao, uint8_t type, uint8_t length);

abr_cache_t *abr_get_version(uint16_t version, ipv6_addr_t *abr_addr);
abr_cache_t *abr_add_context(uint16_t version, ipv6_addr_t *abr_addr,
                             uint8_t cid);
void abr_remove_context(uint8_t cid);

nbr_cache_t *nbr_cache_search(ipv6_addr_t *ipaddr);
uint8_t nbr_cache_add(ipv6_addr_t *ipaddr, ieee_802154_long_t *laddr,
                      uint8_t isrouter, uint8_t state, uint8_t type,
                      uint16_t ltime, ieee_802154_short_t *saddr);
void nbr_cache_auto_rem(void);
void nbr_cache_rem(ipv6_addr_t *addr);
uint16_t icmpv6_csum(uint8_t proto);
def_rtr_lst_t *def_rtr_lst_search(ipv6_addr_t *ipaddr);
void def_rtr_lst_add(ipv6_addr_t *ipaddr, uint32_t rtr_ltime);
void def_rtr_lst_rem(def_rtr_lst_t *entry);
void init_para_prob(ipv6_addr_t *src, ipv6_addr_t *dest, uint8_t code, 
                    uint32_t pointer, uint8_t *packet, uint8_t packet_len);
void init_nbr_sol(ipv6_addr_t *src, ipv6_addr_t *dest, ipv6_addr_t *targ,
                  uint8_t slloa, uint8_t aro);
void init_nbr_adv(ipv6_addr_t *src, ipv6_addr_t *dst, ipv6_addr_t *tgt,
                  uint8_t rso, uint8_t sllao, uint8_t aro, uint8_t aro_state);
void recv_nbr_adv(void);
void recv_nbr_sol(void);
#endif /* SIXLOWND_H*/

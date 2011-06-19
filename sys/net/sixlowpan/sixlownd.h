/* 6LoWPAN ND header file */

#ifndef SIXLOWND_H
#define SIXLOWND_H

#include <stdint.h>
#include "vtimer.h"
#include "sixlowpan.h"
#include "sixlowip.h"

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
/* icmp message types rfc4861 4.*/
#define ICMP_RTR_ADV                    134
#define ICMP_RTR_SOL                    133
#define ICMP_NBR_ADV                    136
#define ICMP_NBR_SOL                    135 
/* stllao option rfc4861 4.6.1 */
#define OPT_STLLAO_LEN                  16
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
#define OPT_ARO_TYPE                    31 // TBD1 
#define OPT_ARO_LEN                     2
#define OPT_ARO_HDR_LEN                 16
#define OPT_ARO_LTIME                   300 // geeigneten wert finden
/* 6lowpan context option */
#define OPT_6CO_TYPE                    32
#define OPT_6CO_MIN_LEN                 2
#define OPT_6CO_MAX_LEN                 3
#define OPT_6CO_HDR_LEN                 8
#define OPT_6CO_LTIME                   5   // geeigneten Wert finden
#define OPT_6CO_FLAG_C                  0x10
#define OPT_6CO_FLAG_CID                0x0F
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
#define DEF_RTR_LST_SIZE                    3 // geeigneten wert finden

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

typedef struct __attribute__ ((packed)) opt_buf_t {
    uint8_t type;
    uint8_t length;    
} opt_buf_t;

typedef struct __attribute__ ((packed)) opt_stllao_t {
    uint8_t type;
    uint8_t length;
} opt_stllao_t;

typedef struct __attribute__ ((packed)) opt_mtu_t {
    uint8_t type;
    uint8_t length;
    uint16_t reserved;
    uint32_t mtu;
} opt_mtu_t;

typedef struct __attribute__ ((packed)) opt_pi_t {
    uint8_t type;
    uint8_t length;
    uint8_t prefix_length;
    uint8_t l_a_reserved1;
    uint32_t val_ltime;
    uint32_t pref_ltime;
    uint32_t reserved2;
    ipv6_addr_t addr; 
} opt_pi_t;

typedef struct __attribute__ ((packed)) opt_aro_t {
    uint8_t type;
    uint8_t length;
    uint8_t status;
    uint8_t reserved1;
    uint16_t reserved2;
    uint16_t reg_ltime;
    ieee_802154_long_t eui64;
} opt_aro_t;

typedef struct __attribute__ ((packed)) opt_6co_hdr_t {
    uint8_t type;
    uint8_t length;
    uint8_t c_length;
    uint8_t c_flags;
    uint16_t reserved;
    uint16_t val_ltime;
} opt_6co_hdr_t;

typedef struct __attribute__ ((packed)) opt_abro_t {
    uint8_t type;
    uint8_t length;
    uint16_t version;
    uint32_t reserved;
    ipv6_addr_t addr;
} opt_abro_t;

typedef struct __attribute__ ((packed)) plist_t {
    uint8_t inuse;  
    uint8_t adv;
    ipv6_addr_t addr;
    uint8_t length;
    uint8_t l_a_reserved1;
    uint32_t val_ltime;
    uint32_t pref_ltime;
    uint8_t infinite;
} plist_t;

struct __attribute__ ((packed)) rtr_adv_t {
    uint8_t hoplimit;
    uint8_t autoconfig_flags;
    uint16_t router_lifetime;
    uint32_t reachable_time;
    uint32_t retrans_timer;
};

struct __attribute__ ((packed)) nbr_sol_t {
    uint32_t reserved;
    ipv6_addr_t tgtaddr;  
};

struct __attribute__ ((packed)) nbr_adv_t {
    uint8_t rso;
    uint8_t reserved[3];
    ipv6_addr_t tgtaddr;  
};

/* authoritive border router cache - draft-draft-ietf-6lowpan-nd-17 */
typedef struct __attribute__((packed)) abr_cache_t {
    uint16_t version;
    ipv6_addr_t abr_addr;
    lowpan_context_t *contexts[LOWPAN_CONTEXT_MAX];
    uint8_t contexts_num;
    plist_t *prefixes[OPT_PI_LIST_LEN];
    uint8_t prefixes_num;
} abr_cache_t;

/* neighbor cache - rfc4861 5.1. */
typedef struct __attribute__ ((packed)) nbr_cache_t {
    uint8_t type;
    uint8_t state;
    uint8_t isrouter;
    ipv6_addr_t addr;
    ieee_802154_long_t laddr;
    ieee_802154_short_t saddr;
    vtimer_t ltime;
} nbr_cache_t;

/* default router list - rfc4861 5.1. */
typedef struct __attribute__ ((packed)) def_rtr_lst_t {
    ipv6_addr_t addr;
    vtimer_t inval_timer;
} def_rtr_lst_t;

void init_rtr_sol(uint8_t sllao);
void recv_rtr_sol(void);
void recv_rtr_adv(void);
void init_rtr_adv(ipv6_addr_t *addr, uint8_t sllao, uint8_t mtu, uint8_t pi, 
                  uint8_t sixco, uint8_t abro);
uint8_t plist_search(ipv6_addr_t *addr);
uint8_t plist_cmp(ipv6_addr_t *addr1, ipv6_addr_t *addr2);
void plist_add(ipv6_addr_t *addr, uint8_t size, uint32_t val_ltime,
             uint32_t pref_ltime, uint8_t adv_opt, uint8_t l_a_reserved1);
void set_llao(opt_stllao_t *sllao, uint8_t type, uint8_t length);
abr_cache_t *abr_update_cache(
                    uint16_t version, ipv6_addr_t abr_addr,
                    lowpan_context_t **contexts, uint8_t contexts_num,
                    plist_t **prefixes, uint8_t prefixes_num);
nbr_cache_t * nbr_cache_search(ipv6_addr_t *ipaddr);
void nbr_cache_add(ipv6_addr_t *ipaddr, ieee_802154_long_t *laddr,
                   uint8_t isrouter, uint8_t state, uint8_t type,
                   uint16_t ltime, ieee_802154_short_t *saddr);
void nbr_cache_auto_rem(void);
void nbr_cache_rem(ipv6_addr_t *addr);
uint16_t icmpv6_csum(uint8_t proto);
uint16_t csum(uint16_t sum, uint8_t *buf, uint16_t len);
def_rtr_lst_t * def_rtr_lst_search(ipv6_addr_t *ipaddr);
void def_rtr_lst_add(ipv6_addr_t *ipaddr, uint32_t rtr_ltime);
void def_rtr_lst_rem(def_rtr_lst_t *entry);
void init_nbr_sol(ipv6_addr_t *src, ipv6_addr_t *dest, ipv6_addr_t *targ,
                  uint8_t slloa, uint8_t aro);
void init_nbr_adv(ipv6_addr_t *src, ipv6_addr_t *dst, ipv6_addr_t *tgt, 
                  uint8_t rso, uint8_t sllao, uint8_t aro, uint8_t aro_state);
void plist_add(ipv6_addr_t *addr, uint8_t size, uint32_t val_ltime,
             uint32_t pref_ltime, uint8_t adv_opt, uint8_t l_a_reserved1);
void recv_nbr_sol(void);
#endif /* SIXLOWND_H*/

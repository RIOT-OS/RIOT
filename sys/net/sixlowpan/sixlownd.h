#include <stdint.h>

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
/* mtu option rfc4861 4.6.4 */
#define OPT_MTU_TYPE                    5
#define OPT_MTU_LEN                     1
#define OPT_MTU_HDR_LEN                 8
/* aro - address registration option draft-ietf-6lowpan-nd-14 4.2 */
#define OPT_ARO_TYPE                    31 // TBD1 
#define OPT_ARO_LEN                     2
#define OPT_ARO_HDR_LEN                 16
#define OPT_ARO_LTIME                   300 // geeigneten wert finden

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

typedef struct __attribute__ ((packed)) pfx_elem_t {
    uint8_t inuse;  
    uint8_t adv;
    ipv6_addr_t addr;
    uint8_t length;
    uint8_t l_a_reserved1;
    uint32_t val_ltime;
    uint32_t pref_ltime;
} pfx_elem_t;

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


void init_rtr_sol(void);
void recv_rtr_sol(void);
void init_rtr_adv(ipv6_addr_t *addr);
uint8_t pfx_chk_list(ipv6_addr_t *addr, uint8_t size);
uint8_t pfx_cmp(ipv6_addr_t *addr1, ipv6_addr_t *addr2);
void pfx_add(ipv6_addr_t *addr, uint8_t size, uint32_t val_ltime,
             uint32_t pref_ltime, uint8_t adv_opt, uint8_t l_a_reserved1);
void set_llao(uint8_t *llao, uint8_t type);

uint16_t icmpv6_csum(uint8_t proto);
uint16_t csum(uint16_t sum, uint8_t *buf, uint16_t len);

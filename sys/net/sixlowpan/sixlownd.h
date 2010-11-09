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
/* icmp message types rfc4861 4.*/
#define ICMP_RTR_ADV                    134
#define ICMP_RTR_SOL                    133 
/* stllao option rfc4861 4.6.1 */
#define OPT_STLLAO_LEN                  16
#define OPT_SLLAO_TYPE                  1
#define OPT_TLLAO_TYPE                  2               
/* mtu option rfc4861 4.6.4 */
#define OPT_MTU_TYPE                    5
#define OPT_MTU_LEN                     1
#define OPT_MTU_HDR_LEN                 8

typedef struct opt_stllao {
  uint8_t type;
  uint8_t length;
} opt_stllao;

typedef struct opt_mtu {
    uint8_t type;
    uint8_t length;
    uint16_t reserved;
    uint32_t mtu;
} opt_mtu;

typedef struct opt_pi {
    uint8_t type;
    uint8_t length;
    uint8_t prefix_length;
    uint8_t reserved1;
    uint32_t valid_lifetime;
    uint32_t preferred_lifetime;
    ipv6_addr prefix; 
} opt_pi;

typedef struct prefix_list {
    ipv6_addr prefix;
    uint8_t inuse;
    uint8_t advertisment;
} prefix_list;

struct rtr_adv {
    uint8_t hoplimit;
    uint8_t autoconfig_flags;
    uint16_t router_lifetime;
    uint32_t reachable_time;
    uint32_t retrans_timer;
};

void send_rtr_sol(void);
uint16_t chksum_calc(uint8_t type); 

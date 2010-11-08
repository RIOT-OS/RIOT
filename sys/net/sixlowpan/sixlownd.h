#include <stdint.h>

#define SLLAO_OPT_LENGTH                16 // 16 if 802.15.4 otherwise 8
#define ND_OPT_TYPE                     0
#define ND_OPT_LENGTH                   1
#define ND_OPT_DATA                     2
/* router advertisment */
#define ND_M_FLAG                       0
#define ND_O_FLAG                       0
#define MAX_RTR_ADV                     3
#define MAX_RTR_ADV_INTERVAL            600
#define RTR_ADV_LENGTH                  12
#define ICMP_RTR_ADV                    134 
/* router solicitation */
#define ICMP_RTR_                       133
#define RTR_SOL_LENGTH                  4
#define RTR_SOL_INTERVAL                4
#define MAX_RTR_SOL                     3
/* mtu option rfc4861 4.6.4*/
#define MTU_OPTION_TYPE                 5
#define MTU_OPTION_LENGTH               1
#define MTU_OPTION_HDR_LENGTH           8

typedef struct option_src_trgt_lla {
  uint8_t type;
  uint8_t length;
} option_src_trgt_lla;

typedef struct option_mtu {
    uint8_t type;
    uint8_t length;
    uint16_t reserved;
    uint32_t mtu;
} option_mtu;

typedef struct option_prefix_info {
    uint8_t type;
    uint8_t length;
    uint8_t prefix_length;
    uint8_t reserved1;
    uint32_t valid_lifetime;
    uint32_t preferred_lifetime;
    ipv6_addr prefix; 
} option_prefix_info;

struct router_advertisement {
    uint8_t hoplimit;
    uint8_t autoconfig_flags;
    uint16_t router_lifetime;
    uint32_t reachable_time;
    uint32_t retrans_timer;
};

void send_rs(void);
uint16_t chksum_calc(uint8_t type); 

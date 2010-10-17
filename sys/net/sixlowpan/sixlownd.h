#include <stdint.h>

#define RS_LENGTH                       4
#define SLLAO_OPT_LENGTH                16 // 16 if 802.15.4 otherwise 8
#define ND_OPT_TYPE                     0
#define ND_OPT_LENGTH                   1
#define ND_OPT_DATA                     2
#define ICMP_ROUTER_SOLICITATION        133
#define ROUTER_SOLICITATION_INTERVAL    4
#define MAX_RTR_SOLICITATIONS        3

typedef struct option_hdr {
  uint8_t type;
  uint8_t length;
} option_hdr;


void send_rs(void);
uint16_t chksum_calc(uint8_t type); 

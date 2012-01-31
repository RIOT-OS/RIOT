#include <stdint.h>
#include <vtimer.h>
#include <mutex.h>
#include "sys/net/sixlowpan/sixlowip.h"
#include "rpl_dodag.h"

#define RPL_PKT_RECV_BUF_SIZE 20
#define RPL_PROCESS_STACKSIZE 3072

void rpl_init();
void rpl_init_root();
rpl_of_t *rpl_get_of_for_ocp(uint16_t ocp);


void rpl_process(void);
void recv_rpl_dio(void);
void recv_rpl_dao(void);
void send_DIO(ipv6_addr_t *destination);
void send_DIS(ipv6_addr_t *destination);

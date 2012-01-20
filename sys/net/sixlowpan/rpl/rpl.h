#include <stdint.h>
#include <vtimer.h>
#include <mutex.h>
#include "sys/net/sixlowpan/sixlowip.h"
#include "rpl_dodag.h"

void rpl_init();
void rpl_init_root();

void rpl_icmp_handler(uint8_t code);
void recv_rpl_dio(void);
void recv_rpl_dao(void);
void send_DIO(void);

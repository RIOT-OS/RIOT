#include <stdio.h>
#include <string.h>
#include <vtimer.h>
#include <thread.h>
#include "sys/net/sixlowpan/sixlowip.h"
#include "sys/net/sixlowpan/sixlowpan.h"
#include "sys/net/sixlowpan/rpl/rpl.h"

void main(void)
{
	uint16_t root = 0x0001;
	ipv6_addr_t std_addr;
	uint16_t r_addr = root;
	ipv6_init_address(&std_addr, 0xABCD,0,0,0,0x1234,0xFFFF,0xFEDC,r_addr);
	sixlowpan_init(TRANSCEIVER_CC1100, r_addr, 0);
    rpl_init();
	if(root == 0x0001){
		rpl_init_root();
	}
	printf("RPL INIT FINISHED\n");
    while(1);
}


#include <stdio.h>
#include "kernel.h"
#include "msg.h"
#include "thread.h"
#include "byteorder.h"
#include "net/gnrc/nettype.h"
#include "net/gnrc/netreg.h"
#include "net/gnrc/netapi.h"
#include "net/gnrc/pktbuf.h"
#include "net/gnrc/pkt.h"
#include "net/gnrc/ipv6.h"
#include "net/ipv6/addr.h"
#include "net/gnrc/ipv6/nc.h"
#include "net/gnrc/netif.h"
#include "net/gnrc/tcp.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define PEER_PORT 2000
//#define PEER_ADDR "fe80::f4c6:4ff:fed2:a73e"
#define PEER_ADDR "fe80::6874:faff:fe20:fe4c"

gnrc_tcp_tcb_t global_tcp;

int main(void)
{
    /* Allocate control structres */
    ipv6_addr_t peer_addr;
    
    /* Config Peer Address */
    ipv6_addr_from_str(&peer_addr, PEER_ADDR);

    /* Initialize TCB and connect to peer */
    gnrc_tcp_tcb_init(&global_tcp);
    gnrc_tcp_open(&global_tcp, 0, (uint8_t *) &peer_addr, sizeof(peer_addr), PEER_PORT, 0);

    /* Call return either after, successful connection establishment or termination */
    if(global_tcp.state == ESTABLISHED){
        printf("Cli: Connection established\n");
    }else if(global_tcp.state == CLOSED){
        printf("Cli: Connection closed\n");
    }

    // Destroy Tr	ansmission Control Block
    gnrc_tcp_tcb_destroy(&global_tcp);

    exit(0);
}

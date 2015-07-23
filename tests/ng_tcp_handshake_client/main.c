#include <stdio.h>
#include "kernel.h"
#include "msg.h"
#include "thread.h"
#include "byteorder.h"
#include "net/ng_nettype.h"
#include "net/ng_netreg.h"
#include "net/ng_netapi.h"
#include "net/ng_pktbuf.h"
#include "net/ng_pkt.h"
#include "net/ng_ipv6.h"
#include "net/ng_ipv6/addr.h"
#include "net/ng_ipv6/nc.h"
#include "net/ng_netbase.h"
#include "net/ng_netif.h"
#include "transceiver.h"
#include "net/ng_tcp.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define PEER_PORT 2000
#define PEER_ADDR "fe80::f06a:5bff:fe2d:be90"

int main(void)
{
    /* Allocate control structres */
    ng_tcp_tcb_t tcb;
    ng_ipv6_addr_t peer_addr;
    
    /* Config Peer Address */
    ng_ipv6_addr_from_str(&peer_addr, PEER_ADDR);

    /* Initialize TCB and connect to peer */
    ng_tcp_tcb_init(&tcb);
    ng_tcp_open(&tcb, 0, (uint8_t *) &peer_addr, sizeof(peer_addr), PEER_PORT, 0);

    /* Call return either after, successful connection establishment or termination */
    if(tcb.state == ESTABLISHED){
        printf("Cli: Connection established\n");
    }else if(tcb.state == CLOSED){
        printf("Cli: Connection closed\n");
    }

    // Destroy Transmission Control Block
    ng_tcp_tcb_destroy(&tcb);

    exit(0);
}

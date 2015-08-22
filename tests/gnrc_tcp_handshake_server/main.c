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

#define LOCL_PORT 2000

int main(void)
{
    gnrc_tcp_tcb_t tcb;
    //gnrc_ipv6_addr_t locl_addr;
    ipv6_addr_t peer_addr = IPV6_ADDR_UNSPECIFIED;

    /* Print current server ipv6-addresses on interface 0 */
    printf("-- TCP Server Running on Port %d\n", LOCL_PORT);
    printf("-- Assigned IP-Addresses: \n");

    kernel_pid_t ifs[GNRC_NETIF_NUMOF];
    char ipv6_addr[IPV6_ADDR_MAX_STR_LEN];
    gnrc_netif_get(ifs);

    gnrc_ipv6_netif_t *entry = gnrc_ipv6_netif_get(ifs[0]);
    for (int i = 0; i < GNRC_IPV6_NETIF_ADDR_NUMOF; i++) {
        if (!ipv6_addr_is_unspecified(&entry->addrs[i].addr)) {
            printf("inet6 addr: ");
            if (ipv6_addr_to_str(ipv6_addr, &entry->addrs[i].addr, IPV6_ADDR_MAX_STR_LEN)) {
                printf("%s/%" PRIu8 "\n", ipv6_addr, entry->addrs[i].prefix_len);
            }
        }
    }

    /* Initiailze TCB and wait till peer connects */
    gnrc_tcp_tcb_init(&tcb);
    gnrc_tcp_open(&tcb, LOCL_PORT, (uint8_t *) &peer_addr, sizeof(peer_addr), 0, AI_PASSIVE);

    if(tcb.state == ESTABLISHED){
        printf("Srv: Connection Established\n");
    } else if(tcb.state == CLOSED){
        printf("Srv: Connection Closed\n");
    }

    // Destroy Transmission Control Block
    gnrc_tcp_tcb_destroy(&tcb);

    exit(0);
}
